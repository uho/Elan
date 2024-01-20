/*
   File: type_table.c
   Handles types

   CVS ID: "$Id: type_table.c,v 1.11 2011/09/02 12:50:29 marcs Exp $"
*/

/* global includes */
#include <stdio.h>
#include <string.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>

/* local includes */
#include "elan_ast.h"
#include "ident.h"
#include "evaluate.h"
#include "contsens.h"
#include "tgt_gen.h"
#include "type_table.h"

/* Global type table */
type_info_list type_table;

string convert_type_text (type t)
	{ char buf[MAXSTRLEN];
	  int ix;

	  /* if already in type list, return the repr */
	  if (t -> tnr)
	     return (type_table -> array[t -> tnr] -> text);
	  else if (t -> tag == TAGUnknown)
	     return (type_table -> array[0] -> text);

	  /* now we must convert */
	  switch (t -> tag)
	    { case TAGRow:
		 { sprintf (buf, "ROW %d %s",
			    t -> Row.sz, convert_type_text (t -> Row.elt));
		   return (new_string (buf));
		 };
	      case TAGStruct:
		 { field_list flds = t -> Struct.flds;
		   strcpy (buf, "STRUCT (");
		   for (ix = 0; ix < flds -> size; ix++)
		      { field fld = flds -> array[ix];
			if (ix) strcat (buf, ", ");
			strcat (buf, convert_type_text (fld -> ftype));
			strcat (buf, " ");
			strcat (buf, fld -> fname);
		      };
		   strcat (buf, ")");
		   return (new_string (buf));
		 };
	      case TAGProc_type:
		 { pdecl_list parms = t -> Proc_type.parms;
		   type rtype = t -> Proc_type.rtype;
		   if (rtype -> tag != TAGVoid)
		      { strcpy (buf, convert_type_text (rtype));
		        strcat (buf, " PROC");
		      }
		   else strcpy (buf, "PROC");
		   if (parms -> size)
		      { strcat (buf, " (");
		        for (ix = 0; ix < parms -> size; ix++)
		           { pdecl pd = parms -> array[ix];
			     if (ix) strcat (buf, ", ");
			     strcat (buf, convert_type_text (pd -> ptype));
			     if (pd -> pacc == acc_var) strcat (buf, " VAR");
		           };
		        strcat (buf, ")");
		      };
		   return (new_string (buf));
		 };
	      case TAGDisplay_type:
		 { type_list flds = t -> Display_type.flds;
		   strcpy (buf, "[ ");
		   for (ix = 0; ix < flds -> size; ix++)
		      { if (ix) strcat (buf, ", ");
			strcat (buf, convert_type_text (flds -> array[ix]));
		      };
		   strcat (buf, "]");
		   return (new_string (buf));
		 };
	      case TAGUnresolved:
		 { type_list typs = t -> Unresolved.typs;
		   strcpy (buf, "<UNRESOLVED> (");
		   for (ix = 0; ix < typs -> size; ix++)
		      { if (ix) strcat (buf, ", ");
		        strcat (buf, convert_type_text (typs -> array[ix]));
		      };
		   strcat (buf, ")");
		   return (new_string (buf));
		 };
	      case TAGTname: return (attach_string (t -> Tname.tname));
	      default: dcg_bad_tag (t -> tag, "convert_type_text");
	    };
	  return (string_nil);
	};

static void add_type_table (char *repr, type t, int stabs_nr)
{ string srepr = new_string (repr);
  type_info new = new_type_info (srepr, t);
  t -> tnr = type_table -> size;
  app_type_info_list (type_table, new);
  new -> stabs_nr = stabs_nr;
}

/*
   Construct a procedure type from a return type and parsed formal arguments
*/
pdecl_list construct_pdecls (decl_list fpars)
{ pdecl_list pdecls = new_pdecl_list ();
  int ix;
  for (ix = 0; ix < fpars -> size; ix++)
    { decl d = fpars -> array[ix];
      if (d -> tag != TAGFormal)
	dcg_internal_error ("construct_pdecls");
      app_pdecl_list (pdecls, new_pdecl (attach_type (d -> typ), d -> Formal.acc));
    };
  return (pdecls);
}

