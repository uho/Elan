/*
   File: imc_gen.c
   Generates intermediate code

   CVS ID: "$Id: imc_gen.c,v 1.10 2011/09/02 12:50:29 marcs Exp $"
*/

/* global includes */
#include <stdio.h>
#include <string.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>

/* local includes */
#include "options.h"
#include "elan_ast.h"
#include "decl_tree.h"
#include "elan_imc.h"
#include "ast_utils.h"
#include "type_table.h"
#include "ident.h"
#include "imc_utils.h"
#include "tgt_gen.h"
#include "imc_gen.h"

/*
   Procedure and operator naming
*/
static string make_routine_name (string pname, int is_op, type rtype, decl_list args)
	{ char buf[MAXSTRLEN];
	  buf[0] = '\0';
	  if (rtype != t_void)
	     { sprintf (buf, convert_type_text (rtype));
	       strcat (buf, " ");
	     };
	  if (is_op) strcat (buf, "OP ");
	  else strcat (buf, "PROC ");
	  strcat (buf, pname);
	  if (args -> size)
	     { int ix;
	       strcat (buf, " (");
	       for (ix = 0; ix < args -> size; ix++)
		  { decl arg = args -> array[ix];
		    if (arg -> tag != TAGFormal)
		       dcg_internal_error ("make_user_routine_name");
		    if (ix) strcat (buf, ", ");
		    strcat (buf, convert_type_text (arg -> typ));
		    if (arg -> Formal.acc == acc_var) strcat (buf, " VAR ");
		    else strcat (buf, " ");
		    strcat (buf, arg -> name);
		  };
	       strcat (buf, ")");
	     };
	  return (new_string (buf));
	};

/*
   Allocate user routine in procs structure
*/
static int allocate_user_routine (string pname, int is_op, type rtype, decl_list args, body pbody)
	{ string routine_name = make_routine_name (pname, is_op, rtype, args);
	  return (allocate_proc (attach_string (pname), routine_name, attach_type (rtype),
				 attach_decl_list (args), attach_body (pbody)));
	};

static int allocate_permanent_routine (string pname, type rtype, decl_list args, body pbody)
	{ string routine_name = make_routine_name (pname, 0, rtype, args);
	  int pnr = allocate_proc (attach_string (pname), routine_name, attach_type (rtype),
				   attach_decl_list (args), attach_body (pbody));
	  set_permanent_call_count (pnr);
	  return (pnr);
	};

/*
   Allocate type routine in procs structure
*/
static int allocate_type_routine (type t, tags_body kind)
	{ pragmat_list no_prags = new_pragmat_list ();
	  decl_list formals = new_decl_list ();
	  string pname = string_nil;
	  body tbody = body_nil;
	  type rtype = type_nil;
	  decl arg = decl_nil;
	  string routine_name;
	  switch (kind)
	     { case TAGAttach:
		  pname = "_attach";
		  arg = new_Formal (0, 0, attach_type (t), new_string ("a"), no_prags, acc_var);
		  tbody = new_Attach (attach_type (t));
		  rtype = attach_type (t);
		  break;
	       case TAGDetach:
		  pname = "_detach";
	          arg = new_Formal (0, 0, attach_type (t), new_string ("a"), no_prags, acc_var);
		  tbody = new_Detach (attach_type (t));
		  rtype = attach_type (t_void);
		  break;
	       case TAGGuard:
		  pname = "_guard";
	          arg = new_Formal (0, 0, attach_type (t), new_string ("a"), no_prags, acc_var);
		  tbody = new_Guard (attach_type (t));
		  rtype = attach_type (t);
		  break;
	       default: dcg_bad_tag (kind, "allocate_type_routine");
	     };
	  app_decl_list (formals, arg);
	  routine_name = make_routine_name (pname, 0, rtype, formals);
	  return (allocate_proc (new_string (pname), routine_name, rtype, formals, tbody));
	};

/*
   Check whether type needs garbage collection
*/
static int type_needs_gc (type t)
	{ switch (t -> tag)
	     { case TAGTname:
		  { decl id = t -> Tname.id;
	  	    if (id -> tag != TAGType)
		       dcg_internal_error ("type_needs_gc");
		    return (type_needs_gc (id -> Type.concr));
		  };
	       case TAGAddr:
	       case TAGInt:
	       case TAGBool:
	       case TAGReal:
	       case TAGNiltype: return (0);
	       case TAGFile:
	       case TAGText:
	       case TAGRow:
	       case TAGStruct: return (1);
	       default: dcg_bad_tag (t -> tag, "type_needs_gc");
	     };
	  return (0);
	};

/*
   Check whether type has subparts that need garbage collection
*/
static int type_has_gc_parts (type t)
	{ switch (t -> tag)
	     { case TAGTname:
		  { decl id = t -> Tname.id;
	  	    if (id -> tag != TAGType)
		       dcg_internal_error ("type_has_gc_parts");
		    return (type_has_gc_parts (id -> Type.concr));
		  };
	       case TAGAddr:
	       case TAGInt:
	       case TAGBool:
	       case TAGReal:
	       case TAGNiltype: return (0);
	       case TAGText: return (0);
	       case TAGFile: return (1);
	       case TAGRow: return (type_needs_gc (t -> Row.elt));
	       case TAGStruct:
		  { field_list flds = t -> Struct.flds;
		    int ix;
		    for (ix = 0; ix < flds -> size; ix++)
		       if (type_needs_gc (flds -> array[ix] -> ftype))
			  return (1);
		    return (0);
		  };
	       default: dcg_bad_tag (t -> tag, "type_has_gc_parts");
	     };
	  return (0);
	};

/*
   Routine to generate attach object
   Note that the attached object may have been copied.
   If this is possible (depending on the type (ROW,STRUCT)),
   the number of the temporary containing the attached value
   is returned.
*/
static int try_generate_attach_object (type t, oprd src)
	{ if (src -> tag != TAGVar) return (0);
	  switch (t -> tag)
	     { case TAGTname:
		  { decl id = t -> Tname.id;
		    if (id -> tag != TAGType) dcg_internal_error ("try_generate_attach_object");
		    return (try_generate_attach_object (id -> Type.concr, src));
		  };
	       case TAGAddr:
	       case TAGInt:
	       case TAGBool:
	       case TAGReal:
	       case TAGNiltype: return (0);
	       case TAGText:
	       case TAGFile:
	          gen_tuple (attach_adr, src, no_oprd (), no_oprd ());
	          return (0);
	       case TAGRow:
	       case TAGStruct:
		  if (!type_has_gc_parts (t))
		     { int vnr = allocate_temporary (t_addr);
		       gen_tuple (attach_adr, src, no_oprd (), new_Var (vnr));
		       return (vnr);
		     }
		  else if (!(t -> tnr))
		     dcg_internal_error ("try_generate_attach_object");
		  else
		     { type_info info = type_table -> array[t -> tnr];
		       pdecl_list pds = new_pdecl_list ();
		       int pnr = info -> attach_nr;
		       int vnr = allocate_temporary (t_addr);
		       app_pdecl_list (pds, new_pdecl (t_addr, acc_var));
		       if (!pnr)
		          { pnr = allocate_type_routine (t, TAGAttach);
			    info -> attach_nr = pnr;
		          };
		       increment_call_count (pnr);
		       gen_tuple (prepare_call, new_Iconst (1), new_Types (pds), no_oprd ());
	               gen_tuple (load_address_parameter, src, new_Iconst (0), no_oprd ());
		       gen_tuple (call_attach_procedure, new_Proc (pnr),
				  new_Iconst (1), new_Var (vnr));
		       return (vnr);
		     };
	       default: dcg_bad_tag (t -> tag, "try_generate_attach_object");
	     };
	  return (0);
	};

/*
   Routine to generate attach with offset
*/
static int try_generate_indirect_attach (type t, oprd src, oprd offset, int refl)
	{ if (src -> tag != TAGVar) return (0);
	  switch (t -> tag)
	     { case TAGTname:
	          { decl id = t -> Tname.id;
	            if (id -> tag != TAGType)
		       dcg_internal_error ("try_generate_indirect_attach:0");
	            return (try_generate_indirect_attach (id -> Type.concr, src, offset, refl));
	          };
	       case TAGAddr:
	       case TAGInt:
	       case TAGBool:
	       case TAGReal:
	       case TAGNiltype: break;
	       case TAGText:
	       case TAGFile:
	          gen_tuple (attach_offset, src, offset, no_oprd ());
	          break;
	       case TAGRow:
	       case TAGStruct:
		  if (!type_has_gc_parts (t))
		     { if (refl)
			  { gen_tuple (attach_offset, src, offset, reflexive ());
			    return (0);
			  }
		       else
			  { int vnr = allocate_temporary (t_addr);
		            gen_tuple (attach_offset, src, offset, new_Var (vnr));
		            return (vnr);
			  };
		     }
		  else if (!(t -> tnr))
		     dcg_internal_error ("try_generate_indirect_attach");
		  else
		     { type_info info = type_table -> array[t -> tnr];
		       int pnr = info -> attach_nr;
		       int vnr = allocate_temporary (t_addr);
		       pdecl_list pds = new_pdecl_list ();
		       app_pdecl_list (pds, new_pdecl (t_addr, acc_var));
		       if (!pnr)
		          { pnr = allocate_type_routine (t, TAGAttach);
			    info -> attach_nr = pnr;
		          };
		       increment_call_count (pnr);
		       gen_tuple (addr_plus_offset, src, offset, new_Var (vnr));
		       gen_tuple (prepare_call, new_Iconst (1), new_Types (pds), no_oprd ());
	               gen_tuple (load_parameter, new_Var (vnr), new_Iconst (0), no_oprd ());
		       if (refl)
			  { gen_tuple (call_attach_procedure, new_Proc (pnr),
				       new_Iconst (1), reflexive ());
			    return (0);
			  }
		       else
			  { int vnr2 = allocate_temporary (t_addr);
			    gen_tuple (call_attach_procedure, new_Proc (pnr),
				       new_Iconst (1), new_Var (vnr2));
			    return (vnr2);
			  };
		     };
		  break;
	       default: dcg_bad_tag (t -> tag, "try_generate_indirect_attach");
	     };
	  return (0);
	};

