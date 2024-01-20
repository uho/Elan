/*
   File: coder.c
   Codes the datastructure definition files

   Copyright (C) 2008 Marc Seutter

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   CVS ID: "$Id: coder.c,v 1.24 2012/08/21 20:05:32 marcs Exp $"
*/

/* include config.h if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* standard includes */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>

/* conditional inclusion to define MAXPATHLEN */
#ifndef WIN32
#include <sys/param.h>
#endif
#ifndef MAXPATHLEN
#define MAXPATHLEN 256
#endif

/* include sys/time.h with or without time.h */
#if TIME_WITH_SYS_TIME
#   include <sys/time.h>
#   include <time.h>
#else
#   if HAVE_SYS_TIME_H
#      include <sys/time.h>
#   else
#      include <time.h>
#   endif
#endif

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>

/* local includes */
#include "dcg_code.h"
#include "dcg_parser2.h"
#include "analyze.h"
#include "options.h"
#include "coder.h"

/*
   Coding of headers and includes
*/
static FILE *header;
static FILE *c_code;
static FILE *c_def;
static FILE *tags_header;
static void try_open_output_files (char *basename)
{ char fname[MAXPATHLEN + 1];
  sprintf (fname, "%s.h", basename);
  if ((header = fopen (fname, "w")) == NULL)
    dcg_panic ("could not open header file %s", fname);
  sprintf (fname, "%s.c", basename);
  if ((c_code = fopen (fname, "w")) == NULL)
    dcg_panic ("could not open code file %s", fname);
  sprintf (fname, "%s.def", basename);
  if ((c_def = fopen (fname, "w")) == NULL)
    dcg_panic ("could not open code file %s", fname);
  if (!code_extra_tags) return;
  sprintf (fname, "%s_tags.h", basename);
  if ((tags_header = fopen (fname, "w")) == NULL)
    dcg_panic ("could not open tags header file %s", fname);
}

static void code_file_headers (char *basename)
{ time_t thetime;
  char *atime;
  time (&thetime);
  atime = ctime (&thetime);
  fprintf (header, "/*\n");
  fprintf (header, "   File: %s.h\n", basename);
  fprintf (header, "   Generated on %s", atime);
  fprintf (header, "*/\n");
  fprintf (header, "#ifndef Inc%c%s\n", toupper (basename[0]), basename + 1);
  fprintf (header, "#define Inc%c%s\n\n", toupper (basename[0]), basename + 1);
  fprintf (c_code, "/*\n");
  fprintf (c_code, "   File: %s.c\n", basename);
  fprintf (c_code, "   Generated on %s", atime);
  fprintf (c_code, "*/\n\n");
  fprintf (c_def, "#\n");
  fprintf (c_def, "#  File: %s.def\n", basename);
  fprintf (c_def, "#  Generated on %s", atime);
  fprintf (c_def, "#\n");
  fprintf (c_def, "#ifndef IncDef%c%s\n", toupper (basename[0]), basename + 1);
  fprintf (c_def, "#define IncDef%c%s\n\n", toupper (basename[0]), basename + 1);
  if (!code_extra_tags) return;
  fprintf (tags_header, "/*\n");
  fprintf (tags_header, "   File: %s_tags.h\n", basename);
  fprintf (tags_header, "   Generated on %s", atime);
  fprintf (tags_header, "*/\n");
  fprintf (tags_header, "#ifndef Inc%c%s_tags\n", toupper (basename[0]), basename + 1);
  fprintf (tags_header, "#define Inc%c%s_tags\n\n", toupper (basename[0]), basename + 1);
}

static void code_std_includes (string bst)
{ fprintf (header, "/* standard includes */\n");
  fprintf (header, "#include <stdio.h>\n\n");
  fprintf (header, "/* support lib includes */\n");
  fprintf (header, "#include <%s.h>\n", bst);
  fprintf (header, "#include <%s_alloc.h>\n", bst);
  fprintf (header, "#include <%s_string.h>\n", bst);
  fprintf (header, "#include <%s_binfile.h>\n\n", bst);
  fprintf (c_code, "/* standard includes */\n");
  fprintf (c_code, "#include <stdio.h>\n\n");
  fprintf (c_code, "/* support lib includes */\n");
  fprintf (c_code, "#include <%s.h>\n", bst);
  fprintf (c_code, "#include <%s_error.h>\n", bst);
  fprintf (c_code, "#include <%s_alloc.h>\n", bst);
  fprintf (c_code, "#include <%s_string.h>\n", bst);
  fprintf (c_code, "#include <%s_dump.h>\n", bst);
  fprintf (c_code, "#include <%s_binfile.h>\n\n", bst);
}

static void code_import_includes ()
{ int ix;
  fprintf (header, "/* Include imported defs */\n");
  for (ix = 0; ix < all_stats -> size; ix++)
     { stat s = all_stats -> array[ix];
       if (s -> tag == TAGImport)
	  fprintf (header, "#include <%s.h>\n", s -> Import.imp);
     };
  fprintf (header, "\n");
}

static void code_local_includes (char *basename)
{ fprintf (c_code, "/* local includes */\n");
  fprintf (c_code, "#include \"%s.h\"\n\n", basename);
}

/*
   Specific coding support
*/
string make_type_name (string lhs, int lvls)
{ char buf[MAXSTRLEN];
  int ix;

  /* construct new if needed */
  if (!lvls) return (attach_string (lhs));
  strcpy (buf, lhs);
  for (ix = 0; ix < lvls; ix++)
    strcat (buf, "_list");
  return (new_string (buf));
}

static string make_def_name (string lhs, int lvls)
{ char buf[MAXSTRLEN];
  int ix;

  /* construct new if needed */
  if (!lvls) return (attach_string (lhs));
  buf[0] = '\0';
  for (ix = 0; ix < lvls; ix++) strcat (buf, "[");
  strcat (buf, lhs);
  for (ix = 0; ix < lvls; ix++) strcat (buf, "]");
  return (new_string (buf));
}

/*
   Generation of introductory type definitions (names for left hand sides)
*/
static void code_lhs_record_definitions ()
{ int ix;
  fprintf (header, "/* Introduce record type definitions */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      if (d -> tag == TAGRecord)
	fprintf (header, "typedef struct str_%s *%s;\n", lhs, lhs);
    };
  fprintf (header, "\n");
}

static void code_lhs_list_definitions ()
{ int ix;
  fprintf (header, "/* Introduce type_list definitions */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string list_nm = make_type_name (lhs, lix + 1);
	  fprintf (header, "typedef struct str_%s *%s;\n", list_nm, list_nm);
	  detach_string (&list_nm);
	};
     };
  fprintf (header, "\n");
}

/*
   Generation of nils of the appropriate type casting
*/
static void code_lhs_record_nils ()
{ int ix;
  fprintf (header, "/* Introduce record nils */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      if (d -> tag == TAGRecord)
	fprintf (header, "#define %s_nil ((%s) NULL)\n", lhs, lhs);
    };
  fprintf (header, "\n");
}

static void code_lhs_list_nils ()
{ int ix;
  fprintf (header, "/* Introduce type_list nils */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string list_nm = make_type_name (lhs, lix + 1);
	  fprintf (header, "#define %s_nil ((%s) NULL)\n", list_nm, list_nm);
	  detach_string (&list_nm);
	};
    };
  fprintf (header, "\n");
}

/*
   Generation of enumeration types and nils
*/
static void code_enumeration_type (def d)
{ string_list sl = d -> Enum.elems;
  string lhs = d -> lhs;
  int ix;
  fprintf (c_def, "> %s;\n", lhs);
  fprintf (header, "typedef enum\n{ ");
  for (ix = 0; ix < sl -> size; ix++)
    fprintf (header, "%s%s", (!ix)?"":(ix % 3 == 0)?",\n  ":", ", sl -> array[ix]);
  fprintf (header, "\n} %s;\n\n", lhs);
  if (!code_extra_tags) return;
  fprintf (tags_header, "typedef enum\n{ ");
  for (ix = 0; ix < sl -> size; ix++)
    fprintf (tags_header, "%s%s", (!ix)?"":(ix % 3 == 0)?",\n  ":", ", sl -> array[ix]);
  fprintf (tags_header, "\n} %s;\n\n", lhs);
}

static void code_enumeration_types ()
{ int ix;
  fprintf (header, "/* Introduce enumeration types */\n");
  if (code_extra_tags)
    fprintf (header, "/* Introduce enumeration types */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      if (d -> tag == TAGEnum) code_enumeration_type (d);
    };
}

static void code_enumeration_nils ()
{ int ix;
  fprintf (header, "/* Introduce enumeration nils */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      if (d -> tag == TAGEnum)
	fprintf (header, "#define %s_nil ((%s) 0)\n", lhs, lhs);
    };
  fprintf (header, "\n");
}