type construct_procedure_type (type rtyp, decl_list fpars)
{ pdecl_list pdecls = construct_pdecls (fpars);
  return (new_Proc_type (pdecls, attach_type (rtyp)));
}

int equivalent_type (type t1, type t2)
	{ if (t1 == t2) return (1);			/* address equivalence */
	  if (t1 -> tag != t2 -> tag) return (0);	/* no equal tags */
	  switch (t1 -> tag)
	     { case TAGRow:
		  if (t1 -> Row.sz != t2 -> Row.sz) return (0);
		  return (equivalent_type (t1 -> Row.elt, t2 -> Row.elt));
	       case TAGStruct:
		  { field_list flds1 = t1 -> Struct.flds;
		    field_list flds2 = t2 -> Struct.flds;
		    int ix;
		    if (flds1 -> size != flds2 -> size) return (0);
		    for (ix = 0; ix < flds1 -> size; ix++)
		       { field fld1 = flds1 -> array[ix];
			 field fld2 = flds2 -> array[ix];
			 if (!streq (fld1 -> fname, fld2 -> fname))
			    return (0);
			 if (!equivalent_type (fld1 -> ftype, fld2 -> ftype))
			    return (0);
		       };
		    return (1);
		  };
	       case TAGProc_type:
		  { pdecl_list parms1 = t1 -> Proc_type.parms;
		    pdecl_list parms2 = t2 -> Proc_type.parms;
		    int ix;
		    if (!equivalent_type (t1 -> Proc_type.rtype, t2 -> Proc_type.rtype))
		       return (0);
		    if (parms1 -> size != parms2 -> size) return (0);
		    for (ix = 0; ix < parms1 -> size; ix++)
		       { pdecl pd1 = parms1 -> array[ix];
			 pdecl pd2 = parms2 -> array[ix];
			 if (!equivalent_type (pd1 -> ptype, pd2 -> ptype)) return (0);
		       };
		    return (1);
		  };
	       case TAGTname:
		  return (t1 -> Tname.id == t2 -> Tname.id);
	       default: dcg_bad_tag (t1 -> tag, "equivalent_type");
	     };
	  return (0);
	};

int is_a_procedure_type (type t, pdecl_list *parms, type *rtype)
	{ if (t -> tag != TAGProc_type) return (0);
	  *parms = t -> Proc_type.parms;
	  *rtype = t -> Proc_type.rtype;
	  return (1);
	};

int is_a_floating_type (type t)
	{ switch (t -> tag)
	     { case TAGTname:
		  { decl id = t -> Tname.id;
		    if (id -> tag != TAGType)
		       dcg_internal_error ("is_a_floating_type");
		    return (is_a_floating_type (id -> Type.concr));
		  };
	       case TAGDisplay_type:
	       case TAGProc_type:
	       case TAGVoid: 
	       case TAGInt:
	       case TAGBool:
	       case TAGAddr:
	       case TAGText:
	       case TAGFile:
	       case TAGNiltype:
	       case TAGRow:
	       case TAGStruct: return (0);
	       case TAGReal: return (1);
	       default: dcg_bad_tag (t -> tag, "is_a_floating_type");
	     };
	  return (0);
	};

int is_a_niltype (type t)
	{ switch (t -> tag)
	     { case TAGError:
	       case TAGInt:
	       case TAGBool:
	       case TAGReal:
	       case TAGVoid:
	       case TAGUnresolved:
	       case TAGUnknown: return (0);
	       case TAGFile: /* these ones may or may not */
	       case TAGText:
	       case TAGAddr:
	       case TAGProc_type:
	       case TAGNiltype: return (0);
	       case TAGDisplay_type:
	       case TAGRow:
	       case TAGStruct:
	       case TAGUnion: return (1);
	       case TAGTname:
		  { decl d = t -> Tname.id;
		    if (d -> tag != TAGType) dcg_internal_error ("is_a_niltype");
		    return (is_a_niltype (d -> Type.concr));
		  };
	       default: dcg_bad_tag (t -> tag, "is_a_niltype");
	     };
	  return (0);
	};

static int is_a_display_type (type at)
	{ return (at -> tag == TAGDisplay_type);
	};

int actual_matches_formal_type (type at, type ft)
	{ if (equivalent_type (at, ft)) return (1);
	  if (at -> tag == TAGUnknown) return (1);
	  return (feebly_coercable (at, ft));
	};