/*
   Routine to generate detach on objects
*/
static void try_generate_detach_object (type t, oprd dst)
	{ if (dst -> tag != TAGVar)
	     dcg_internal_error ("try_generate_detach_object");
	  switch (t -> tag)
	     { case TAGTname:
	          { decl id = t -> Tname.id;
	            if (id -> tag != TAGType) dcg_internal_error ("try_generate_detach_object");
		       try_generate_detach_object (id -> Type.concr, dst);
	          };
	       case TAGAddr:
	       case TAGInt:
	       case TAGBool:
	       case TAGReal:
	       case TAGNiltype: break;
	       case TAGText:
	          gen_tuple (detach_adr, dst, no_oprd (), no_oprd ());
	          break;
	       case TAGFile:
		  { string rname = new_string ("rts_detach_file");
		    oprd rproc = new_Rts (rname);
		    pdecl_list pds = new_pdecl_list ();
		    app_pdecl_list (pds, new_pdecl (attach_type (t), acc_var));
		    gen_tuple (prepare_call, new_Iconst (1), new_Types (pds), no_oprd ());
	            gen_tuple (load_address_parameter, dst, new_Iconst (0), no_oprd ());
		    gen_tuple (call_detach_procedure, rproc, new_Iconst (1), no_oprd ());
		  }; break;
	       case TAGRow:
	       case TAGStruct:
		  if (!type_has_gc_parts (t))
		     gen_tuple (detach_adr, dst, no_oprd (), no_oprd ());
		  else if (!(t -> tnr))
		     dcg_internal_error ("try_generate_detach_object");
		  else
		     { type_info info = type_table -> array[t -> tnr];
		       pdecl_list pds = new_pdecl_list ();
		       int pnr = info -> detach_nr;
		       app_pdecl_list (pds, new_pdecl (attach_type (t), acc_var));
		       if (!pnr)
		          { pnr = allocate_type_routine (t, TAGDetach);
			    info -> detach_nr = pnr;
		          };
		       increment_call_count (pnr);
		       gen_tuple (prepare_call, new_Iconst (1), new_Types (pds), no_oprd ());
	               gen_tuple (load_address_parameter, dst, new_Iconst (0), no_oprd ());
		       gen_tuple (call_detach_procedure, new_Proc (pnr),
				  new_Iconst (1), no_oprd ());
		     };
		  break;
	       default: dcg_bad_tag (t -> tag, "try_generate_detach_object");
	     };
	};

/*
   Routine to generate indirect detach with offset
*/
static void try_generate_indirect_detach (type t, oprd dst, oprd offset)
	{ if (dst -> tag != TAGVar)
	     dcg_internal_error ("try_generate_indirect_detach");
	  switch (t -> tag)
	     { case TAGTname:
	          { decl id = t -> Tname.id;
		    if (id -> tag != TAGType)
		       dcg_internal_error ("try_generate_indirect_detach");
		    try_generate_indirect_detach (id -> Type.concr, dst, offset);
		  };
	       case TAGAddr:
	       case TAGInt:
	       case TAGBool:
	       case TAGReal:
	       case TAGNiltype: break;
	       case TAGText:
	          gen_tuple (detach_offset, dst, offset, no_oprd ());
	          break;
	       case TAGFile:
		  { string rname = new_string ("rts_detach_file");
		    pdecl_list pds = new_pdecl_list ();
		    oprd rproc = new_Rts (rname);
		    int vnr = allocate_temporary (t_addr);
		    app_pdecl_list (pds, new_pdecl (attach_type (t), acc_var));
		    gen_tuple (addr_plus_offset, dst, offset, new_Var (vnr));
		    gen_tuple (prepare_call, new_Iconst (1), new_Types (pds), no_oprd ());
	            gen_tuple (load_parameter, new_Var (vnr), new_Iconst (0), no_oprd ());
		    gen_tuple (call_detach_procedure, rproc, new_Iconst (1), no_oprd ());
		  }; break;
	       case TAGRow:
	       case TAGStruct:
		  if (!type_has_gc_parts (t))
		     gen_tuple (detach_offset, dst, offset, no_oprd ());
		  else if (!(t -> tnr))
		     dcg_internal_error ("try_generate_indirect_detach");
		  else
		     { type_info info = type_table -> array[t -> tnr];
		       pdecl_list pds = new_pdecl_list ();
		       int vnr = allocate_temporary (t_addr);
		       int pnr = info -> detach_nr;
		       if (!pnr)
		          { pnr = allocate_type_routine (t, TAGDetach);
			    info -> detach_nr = pnr;
		          };
		       increment_call_count (pnr);
		       app_pdecl_list (pds, new_pdecl (attach_type (t), acc_var));
		       gen_tuple (addr_plus_offset, dst, offset, new_Var (vnr));
		       gen_tuple (prepare_call, new_Iconst (1), new_Types (pds), no_oprd ());
	               gen_tuple (load_parameter, new_Var (vnr), new_Iconst (0), no_oprd ());
		       gen_tuple (call_detach_procedure, new_Proc (pnr),
				  new_Iconst (1), no_oprd ());
		     };
		  break;
	       default: dcg_bad_tag (t -> tag, "try_generate_indirect_detach");
	     };
	};

/*
   Routine to generate var guard (pointed to by idst)
   Note that after setting the guard bit (and optionally copying the object),
   the guard temporary is set with an indirect copy.
*/
static void try_generate_var_guard (type t, oprd idst, int_list act_guards)
	{ switch (t -> tag)
	     { case TAGTname:
	          { decl id = t -> Tname.id;
		    if (id -> tag != TAGType)
		       dcg_internal_error ("try_generate_var_guard:0");
		    try_generate_var_guard (id -> Type.concr, idst, act_guards);
		  };
	       case TAGAddr:
	       case TAGInt:
	       case TAGBool:
	       case TAGReal:
	       case TAGNiltype:
	       case TAGFile:
	       case TAGText: break;
	       case TAGRow:
	       case TAGStruct:
		  if (!type_has_gc_parts (t))
		     { int isize = indirect_size_from_type (t);
		       int vnr = allocate_temporary (t);
		       app_int_list (act_guards, vnr);
		       gen_tuple (guard, idst, new_Iconst (isize), no_oprd ());
		       gen_tuple (copy_indirect_adr, idst, no_oprd (), new_Var (vnr));
		     }
		  else if (!(t -> tnr))
		     dcg_internal_error ("try_generate_var_guard");
		  else
		     { type_info info = type_table -> array[t -> tnr];
		       pdecl_list pds = new_pdecl_list ();
		       int pnr = info -> guard_nr;
		       int vnr = allocate_temporary (t);
		       app_int_list (act_guards, vnr);
		       if (!pnr)
		          { pnr = allocate_type_routine (t, TAGGuard);
			    info -> guard_nr = pnr;
		          };
		       increment_call_count (pnr);
		       app_pdecl_list (pds, new_pdecl (attach_type (t), acc_var));
		       gen_tuple (prepare_call, new_Iconst (1), new_Types (pds), no_oprd ());
	               gen_tuple (load_parameter, idst, new_Iconst (0), no_oprd ());
		       gen_tuple (call_guard_procedure, new_Proc (pnr),
				  new_Iconst (1), no_oprd ());
		       gen_tuple (copy_indirect_adr, idst, no_oprd (), new_Var (vnr));
		     };
		  break;
	       default: dcg_bad_tag (t -> tag, "try_generate_var_guard");
	     };
	};

/*
   Routines to generate code to copy values
   For ints, bools or addresses, just copy
   For other types (always addresses of some kind),
   attach the source, detach the destination, then copy (address)
*/
static void generate_copy (type t, oprd src, oprd dst)
	{ if (equal_oprd (src, dst)) return;	/* optimization */
	  switch (t -> tag)
	     { case TAGTname:
		  { decl id = t -> Tname.id;
		    if (id -> tag != TAGType) dcg_internal_error ("generate_copy");
		    generate_copy (id -> Type.concr, src, dst);
		  }; break;
	       case TAGAddr: gen_tuple (copy_adr, src, no_oprd (), dst); break;
	       case TAGInt:
	       case TAGBool: gen_tuple (copy_int, src, no_oprd (), dst); break;
	       case TAGReal: gen_tuple (copy_real, src, no_oprd (), dst); break;
	       case TAGFile:
	       case TAGText:
	       case TAGRow:
	       case TAGStruct:
		  { int vnr = try_generate_attach_object (t, src);
		    try_generate_detach_object (t, dst);
		    if (vnr)
		       gen_tuple (copy_adr, new_Var (vnr), no_oprd (), dst);
		    else gen_tuple (copy_adr, src, no_oprd (), dst);
		    if (vnr) release_variable (vnr);
		  }; break;
	       default: dcg_bad_tag (t -> tag, "generate_copy");
	     };
	};

static void generate_copy_indirect (type t, oprd src, oprd dst)
	{ switch (t -> tag)
	     { case TAGTname:
		  { decl id = t -> Tname.id;
		    if (id -> tag != TAGType) dcg_internal_error ("generate_copy_indirect");
		    generate_copy_indirect (id -> Type.concr, src, dst);
		  }; break;
	       case TAGInt:
	       case TAGBool:
		  gen_tuple (copy_int_indirect, src, no_oprd (), dst);
		  break;
	       case TAGReal:
		  gen_tuple (copy_real_indirect, src, no_oprd (), dst);
		  break;
	       case TAGAddr:
		  gen_tuple (copy_adr_indirect, src, no_oprd (), dst);
		  break;
	       case TAGFile:
	       case TAGText:
	       case TAGRow:
	       case TAGStruct:
		  { int vnr = try_generate_attach_object (t, src);
		    try_generate_indirect_detach (t, dst, new_Iconst (0));
		    if (vnr)
		       gen_tuple (copy_adr_indirect, new_Var (vnr), no_oprd (), dst);
		    else gen_tuple (copy_adr_indirect, src, no_oprd (), dst);
		    if (vnr) release_variable (vnr);
		  }; break;
	       default: dcg_bad_tag (t -> tag, "generate_copy_indirect");
	     };
	};