/*
   Generation of the right hand sides of record definitions
*/
static void code_record_constructors (vfield_list vfields)
{ int ix;
  for (ix = 0; ix < vfields -> size; ix++)
    { string cons = vfields -> array[ix] -> cons;
      fprintf (header, "#define %s u.ue_%s\n", cons, cons);
    };
}

static void code_rhs_constructors ()
{ int ix;
  fprintf (header, "/* introduce constructors */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      if (d -> tag == TAGRecord)
	code_record_constructors (d -> Record.variant);
    };
  fprintf (header, "\n");
}

static void code_constructor_tags (string type, vfield_list vfl)
{ int ix;
  if (!vfl -> size) return;
  fprintf (header, "typedef enum en_tags_%s\n", type);
  fprintf (header, "{ ");
  for (ix = 0; ix < vfl -> size; ix++)
    fprintf (header, "%sTAG%s", (ix != 0)?",\n  ":"", vfl -> array[ix] -> cons);
  fprintf (header, "\n} tags_%s;\n\n", type);
  if (!code_extra_tags) return;
  fprintf (tags_header, "typedef enum en_tags_%s\n", type);
  fprintf (tags_header, "{ ");
  for (ix = 0; ix < vfl -> size; ix++)
    fprintf (tags_header, "%sTAG%s", (ix != 0)?",\n  ":"", vfl -> array[ix] -> cons);
  fprintf (tags_header, "\n} tags_%s;\n\n", type);
}

static void code_rhs_tags ()
{ int ix;
  fprintf (header, "/* Introduce constructor tags */\n");
  if (code_extra_tags)
    fprintf (tags_header, "/* Introduce constructor tags */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      if (d -> tag == TAGRecord)
	code_constructor_tags (d -> lhs, d -> Record.variant);
    };
}

static void code_type (FILE *file, type t)
{ switch (t -> tag)
    { case TAGTname:
	fprintf (file, "%s", t -> Tname.tname);
	break;
      case TAGTlist:
	code_type (file, t -> Tlist.etyp);
	fprintf (file, "_list");
	break;
      default: dcg_bad_tag (t -> tag, "code_type");
    };
}

static void code_constructor_parts (vfield f)
{ string cons = f -> cons;
  field_list parts = f -> parts;
  int ix;
  if (!parts -> size) return;
  fprintf (header, "/* Introduce structure for constructor %s */\n", cons);
  fprintf (header, "typedef struct str_%s\n{ ", cons);
  for (ix = 0; ix < parts -> size; ix++)
    { field part = parts -> array[ix];
      fprintf (header, "%s", (ix != 0)?"\n  ":"");
      code_type (header, part -> ftype);
      fprintf (header, " %s;", part -> fname);
    };
  fprintf (header, "\n} C_%s;\n\n", cons);
}

static void code_record_parts (vfield_list vfl)
{ int ix;
  if (!vfl -> size) return;
  for (ix = 0; ix < vfl -> size; ix++)
    code_constructor_parts (vfl -> array[ix]);
}

static void code_rhs_parts ()
{ int ix;
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      if (d -> tag == TAGRecord)
	code_record_parts (d -> Record.variant);
    };
}

static void code_rhs_record (def d)
{ string lhs = d -> lhs;
  field_list fixed = d -> Record.fixed;
  vfield_list variant = d -> Record.variant;
  int ix;
  fprintf (c_def, "> %s;\n", lhs);
  fprintf (header, "/* Introduce structure of type %s */\n", lhs);
  fprintf (header, "struct str_%s\n{ ", lhs);
  for (ix = 0; ix < fixed -> size; ix++)
    { field f = fixed -> array[ix];
      fprintf (header, "%s", (ix != 0)?"\n  ":"");
      code_type (header, f -> ftype);
      fprintf (header, " %s;", f -> fname);
    };
  if (variant -> size != 0)
    { int hadfirst = 0;
      if (fixed -> size != 0) fprintf (header, "\n  ");
      fprintf (header, "tags_%s tag;\n", lhs);
      fprintf (header, "  union uni_%s\n    { ", lhs);
      for (ix = 0; ix < variant -> size; ix++)
	{ vfield p = variant -> array[ix];
	  string cons = p -> cons;
	  if (p -> parts -> size != 0)
	    { if (hadfirst) fprintf (header, "\n      ");
	      fprintf (header, "C_%s ue_%s;", cons, cons);
	      hadfirst = 1;
	    };
	};
      fprintf (header, "\n    } u;");
    };
  fprintf (header, "\n};\n\n");
}

static void code_rhs_records ()
{ int ix;
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      if (d -> tag == TAGRecord) code_rhs_record (d);
    };
}

/*
   Generation of list types
*/
static void code_rhs_list_definition (string elt_nm, string elt_def)
{ fprintf (header, "/* Introduce structure of type %s_list */\n", elt_nm);
  fprintf (header, "struct str_%s_list\n", elt_nm);
  fprintf (header, "{ int size;\n");
  fprintf (header, "  int room;\n");
  fprintf (header, "  %s *array;\n", elt_nm);
  fprintf (header, "};\n\n");
  fprintf (c_def, "> [%s];\n", elt_def);
}

static void code_rhs_list_definitions ()
{ int ix;
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
	  string elt_def = make_def_name (lhs, lix);
	  code_rhs_list_definition (elt_nm, elt_def);
	  detach_string (&elt_nm);
	  detach_string (&elt_def);
	};
    };
}

static void code_type_definitions ()
{ code_lhs_record_definitions ();
  code_lhs_list_definitions ();
  code_enumeration_types ();
  code_import_includes ();
  code_lhs_record_nils ();
  code_lhs_list_nils ();
  code_enumeration_nils ();
  code_rhs_constructors ();
  code_rhs_tags ();
  code_rhs_parts ();
  code_rhs_records ();
  code_rhs_list_definitions ();
}

/*
   Generation of list allocation routines
*/
static void code_init_list_definition (string elt_nm, string bst)
{ fprintf (c_code, "/* Allocate new %s_list */\n", elt_nm);
  fprintf (header, "%s_list init_%s_list (int room);\n", elt_nm, elt_nm);
  fprintf (c_code, "%s_list init_%s_list (int room)\n", elt_nm, elt_nm);
  fprintf (c_code, "{ %s_list new = (%s_list) ", elt_nm, elt_nm);
  fprintf (c_code, "%s_malloc (sizeof (struct str_%s_list));\n", bst, elt_nm);
  fprintf (c_code, "  int safe_room = (room < 2)?2:room;\n");
  fprintf (c_code, "  new -> size = 0;\n");
  fprintf (c_code, "  new -> room = safe_room;\n");
  fprintf (c_code, "  new -> array = (%s *) ", elt_nm);
  fprintf (c_code, "%s_calloc (safe_room, sizeof (%s));\n", bst, elt_nm);
  fprintf (c_code, "  return (new);\n");
  fprintf (c_code, "}\n\n");
}

static void code_init_list_definitions (string bst)
{ int ix;
  fprintf (header, "/* Introduce list allocation routines */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
          code_init_list_definition (elt_nm, bst);
	  detach_string (&elt_nm);
	};
     };
  fprintf (header, "\n");
}

static void code_room_list_definition (string elt_nm, string bst)
{ fprintf (c_code, "/* Announce to use 'room' chunks for %s_list */\n", elt_nm);
  fprintf (header, "void room_%s_list (%s_list l, int room);\n", elt_nm, elt_nm);
  fprintf (c_code, "void room_%s_list (%s_list l, int room)\n", elt_nm, elt_nm);
  fprintf (c_code, "{ if (room <= l -> room) return;\n");
  fprintf (c_code, "  %s_recalloc ((void **) &l -> array, ", bst);
  fprintf (c_code, "room, sizeof (%s));\n", elt_nm);
  fprintf (c_code, "  l -> room = room;\n");
  fprintf (c_code, "}\n\n");
}