int actual_match_formal_parms (pdecl_list atl, pdecl_list ftl)
	{ int ix;
	  if (atl -> size != ftl -> size) return (0);
	  for (ix = 0; ix < atl -> size; ix++)
	     if (!actual_matches_formal_type (atl -> array[ix] -> ptype,
					      ftl -> array[ix] -> ptype))
		return (0);
	  return (1);
	};

static int display_matchable (type at, type ft)
	{ type_list dpy_types = at -> Display_type.flds;
	  int ix;
	  switch (ft -> tag)
	     { case TAGError:
	       case TAGInt:
	       case TAGBool:
	       case TAGReal:
	       case TAGAddr:
	       case TAGVoid:
	       case TAGFile:
	       case TAGText:
	       case TAGProc_type:
	       case TAGNiltype: return (0);
	       case TAGDisplay_type: dcg_internal_error ("display_matchable");
	       case TAGRow:
		  { type elt = ft -> Row.elt;
		    if (ft -> Row.sz != dpy_types -> size) return (0);
		    for (ix = 0; ix < dpy_types -> size; ix++)
		       if (!actual_matches_formal_type (dpy_types -> array[ix], elt))
			  return (0);
		    return (1);
		  };
	       case TAGStruct:
		  { field_list flds = ft -> Struct.flds;
		    if (flds -> size != dpy_types -> size) return (0);
		    for (ix = 0; ix < dpy_types -> size; ix++)
		       if (!actual_matches_formal_type (dpy_types -> array[ix],
							     flds -> array[ix] -> ftype))
			  return (0);
		    return (1);
		  };
	       case TAGTname:
		  { decl d = ft -> Tname.id;
		    if (d -> tag != TAGType) dcg_internal_error ("display_matchable");
		    return (0);
		  };
	       default: dcg_bad_tag (ft -> tag, "display_matchable");
	     };
	  return (0);
	};

static int is_an_unresolved_type (type at)
	{ return (at -> tag == TAGUnresolved);
	};

static int partially_matchable (type at, type ft)
	{ type_list atl = at -> Unresolved.typs;
	  int ix;
	  for (ix = 0; ix < atl -> size; ix++)
	     if (actual_matches_formal_type (atl -> array[ix], ft))
		return (1);
	  return (0);
	};

int feebly_coercable (type at, type ft)
	{ pdecl_list aparms, fparms;
	  type artype, frtype;

	  /* Match niltype */
	  if (is_a_niltype (at) && equivalent_type (ft, t_niltype)) return (1);

	  /* Match a display with a row/struct */
	  if (is_a_display_type (at) && display_matchable (at, ft)) return (1);

	  /* Match an unresolved type if one of its parts matches */
	  if (is_an_unresolved_type (at) && partially_matchable (at, ft)) return (1);

	  /* Match procedure types */
	  if (is_a_procedure_type (at, &aparms, &artype) &&
	      is_a_procedure_type (ft, &fparms, &frtype))
	     return (actual_match_formal_parms (aparms, fparms) &&
		     actual_matches_formal_type (artype, frtype));

	  /* Match for deproceduring */
	  if (is_a_procedure_type (at, &aparms, &artype))
	     return ((aparms -> size == 0) & actual_matches_formal_type (artype, ft));
	  return (0);
	};

static int my_stabs_nr;
static type try_add_type_table (type t, int stabs_needed)
	{ string srepr;
	  int ix;
	  for (ix = 0; ix < type_table -> size; ix++)
	     { type_info t2 = type_table -> array[ix];
	       if (equivalent_type (t, t2 -> typ))
		  { /* Note order of attach/detach */
		    type new = attach_type (t2 -> typ);
		    detach_type (&t);
		    return (new);
		  };
	     };

	  /* this type is a new one, create repr and enter */
	  srepr = convert_type_text (t);
	  my_stabs_nr += stabs_needed;
	  add_type_table (srepr, t, my_stabs_nr - 1);
	  dcg_hint ("      added type %s with stabs nr %d", srepr, my_stabs_nr - 1);
	  return (attach_type (t));
	};