static void generate_indirect_copy (type t, oprd src, oprd dst)
	{ switch (t -> tag)
	     { case TAGTname:
		  { decl id = t -> Tname.id;
		    if (id -> tag != TAGType) dcg_internal_error ("generate_indirect_copy");
		    generate_indirect_copy (id -> Type.concr, src, dst);
		  }; break;
	       case TAGInt:
	       case TAGBool:
		  gen_tuple (copy_indirect_int, src, no_oprd (), dst);
		  break;
	       case TAGReal:
		  gen_tuple (copy_indirect_real, src, no_oprd (), dst);
		  break;
	       case TAGAddr:
		  gen_tuple (copy_indirect_adr, src, no_oprd (), dst);
		  break;
	       case TAGFile:
	       case TAGText:
	       case TAGRow:
	       case TAGStruct:
		  { int vnr = try_generate_indirect_attach (t, src, new_Iconst (0), 0);
		    try_generate_detach_object (t, dst);
		    if (vnr)
		       gen_tuple (copy_adr, new_Var (vnr), no_oprd (), dst);
		    else gen_tuple (copy_indirect_adr, src, no_oprd (), dst);
		    if (vnr) release_variable (vnr);
		  }; break;
	       default: dcg_bad_tag (t -> tag, "generate_indirect_copy");
	     };
	};

static void try_generate_detach_variable (int vnr)
	{ var tmp = im_vars -> array[vnr];
	  try_generate_detach_object (tmp -> vtype, new_Var (vnr));
	};

/*
   Creation and deallocation of variables
*/
static void create_variables (decl_tree pkdecls, int global)
	{ int ix; 
	  if (pkdecls == decl_tree_nil) return;
	  create_variables (pkdecls -> left, global);
	  for (ix = 0; ix < pkdecls -> decls -> size; ix++)
	     { decl d = pkdecls -> decls -> array[ix];
	       if (d -> tag == TAGObject_decl)
		  { int vnr = create_variable (d, global);
		    if (type_needs_gc (d -> typ))
		       gen_tuple (undefine, no_oprd (), no_oprd (), new_Var (vnr));
		  }
	     };
	  create_variables (pkdecls -> right, global);
	};

/* Note not the formal parameters */
static void release_variables (decl_tree pkdecls)
	{ int ix;
	  if (pkdecls == decl_tree_nil) return;
	  release_variables (pkdecls -> left);
	  for (ix = 0; ix < pkdecls -> decls -> size; ix++)
	     { decl d = pkdecls -> decls -> array[ix];
	       if (d -> tag == TAGObject_decl)
		  { int vnr = pick_variable (d);
		    try_generate_detach_variable (vnr);
		    release_variable (vnr);
		  };
	     };
	  release_variables (pkdecls -> right);
	};

/*
   allocate temporary and undefine it
*/
static int new_temporary (type t)
	{ int vnr = allocate_temporary (t);
	  if (type_needs_gc (t))
	     gen_tuple (undefine, no_oprd (), no_oprd (), new_Var (vnr));
	  return (vnr);
	};

/*
   We will use four kinds of translation
   T (value, construct, >oprd, type>): translate construct with result in oprd
   T (addr,  construct, >oprd, type>): translate construct with
                         	       address of result in oprd
   T (oprd,  construct, oprd>, type>): translate construct with result in oprd;
                                 	return oprd for use in translation
   T (void,  construct, --, void>):     translate construct with no result
*/
static xkind defer_oprd_translation (xkind k, type t, oprd *dst, refadm_list adm)
	{ refadm top = adm -> array[adm -> size - 1];
	  int_list temps = top -> active_temps;
	  int vnr;

	  if (k != k_oprd) return (k);
	  vnr = new_temporary (t);
	  app_int_list (temps, vnr);
	  *dst = new_Var (vnr);
	  return (k_value);
	};

/*
   T (void,      , --, void):.
   T (>kind, unit1; unit2; ... ; unit n, >oprd>, type):
      T (void, unit1, --) + detach used temporaries
      T (void, unit2, --) + detach used temporaries
        :
      T (void, unit n-1, --) + detach used temporaries
      T (>kind, >unit n, >dest>, type) + detach used temporaries.
*/
static type translate_node (xkind k, node n, oprd *dst, refadm_list adm, int_list act_guards);
static type translate_paragraph (xkind k, node_list par, oprd *dst,
				 refadm_list adm, int_list act_guards)
	{ refadm top = adm -> array[adm -> size - 1];
	  int_list temps = top -> active_temps;
	  int_list new_guards = new_int_list ();
	  int nr_nodes = par -> size;
	  type rtype = type_nil;
	  int ix;
	  for (ix = 0; ix < nr_nodes; ix++)
	     { node curr_node = par -> array[ix];
	       oprd empty;
	       int iy;
	       int hw = temps -> size;

	       /* translate node */
	       tuple ins_loc = get_next_location ();

	       if (generate_stabs && top -> trc)
		  gen_tuple (sline, new_Iconst (curr_node -> line),
				    new_Iconst (curr_node -> column), no_oprd ());

	       if (ix == nr_nodes - 1)
	          rtype = translate_node (k, curr_node, dst, adm, new_guards);
	       else (void) translate_node (k_void, curr_node, &empty, adm, new_guards);

	       /* if possible detach generated guards */
	       if ((ix != nr_nodes - 1) || (k != k_addr))
		  { for (iy = 0; iy < new_guards -> size; iy++)
		       { int vnr = new_guards -> array[iy];
		         insert_tuple (ins_loc, undefine, no_oprd (), no_oprd (), new_Var (vnr));

		         /* Note we only call rts_detach and not its type detach procedure */
		         gen_tuple (detach_adr, new_Var (vnr), no_oprd (), no_oprd ());
		       };
		    /* Do not detach more */
		    new_guards -> size = 0;
		  }
	       else
		  /* Not possible to detach here; pass them on to the surrounding environment */
	          for (iy = 0; iy < new_guards -> size; iy++)
		     app_int_list (act_guards, new_guards -> array[iy]);

	       /* detach temporaries */
	       for (iy = temps -> size - 1; hw <= iy; iy--)
	          { int vnr = temps -> array[iy];
		    try_generate_detach_variable (vnr);
		    release_variable (vnr);
		    del_int_list (temps, iy);
		  };
	     };
	  detach_int_list (&new_guards);
	  return (rtype);
	};

/*
   T (void, object :: src, --, void):
      T (value, src, obj) where obj stands for the runtime object
*/
static type translate_initialization (xkind k, node init, refadm_list adm, int_list act_guards)
	{ oprd obj = new_Var (pick_variable (init -> Init.id));
	  if (k != k_void) dcg_internal_error ("translate_initialization");
	  translate_node (k_value, init -> Init.src, &obj, adm, act_guards);
	  return (t_void);
	};

/*
   The assignment is translated in two ways, depending whether
   the left hand side is a simple identifier or not. Currently
   it is not yet possible to have a complex left hand side. This
   will change when the conditional will be introduced to MINI
  
   T (void, dest := src, --, void):
      IF dest is an identifier application
      THEN Tsass (dest := src).
      ELSE Tcass (dest := src).
      FI

   First the simple case (goes like the initialization).

   Tsass (name := rhs):
      T (value, src, obj) where obj stands for the runtime variable    
*/
static void translate_simple_assign (node ass, refadm_list adm, int_list act_guards)
	{ decl dest_id = ass -> Assign.dest -> Appl.id;
	  oprd obj = new_Var (pick_variable (dest_id));
	  (void) translate_node (k_value, ass -> Assign.src, &obj, adm, act_guards);
	};

/*
   The complex assign
   Tcass (dest := src):
      T (addr, dest, >temp)
      T (oprd, src, oprd>)
      gen copy indirect (typeof (src), oprd, temp);
*/
static void translate_complex_assign (node ass, refadm_list adm, int_list act_guards)
	{ refadm top = adm -> array[adm -> size - 1];
	  int_list temps = top -> active_temps;
	  int vnr = new_temporary (t_addr);
	  oprd temp, rop;
	  type rtype;

	  app_int_list (temps, vnr);
	  temp = new_Var (vnr);
	  (void) translate_node (k_addr, ass -> Assign.dest, &temp, adm, act_guards);
	  rtype = translate_node (k_oprd, ass -> Assign.src, &rop, adm, act_guards);
	  generate_copy_indirect (rtype, rop, temp);
	};

static type translate_assignment (xkind k, node ass, refadm_list adm, int_list act_guards)
	{ if (k != k_void) dcg_internal_error ("translate_assignment");
	  if (ass -> Assign.dest -> tag == TAGAppl)
	     translate_simple_assign (ass, adm, act_guards);
	  else translate_complex_assign (ass, adm, act_guards);
	  return (t_void);
	};

/*
   T (>kind, IF enq THEN thenpart ELSE elsepart FI, >oprd>):
      T (oprd, enq, enq_oprd>)
      [ jump if false, enq_oprd , --, L1 ]
      T (kind, then part, oprd)
      [ jump, --, --, L2 ]
   L1:T (kind, else part, oprd)
   L2:
*/
static type translate_conditional (xkind k, node cond, oprd *dst,
				    refadm_list adm, int_list act_guards)
	{ type ctype = cond -> Cond.typ;
	  xkind newk = defer_oprd_translation (k, ctype, dst, adm);
	  int else_label = new_label ();
	  int fi_label = new_label ();
	  oprd enq_op;

	  /* code test condition */
	  translate_node (k_oprd, cond -> Cond.enq, &enq_op, adm, act_guards);
	  gen_tuple (jump_if_int_equal_int, enq_op, new_Iconst (0), new_Lab (else_label));

	  /* code then part */
	  translate_paragraph (newk, cond -> Cond.thenp, dst, adm, act_guards);
	  gen_tuple (jump, no_oprd (), no_oprd (), new_Lab (fi_label));

	  /* code else part */
	  set_label (else_label);
	  translate_paragraph (newk, cond -> Cond.elsep, dst, adm, act_guards);
	  set_label (fi_label);
	  return (ctype);
	};