static void code_room_list_definitions (string bst)
{ int ix;
  fprintf (header, "/* Introduce room allocation routines */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
	  code_room_list_definition (elt_nm, bst);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_new_list_definitions ()
{ int ix;
  fprintf (header, "/* Introduce list allocation shorthands */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string list_nm = make_type_name (lhs, lix + 1);
	  fprintf (header, "#define new_%s() init_%s(2)\n", list_nm, list_nm);
	};
    };
  fprintf (header, "\n");
}

/*
   Generation of record allocation routines
*/
static int nr_traversable_fields (field_list fl)
{ int ix;
  int nr = 0;
  for (ix = 0; ix < fl -> size; ix++)
     if (fl -> array[ix] -> ftrav) nr++;
  return (nr);
}

static void code_field_list (FILE *file, field_list fl)
{ int ix;
  int coded = 0;
  for (ix = 0; ix < fl -> size; ix++)
    { field f = fl -> array[ix];
      type typ = f -> ftype;
      string nm = f -> fname;
      if (!f -> ftrav) continue;		/* Ok */
      if (coded) fprintf (file, ", ");
      code_type (file, typ);
      fprintf (file, " %s", nm);
      coded = 1;
    };
}

static void code_fill_field_list (string cons, field_list fl)
{ int ix;
  for (ix = 0; ix < fl -> size; ix++)
    { field f = fl -> array[ix];
      string nm = f -> fname;
      if (!f -> ftrav)
	{ if (cons) fprintf (c_code, "  new -> %s.%s = ", cons, nm);
	  else fprintf (c_code, "  new -> %s = ", nm);
	  code_type (c_code, f -> ftype);
	  fprintf (c_code, "_nil;\n");
	}
      else if (cons)
	fprintf (c_code, "  new -> %s.%s = %s;\n", cons, nm, nm);
      else fprintf (c_code, "  new -> %s = %s;\n", nm, nm);
    };
}

static void code_new_variant_record_definition
		(string lhs, field_list fixed, vfield variant, string bst)
{ string cons = variant -> cons;
  field_list parts = variant -> parts;
  int nfixed = nr_traversable_fields (fixed);
  int nparts = nr_traversable_fields (parts);
  fprintf (header, "%s new_%s (", lhs, cons);
  fprintf (c_code, "/* Allocate new %s record for constructor %s */\n", lhs, cons);
  fprintf (c_code, "%s new_%s (", lhs, cons);
  if (nfixed)
    { code_field_list (header, fixed);
      code_field_list (c_code, fixed);
      if (nparts) fprintf (header, ",\n\t\t");
      if (nparts) fprintf (c_code, ",\n\t\t");
    };
  code_field_list (header, parts);
  code_field_list (c_code, parts);
  fprintf (header, ");\n");
  fprintf (c_code, ")\n");
  fprintf (c_code, "{ %s new = (%s) %s_malloc ", lhs, lhs, bst);
  fprintf (c_code, "(sizeof (struct str_%s));\n", lhs);
  code_fill_field_list (string_nil, fixed);
  fprintf (c_code, "  new -> tag = TAG%s;\n", cons);
  code_fill_field_list (cons, parts);
  fprintf (c_code, "  return (new);\n");
  fprintf (c_code, "}\n\n");
}

static void code_new_variant_record_definitions (def d, string bst)
{ string lhs = d -> lhs;
  field_list fixed = d -> Record.fixed;
  vfield_list variant = d -> Record.variant;
  int ix;
  for (ix = 0; ix < variant -> size; ix++)
     code_new_variant_record_definition (lhs, fixed, variant -> array[ix], bst);
}

static void code_new_fixed_record_definition (def d, string bst)
{ string lhs = d -> lhs;
  field_list fixed = d -> Record.fixed;
  fprintf (header, "%s new_%s (", lhs, lhs);
  fprintf (c_code, "/* Allocate new %s record */\n", lhs);
  fprintf (c_code, "%s new_%s (", lhs, lhs);
  code_field_list (header, fixed);
  code_field_list (c_code, fixed);
  fprintf (header, ");\n");
  fprintf (c_code, ")\n");
  fprintf (c_code, "{ %s new = (%s) %s_malloc ", lhs, lhs, bst);
  fprintf (c_code, "(sizeof (struct str_%s));\n", lhs);
  code_fill_field_list (string_nil, fixed);
  fprintf (c_code, "  return (new);\n");
  fprintf (c_code, "};\n\n");
}

static void code_new_record_definitions (string bst)
{ int ix;
  fprintf (header, "/* Introduce type allocation routines */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      if (d -> tag == TAGRecord)
	{ if (d -> Record.variant -> size)
	    code_new_variant_record_definitions (d, bst);
	  else code_new_fixed_record_definition (d, bst);
	};
    };
  fprintf (header, "\n");
}

/*
   Generation of attach macros and detach routines
*/
static void code_attach_type_definitions (string bst)
{ int ix;
  fprintf (header, "/* Introduce attaching of types */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      switch (d -> tag)
	{ case TAGRecord:
	    fprintf (header, "#define attach_%s(orig)", lhs);
	    fprintf (header, "(%s) %s_attach ((char *) orig)\n", lhs, bst);
	    fprintf (header, "#define att_%s(orig)", lhs);
	    fprintf (header, "(void) %s_attach ((char *) orig)\n", bst);
	    break;
	  case TAGEnum:
	    fprintf (header, "#define attach_%s(orig) (orig)\n", lhs);
	    fprintf (header, "#define att_%s(orig)\n", lhs);
	  case TAGPrimitive: break;
	    default: dcg_bad_tag (d -> tag, "code_attach_type_definitions");
	};
    };
  fprintf (header, "\n");
}

static void code_attach_type_list_definitions (string bst)
{ int ix;
  fprintf (header, "/* Introduce attaching of lists */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string list_nm = make_type_name (lhs, lix + 1);
	  fprintf (header, "#define attach_%s(orig) ", list_nm);
	  fprintf (header, "(%s) %s_attach ((char *) orig)\n", list_nm, bst);
	  fprintf (header, "#define att_%s(orig) ", list_nm);
	  fprintf (header, "(void) %s_attach ((char *) orig)\n", bst);
	  detach_string (&list_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_detach_field_list (string cons, field_list fl)
{ int ix;
  for (ix = 0; ix < fl -> size; ix++)
    { field f = fl -> array[ix];
      string nm = f -> fname;
      if (!f -> ftrav) continue;	/* Ok */
      fprintf (c_code, "%sdetach_", (cons)?"\t":"  ");
      code_type (c_code, f -> ftype);
      if (cons) fprintf (c_code, " (&(old -> %s.%s));\n", cons, nm);
      else fprintf (c_code, " (&(old -> %s));\n", nm);
    };
}

static void code_detach_record_definition (def d, string bst)
{ string lhs = d -> lhs;
  fprintf (header, "void detach_%s (%s *optr);\n", lhs, lhs);
  fprintf (header, "#define det_%s(optr) detach_%s(optr)\n", lhs, lhs);
  fprintf (c_code, "/* Recursively detach a %s */\n", lhs);
  fprintf (c_code, "void detach_%s (%s *optr)\n", lhs, lhs);
  fprintf (c_code, "{ %s old = (%s) %s_predetach ((void **) optr);\n", lhs, lhs, bst);
  fprintf (c_code, "  if (old == %s_nil) return;\n", lhs);
  code_detach_field_list (string_nil, d -> Record.fixed);
  if (d -> Record.variant -> size)
    { vfield_list vfl = d -> Record.variant;
      int ix;
      fprintf (c_code, "  switch (old -> tag)\n    { ");
      for (ix = 0; ix < vfl -> size; ix++)
	{ vfield f = vfl -> array[ix];
	  string cons = f -> cons;
	  fprintf (c_code, "%scase TAG%s:\n", (ix)?"      ":"", cons);
	  code_detach_field_list (cons, f -> parts);
	  fprintf (c_code, "\tbreak;\n");
	};
      fprintf (c_code, "      default:\n");
      fprintf (c_code, "\tdcg_bad_tag ((int) old -> tag, \"detach_%s\");\n", lhs);
      fprintf (c_code, "    };\n");
    };
  fprintf (c_code, "  %s_detach ((void **) &old);\n", bst);
  fprintf (c_code, "}\n\n");
}

static void code_detach_type_definitions (string bst)
{ int ix;
  fprintf (header, "/* Introduce detaching of enumeration types */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      if (d -> tag == TAGEnum)
	{ fprintf (header, "#define detach_%s(old)\n", d -> lhs);
	  fprintf (header, "#define det_%s(old)\n", d -> lhs);
	};
    };
  fprintf (header, "\n/* Introduce detaching of record types */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      if (d -> tag == TAGRecord) code_detach_record_definition (d, bst);
    };
  fprintf (header, "\n");
}

static void code_detach_type_list_definition (string elt_nm, string bst)
{ fprintf (c_code, "/* Recursively detach a %s_list */\n", elt_nm);
  fprintf (header, "void detach_%s_list (%s_list *lp);\n", elt_nm, elt_nm);
  fprintf (c_code, "void detach_%s_list (%s_list *lp)\n", elt_nm, elt_nm);
  fprintf (c_code, "{ int ix;\n");
  fprintf (c_code, "  %s_list old = (%s_list) %s_predetach ((void **) lp);\n", elt_nm, elt_nm, bst);
  fprintf (c_code, "  if (old == %s_list_nil) return;\n", elt_nm);
  fprintf (c_code, "  for (ix = 0; ix < old -> size; ix++)\n");
  fprintf (c_code, "    detach_%s (&(old -> array[ix]));\n", elt_nm);
  fprintf (c_code, "  %s_detach ((void **) &(old -> array));\n", bst);
  fprintf (c_code, "  %s_detach ((void **) &old);\n", bst);
  fprintf (c_code, "}\n\n");
}

static void code_detach_type_list_definitions (string bst)
{ int ix;
  fprintf (header, "/* Introduce detaching of lists */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
	  code_detach_type_list_definition (elt_nm, bst);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_nonrec_detach_type_list_definition (string elt_nm, string bst)
{ fprintf (c_code, "/* Detach a %s_list without detaching elements */\n", elt_nm);
  fprintf (header, "void nonrec_detach_%s_list (%s_list *lp);\n", elt_nm, elt_nm);
  fprintf (c_code, "void nonrec_detach_%s_list (%s_list *lp)\n", elt_nm, elt_nm);
  fprintf (c_code, "{ %s_list old = (%s_list) %s_predetach ((void **) lp);\n", elt_nm, elt_nm, bst);
  fprintf (c_code, "  if (old == %s_list_nil) return;\n", elt_nm);
  fprintf (c_code, "  %s_detach ((void **) &(old -> array));\n", bst);
  fprintf (c_code, "  %s_detach ((void **) &old);\n", bst);
  fprintf (c_code, "}\n\n");
}

static void code_nonrec_detach_type_list_definitions (string bst)
{ int ix;
  fprintf (header, "/* Introduce nonrecursive detaching of lists */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
	  code_nonrec_detach_type_list_definition (elt_nm, bst);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_basic_type_handling (string bst)
{ code_init_list_definitions (bst);
  code_room_list_definitions (bst);
  code_new_list_definitions ();
  code_new_record_definitions (bst);
  code_attach_type_definitions (bst);
  code_attach_type_list_definitions (bst);
  code_detach_type_definitions (bst);
  code_detach_type_list_definitions (bst);
  code_nonrec_detach_type_list_definitions (bst);
}

/*
   Generation of list modification routines
*/
static void code_append_list_definition (string elt_nm, string bst)
{ fprintf (c_code, "/* Append element to %s_list */\n", elt_nm);
  fprintf (header, "%s_list append_%s_list (%s_list l, %s el);\n", elt_nm, elt_nm, elt_nm, elt_nm);
  fprintf (c_code, "%s_list append_%s_list (%s_list l, %s el)\n", elt_nm, elt_nm, elt_nm, elt_nm);
  fprintf (c_code, "{ %s_cknonshared ((char *) l);\n", bst);
  fprintf (c_code, "  if (l -> size == l -> room)\n");
  fprintf (c_code, "    room_%s_list (l, l -> size << 1);\n", elt_nm);
  fprintf (c_code, "  l -> array[l -> size] = el;\n");
  fprintf (c_code, "  l -> size++;\n");
  fprintf (c_code, "  return (l);\n");
  fprintf (c_code, "}\n\n");
}

static void code_append_list_definitions (string bst)
{ int ix;
  fprintf (header, "/* Introduce list append routines */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
          code_append_list_definition (elt_nm, bst);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_app_list_definitions ()
{ int ix;
  fprintf (header, "/* Introduce void shorthands for list append routines */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
	  fprintf (header, "#define app_%s_list(l,el) ", elt_nm);
	  fprintf (header, "(void) append_%s_list (l,el)\n", elt_nm);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_concat_list_definition (string elt_nm, string bst)
{ fprintf (header, "%s_list concat_%s_list ", elt_nm, elt_nm);
  fprintf (header, "(%s_list l1, %s_list l2);\n", elt_nm, elt_nm);
  fprintf (c_code, "/* Concatenate two %s_lists */\n", elt_nm);
  fprintf (c_code, "%s_list concat_%s_list ", elt_nm, elt_nm);
  fprintf (c_code, "(%s_list l1, %s_list l2)\n", elt_nm, elt_nm);
  fprintf (c_code, "{ int ix;\n");
  fprintf (c_code, "  %s_cknonshared ((char *) l1);\n", bst);
  fprintf (c_code, "  room_%s_list (l1, l1 -> size + l2 -> size);\n", elt_nm);
  fprintf (c_code, "  for (ix = 0; ix < l2 -> size; ix++)\n");
  fprintf (c_code, "    l1 -> array[l1 -> size + ix] = l2 -> array[ix];\n");
  fprintf (c_code, "  l1 -> size += l2 -> size;\n");
  fprintf (c_code, "  return (l1);\n");
  fprintf (c_code, "}\n\n");
};

static void code_concat_list_definitions (string bst)
{ int ix;
  fprintf (header, "/* Introduce list concat routines */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
          code_concat_list_definition (elt_nm, bst);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_conc_list_definitions ()
{ int ix;
  fprintf (header, "/* Introduce void shorthands for list concat routines */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
	  fprintf (header, "#define conc_%s_list(l1,l2) ", elt_nm);
	  fprintf (header, "(void) concat_%s_list (l1,l2)\n", elt_nm);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_insert_list_definition (string elt_nm, string bst)
{ fprintf (header, "%s_list insert_%s_list ", elt_nm, elt_nm);
  fprintf (header, "(%s_list l, int pos, %s el);\n", elt_nm, elt_nm);
  fprintf (c_code, "/* Insert element into %s_list at pos 'pos' */\n", elt_nm);
  fprintf (c_code, "%s_list insert_%s_list ", elt_nm, elt_nm);
  fprintf (c_code, "(%s_list l, int pos, %s el)\n", elt_nm, elt_nm);
  fprintf (c_code, "{ int ix;\n");
  fprintf (c_code, "  %s_cknonshared ((char *) l);\n", bst);
  fprintf (c_code, "  if ((pos < 0) || (pos > l -> size))\n");
  fprintf (c_code, "    dcg_panic (\"insert_%s_list: position %%d out of range\", pos);\n", elt_nm);
  fprintf (c_code, "  if (l -> size == l -> room)\n");
  fprintf (c_code, "    room_%s_list (l, l -> size << 1);\n", elt_nm);
  fprintf (c_code, "  for (ix = l -> size; pos < ix; ix--)\n");
  fprintf (c_code, "    l -> array[ix] = l -> array[ix-1];\n");
  fprintf (c_code, "  l -> array[pos] = el;\n");
  fprintf (c_code, "  l -> size++;\n");
  fprintf (c_code, "  return (l);\n");
  fprintf (c_code, "}\n\n");
};

static void code_insert_list_definitions (string bst)
{ int ix;
  fprintf (header, "/* Introduce list insertion routines */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
          code_insert_list_definition (elt_nm, bst);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_ins_list_definitions ()
{ int ix;
  fprintf (header, "/* Introduce void shorthands for list insertion routines */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
	  fprintf (header, "#define ins_%s_list(l,pos,el) ", elt_nm);
	  fprintf (header, "(void) insert_%s_list (l,pos,el)\n", elt_nm);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_delete_list_definition (string elt_nm, string bst)
{ fprintf (c_code, "/* Delete element from %s_list at pos 'pos' */\n", elt_nm);
  fprintf (header, "%s_list delete_%s_list (%s_list l, int pos);\n", elt_nm, elt_nm, elt_nm);
  fprintf (c_code, "%s_list delete_%s_list (%s_list l, int pos)\n", elt_nm, elt_nm, elt_nm);
  fprintf (c_code, "{ int ix;\n");
  fprintf (c_code, "  %s_cknonshared ((char *) l);\n", bst);
  fprintf (c_code, "  if ((pos < 0) || (pos >= l -> size))\n");
  fprintf (c_code, "    dcg_panic (\"delete_%s_list: position %%d out of range\", pos);\n", elt_nm);
  fprintf (c_code, "  for (ix = pos; ix < l -> size - 1; ix++)\n");
  fprintf (c_code, "    l -> array[ix] = l -> array[ix+1];\n");
  fprintf (c_code, "  l -> size--;\n");
  fprintf (c_code, "  return (l);\n");
  fprintf (c_code, "}\n\n");
}

static void code_delete_list_definitions (string bst)
{ int ix;
  fprintf (header, "/* Introduce list deletion routines */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
          code_delete_list_definition (elt_nm, bst);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_del_list_definitions ()
{ int ix;
  fprintf (header, "/* Introduce void shorthands for list deletion routines */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
	  fprintf (header, "#define del_%s_list(l,pos) ", elt_nm);
	  fprintf (header, "(void) delete_%s_list (l,pos)\n", elt_nm);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_list_modification_routines (string bst)
{ code_append_list_definitions (bst);
  code_app_list_definitions ();
  code_concat_list_definitions (bst);
  code_conc_list_definitions ();
  code_insert_list_definitions (bst);
  code_ins_list_definitions ();
  code_delete_list_definitions (bst);
  code_del_list_definitions ();
}

/*
   Generation of equality tests
*/
static void code_cmp_field_list (string cons, field_list fl)
{ int ix;
  for (ix = 0; ix < fl -> size; ix++)
    { field f = fl -> array[ix];
      string nm = f -> fname;
      if (!f -> ftrav) continue;		/* Ok */
      fprintf (c_code, "%s", (cons)?"\t":"  ");
      fprintf (c_code, "if ((s = cmp_");
      code_type (c_code, f -> ftype);
      fprintf (c_code, " (a -> ");
      if (cons) fprintf (c_code, "%s.%s", cons, nm);
      else fprintf (c_code, "%s", nm);
      fprintf (c_code, ", b -> ");
      if (cons) fprintf (c_code, "%s.%s", cons, nm);
      else fprintf (c_code, "%s", nm);
      fprintf (c_code, "))) return (s);\n");
    };
}

static void code_cmp_record_definition (def d)
{ string lhs = d -> lhs;
  int local_needed = (nr_traversable_fields (d -> Record.fixed) > 0) ||
		     (d -> Record.variant -> size);
  fprintf (c_code, "/* Comparison test for a %s */\n", lhs);
  fprintf (header, "int cmp_%s (%s a, %s b);\n", lhs, lhs, lhs);
  fprintf (c_code, "int cmp_%s (%s a, %s b)\n", lhs, lhs, lhs);
  fprintf (c_code, "{ %s\n", (local_needed)?"int s;":"");
  fprintf (c_code, "  if (a == b) return (0);\n");
  fprintf (c_code, "  if (a == %s_nil) return (-1);\n", lhs);
  fprintf (c_code, "  if (b == %s_nil) return (1);\n", lhs);
  code_cmp_field_list (string_nil, d -> Record.fixed);
  if (d -> Record.variant -> size)
     { vfield_list vfl = d -> Record.variant;
       int ix;
       fprintf (c_code, "  if ((s = cmp_int ((int)(a -> tag), (int)(b -> tag)))) ");
       fprintf (c_code, "return (s);\n");
       fprintf (c_code, "  switch (a -> tag)\n    { ");
       for (ix = 0; ix < vfl -> size; ix++)
	  { vfield f = vfl -> array[ix];
	    string cons = f -> cons;
	    fprintf (c_code, "%scase TAG%s:\n", (ix)?"      ":"", cons);
	    code_cmp_field_list (cons, f -> parts);
	    fprintf (c_code, "\tbreak;\n");
	  };
       fprintf (c_code, "      default:\n");
       fprintf (c_code, "\tdcg_bad_tag (a -> tag, \"cmp_%s\");\n", lhs);
       fprintf (c_code, "    };\n");
     };
  fprintf (c_code, "  return (0);\n");
  fprintf (c_code, "}\n\n");
};

static void code_cmp_enumeration_definition (def d)
{ string lhs = d -> lhs;
  fprintf (header, "#define cmp_%s(a,b) (cmp_int ((int)(a),(int)(b)))\n", lhs);
}

static void code_cmp_definitions ()
{ int ix;
  fprintf (header, "/* Equality test for types */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      switch (d -> tag)
	{ case TAGRecord: code_cmp_record_definition (d); break;
	  case TAGEnum: code_cmp_enumeration_definition (d);
	  case TAGPrimitive: break;
	  default: dcg_bad_tag (d -> tag, "code_cmp_definitions");
	};
    };
  fprintf (header, "\n");
}

static void code_cmp_list_definition (string elt_nm)
{ fprintf (c_code, "/* Comparison test for a %s_list */\n", elt_nm);
  fprintf (header, "int cmp_%s_list (%s_list a, %s_list b);\n", elt_nm, elt_nm, elt_nm);
  fprintf (c_code, "int cmp_%s_list (%s_list a, %s_list b)\n", elt_nm, elt_nm, elt_nm);
  fprintf (c_code, "{ int ix, stat, size;\n");
  fprintf (c_code, "  if (a == b) return (0);\n");
  fprintf (c_code, "  if (a == %s_list_nil) return (-1);\n", elt_nm);
  fprintf (c_code, "  if (b == %s_list_nil) return (1);\n", elt_nm);
  fprintf (c_code, "  size = a -> size;\n");
  fprintf (c_code, "  if (b -> size < size) size = b -> size;\n");
  fprintf (c_code, "  for (ix = 0; ix < size; ix++)\n");
  fprintf (c_code, "    if ((stat = cmp_%s (a -> array[ix], b -> array[ix])))\n", elt_nm);
  fprintf (c_code, "      return (stat);\n");
  fprintf (c_code, "  return (cmp_int (a -> size, b -> size));\n");
  fprintf (c_code, "}\n\n");
}

static void code_cmp_list_definitions ()
{ int ix;
  fprintf (header, "/* Comparison tests for lists */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
          code_cmp_list_definition (elt_nm);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_equal_definitions ()
{ int ix;
  fprintf (header, "/* Equality shorthands for types */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      switch (d -> tag)
	{ case TAGRecord:
	  case TAGEnum:
	    fprintf (header, "#define equal_%s(a,b) !cmp_%s ((a),(b))\n", lhs, lhs);
	  case TAGPrimitive:
	    break;
	  default: dcg_bad_tag (d -> tag, "code_equal_definitions");
	};
    };
  fprintf (header, "\n");
}

static void code_equal_list_definitions ()
{ int ix;
  fprintf (header, "/* Equality shorthands for lists */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
	  fprintf (header, "#define equal_%s_list(a,b) ", elt_nm);
	  fprintf (header, "!cmp_%s_list((a),(b))\n", elt_nm);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_comparison_tests ()
{ code_cmp_definitions ();
  code_cmp_list_definitions ();
  code_equal_definitions ();
  code_equal_list_definitions ();
};

/*
   Generation of pretty printing routines
*/
static void code_estimate_field_list (string cons, field_list fl)
{ int ix;
  for (ix = 0; ix < fl -> size; ix++)
    { field f = fl -> array[ix];
      string nm = f -> fname;
      if (!f -> ftrav)
	fprintf (c_code, "%ssize += PTRWIDTH;\n", (cons)?"\t":"  ");
      else
	{ fprintf (c_code, "%ssize += est_", (cons)?"\t":"  ");
	  code_type (c_code, f -> ftype);
	  fprintf (c_code, " (old -> ");
	  if (cons) fprintf (c_code, "%s.%s", cons, nm);
	  else fprintf (c_code, "%s", nm);
	  fprintf (c_code, ") + 2;\n");
	};
    };
};

static void code_estimate_record_definition (def d)
{ string lhs = d -> lhs;
  fprintf (c_code, "/* Estimate printing a %s */\n", lhs);
  fprintf (header, "int est_%s (%s old);\n", lhs, lhs);
  fprintf (c_code, "int est_%s (%s old)\n", lhs, lhs);
  fprintf (c_code, "{ int size = 0;\n");
  fprintf (c_code, "  if (old == %s_nil) return (2);\n", lhs);
  code_estimate_field_list (string_nil, d -> Record.fixed);
  if (d -> Record.variant -> size)
    { vfield_list vfl = d -> Record.variant;
      int ix;
      fprintf (c_code, "  switch (old -> tag)\n    { ");
      for (ix = 0; ix < vfl -> size; ix++)
	{ vfield f = vfl -> array[ix];
	  string cons = f -> cons;
	  fprintf (c_code, "%scase TAG%s:\n", (ix)?"      ":"", cons);
	  fprintf (c_code, "\tsize += %d;\n", (int) strlen (cons) + 2);
	  code_estimate_field_list (cons, f -> parts);
	  fprintf (c_code, "\tbreak;\n");
	};
      fprintf (c_code, "      default:\n");
      fprintf (c_code, "\tdcg_bad_tag (old -> tag, \"est_%s\");\n", lhs);
      fprintf (c_code, "    };\n");
    };
  fprintf (c_code, "  return (size + 2);\n");
  fprintf (c_code, "}\n\n");
}

static void code_estimate_enumeration_definition (def d)
{ int ix;
  string lhs = d -> lhs;
  string_list sl = d -> Enum.elems;
  fprintf (c_code, "/* Estimate a %s */\n", lhs);
  fprintf (header, "int est_%s (%s old);\n", lhs, lhs);
  fprintf (c_code, "int est_%s (%s old)\n", lhs, lhs);
  fprintf (c_code, "{ switch (old)\n");
  for (ix = 0; ix < sl -> size; ix++)
    { string elem = sl -> array [ix];
      fprintf (c_code, "    %scase %s: return (%d);\n", (ix)?"  ":"{ ", elem, (int) strlen (elem));
    };
  fprintf (c_code, "      default:\n");
  fprintf (c_code, "\tdcg_bad_tag (old, \"est_%s\");\n", lhs);
  fprintf (c_code, "    };\n");
  fprintf (c_code, "  return (0);\n");
  fprintf (c_code, "}\n\n");
}

static void code_estimate_type_definitions ()
{ int ix;
  fprintf (header, "/* Estimating printing of types */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      switch (d -> tag)
	{ case TAGRecord: code_estimate_record_definition (d); break;
	  case TAGEnum:   code_estimate_enumeration_definition (d);
	  case TAGPrimitive: break;
	  default: dcg_bad_tag (d -> tag, "code_estimate_type_definitions");
	};
    };
  fprintf (header, "\n");
}

static void code_estimate_type_list_definition (string elt_nm)
{ fprintf (c_code, "/* Estimate printing a %s_list */\n", elt_nm);
  fprintf (header, "int est_%s_list (%s_list old);\n", elt_nm, elt_nm);
  fprintf (c_code, "int est_%s_list (%s_list old)\n", elt_nm, elt_nm);
  fprintf (c_code, "{ int ix;\n");
  fprintf (c_code, "  int size = 0;\n");
  fprintf (c_code, "  if (old == %s_list_nil) return (2);\n", elt_nm);
  fprintf (c_code, "  if (!old -> size) return (2);\n");
  fprintf (c_code, "  for (ix = 0; ix < old -> size; ix++)\n");
  fprintf (c_code, "    size += est_%s (old -> array[ix]) + 2;\n", elt_nm);
  fprintf (c_code, "  return (size + 2);\n");
  fprintf (c_code, "}\n\n");
}

static void code_estimate_type_list_definitions ()
{ int ix;
  fprintf (header, "/* Estimate printing of lists */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
          code_estimate_type_list_definition (elt_nm);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_pretty_print_field_list (string cons, field_list fl)
{ int ix;
  if (!fl -> size) return;
  if (cons)
    fprintf (c_code, "\tpppdelim (f, mhoriz, ind, ',');\n");
  for (ix = 0; ix < fl -> size; ix++)
    { field f = fl -> array[ix];
      string nm = f -> fname;
      if (!f -> ftrav)
	{ fprintf (c_code, (cons)?"\t":"  ");
	  fprintf (c_code, "ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> ");
	  if (cons) fprintf (c_code, "%s.%s);\n", cons, nm);
	  else fprintf (c_code, "%s);\n", nm);
	}
      else
	{ fprintf (c_code, "%sppp_", (cons)?"\t":"  ");
	  code_type (c_code, f -> ftype);
	  fprintf (c_code, " (f, mhoriz, ind + 2, old -> ");
	  if (cons) fprintf (c_code, "%s.%s", cons, nm);
	  else fprintf (c_code, "%s", nm);
	  fprintf (c_code, ");\n");
	};
      if (ix != fl -> size - 1)
	fprintf (c_code, "%spppdelim (f, mhoriz, ind, ',');\n", (cons)?"\t":"  ");
    };
}

static void code_pretty_print_record_definition (def d)
{ string lhs = d -> lhs;
  fprintf (c_code, "/* Pretty print a %s */\n", lhs);
  fprintf (header, "void ppp_%s (FILE *f, int horiz, int ind, %s old);\n", lhs, lhs);
  fprintf (c_code, "void ppp_%s (FILE *f, int horiz, int ind, %s old)\n", lhs, lhs);
  fprintf (c_code, "{ int mhoriz;\n");
  fprintf (c_code, "  if (old == %s_nil) { pppstring (f, \"<>\"); return; };\n", lhs);
  fprintf (c_code, "  mhoriz = horiz || (est_%s (old) + ind < MAXWIDTH);\n",lhs);
  fprintf (c_code, "  pppdelim (f, mhoriz, ind, '(');\n");
  code_pretty_print_field_list (string_nil, d -> Record.fixed);
  if (d -> Record.variant -> size)
    { vfield_list vfl = d -> Record.variant;
      int ix;
      if (d -> Record.fixed -> size)
        fprintf (c_code, "  pppdelim (f, mhoriz, ind, ',');\n");
      fprintf (c_code, "  switch (old -> tag)\n    { ");
      for (ix = 0; ix < vfl -> size; ix++)
	{ vfield f = vfl -> array[ix];
	  string cons = f -> cons;
	  fprintf (c_code, "%scase TAG%s:\n", (ix)?"      ":"", cons);
	  fprintf (c_code, "\tpppstring (f, \"%s\");\n", cons);
	  code_pretty_print_field_list (cons, f -> parts);
	  fprintf (c_code, "\tbreak;\n");
	};
      fprintf (c_code, "      default:\n");
      fprintf (c_code, "\tdcg_bad_tag (old -> tag, \"ppp_%s\");\n", lhs);
      fprintf (c_code, "    };\n");
    };
  fprintf (c_code, "  pppdelim (f, mhoriz, ind, ')');\n");
  fprintf (c_code, "}\n\n");
}

static void code_pretty_print_enumeration_definition (def d)
{ int ix;
  string lhs = d -> lhs;
  string_list sl = d -> Enum.elems;
  fprintf (c_code, "/* Pretty print a %s */\n", lhs);
  fprintf (header, "void ppp_%s (FILE *f, int horiz, int ind, %s old);\n", lhs, lhs);
  fprintf (c_code, "void ppp_%s (FILE *f, int horiz, int ind, %s old)\n", lhs, lhs);
  fprintf (c_code, "{ switch (old)\n");
  for (ix = 0; ix < sl -> size; ix++)
    { string elem = sl -> array [ix];
      fprintf (c_code, "     %s case %s:\n", (ix)?" ":"{", elem);
      fprintf (c_code, "\tpppstring (f, \"%s\");\n", elem);
      fprintf (c_code, "\tbreak;\n");
    };
  fprintf (c_code, "      default:\n");
  fprintf (c_code, "\tdcg_bad_tag (old, \"ppp_%s\");\n", lhs);
  fprintf (c_code, "    };\n");
  fprintf (c_code, "}\n\n");
}

static void code_pretty_print_type_definitions ()
{ int ix;
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      switch (d -> tag)
	{ case TAGRecord: code_pretty_print_record_definition (d); break;
	  case TAGEnum: code_pretty_print_enumeration_definition (d);
	  case TAGPrimitive: break;
	  default: dcg_bad_tag (d -> tag, "code_pretty_print_type_definitions");
	};
    };
  fprintf (header, "\n");
}

static void code_pretty_print_type_list_definition (string elt_nm)
{ fprintf (c_code, "/* Pretty print a %s_list */\n", elt_nm);
  fprintf (header, "void ppp_%s_list (FILE *f, int horiz, int ind, %s_list old);\n",elt_nm, elt_nm);
  fprintf (c_code, "void ppp_%s_list (FILE *f, int horiz, int ind, %s_list old)\n", elt_nm, elt_nm);
  fprintf (c_code, "{ int ix, mhoriz;\n");
  fprintf (c_code, "  if (old == %s_list_nil) { pppstring (f, \"<>\"); return; };\n", elt_nm);
  fprintf (c_code, "  if (!old -> size) { pppstring (f, \"[]\"); return; };\n");
  fprintf (c_code, "  mhoriz = horiz || (est_%s_list (old) + ind < MAXWIDTH);\n", elt_nm);
  fprintf (c_code, "  pppdelim (f, mhoriz, ind, '[');\n");
  fprintf (c_code, "  for (ix = 0; ix < old -> size; ix++)\n");
  fprintf (c_code, "    { ppp_%s (f, mhoriz, ind + 2, old -> array[ix]);\n", elt_nm);
  fprintf (c_code, "      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');\n");
  fprintf (c_code, "    };\n");
  fprintf (c_code, "  pppdelim (f, mhoriz, ind, ']');\n");
  fprintf (c_code, "}\n\n");
}

static void code_pretty_print_type_list_definitions ()
{ int ix;
  fprintf (header, "/* Introduce pretty printing of lists */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
          code_pretty_print_type_list_definition (elt_nm);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_pretty_print_type_shorthands ()
{ int ix;
  fprintf (header, "/* Introduce pretty printing shorthands */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      if (d -> tag != TAGPrimitive)
	fprintf (header, "#define pp_%s(f,x) ppp_%s (f,0,0,x)\n", lhs, lhs);
    };
  fprintf (header, "\n");
}

static void code_pretty_print_type_list_shorthands ()
{ int ix;
  fprintf (header, "/* Introduce pretty printing shorthands for lists */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
	  fprintf (header, "#define pp_%s_list(f,x) ppp_%s_list (f,0,0,x)\n", elt_nm, elt_nm);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_pretty_printing_routines ()
{ code_estimate_type_definitions ();
  code_estimate_type_list_definitions ();
  code_pretty_print_type_definitions ();
  code_pretty_print_type_list_definitions ();
  code_pretty_print_type_shorthands ();
  code_pretty_print_type_list_shorthands ();
}

/*
   Generation of transput routines
   Try code the saving of datastructures (will be changed)
*/
static void code_save_field_list (string cons, field_list fl)
{ int ix;
  for (ix = 0; ix < fl -> size; ix++)
    { field f = fl -> array[ix];
      string nm = f -> fname;
      if (!f -> ftrav) continue;		/* Ok */
      fprintf (c_code, "%ssave_", (cons)?"\t":"  ");
      code_type (c_code, f -> ftype);
      if (cons) fprintf (c_code, " (bf, old -> %s.%s);\n", cons, nm);
      else fprintf (c_code, " (bf, old -> %s);\n", nm);
    };
}

static void code_save_record_definition (def d)
{ string lhs = d -> lhs;
  fprintf (c_code, "/* Recursively save a %s */\n", lhs);
  fprintf (header, "void save_%s (BinFile bf, %s old);\n", lhs, lhs);
  fprintf (c_code, "void save_%s (BinFile bf, %s old)\n", lhs, lhs);
  fprintf (c_code, "{ /* save fixed fields before variant ones */\n");
  code_save_field_list (string_nil, d -> Record.fixed);
  if (d -> Record.variant -> size)
    { vfield_list vfl = d -> Record.variant;
      int ix;
      fprintf (c_code, "  save_char (bf, (char) old -> tag);\n");
      fprintf (c_code, "  switch (old -> tag)\n    { ");
      for (ix = 0; ix < vfl -> size; ix++)
	{ vfield f = vfl -> array[ix];
	  string cons = f -> cons;
	  fprintf (c_code, "%scase TAG%s:\n", (ix)?"      ":"", cons);
	  code_save_field_list (cons, f -> parts);
	  fprintf (c_code, "\tbreak;\n");
	};
      fprintf (c_code, "      default:\n");
      fprintf (c_code, "\tdcg_bad_tag (old -> tag, \"save_%s\");\n", lhs);
      fprintf (c_code, "    };\n");
    };
  fprintf (c_code, "}\n\n");
}

static void code_save_type_definitions ()
{ int ix;
  fprintf (header, "/* Introduce saving of types */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
     { def d = all_defs -> array[ix];
       if (d -> tag == TAGEnum) 
	  fprintf (header, "#define save_%s(bf,x) save_char(bf, (char) x)\n", d -> lhs);
     };
  for (ix = 0; ix < all_defs -> size; ix++)
     { def d = all_defs -> array[ix];
       if (d -> tag == TAGRecord) code_save_record_definition (d);
     };
  fprintf (header, "\n");
}

static void code_save_type_list_definition (string elt_nm)
{ fprintf (c_code, "/* Recursively save a %s_list */\n", elt_nm);
  fprintf (header, "void save_%s_list (BinFile bf, %s_list l);\n", elt_nm, elt_nm);
  fprintf (c_code, "void save_%s_list (BinFile bf, %s_list l)\n", elt_nm, elt_nm);
  fprintf (c_code, "{ int ix;\n");
  fprintf (c_code, "  savesize (bf, l -> size);\n");
  fprintf (c_code, "  for (ix = 0; ix < l -> size; ix++)\n");
  fprintf (c_code, "    save_%s (bf, l -> array[ix]);\n", elt_nm);
  fprintf (c_code, "}\n\n");
}

static void code_save_type_list_definitions ()
{ int ix;
  fprintf (header, "/* Introduce saving of lists */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
          code_save_type_list_definition (elt_nm);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

/*
   Try code the loading of datastructures
*/
static void code_load_enum_definition (def d)
{ string lhs = d -> lhs;
  fprintf (c_code, "/* Load a %s */\n", lhs);
  fprintf (header, "void load_%s (BinFile bf, %s *x);\n", lhs, lhs);
  fprintf (c_code, "void load_%s (BinFile bf, %s *x)\n", lhs, lhs);
  fprintf (c_code, "{ char ch;\n");
  fprintf (c_code, "  load_char (bf, &ch);\n");
  fprintf (c_code, "  *x = (%s) ch;\n", lhs);
  fprintf (c_code, "}\n\n");
}

static void code_load_field_list (string cons, field_list fl)
{ int ix;
  for (ix = 0; ix < fl -> size; ix++)
    { field f = fl -> array[ix];
      string nm = f -> fname;
      if (!f -> ftrav)
	{ if (cons) fprintf (c_code, "\tnew -> %s.%s = ", cons, nm);
	  else fprintf (c_code, "  new -> %s = ", nm);
	  code_type (c_code, f -> ftype);
	  fprintf (c_code, "_nil;\n");
	}
      else
	{ fprintf (c_code, "%sload_", (cons)?"\t":"  ");
          code_type (c_code, f -> ftype);
          if (cons)
	    fprintf (c_code, " (bf, &new -> %s.%s);\n", cons, nm);
          else fprintf (c_code, " (bf, &new -> %s);\n", nm);
	};
    };
}

static void code_load_record_definition (def d, string bst)
{ string lhs = d -> lhs;
  fprintf (c_code, "/* Recursively load a %s */\n", lhs);
  fprintf (header, "void load_%s (BinFile bf, %s *x);\n", lhs, lhs);
  fprintf (c_code, "void load_%s (BinFile bf, %s *x)\n", lhs, lhs);
  fprintf (c_code, "{ /* load fixed fields before variant ones */\n");
  fprintf (c_code, "  %s new = (%s) %s_malloc (sizeof (struct str_%s));\n", lhs, lhs, bst, lhs);
  if (d -> Record.variant -> size)
    fprintf (c_code, "  char ch;\n");
  code_load_field_list (string_nil, d -> Record.fixed);
  if (d -> Record.variant -> size)
    { vfield_list vfl = d -> Record.variant;
      int ix;
      fprintf (c_code, "  load_char (bf, &ch);\n");
      fprintf (c_code, "  new -> tag = (tags_%s) ch;\n", lhs);
      fprintf (c_code, "  switch (new -> tag)\n    { ");
      for (ix = 0; ix < vfl -> size; ix++)
	{ vfield f = vfl -> array[ix];
	  string cons = f -> cons;
	  fprintf (c_code, "%scase TAG%s:\n", (ix)?"      ":"", cons);
	  code_load_field_list (cons, f -> parts);
	  fprintf (c_code, "\tbreak;\n");
	};
      fprintf (c_code, "      default:\n");
      fprintf (c_code, "\tdcg_bad_tag (new -> tag, \"load_%s\");\n", lhs);
      fprintf (c_code, "    };\n");
    };
  fprintf (c_code, "  *x = new;\n");
  fprintf (c_code, "}\n\n");
}

static void code_load_type_definitions (string bst)
{ int ix;
  fprintf (header, "/* Introduce loading of types */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      switch (d -> tag)
	{ case TAGEnum: code_load_enum_definition (d); break;
	  case TAGRecord: code_load_record_definition (d, bst);
	  default: break;
	};
    };
  fprintf (header, "\n");
}

static void code_load_type_list_definition (string elt_nm)
{ fprintf (c_code, "/* Recursively load a %s_list */\n", elt_nm);
  fprintf (header, "void load_%s_list (BinFile bf, %s_list *l);\n", elt_nm, elt_nm);
  fprintf (c_code, "void load_%s_list (BinFile bf, %s_list *l)\n", elt_nm, elt_nm);
  fprintf (c_code, "{ int ix, size;\n");
  fprintf (c_code, "  %s_list new;\n", elt_nm);
  fprintf (c_code, "  loadsize (bf, &size);\n");
  fprintf (c_code, "  new = init_%s_list (size);\n", elt_nm);
  fprintf (c_code, "  new -> size = size;\n");
  fprintf (c_code, "  for (ix = 0; ix < size; ix++)\n");
  fprintf (c_code, "    load_%s (bf, &new -> array[ix]);\n", elt_nm);
  fprintf (c_code, "  *l = new;\n");
  fprintf (c_code, "}\n\n");
}

static void code_load_type_list_definitions ()
{ int ix;
  fprintf (header, "/* Introduce loading of lists */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
          code_load_type_list_definition (elt_nm);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_transput_routines (string bst)
{ code_save_type_definitions ();
  code_save_type_list_definitions ();
  code_load_type_definitions (bst);
  code_load_type_list_definitions ();
}

/*
   Generation of recursive copying routines
*/
static void code_rdup_enum_definition (def d)
{ string lhs = d -> lhs;
  fprintf (header, "#define rdup_%s(orig) (orig)\n", lhs);
};

static void code_rdup_field_list (string cons, field_list fl)
{ int ix;
  for (ix = 0; ix < fl -> size; ix++)
    { field f = fl -> array[ix];
      string nm = f -> fname;
      if (!f -> ftrav) continue;		/* Ok */
      fprintf (c_code, "%s", (cons)?"\t":"  ");
      if (cons) fprintf (c_code, "new -> %s.%s = rdup_", cons, nm);
      else fprintf (c_code, "new -> %s = rdup_", nm);
      code_type (c_code, f -> ftype);
      if (cons) fprintf (c_code, " (old -> %s.%s);\n", cons, nm);
      else fprintf (c_code, " (old -> %s);\n", nm);
    };
}

static void code_rdup_record_definition (def d, string bst)
{ string lhs = d -> lhs;
  fprintf (c_code, "/* Recursively duplicate a %s */\n", lhs);
  fprintf (header, "%s rdup_%s (%s old);\n", lhs, lhs, lhs);
  fprintf (c_code, "%s rdup_%s (%s old)\n", lhs, lhs, lhs);
  fprintf (c_code, "{ /* allocate new record */\n");
  fprintf (c_code, "  %s new;\n", lhs);
  fprintf (c_code, "  if (old == %s_nil) return (old);\n", lhs);
  fprintf (c_code, "  new = (%s) %s_malloc (sizeof (struct str_%s));\n\n", lhs, bst, lhs);
  fprintf (c_code, "  /* duplicate fixed fields before variant ones */\n");
  code_rdup_field_list (string_nil, d -> Record.fixed);
  if (d -> Record.variant -> size)
    { vfield_list vfl = d -> Record.variant;
      int ix;

      fprintf (c_code, "  new -> tag = old -> tag;\n");
      fprintf (c_code, "  switch (old -> tag)\n    { ");
      for (ix = 0; ix < vfl -> size; ix++)
	{ vfield f = vfl -> array[ix];
	  string cons = f -> cons;
	  fprintf (c_code, "%scase TAG%s:\n", (ix)?"      ":"", cons);
	  code_rdup_field_list (cons, f -> parts);
	  fprintf (c_code, "\tbreak;\n");
	};
      fprintf (c_code, "      default:\n");
      fprintf (c_code, "\tdcg_bad_tag (old -> tag, \"rdup_%s\");\n", lhs);
      fprintf (c_code, "    };\n");
    };
  fprintf (c_code, "  return (new);\n");
  fprintf (c_code, "}\n\n");
}

static void code_rdup_type_routines (string bst)
{ int ix;
  fprintf (header, "/* Introduce recursive duplication of types */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      switch (d -> tag)
	{ case TAGEnum:
	    code_rdup_enum_definition (d); 
	    break;
	  case TAGRecord:
	    code_rdup_record_definition (d, bst);
	  default: break;
	};
    };
  fprintf (header, "\n");
}

static void code_rdup_type_list_definition (string elt_nm)
{ fprintf (c_code, "/* Recursively duplicate a %s_list */\n", elt_nm);
  fprintf (header, "%s_list rdup_%s_list (%s_list old);\n", elt_nm, elt_nm, elt_nm);
  fprintf (c_code, "%s_list rdup_%s_list (%s_list old)\n", elt_nm, elt_nm, elt_nm);
  fprintf (c_code, "{ %s_list new;\n", elt_nm);
  fprintf (c_code, "  int ix;\n");
  fprintf (c_code, "  if (old == %s_list_nil) return (old);\n", elt_nm);
  fprintf (c_code, "  new = init_%s_list (old -> size);\n", elt_nm);
  fprintf (c_code, "  for (ix = 0; ix < old -> size; ix++)\n");
  fprintf (c_code, "    app_%s_list (new, rdup_%s (old -> array[ix]));\n", elt_nm, elt_nm);
  fprintf (c_code, "  return (new);\n");
  fprintf (c_code, "}\n\n");
}

static void code_rdup_type_list_routines ()
{ int ix;
  fprintf (header, "/* Introduce recursive copying of lists */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
	  code_rdup_type_list_definition (elt_nm);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_rdup_routines (string bst)
{ code_rdup_type_routines (bst);
  code_rdup_type_list_routines ();
}

/*
   Because of problems with the EAG3 compiler, code will be generated
   that can be called if too many detaches in the code take place.
   This code is only intended to debug these kinds of problems. Maybe
   a msck (memory system consistency check) will have to be written or
   generated at some time in the future, but for the moment this might
   suffice.
*/
static void code_check_refcount_enum_definition (def d)
{ string lhs = d -> lhs;
  fprintf (header, "#define check_not_freed_%s(curr)\n", lhs);
};

static void code_check_refcount_field_list (string cons, field_list fl)
{ int ix;
  for (ix = 0; ix < fl -> size; ix++)
    { field f = fl -> array[ix];
      string nm = f -> fname;
      if (!f -> ftrav) continue;		/* Ok */
      fprintf (c_code, "%scheck_not_freed_", (cons)?"\t":"  ");
      code_type (c_code, f -> ftype);
      if (cons) fprintf (c_code, " (curr -> %s.%s);\n", cons, nm);
      else fprintf (c_code, " (curr -> %s);\n", nm);
    };
}

static void code_check_refcount_record_definition (def d, string bst)
{ string lhs = d -> lhs;
  fprintf (c_code, "/* Recursively check the ref counts of a %s */\n", lhs);
  fprintf (header, "void check_not_freed_%s (%s curr);\n", lhs, lhs);
  fprintf (c_code, "void check_not_freed_%s (%s curr)\n", lhs, lhs);
  fprintf (c_code, "{ if (curr == %s_nil) return;\n", lhs);
  code_check_refcount_field_list (string_nil, d -> Record.fixed);
  if (d -> Record.variant -> size)
    { vfield_list vfl = d -> Record.variant;
      int ix;
      fprintf (c_code, "  switch (curr -> tag)\n    { ");
      for (ix = 0; ix < vfl -> size; ix++)
	{ vfield f = vfl -> array[ix];
	  string cons = f -> cons;
	  fprintf (c_code, "%scase TAG%s:\n", (ix)?"      ":"", cons);
	  code_check_refcount_field_list (cons, f -> parts);
	  fprintf (c_code, "\tbreak;\n");
	};
      fprintf (c_code, "      default:\n");
      fprintf (c_code, "\tdcg_bad_tag (curr -> tag, \"check_not_freed_%s\");\n", lhs);
      fprintf (c_code, "    };\n");
    };
  fprintf (c_code, "}\n\n");
}

static void code_check_refcount_type_routines (string bst)
{ int ix;
  fprintf (header, "/* Introduce recursive refcount checking of types */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      switch (d -> tag)
	{ case TAGEnum:
	    code_check_refcount_enum_definition (d); 
	    break;
	  case TAGRecord:
	    code_check_refcount_record_definition (d, bst);
	  default: break;
	};
    };
  fprintf (header, "\n");
}

static void code_check_refcount_type_list_definition (string elt_nm)
{ fprintf (c_code, "/* Recursively check the refcounts of a %s_list */\n", elt_nm);
  fprintf (header, "void check_not_freed_%s_list (%s_list curr);\n", elt_nm, elt_nm);
  fprintf (c_code, "void check_not_freed_%s_list (%s_list curr)\n", elt_nm, elt_nm);
  fprintf (c_code, "{ int ix;\n");
  fprintf (c_code, "  if (curr == %s_list_nil) return;\n", elt_nm);
  fprintf (c_code, "  for (ix = 0; ix < curr -> size; ix++)\n");
  fprintf (c_code, "    check_not_freed_%s (curr -> array[ix]);\n", elt_nm);
  fprintf (c_code, "}\n\n");
}

static void code_check_refcount_type_list_routines ()
{ int ix;
  fprintf (header, "/* Introduce recursive refcount checking of lists */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
	  code_check_refcount_type_list_definition (elt_nm);
	  detach_string (&elt_nm);
	};
    };
  fprintf (header, "\n");
}

static void code_check_refcount_routines (string bst)
{ code_check_refcount_type_routines (bst);
  code_check_refcount_type_list_routines ();
}

/*
   Generation of all c_code C functions
*/
static void code_routine_definitions (string bst)
{ code_basic_type_handling (bst);
  code_list_modification_routines (bst);
  code_comparison_tests ();
  if (code_pretty_print) code_pretty_printing_routines ();
  if (code_transput) code_transput_routines (bst);
  if (code_rdup) code_rdup_routines (bst);
  code_check_refcount_routines (bst);
}

static void code_header_trailer (char *basename)
{ fprintf (header, "#endif /* Inc%c%s */\n", toupper (basename[0]), basename + 1);
  fprintf (c_def, "#endif\n");
  if (!code_extra_tags) return;
  fprintf (tags_header, "#endif /* Inc%c%s_tags */\n", toupper (basename[0]), basename + 1);
}

static void close_output_files ()
{ fclose (header);
  fclose (c_code);
  fclose (c_def);
  if (code_extra_tags)
    fclose (tags_header);
}

void code (char *basename)
{ dcg_hint ("coding definitions...");
  try_open_output_files (basename);
  code_file_headers (basename);
  code_std_includes (basic_types);
  code_local_includes (basename);
  code_type_definitions ();
  code_routine_definitions (basic_types);
  code_header_trailer (basename);
  close_output_files ();
}