/* enter_type vernietigt zijn argument niet! */
static type evaluate_type_declaration (decl td);
static type evaluate_type_synonym (decl td);
static type enter_type (type t);
static type enter_row_type (type t)
	{ int sz = evaluate_intval (t -> Proto_row.sz);
	  type elt = enter_type (t -> Proto_row.elt);
	  return (try_add_type_table (new_Row (sz, elt), 1));
	};

static type enter_struct_type (type t)
	{ field_list flds = t -> Struct.flds;
	  field_list new = init_field_list (flds -> size);
	  int ix;
	  for (ix = 0; ix < flds -> size; ix++)
	     { field fld = flds -> array[ix];
	       type nftype = enter_type (fld -> ftype);
	       string nfname = attach_string (fld -> fname);
	       app_field_list (new, new_field (nftype, nfname));
	     };
	  return (try_add_type_table (new_Struct (new), 2));
	};

static type enter_proc_type (type t)
	{ pdecl_list parms = t -> Proc_type.parms;
	  pdecl_list new = init_pdecl_list (parms -> size);
	  type nrtype;
	  int ix;
	  for (ix = 0; ix < parms -> size; ix++)
	     { pdecl pd = parms -> array[ix];
	       type nptype = enter_type (pd -> ptype);
	       app_pdecl_list (new, new_pdecl (nptype, pd -> pacc));
	     };
	  nrtype = enter_type (t -> Proc_type.rtype);
	  return (try_add_type_table (new_Proc_type (new, nrtype), 1));
	};

static type enter_type_name (type t)
	{ decl td = identify_type_name (t -> Proto_tname.tname,
			t -> Proto_tname.line, t -> Proto_tname.column);
	  if (td == decl_nil)
	     return (attach_type (t_error));
	  switch (td -> tag)
	     { case TAGSynonym_type: return (evaluate_type_synonym (td));
	       case TAGType: return (evaluate_type_declaration (td));
	       default: dcg_bad_tag (td -> tag, "enter_type_name");
	     };
	  return (type_nil);
	};

static type enter_type (type t)
	{ if (t -> tnr) return (attach_type (t)); /* already entered */
	  switch (t -> tag)
	     { /* For included types */
	       case TAGInt:	    return (attach_type (t_int));
	       case TAGBool:	    return (attach_type (t_bool));
	       case TAGReal:	    return (attach_type (t_real));
	       case TAGText:	    return (attach_type (t_text));
	       case TAGVoid:	    return (attach_type (t_void));
	       case TAGAddr:	    return (attach_type (t_addr));
	       case TAGFile:	    return (attach_type (t_file));
	       case TAGNiltype:	    return (attach_type (t_niltype));

	       /* For constructed types */
	       case TAGProto_row:   return (enter_row_type (t));
	       case TAGStruct:	    return (enter_struct_type (t));
	       case TAGProto_tname: return (enter_type_name (t));
	       case TAGProc_type:   return (enter_proc_type (t));
	       default: dcg_bad_tag (t -> tag, "enter_type");
	     };
	  return (type_nil);
	};

void init_type_table ()
	{ /* distinct nr of types in standard prelude and compiler */
	  type_table = init_type_info_list (16);
	  add_type_table ("<UNKNOWN>",  new_Unknown (), 4);
	  add_type_table ("<ERROR>",	new_Error (), 4);
	  add_type_table ("<NON VOID>", new_Any_non_void (), 4);
	  add_type_table ("<ROW>",	new_Any_row (), 7);
	  add_type_table ("<STRUCT>",   new_Any_struct (), 7);
	  add_type_table ("<OBJECT>",	new_Any_object (), 7);
	  add_type_table ("<ADDR>",	new_Addr (), 7);
	  add_type_table ("INT",	new_Int (), 9);
	  add_type_table ("BOOL",	new_Bool (), 10);
	  add_type_table ("REAL",	new_Real (), 11);
	  add_type_table ("TEXT",	new_Text (), 12);
	  add_type_table ("VOID",	new_Void (), 4);
	  add_type_table ("FILE",	new_File (), 13);
	  add_type_table ("NILTYPE",	new_Niltype (), 4);
	  my_stabs_nr = 14;
	};