/*
   numerical choice
*/
static void translate_case_labels (oprd enq_op, int_list els, int endalt_label)
	{ int size = els -> size;
	  int ix;
	  int alt_label = (size > 1)?new_label ():0;
	  for (ix = 0; ix < size - 1; ix++)
	     gen_tuple (jump_if_int_equal_int, enq_op, new_Iconst (els -> array[ix]),
			new_Lab (alt_label));
	  gen_tuple (jump_if_int_notequal_int, enq_op, new_Iconst (els -> array[size - 1]),
		     new_Lab (endalt_label));
	  if (alt_label) set_label (alt_label);
	};

static void translate_case_part (xkind k, case_part cp, oprd *dst, oprd enq_op,
				  int endselect_label, refadm_list adm, int_list act_guards)
	{ int endalt_label = new_label ();
	  translate_case_labels (enq_op, cp -> els, endalt_label);
	  translate_paragraph (k, cp -> lcod, dst, adm, act_guards);
	  gen_tuple (jump, no_oprd (), no_oprd (), new_Lab (endselect_label));
	  set_label (endalt_label);
	};

static type translate_numerical_choice (xkind k, node num, oprd *dst,
					 refadm_list adm, int_list act_guards)
	{ type ntype = num -> Num.typ;
	  xkind newk = defer_oprd_translation (k, ntype, dst, adm);
	  case_part_list cps = num -> Num.cps;
	  int endselect_label = new_label ();
	  oprd enq_op;
	  int ix;

	  /* code evaluation of select part */
	  translate_node (k_oprd, num -> Num.enq, &enq_op, adm, act_guards);

	  /* code case parts */
	  for (ix = 0; ix < cps -> size; ix++)
	     translate_case_part (k, cps -> array[ix], dst, enq_op,
				  endselect_label, adm, act_guards);

	  /* code otherwise part */
	  translate_paragraph (newk, num -> Num.oth, dst, adm, act_guards);

	  set_label (endselect_label);
	  return (ntype);
	};

/*
   The simple repetition and generalized repetition share some code.
*/

/*
   create loop variable
*/
static oprd make_loop_variable (node rep, refadm_list adm)
	{ if (rep -> For.id == decl_nil)
	     { refadm top = adm -> array[adm -> size - 1];
	       int_list temps = top -> active_temps;
	       int vnr = new_temporary (t_int);
	       app_int_list (temps, vnr);
	       return (new_Var (vnr));
	     }
	  else return (new_Var (pick_variable (rep -> For.id)));
	};

/*
   Tfrompart (>from, >loop_var)
      IF from != default
      THEN T (value, from, >loop_var)
      ELSE < :=I, #1, --, loop_var)
      FI
*/
static void translate_from_part (node from, oprd loop_var, refadm_list adm, int_list act_guards)
	{ if (from -> tag != TAGDefault)
	     translate_node (k_value, from, &loop_var, adm, act_guards);
	  else gen_tuple (copy_int, new_Iconst (1), no_oprd (), loop_var);
	};

/*
   Ttopart (>to, >loop_var)
      IF to != default
      THEN
	 T (oprd, to, >final_val)
	 final_val
      ELSE no_oprd ()
      FI
*/
static oprd translate_to_part (node to, refadm_list adm, int_list act_guards)
	{ refadm top = adm -> array[adm -> size - 1];
	  int_list temps = top -> active_temps;
	  int final_vnr;
	  oprd final_val;
	  if (to -> tag == TAGDefault) return (no_oprd ());
	  final_vnr = new_temporary (t_int);
	  app_int_list (temps, final_vnr);
	  final_val = new_Var (final_vnr);
	  translate_node (k_value, to, &final_val, adm, act_guards);
	  return (final_val);
	};

/*
   Tcomp (>
   attachen van oprds lijkt me wel zinvol
*/
static void translate_loopvar_comparison (oprd loop_var, oprd final_val, int dir, int loop_end)
	{ if (final_val -> tag == TAGNop) return;
	  if (dir) gen_tuple (jump_if_int_less_than_int, loop_var, final_val, new_Lab (loop_end));
	  else gen_tuple (jump_if_int_greater_than_int, loop_var, final_val, new_Lab (loop_end));
	};

/*
   Twhilepart (>wenq, Lend)
      IF wenq != default
      THEN
         T (>oprd, >wenq, wenq_op>)
         < IFF, wenq_op, --, Lend >
      FI
*/
static void translate_while_part (node wenq, int loop_end, refadm_list adm, int_list act_guards)
	{ oprd wenq_op;
	  if (wenq -> tag == TAGDefault) return;
	  translate_node (k_oprd, wenq, &wenq_op, adm, act_guards);
	  gen_tuple (jump_if_int_equal_int, wenq_op, new_Iconst (0), new_Lab (loop_end));
	};

static void translate_until_part (node uenq, int loop_iter, int loop_end,
				  refadm_list adm, int_list act_guards, int rep)
	{ if (uenq -> tag != TAGDefault) 
	     { oprd uenq_op;
	       translate_node (k_oprd, uenq, &uenq_op, adm, act_guards);
	       if (rep) gen_tuple (jump_if_int_notequal_int, uenq_op,
				   new_Iconst (0), new_Lab (loop_end));
	       else gen_tuple (jump_if_int_equal_int, uenq_op,
			       new_Iconst (0), new_Lab (loop_iter));
	     }
	  else if (!rep)
	     gen_tuple (jump, no_oprd (), no_oprd (), new_Lab (loop_iter));
	};

static void translate_loopvar_change (oprd loop_var, int dir)
	{ if (dir)
	     gen_tuple (int_minus_int, loop_var, new_Iconst (1), loop_var);
	  else gen_tuple (int_plus_int, loop_var, new_Iconst (1), loop_var);
	};

/*
   T (>void, >WHILE cond1 REP body UNTIL cond2 ENDREP, --):
   Lb:
      IF cond1 preset
      THEN
	 T(>oprd, >cond1, cond1_oprd>)
         [ jif==I, cond1_oprd, #0, Le ]
      FI
      T (>void, body, --)
      IF cond2 present
      THEN
	 T(>oprd, >cond2, cond2_oprd>)
         [ jif==I, cond2_oprd, #0, Lb ]
      ELSE
         [ jmp, --, --, Lb ]
      FI
   Le:
*/
static type translate_while (xkind k, node whle, refadm_list adm, int_list act_guards)
	{ int loop_iter = new_label ();
	  int loop_end = new_label ();
	  oprd empty;
	  if (k != k_void) dcg_internal_error ("translate_while");

	  /* code test loop condition */
	  set_label (loop_iter);
	  translate_while_part (whle -> While.wenq, loop_end, adm, act_guards);
	  translate_paragraph (k_void, whle -> While.body, &empty, adm, act_guards);
	  translate_until_part (whle -> While.uenq, loop_iter, loop_end, adm, act_guards, 0);

	  /* code loop end */
	  set_label (loop_end);
	  return (t_void);
	};

static type translate_repetition (xkind k, node rep, refadm_list adm, int_list act_guards)
	{ oprd loop_var = make_loop_variable (rep, adm);
	  int loop_iter = new_label ();
	  int loop_end = new_label ();
	  oprd final_val;
	  oprd empty;
	  if (k != k_void) dcg_internal_error ("translate_repetition");

	  /* code loop initialization */
	  translate_from_part (rep -> For.from, loop_var, adm, act_guards);
	  final_val = translate_to_part (rep -> For.to, adm, act_guards);

	  /* code loop start */
	  set_label (loop_iter);
	  translate_loopvar_comparison (loop_var, final_val, rep -> For.dir, loop_end);
	  translate_while_part (rep -> For.wenq, loop_end, adm, act_guards);
	  translate_paragraph (k_void, rep -> For.body, &empty, adm, act_guards);
	  translate_until_part (rep -> For.uenq, loop_iter, loop_end, adm, act_guards, 1);
	  translate_loopvar_change (loop_var, rep -> For.dir);
	  gen_tuple (jump, no_oprd (), no_oprd (), new_Lab (loop_iter));

	  /* code loop end */
	  set_label (loop_end);
	  return (t_void);
	};

/*
   T (>void, >LEAVE alg WITH result, --)
*/
static type translate_leave (xkind k, node lv, refadm_list adm)
	{ decl alg = lv -> Leave.id;
	  int_list act_guards = new_int_list ();
	  refadm left_adm;
	  oprd result_oprd;
	  int alg_idx = -1;
	  int ix;

	  /* self check */
	  if (k != k_void) dcg_internal_error ("translate_leave");

	  /* try find administration of left algorithm */
	  if (alg -> tag != TAGRefinement) alg_idx = 0;	/* leave PROC/OP */
	  else
	     for (ix = 0; ix < adm -> size; ix++)
		{ refadm d = adm -> array[ix];
		  if (d -> algorithm == alg)
		     { alg_idx = ix;
		       break;
		     };
		}
	  if (alg_idx == -1) dcg_internal_error ("translate_leave");
	  left_adm = adm -> array[alg_idx];

	  /* generate result */
	  result_oprd = attach_oprd (left_adm -> result);
	  translate_node (left_adm -> kind, lv -> Leave.with, &result_oprd, adm, act_guards);
	  /* add newly created guards to those of the left algorithm */
	
	  /* detach temporaries created on our way */
	  for (ix = alg_idx; ix < adm -> size; ix++)
	     { int_list temps = adm -> array[ix] -> active_temps;
	       int iy;
	       for (iy = 0; iy < temps -> size; iy++)
		  try_generate_detach_variable (temps -> array[iy]);
	     };

	  /* create jump and be done with */
	  if (!left_adm -> lab) left_adm -> lab = new_label ();
	  gen_tuple (jump, no_oprd (), no_oprd (), new_Lab (left_adm -> lab));
	  return (t_void);
	};

/*
   T (kind, [ a1,a2,...,an ], >oprd>):
*/
static void generate_dpy_fill_code (node elt, int ix, type dpy_type,
				     int dnr, int anr, refadm_list adm, int_list act_guards)
	{ int offset = 0;
	  type ftype = type_nil;
	  oprd elt_oprd;

	  /* code to calculate field */
	  translate_node (k_oprd, elt, &elt_oprd, adm, act_guards);

	  /* determine field type and offset */
	  switch (dpy_type -> tag)
	     { case TAGStruct:
		  { field_list flds = dpy_type -> Struct.flds;
		    field fld = flds -> array[ix];
		    ftype = fld -> ftype;
		    offset = fld -> offset;
		  }; break;
	       case TAGRow:
		  { ftype = dpy_type -> Row.elt;
		    offset = ix * size_from_type (ftype);
		  }; break;
	       default: dcg_bad_tag (dpy_type -> tag, "generate_dpy_fill_code");
	     };

	  /* code to add offset to malloced structure and fill field */
	  gen_tuple (addr_plus_offset, new_Var (dnr), new_Iconst (offset), new_Var (anr));
	  generate_copy_indirect (ftype, elt_oprd, new_Var (anr));
	};

static type translate_display (xkind k, node dpy, oprd *dst, refadm_list adm, int_list act_guards)
	{ refadm top = adm -> array[adm -> size - 1];
          int_list temps = top -> active_temps;
	  node_list elts = dpy -> Display.elts;
	  type dpy_type = dpy -> Display.typ;
	  int ix;
	  switch (k)
	     { case k_void:
		  { for (ix = 0; ix < elts -> size; ix++)
		       (void) translate_node (k_void, elts -> array[ix], dst, adm, act_guards);
		    return (t_void);
		  };
	       case k_value:
	       case k_oprd:
		  { int isize = indirect_size_from_type (dpy_type);
		    int dnr = new_temporary (dpy_type);
		    int anr = new_temporary (t_addr);
		    int ix;
		    app_int_list (temps, dnr);
		    gen_tuple (allocate, new_Iconst (isize), no_oprd (), new_Var (dnr));
		    for (ix = 0; ix < elts -> size; ix++)
		       generate_dpy_fill_code (elts -> array[ix], ix, dpy_type,
					       dnr, anr, adm, act_guards);
		    if (k == k_oprd) *dst = new_Var (dnr);
		    else generate_copy (dpy_type, new_Var (dnr), *dst);
		    release_variable (anr);
		  }; break;
	       case k_addr: dcg_internal_error ("translate_display");
	       default: dcg_bad_tag (k, "translate_display");
	     };
	  return (dpy_type);
	};

/*
   T (kind, a[idx], >oprd>)
*/
static type translate_subscription (xkind k, node sub, oprd *dst,
				     refadm_list adm, int_list act_guards)
	{ node arr = sub -> Sub.arr;
	  node idx = sub -> Sub.index;
	  type rtype = sub -> Sub.rtype;
	  int upb = rtype -> Row.sz;
	  type etype = rtype -> Row.elt;
	  int esize = size_from_type (etype);
	  oprd arr_oprd, idx_oprd;
	  xkind newk = defer_oprd_translation (k, etype, dst, adm);
	  switch (newk)
	     { case k_void:
		  { (void) translate_node (k_void, arr, &arr_oprd, adm, act_guards);
		    (void) translate_node (k_void, idx, &idx_oprd, adm, act_guards);
		    return (t_void);
		  };
	       case k_value:
		  { int idx_nr = new_temporary (t_int);
		    int adr_nr = new_temporary (t_addr);
		    (void) translate_node (k_oprd, arr, &arr_oprd, adm, act_guards);
		    (void) translate_node (k_oprd, idx, &idx_oprd, adm, act_guards);
		    /* nog inbouwen: check op ongedefinieerde waarden */
		    gen_tuple (row_index, idx_oprd, new_Iconst (upb), new_Var (idx_nr));
		    gen_tuple (int_times_int, new_Var (idx_nr),
			       new_Iconst (esize), new_Var (idx_nr));
		    gen_tuple (addr_plus_offset, arr_oprd, new_Var (idx_nr), new_Var (adr_nr));
		    generate_indirect_copy (etype, new_Var (adr_nr), *dst);
		  }; break;
	       case k_addr:
		  { int idx_nr = new_temporary (t_int);
		    int adr_nr = new_temporary (t_addr);
		    arr_oprd = new_Var (adr_nr);
		    (void) translate_node (k_addr, arr, &arr_oprd, adm, act_guards);
		    try_generate_var_guard (rtype, arr_oprd, act_guards);
		    (void) translate_node (k_oprd, idx, &idx_oprd, adm, act_guards);
		    gen_tuple (row_index, idx_oprd, new_Iconst (upb), new_Var (idx_nr));
		    gen_tuple (int_times_int, new_Var (idx_nr),
			       new_Iconst (esize), new_Var (idx_nr));
		    gen_tuple (copy_indirect_adr, attach_oprd (arr_oprd),
			       no_oprd (), new_Var (adr_nr));
		    gen_tuple (addr_plus_offset, new_Var (adr_nr), new_Var (idx_nr), *dst);
		  }; break;
	       default: dcg_bad_tag (newk, "translate_subscription");
	     };
	  return (etype);
	};

/*
   T (kind, a.sel, >oprd>)
*/
static type translate_selection (xkind k, node sel, oprd *dst,
				  refadm_list adm, int_list act_guards)
	{ node str = sel -> Select.str;
	  field fld = sel -> Select.fld;
	  type stype = sel -> Select.stype;
	  oprd str_oprd;
	  xkind newk = defer_oprd_translation (k, fld -> ftype, dst, adm);
	  switch (newk)
	     { case k_void:
		  (void) translate_node (k_void, str, &str_oprd, adm, act_guards);
		  return (t_void);
	       case k_value:
		  { int adr_nr = new_temporary (t_addr);
		    (void) translate_node (k_oprd, str, &str_oprd, adm, act_guards);
		    /* nog inbouwen: check op ongedefinieerde waarden */
		    gen_tuple (addr_plus_offset, str_oprd,
			       new_Iconst (fld -> offset), new_Var (adr_nr));
		    generate_indirect_copy (fld -> ftype, new_Var (adr_nr), *dst);
		  }; break;
	       case k_addr:
		  { int adr_nr = new_temporary (t_addr);
		    str_oprd = new_Var (adr_nr);
		    (void) translate_node (k_addr, str, &str_oprd, adm, act_guards);
		    try_generate_var_guard (stype, str_oprd, act_guards);
		    gen_tuple (copy_indirect_adr, attach_oprd (str_oprd),
			       no_oprd (), new_Var (adr_nr));
		    gen_tuple (addr_plus_offset, new_Var (adr_nr), 
			       new_Iconst (fld -> offset), *dst);
		  }; break;
	       default: dcg_bad_tag (newk, "translate_subscription");
	     };
	  return (fld -> ftype);
	};

/*
   T (>kind, T:a, >oprd>):
      T (>kind, >a, >oprd>).
*/
static type translate_abstractor (xkind k, node abs, oprd *dst,
				   refadm_list adm, int_list act_guards)
	{ (void) translate_node (k, abs -> Abstr.arg, dst, adm, act_guards);
	  return (abs -> Abstr.id -> typ);
	};

/*
   T (kind, CONCR a, >oprd>):
      T (>kind, >a, >oprd>).
*/
static type translate_concretizer (xkind k, node conc, oprd *dst,
				   refadm_list adm, int_list act_guards)
	{ (void) translate_node (k, conc -> Concr.arg, dst, adm, act_guards);
	  return (conc -> Concr.id -> Type.concr);
	};

/*
   translate parameter of an INTERNAL/EXTERNAL routine
   CONST parameters of user routines generate a copy
   CONST parameters get translated as oprd,
   VAR parameters as addr leaving the address in a temporary
   PROC parameters as addr leaving the procedure address in a temporary
*/
static oprd translate_actual_parameter (type fp_typ, e_access fp_acc, node arg,
					int is_user_routine, refadm_list adm, int_list act_guards)
	{ refadm top = adm -> array[adm -> size - 1];
	  int_list temps = top -> active_temps;
	  oprd parm;
	  switch (fp_acc)
	     { case acc_const:
		  if (is_user_routine && type_needs_gc (fp_typ))
		     { int vnr = new_temporary (fp_typ);
		       app_int_list (temps, vnr);
		       parm = new_Var (vnr);
		       translate_node (k_value, arg, &parm, adm, act_guards);
		     }
		  else (void) translate_node (k_oprd, arg, &parm, adm, act_guards);
	          break;
	       case acc_var:
		  { int vnr = new_temporary (t_addr);
		    app_int_list (temps, vnr);
		    parm = new_Var (vnr);
		    translate_node (k_addr, arg, &parm, adm, act_guards);
		    if (is_user_routine)
		       try_generate_var_guard (fp_typ, parm, act_guards);
		  }; break;
	       case acc_proc:
		  { int vnr = new_temporary (t_addr);
		    app_int_list (temps, vnr);
		    parm = new_Var (vnr);
		    translate_node (k_addr, arg, &parm, adm, act_guards);
		  }; break;
	       default: dcg_bad_tag (fp_acc, "translate_actual_parameter");
	     };
	  return (parm);
	};

static oprd_list translate_actual_parameters (pdecl_list pdecls, node_list args, int is_user_rout,
					      refadm_list adm, int_list act_guards)		  
	{ oprd_list parms = new_oprd_list ();
	  int ix;
	  for (ix = 0; ix < pdecls -> size; ix++)
	     { pdecl pd = pdecls -> array[ix];
	       oprd parm = translate_actual_parameter (pd -> ptype, pd -> pacc, args -> array[ix],
						       is_user_rout, adm, act_guards);
	       app_oprd_list (parms, parm);
	     };
	  return (parms);
	};