static type evaluate_type_declaration (decl td)
	{ type lhs, rhs;

	  /* if already checked, done with it */
	  if (td -> Type.stat == type_checked)
	     return (attach_type (td -> typ));

	  /* if being checked, it is recursive */
	  if (td -> Type.stat == being_checked)
	     { td -> Type.rec = 1;
	       td -> Type.stat = type_checked;
	       return (attach_type (td -> typ));
	     };

	  /* create a self referencing type name */
	  rhs = td -> typ;
	  lhs = new_Tname (attach_string (td -> name));
	  lhs -> Tname.id = td;
	  td -> typ = try_add_type_table (lhs, 0);

	  /* mark declaration as being checked and check rhs */
	  td -> Type.stat = being_checked;
	  td -> Type.concr = enter_type (rhs);
	  detach_type (&rhs);
	  return (attach_type (td -> typ));
	};

static type evaluate_type_synonym (decl td)
	{ type old, new;

	  /* if already checked, done with it */
	  if (td -> Synonym_type.stat == type_checked)
	     return (attach_type (td -> typ));

	  /* if being checked, it is recursive */
	  if (td -> Synonym_type.stat == being_checked)
	     { contsens_error (td -> line, td -> column,
			       "LET declaration of %s is recursive", td -> name);
	       td -> Synonym_type.stat = type_checked;
	       td -> Synonym_type.rec = 1;
	       td -> typ = attach_type (t_error);
	       return (attach_type (t_error));
	     };

	  /* mark declaration as being checked, check rhs */
	  td -> Synonym_type.stat = being_checked;
	  old = td -> typ;
	  new = enter_type (old);

	  /* if still marked as being checked, no recursion, mark checked */
	  if (td -> Synonym_type.stat == being_checked)
	     { td -> Synonym_type.stat = type_checked;
	       td -> typ = new;
	       detach_type (&old);
	     };
	  return (attach_type (td -> typ));
	};

void evaluate_type_declarations (decl_list decls)
	{ int ix;
	  for (ix = 0; ix < decls -> size; ix++)
	     { decl td = decls -> array[ix];
	       switch (td -> tag)
		  { case TAGType:
		       { type t = evaluate_type_declaration (td);
			 detach_type (&t);
		       }; break;
		    case TAGSynonym_type:
		       { type t = evaluate_type_synonym (td);
			 detach_type (&t);
		       }; break;
		    case TAGObject_decl:
		    case TAGProc_decl:
		    case TAGOp_decl:
		    case TAGRefinement:
		    case TAGSynonym_value: break;
		    default: dcg_bad_tag (td -> tag, "evaluate_type_decls");
		  };
	     };
	};

static void replace_decl_type (decl d)
	{ type old = d -> typ;
	  d -> typ = enter_type (old);
	  detach_type (&old);
	};

static void unify_sdecl_types (decl d)
	{ switch (d -> tag)
	     { case TAGType:
	       case TAGSynonym_type:
	       case TAGRefinement:
	       case TAGSynonym_value: break;
	       case TAGProc_decl: unify_decl_types (d -> Proc_decl.fpars);
	       case TAGObject_decl:
	       case TAGFormal: replace_decl_type (d); break;
	       case TAGOp_decl:
	          { unify_decl_types (d -> Op_decl.fpars);
		    replace_decl_type (d);
		  }; break;
	       default: dcg_bad_tag (d -> tag, "unify_sdecl_types");
	     };
	};

void unify_decl_types (decl_list decls)
	{ int ix;
	  for (ix = 0; ix < decls -> size; ix++)
	     unify_sdecl_types (decls -> array[ix]);
	};

/*
   Calculate the offsets within a structure
   Note we generate byte offsets
*/
static void do_struct_layout (type t)
	{ field_list flds = t -> Struct.flds;
	  int offset = 0;
	  int ix;
	  for (ix = 0; ix < flds -> size; ix++)
	     { field fld = flds -> array[ix];
	       int size = size_from_type (fld -> ftype);
	       if ((size & 8) && (offset & 4)) offset += 4;
	       fld -> offset = offset;
	       offset += size;
	     };
	};

void register_field_offsets ()
	{ int ix;
	  for (ix = 0; ix < type_table -> size; ix++)
	     { type_info info = type_table -> array[ix];
	       type t = info -> typ;
	       if (t -> tag == TAGStruct) do_struct_layout (t);
	     };
	};