/* 
   Determine what the result operand must be:
      IF rtype = VOID
      THEN result_oprd := --;
      ELSIF kind = void
      THEN result_oprd := temporary (rtype)
      ELSIF kind = oprd
      THEN oprd := result_oprd := temporary (rtype)
      ELSIF kind = addr
      THEN abort
      ELSIF type_needs_gc (rtype)
      THEN result_oprd := temporary (rtype)
      ELSE oprd := result_oprd
      FI;
*/
static oprd create_result_operand (xkind k, type rtype, oprd *dst, refadm_list adm)
	{ refadm top = adm -> array[adm -> size - 1];
	  int_list temps = top -> active_temps;
	  oprd tmp = no_oprd ();
	  int vnr;
	  if (rtype == t_void)
	     { if (k != k_void) dcg_internal_error ("create_result_oprd");
	       return (no_oprd ());
	     };
	  switch (k)
	     { case k_void:
		  { vnr = new_temporary (rtype);
		    app_int_list (temps, vnr);
		    tmp = new_Var (vnr);
		  }; break;
	       case k_oprd:
		  { vnr = new_temporary (rtype);
		    app_int_list (temps, vnr);
		    tmp = new_Var (vnr);
		    *dst = tmp;
		  }; break;
	       case k_addr:
		  dcg_internal_error ("create_result_oprd");
	       case k_value:
		  if (type_needs_gc (rtype))
		     { vnr = new_temporary (rtype);
		       app_int_list (temps, vnr);
		       tmp = new_Var (vnr);
		     }
		  else tmp = *dst;
		  break;
	       default: dcg_bad_tag (k, "create_result_oprd");
	     };
	  return (tmp);
	};

static void may_be_copy_result (xkind k, type rtype, oprd tmp, oprd *dst)
	{ if ((k == k_value) && (type_needs_gc (rtype)))
	     generate_copy (rtype, tmp, *dst);
	};

/*
   code push arguments
*/
static int code_push_args (pdecl_list pdecls, oprd_list parms)
	{ oprd typs = new_Types (attach_pdecl_list (pdecls));
	  int total_size = 0;
	  int arg_nr, ix;

	  /* total size still needed when the typing is known? */
	  for (ix = 0; ix < pdecls -> size; ix++)
	     total_size += regsize_from_arg (pdecls -> array[ix]);
	  gen_tuple (prepare_call, new_Iconst (total_size), typs, no_oprd ());
	  arg_nr = total_size;
	  for (ix = pdecls -> size - 1; 0 <= ix; ix--)
	     { arg_nr -= regsize_from_arg (pdecls -> array[ix]);
	       gen_tuple (load_parameter, parms -> array[ix], new_Iconst (arg_nr), no_oprd ());
	     };
	  return (total_size);
	};

static void generate_actual_call (xkind k, type rtype, pdecl_list pdecls, oprd called,
				  oprd_list parms, oprd resop, oprd *dst)
	{ int tsize = code_push_args (pdecls, parms);
	  gen_tuple (call_procedure, called, new_Iconst (tsize), resop);
	  may_be_copy_result (k, rtype, resop, dst);
	};

/*
   T (kind, dynp (args), >oprd>):
      rtype = typeof (dynp);		from node
      create_result_oprd (kind, rtype, oprd);
      Tparms (formals (p), args, parms>)
      [ pcall, #args, --, -- ]
      [ lpar, parm, #0, -- ]
	 :
      [ lpar, parm, #args-1, -- ]
      [ call, rts-proc, --, result_oprd ]
      may_be_copy_result (type, result_oprd, oprd)
*/
static type translate_dyn_call (xkind k, node dyn_call, oprd *dst,
				refadm_list adm, int_list act_guards)
	{ node pnode = dyn_call -> Dyn_call.proc;
	  node_list args = dyn_call -> Dyn_call.args;
	  refadm top = adm -> array[adm -> size - 1];
	  int_list temps = top -> active_temps;
	  int pvnr = new_temporary (t_addr);
	  pdecl_list pdecls;
	  type ptype, rtype;
	  oprd pobj, resop;
	  oprd_list parms;

	  app_int_list (temps, pvnr);
	  pobj = new_Var (pvnr);
	  ptype = translate_node (k_addr, pnode, &pobj, adm, act_guards);
	  if (!is_a_procedure_type (ptype, &pdecls, &rtype))
	     dcg_internal_error ("translate_dyn_call");
	  resop = create_result_operand (k, rtype, dst, adm);
	  parms = translate_actual_parameters (pdecls, args, 1, adm, act_guards);
	  generate_actual_call (k, rtype, pdecls, pobj, parms, resop, dst);
	  return (rtype);
	};

/*
   T (kind, p (args), >oprd>):
      rtype = typeof (p);
      create_result_oprd (kind, rtype, oprd);
      Tparms (formals (p), args, parms>)
      IF p is an INTERNAL proc
      THEN
         [ internal code, parms0, parms1, result_oprd ]
	 may_be_copy_result (type, result_oprd, oprd)
      ELSE
	 [ pcall, #args, --, -- ]
         [ lpar, parm, #0, -- ]
	 :
	 [ lpar, parm, #args-1, -- ]
         [ call, rts-proc, --, result_oprd ]
	 may_be_copy_result (type, result_oprd, oprd)
      FI
*/
static type translate_call (xkind k, node call, oprd *dst, refadm_list adm, int_list act_guards)
	{ decl id = call -> Call.id;
	  node_list args = call -> Call.args;
	  type rtype = id -> typ;
	  decl_list fpars = id -> Proc_decl.fpars;
	  body bdy = id -> Proc_decl.bdy;
	  oprd resop = create_result_operand (k, rtype, dst, adm);
	  int is_user_proc = (bdy -> tag == TAGRoutine);
	  pdecl_list pdecls = construct_pdecls (fpars);
	  oprd_list parms = translate_actual_parameters (pdecls, args, is_user_proc,
							 adm, act_guards);
	  switch (bdy -> tag)
	     { case TAGInternal:
		  { opcode opc = convert_text_opcode (bdy -> Internal.tname);
		    oprd parm2 = no_oprd ();
		    oprd parm1 = no_oprd ();
		    switch (args -> size)
		       { case 2: parm2 = parms -> array[1];
			 case 1: parm1 = parms -> array[0];
			 case 0: break;
			 default: dcg_internal_error ("translate_call");
		       };
		    gen_tuple (opc, parm1, parm2, resop);
		    may_be_copy_result (k, rtype, resop, dst);
		  }; break;
	       case TAGExternal:
		  { oprd called = new_Rts (bdy -> External.ename);
		    generate_actual_call (k, rtype, pdecls, called, parms, resop, dst);
		  }; break;
	       case TAGRoutine:
		  { int pnr = bdy -> Routine.pnr;
		    oprd called;
		    if (!pnr) 
		       { pnr = allocate_user_routine (id -> name, 0, rtype, fpars, bdy);
			 bdy -> Routine.pnr = pnr;
		       };
		    increment_call_count (pnr);
		    called = new_Proc (pnr);
		    generate_actual_call (k, rtype, pdecls, called, parms, resop, dst);
		  }; break;
	       default: dcg_bad_tag (bdy -> tag, "translate_call");
	     };
	  return (rtype);
	};

/*
   T (kind, a DOP b, >oprd>)
      rtype = typeof (DOP);
      create_result_operand (kind, rtype, oprd);
      Tparm (formal1 (DOP), a, parm1>)
      Tparm (formal2 (DOP), b, parm2>)
      IF DOP is an INTERNAL op
      THEN
         [ internal code, parm1, parm2, result_oprd ]
	 may_be_copy_result (type, result_oprd, oprd)
      ELSE
	 [ pcall, ##2, --, -- ]
         [ lpar, parm2, --, ##1 ]
         [ lpar, parm1, --, ##0 ]
         [ call, rts-op, --, result_oprd ]
	 may_be_copy_result (type, result_oprd, oprd)
      FI
*/
static type translate_dyop (xkind k, node dyop, oprd *dst, refadm_list adm, int_list act_guards)
	{ decl id = dyop -> Dyop.id;
	  node arg1 = dyop -> Dyop.arg1;
	  node arg2 = dyop -> Dyop.arg2;
	  type rtype = id -> typ;
	  decl_list fpars = id -> Op_decl.fpars;
	  decl fpar1 = fpars -> array[0];
	  decl fpar2 = fpars -> array[1];
	  body bdy = id -> Op_decl.bdy;
	  pdecl_list pdecls = construct_pdecls (fpars);
	  oprd resop = create_result_operand (k, rtype, dst, adm);
	  oprd_list parms = new_oprd_list ();
	  int is_user_op = (bdy -> tag == TAGRoutine);
	  oprd parm1 = translate_actual_parameter (fpar1 -> typ, fpar1 -> Formal.acc, arg1,
						   is_user_op, adm, act_guards);
	  oprd parm2 = translate_actual_parameter (fpar2 -> typ, fpar2 -> Formal.acc, arg2,
						   is_user_op, adm, act_guards);
	  app_oprd_list (parms, parm1);
	  app_oprd_list (parms, parm2);
	  switch (bdy -> tag)
	     { case TAGInternal:
		  { opcode opc = convert_text_opcode (bdy -> Internal.tname);
		    gen_tuple (opc, parm1, parm2, resop);
		    may_be_copy_result (k, rtype, resop, dst);
		  }; break;
	       case TAGExternal:
		  { oprd called = new_Rts (bdy -> External.ename);
		    generate_actual_call (k, rtype, pdecls, called, parms, resop, dst);
		  }; break;
	       case TAGRoutine:
		  { int pnr = bdy -> Routine.pnr;
		    oprd called;
		    if (!pnr) 
		       { pnr = allocate_user_routine (id -> name, 1, rtype, fpars, bdy);
			 bdy -> Routine.pnr = pnr;
		       };
		    increment_call_count (pnr);
		    called = new_Proc (pnr);
		    generate_actual_call (k, rtype, pdecls, called, parms, resop, dst);
		  }; break;
	       default: dcg_bad_tag (bdy -> tag, "translate_dyop");
	     };
	  /* detach_oprd_list (&parms); */
	  /* detach_oprd (&resop); */
	  return (rtype);
	};

/*
   T (kind, a MOP b, >oprd>)
      rtype = typeof (MOP);
      create_result_operand (kind, rtype, oprd);
      Tparm (formal (MOP), a, parm1>)
      IF MOP is an INTERNAL op
      THEN
         [ internal code, parm1, --, result_oprd ]
	 may_be_copy_result (type, result_oprd, oprd)
      ELSE
	 [ pcall, ##1, --, -- ]
         [ lpar, parm, --, ##1 ]
         [ call, rts-op, --, result_oprd ]
	 may_be_copy_result (type, result_oprd, oprd)
      FI
*/
static type translate_monop (xkind k, node monop, oprd *dst, refadm_list adm, int_list act_guards)
	{ decl id = monop -> Monop.id;
	  node arg = monop -> Monop.arg;
	  type rtype = id -> typ;
	  decl_list fpars = id -> Op_decl.fpars;
	  decl fpar = fpars -> array[0];
	  body bdy = id -> Op_decl.bdy;
	  oprd resop = create_result_operand (k, rtype, dst, adm);
	  pdecl_list pdecls = construct_pdecls (fpars);
	  oprd_list parms = new_oprd_list ();
	  int is_user_op = (bdy -> tag == TAGRoutine);
	  oprd parm = translate_actual_parameter (fpar -> typ, fpar -> Formal.acc, arg,
						  is_user_op, adm, act_guards);
	  app_oprd_list (parms, parm);
	  switch (bdy -> tag)
	     { case TAGInternal:
		  { opcode opc = convert_text_opcode (bdy -> Internal.tname);
		    gen_tuple (opc, parm, no_oprd (), resop);
		    may_be_copy_result (k, rtype, resop, dst);
		  }; break;
	       case TAGExternal:
		  { oprd called = new_Rts (bdy -> External.ename);
		    generate_actual_call (k, rtype, pdecls, called, parms, resop, dst);
		  }; break;
	       case TAGRoutine:
		  { int pnr = bdy -> Routine.pnr;
		    oprd called;
		    if (!pnr) 
		       { pnr = allocate_user_routine (id -> name, 1, rtype, fpars, bdy);
			 bdy -> Routine.pnr = pnr;
		       };
		    increment_call_count (pnr);
		    called = new_Proc (pnr);
		    generate_actual_call (k, rtype, pdecls, called, parms, resop, dst);
		  }; break;
	       default: dcg_bad_tag (bdy -> tag, "translate_monop");
	     };
	  return (rtype);
	};

/*
   T (kind, ref name, >oprd>):
      defer oprd xlat to value xlat (>kind>, oprd)
      T (kind, refinement body, oprd).
*/
static type translate_refinement_application (xkind k, node appl, oprd *dst,
					       refadm_list adm, int_list act_guards)
	{ decl ref = attach_decl (appl -> Ref_appl.id);
	  xkind defk = defer_oprd_translation (k, ref -> typ, dst, adm);
	  oprd result = (k == k_void)?no_oprd ():attach_oprd (*dst);
	  int_list new_temps = new_int_list ();
	  refadm new_top = new_refadm (ref, defk, result, new_temps);
	  new_top -> trc = ref -> Refinement.trc;
	  app_refadm_list (adm, new_top);
	  translate_paragraph (defk, ref -> Refinement.rcode, &result, adm, act_guards);
	  if (new_top -> lab) set_label (new_top -> lab);
	  del_refadm_list (adm, adm -> size - 1);
	  detach_refadm (&new_top);
	  return (ref -> typ);
	};

/*
   T (void, name, --):.
   T (oprd, name, name-oprd>):.
   T (value, name, >dst):
      generate_copy (typeof (name), name, dst)
   T (addr, name, >dst):
      [ :=&, name, --, dst ]
   Some special care is taken for predefined procedures
*/
static void generate_procedure_address (decl d, oprd dst)
	{ body bdy = d -> Proc_decl.bdy;
	  oprd obj = oprd_nil;
	  
	  switch (bdy -> tag)
	     { case TAGExternal: obj = new_Rts (bdy -> External.ename); break;
	       case TAGInternal: dcg_internal_error ("generate procedure address");
	       case TAGRoutine:
		  { int pnr = bdy -> Routine.pnr;
		    if (!pnr) 
		       { pnr = allocate_permanent_routine (d -> name, d -> typ,
							   d -> Proc_decl.fpars, bdy);
			 bdy -> Routine.pnr = pnr;
		       };
		    obj = new_Proc (pnr);
		  }; break;
	       default: dcg_bad_tag (bdy -> tag, "generate_procedure_address");
	     };
	  gen_tuple (copy_adr_of, obj, no_oprd (), dst);
	};

static type translate_identifier_application (xkind k, node appl, oprd *dst)
	{ decl d = appl -> Appl.id;
	  oprd obj;
	  if (d -> tag == TAGProc_decl)
	     { if (k != k_addr) dcg_internal_error ("translate_identifier_application");
	       generate_procedure_address (d, *dst);
	       return (type_from_decl (d));
	     };

	  obj = new_Var (pick_variable (d));
	  switch (k)
	     { case k_oprd: *dst = obj; 
	       case k_void: break;
	       case k_value:
		  generate_copy (d -> typ, obj, *dst);
		  break;
	       case k_addr:
		  if (d -> typ -> tag == TAGProc_type)	/* Formal procedure parameter */
		     gen_tuple (copy_adr, obj, no_oprd (), *dst);
		  else gen_tuple (copy_adr_of, obj, no_oprd (), *dst);
		  break;
	       default: dcg_bad_tag (k, "translate_identifier_application");
	     };
	  return (d -> typ);
	};

/*
   NIL may be voided or delivered by itself 
   T (>void, >NIL, >--):.
   T (>oprd, >NIL, Rts_nil>): 
   T (>value, >NIL, >dst): [ copy_adr, NIL, --, dst ]
   T (>addr, >NIL, >dst): abort
*/
static type translate_nil (xkind k, node nil, oprd *dst)
	{ oprd nilop = new_Rts_nil ();
	  type nilt = nil -> Nil.typ;
	  switch (k)
	     { case k_oprd: *dst = nilop;
	       case k_void: break;
	       case k_value:
		  generate_copy (nilt, nilop, *dst);
		  break;
	       case k_addr: dcg_internal_error ("translate_nil");
	       default: dcg_bad_tag (k, "translate_nil");
	     };
	  return (nilt);
	};

/*
   Denoters are voided, delivered by themselves or copied into.
   T (>void, >denoter, >--):.
   T (>oprd, >denoter, denoter_oprd>):.
   T (>value, >denoter, >dst):
     generate_copy (typeof (denoter), denoter_oprd, dst)
   T (>addr, >denoter, >dst): abort
*/
static oprd convert_value_oprd (value val)
	{ switch (val -> tag)
	     { case TAGIval: return (new_Iconst (val -> Ival.i));
	       case TAGBval: return (new_Iconst (val -> Bval.b));
	       case TAGRval: return (new_Rconst (val -> Rval.r));
	       case TAGTval: return (new_Tconst (val -> Tval.t));
	       default: dcg_bad_tag (val -> tag, "convert_value_oprd");
	     };
	  return (oprd_nil);
	};

static type translate_denoter (xkind k, node den, oprd *dst)
	{ value val = den -> Denoter.val;
	  type t = type_from_value (val);
	  oprd conop = convert_value_oprd (val);
	  switch (k)
	     { case k_oprd: *dst = conop;
	       case k_void: break;
	       case k_value:
		  generate_copy (t, conop, *dst);
	          break;
	       case k_addr: dcg_internal_error ("translate_denoter");
	       default: dcg_bad_tag (k, "translate_denoter");
	     };
	  return (t);
	};

/*
   T (>void, >SKIP, >--):
*/
static type translate_skip (xkind k, node skip)
	{ if (k != k_void) dcg_internal_error ("translate_skip");
	  return (t_void);
	};

static type translate_node (xkind k, node n, oprd *dst, refadm_list adm, int_list act_guards)
	{ switch (n -> tag)
	     { case TAGInit:	return (translate_initialization (k, n, adm, act_guards));
	       case TAGAssign:  return (translate_assignment (k, n, adm, act_guards));
	       case TAGCond:	return (translate_conditional (k, n, dst, adm, act_guards));
	       case TAGNum:	return (translate_numerical_choice (k, n, dst, adm, act_guards));
	       case TAGFor:     return (translate_repetition (k, n, adm, act_guards));
	       case TAGWhile:   return (translate_while (k, n, adm, act_guards));
	       case TAGLeave:	return (translate_leave (k, n, adm));
	       case TAGDyn_call:return (translate_dyn_call (k, n, dst, adm, act_guards));
	       case TAGCall:	return (translate_call (k, n, dst, adm, act_guards));
	       case TAGDisplay:	return (translate_display (k, n, dst, adm, act_guards));
	       case TAGSub:	return (translate_subscription (k, n, dst, adm, act_guards));
	       case TAGSelect:	return (translate_selection (k, n, dst, adm, act_guards));
	       case TAGAbstr:	return (translate_abstractor (k, n, dst, adm, act_guards));
	       case TAGConcr:	return (translate_concretizer (k, n, dst, adm, act_guards));
	       case TAGDyop:	return (translate_dyop (k, n, dst, adm, act_guards));
	       case TAGMonop:	return (translate_monop (k, n, dst, adm, act_guards));
	       case TAGRef_appl:
		  return (translate_refinement_application (k, n, dst, adm, act_guards));
	       case TAGAppl:	return (translate_identifier_application (k, n, dst));
	       case TAGNil:	return (translate_nil (k, n, dst));
	       case TAGDenoter: return (translate_denoter (k, n, dst));
	       case TAGSkip:    return (translate_skip (k, n));
	       default: dcg_bad_tag (n -> tag, "translate_node");
	     };
	  return (type_nil);
	};

static void translate_program (packet_list pks)
	{ refadm_list adm = new_refadm_list ();
	  int_list temps = new_int_list ();
	  int_list act_guards = new_int_list ();
	  oprd empty = no_oprd ();
	  proc mainp;
	  int ix;

	  (void) allocate_proc (new_string ("_main"), new_string ("VOID _main ()"),
				attach_type (t_void), new_decl_list (), body_nil);
	  app_refadm_list (adm, new_refadm (decl_nil, k_void, empty, temps));
	  mainp = im_procs -> array[0];
	  set_vnr_lw_mark ();
	  gen_start_tuple (mainp, start_program, 0);
	  for (ix = 0; ix < pks -> size; ix++)
	     { packet pk = pks -> array[ix];
	       create_variables (pk -> pkdecls, 1);
	       translate_paragraph (k_void, pk -> pcode, &empty, adm, act_guards);
	     };
	  gen_tuple (end_program, no_oprd (), no_oprd (), no_oprd ());
	  register_locals_with_proc (mainp);
	  make_basic_blocks (mainp);
	  /* free adm */
	};

static void translate_user_routine (int proc_idx)
	{ proc thisp = im_procs -> array[proc_idx];
	  refadm_list adm = new_refadm_list ();
	  int_list temps = new_int_list ();
	  int_list act_guards = new_int_list ();
	  refadm alg_bot;
	  create_uproc_args (thisp -> args);
	  set_vnr_lw_mark ();
	  gen_start_tuple (thisp, enter_procedure, proc_idx);
	  create_variables (thisp -> cbody -> Routine.rdecls, 0);
	  if (thisp -> rtyp == t_void)
	     { oprd empty = no_oprd ();
	       alg_bot = new_refadm (decl_nil, k_void, empty, temps);
	       alg_bot -> trc = thisp -> cbody -> Routine.trc;
	       app_refadm_list (adm, alg_bot);
	       translate_paragraph (k_void, thisp -> cbody -> Routine.rcode,
				    &empty, adm, act_guards);
	       if (alg_bot -> lab) set_label (alg_bot -> lab);
	       release_variables (thisp -> cbody -> Routine.rdecls);
	       gen_tuple (leave_procedure, new_Proc (proc_idx), no_oprd (), no_oprd ());
	     }
	  else
	     { int result_tmp = new_temporary (thisp -> rtyp);
	       oprd dst = new_Var (result_tmp);
	       alg_bot = new_refadm (decl_nil, k_value, dst, temps);
	       alg_bot -> trc = thisp -> cbody -> Routine.trc;
	       app_refadm_list (adm, alg_bot);
	       translate_paragraph (k_value, thisp -> cbody -> Routine.rcode,
				    &dst, adm, act_guards);
	       if (alg_bot -> lab) set_label (alg_bot -> lab);
	       release_variables (thisp -> cbody -> Routine.rdecls);
	       gen_tuple (leave_procedure, new_Proc (proc_idx),
			  new_Var (result_tmp), no_oprd ());
	     };
	  register_locals_with_proc (thisp);
	  make_basic_blocks (thisp);
	  /* free adm */
	};

/*
   Generation of type procedures
*/
static void generate_row_element_attaches (type t, int vnr)
	{ type elt = t -> Row.elt;
	  int upb = t -> Row.sz;
	  int esize = size_from_type (elt);
	  int lnr, lbl;
	  if (!type_needs_gc (elt)) return;
	  lnr = allocate_temporary (t_int);
	  gen_tuple (copy_int, new_Iconst (0), no_oprd (), new_Var (lnr));
	  lbl = new_label ();
	  set_label (lbl);
	  try_generate_indirect_attach (elt, new_Var (vnr), new_Var (lnr), 1);
	  gen_tuple (int_plus_int, new_Var (lnr), new_Iconst (esize), new_Var (lnr));
	  gen_tuple (jump_if_int_less_than_int, new_Var (lnr),
		     new_Iconst (upb * esize), new_Lab (lbl));
	};

static void generate_struct_element_attaches (type t, int vnr)
	{ field_list flds = t -> Struct.flds;
	  int ix;
	  for (ix = 0; ix < flds -> size; ix++)
	     { field fld = flds -> array[ix];
	       try_generate_indirect_attach (fld -> ftype, new_Var (vnr),
					     new_Iconst (fld -> offset), 1);
	     };
	};

static void translate_guard_routine (int proc_idx)
	{ proc thisp = im_procs -> array[proc_idx];
	  type t = thisp -> cbody -> Attach.t;
	  int arg_nr, anr, vnr, end_lab;
	  int isize = indirect_size_from_type (t);
	  create_uproc_args (thisp -> args);
	  set_vnr_lw_mark ();
	  gen_start_tuple (thisp, enter_procedure, proc_idx);
	  arg_nr = thisp -> args -> array[0] -> Formal.vnr;
	  anr = allocate_temporary (t);
	  vnr = allocate_temporary (t);
	  end_lab = new_label ();
	  gen_tuple (copy_adr_of, new_Var (arg_nr), no_oprd (), new_Var (anr));
	  gen_tuple (guard, new_Var (anr), new_Iconst (isize), new_Var (vnr));
	  gen_tuple (jump_if_null, new_Var (vnr), no_oprd (), new_Lab (end_lab));
	  switch (t -> tag)
	     { case TAGRow: generate_row_element_attaches (t, vnr); break;
	       case TAGStruct: generate_struct_element_attaches (t, vnr); break;
	       default: dcg_bad_tag (t -> tag, "translate_detach_routine");
	     };
	  set_label (end_lab);
	  gen_tuple (leave_procedure, new_Proc (proc_idx), new_Var (vnr), no_oprd ());
	  register_locals_with_proc (thisp);
	  make_basic_blocks (thisp);
	};

static void translate_attach_routine (int proc_idx)
	{ proc thisp = im_procs -> array[proc_idx];
	  type t = thisp -> cbody -> Attach.t;
	  int arg_nr, vnr, end_lab;
	  create_uproc_args (thisp -> args);
	  set_vnr_lw_mark ();
	  gen_start_tuple (thisp, enter_procedure, proc_idx);
	  arg_nr = thisp -> args -> array[0] -> Formal.vnr;
	  vnr = allocate_temporary (t);
	  end_lab = new_label ();
	  gen_tuple (attach_adr, new_Var (arg_nr), no_oprd (), new_Var (vnr));
	  gen_tuple (jump_if_addr_equal_addr, new_Var (arg_nr),
		     new_Var (vnr), new_Lab (end_lab));
	  switch (t -> tag)
	     { case TAGRow: generate_row_element_attaches (t, vnr); break;
	       case TAGStruct: generate_struct_element_attaches (t, vnr); break;
	       default: dcg_bad_tag (t -> tag, "translate_detach_routine");
	     };
	  set_label (end_lab);
	  gen_tuple (leave_procedure, new_Proc (proc_idx), new_Var (vnr), no_oprd ());
	  register_locals_with_proc (thisp);
	  make_basic_blocks (thisp);
	};

static void generate_row_element_detaches (type t, int vnr)
	{ type elt = t -> Row.elt;
	  int upb = t -> Row.sz;
	  int esize = size_from_type (elt);
	  int lnr, lbl;
	  if (!type_needs_gc (elt)) return;
	  lnr = allocate_temporary (t_int);
	  gen_tuple (copy_int, new_Iconst (0), no_oprd (), new_Var (lnr));
	  lbl = new_label ();
	  set_label (lbl);
	  try_generate_indirect_detach (elt, new_Var (vnr), new_Var (lnr));
	  gen_tuple (int_plus_int, new_Var (lnr), new_Iconst (esize), new_Var (lnr));
	  gen_tuple (jump_if_int_less_than_int, new_Var (lnr),
		     new_Iconst (upb * esize), new_Lab (lbl));
	};

static void generate_struct_element_detaches (type t, int vnr)
	{ field_list flds = t -> Struct.flds;
	  int ix;
	  for (ix = 0; ix < flds -> size; ix++)
	     { field fld = flds -> array[ix];
	       try_generate_indirect_detach (fld -> ftype, new_Var (vnr),
					     new_Iconst (fld -> offset));
	     };
	};

static void translate_detach_routine (int proc_idx)
	{ proc thisp = im_procs -> array[proc_idx];
	  type t = thisp -> cbody -> Attach.t;
	  int arg_nr, vnr, end_lab;
	  create_uproc_args (thisp -> args);
	  set_vnr_lw_mark ();
	  gen_start_tuple (thisp, enter_procedure, proc_idx);
	  arg_nr = thisp -> args -> array[0] -> Formal.vnr;
	  vnr = allocate_temporary (t);
	  end_lab = new_label ();
	  gen_tuple (predetach_adr, new_Var (arg_nr), no_oprd (), new_Var (vnr));
	  gen_tuple (jump_if_null, new_Var (vnr), no_oprd (), new_Lab (end_lab));
	  switch (t -> tag)
	     { case TAGRow: generate_row_element_detaches (t, vnr); break;
	       case TAGStruct: generate_struct_element_detaches (t, vnr); break;
	       default: dcg_bad_tag (t -> tag, "translate_detach_routine");
	     };
	  gen_tuple (detach_adr, new_Var (vnr), no_oprd (), no_oprd ());
	  set_label (end_lab);
	  gen_tuple (leave_procedure, new_Proc (proc_idx), no_oprd (), no_oprd ());
	  register_locals_with_proc (thisp);
	  make_basic_blocks (thisp);
	};

static void translate_procedures ()
	{ int proc_idx = 1;
	  while (proc_idx < im_procs -> size)
	     { body thisb = im_procs -> array[proc_idx] -> cbody;
	       switch (thisb -> tag)
		  { case TAGRoutine: translate_user_routine (proc_idx); break;
		    case TAGGuard:   translate_guard_routine (proc_idx); break;
		    case TAGAttach:  translate_attach_routine (proc_idx); break;
		    case TAGDetach:  translate_detach_routine (proc_idx); break;
		    default: dcg_bad_tag (thisb -> tag, "translate_procedures");
		  };
	       proc_idx++;
	     };
	};

void intermediate_code_generation (packet_list pks)
	{ dcg_warning (0, "   generating intermediate code...");
	  register_field_offsets ();
	  init_imc ();
	  translate_program (pks);
	  translate_procedures ();
	  try_report_imc ();
	  if (dump_imcgen) dump_imc_program ();
	  dcg_panic_if_errors ();
	};
