/*
   File: xform_coder.c
   Codes a datastructure transformer

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

   CVS ID: "$Id: xform_coder.c,v 1.13 2011/10/14 11:19:45 marcs Exp $"
*/

/* include config.h if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* standard includes */
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>

/* Conditional inclusion to define MAXPATHLEN */
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
#include "coder.h"
#include "xform_coder.h"

static FILE *header;
static FILE *c_code;
static void try_open_output_files (char *basename, string pname)
{ char fname[MAXPATHLEN + 1];
  sprintf (fname, "%s_%s.h", pname, basename);
  if (!(header = fopen (fname, "w")))
    dcg_panic ("could not open header file %s", fname);
  sprintf (fname, "%s_%s.c", pname, basename);
  if (!(c_code = fopen (fname, "w")))
    dcg_panic ("could not open code file %s", fname);
}

static void code_file_headers (char *basename, string pname)
{ time_t thetime;
  char *atime;
  time (&thetime);
  atime = ctime (&thetime);
  fprintf (header, "/*\n");
  fprintf (header, "   File: %s_%s.h\n", pname, basename);
  fprintf (header, "   Generated on %s", atime);
  fprintf (header, "*/\n");
  fprintf (header, "#ifndef Inc%c%s%c%s\n",
	   toupper (pname[0]), pname + 1,
	   toupper (basename[0]), basename + 1);
  fprintf (header, "#define Inc%c%s%c%s\n",
	   toupper (pname[0]), pname + 1,
	   toupper (basename[0]), basename + 1);
  fprintf (c_code, "/*\n");
  fprintf (c_code, "   File: %s_%s.c\n", pname, basename);
  fprintf (c_code, "   Generated on %s", atime);
  fprintf (c_code, "*/\n\n");
}

static void code_std_includes ()
{ fprintf (header, "#include <dcg.h>\n");
  fprintf (c_code, "/* standard includes */\n");
  fprintf (c_code, "#include <stdio.h>\n\n");
  fprintf (c_code, "/* libdcg includes */\n");
  fprintf (c_code, "#include <dcg.h>\n");
  fprintf (c_code, "#include <dcg_error.h>\n");
  fprintf (c_code, "#include <dcg_alloc.h>\n\n");
  fprintf (c_code, "#include <dcg_string.h>\n\n");
}

static void code_local_includes (string basename, string_list ex_names, string pname)
{ int ix;
  fprintf (header, "#include <%s.h>\n", basename);
  fprintf (c_code, "/* local includes */\n");
  fprintf (c_code, "#include <%s.h>\n", basename);
  for (ix = 0; ix < ex_names -> size; ix++)
    fprintf (c_code, "#include <%s_%s.h>\n", pname, ex_names -> array[ix]);
  fprintf (c_code, "#include <%s_%s.h>\n\n", pname, basename);
  fprintf (header, "\n");
}

/*
   Transformer code generation
*/
static void code_type (FILE *file, type t)
{ switch (t -> tag)
    { case TAGTname:
	fprintf (file, "%s", t -> Tname.tname);
	break;
      case TAGTlist:
	code_type (file, t -> Tlist.etyp);
	fprintf (file, "_list");
	break;
      default:
	dcg_panic ("no such tag %d in code_type", (int) t -> tag);
    };
}

static void code_xform_field_list (string cons, field_list fl, string pname, string ptype)
{ int ix;
  for (ix = 0; ix < fl -> size; ix++)
    { field f = fl -> array[ix];
      string nm = f -> fname;
      if (!f -> ftrav) continue;
      fprintf (c_code, "%s%s_", (cons)?"\t":"  ", pname);
      code_type (c_code, f -> ftype);
      if (cons) fprintf (c_code, " (d -> %s.%s", cons, nm);
      else fprintf (c_code, " (d -> %s", nm);
      if (ptype != NULL) fprintf (c_code, ", xp");
      fprintf (c_code, ");\n");
    };
}

static int lhs_on_list (string lhs, string_list ex_names)
{ int ix;
  for (ix = 0; ix < ex_names -> size; ix++)
    if (streq (lhs, ex_names -> array[ix])) return (1);
  return (0);
}

static void code_xform_record_definition (def d, string_list ex_names, string pname, string ptype)
{ string lhs = d -> lhs;
  if (lhs_on_list (lhs, ex_names)) return;
  fprintf (c_code, "/* Recursively transform a %s */\n", lhs);
  fprintf (header, "void %s_%s (%s d, %s xp);\n", pname, lhs, lhs, ptype);
  fprintf (c_code, "void %s_%s (%s d, %s xp)\n", pname, lhs, lhs, ptype);
  fprintf (c_code, "{ /* transform fixed fields before variant ones */\n");
  code_xform_field_list (string_nil, d -> Record.fixed, pname, ptype);
  if (d -> Record.variant -> size)
    { vfield_list vfl = d -> Record.variant;
      int ix;
      fprintf (c_code, "  switch (d -> tag)\n    { ");
      for (ix = 0; ix < vfl -> size; ix++)
	{ vfield f = vfl -> array[ix];
	  string cons = f -> cons;
	  fprintf (c_code, "%scase TAG%s:\n", (ix)?"      ":"", cons);
	  code_xform_field_list (cons, f -> parts, pname, ptype);
	  fprintf (c_code, "\tbreak;\n");
	};
      fprintf (c_code, "      default: dcg_bad_tag (d -> tag, \"%s_%s\");\n", pname,  lhs);
      fprintf (c_code, "    };\n");
    };
  fprintf (c_code, "}\n\n");
}

static void code_xform_type_definitions (string_list ex_names, string pname, string ptype)
{ int ix;
  fprintf (header, "/* Introduce transforming of enumeration types */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      if (d -> tag == TAGEnum)
	fprintf (header, "#define %s_%s(d,xp)\n", pname, d -> lhs);
    };
  fprintf (header, "\n/* Introduce transforming of record types */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      if (d -> tag == TAGRecord)
	code_xform_record_definition (d, ex_names, pname, ptype);
    };
  fprintf (header, "\n");
}

static void code_xform_type_list_definition (string pname, string elt_nm, string ptype)
{ fprintf (c_code, "/* Recursively transform a %s_list */\n", elt_nm);
  fprintf (header, "void %s_%s_list (%s_list l, %s xp);\n", pname, elt_nm, elt_nm, ptype);
  fprintf (c_code, "void %s_%s_list (%s_list l, %s xp)\n", pname, elt_nm, elt_nm, ptype);
  fprintf (c_code, "{ int ix;\n");
  fprintf (c_code, "  for (ix = 0; ix < l -> size; ix++)\n");	
  fprintf (c_code, "    %s_%s (l -> array[ix], xp);\n", pname, elt_nm);
  fprintf (c_code, "};\n\n");
}

static void code_xform_type_list_definitions (string pname, string ptype)
{ int ix;
  fprintf (header, "/* Introduce transforming of lists */\n");
  for (ix = 0; ix < all_defs -> size; ix++)
    { def d = all_defs -> array[ix];
      string lhs = d -> lhs;
      int lix;
      for (lix = d -> implsts; lix < d -> nrlsts; lix++)
	{ string elt_nm = make_type_name (lhs, lix);
	  code_xform_type_list_definition (pname, elt_nm, ptype);
	  detach_string (&elt_nm);
	};
    };
}

/*
   Drive the coding of the c_code
*/
static void code_routine_definitions (string_list ex_names, string pname, string ptype)
{ code_xform_type_definitions (ex_names, pname, ptype);
  code_xform_type_list_definitions (pname, ptype);
}

static void code_header_trailer (char *basename)
{ fprintf (header, "#endif /* IncXform%c%s */\n", toupper (basename[0]), basename + 1);
}

static void close_output_files ()
{ fclose (header);
  fclose (c_code);
}

void code_xformer (char *basename, string_list ex_names, string pname, string ptype)
{ dcg_hint ("coding transformer definitions...");
  try_open_output_files (basename, pname);
  code_file_headers (basename, pname);
  code_std_includes ();
  code_local_includes (basename, ex_names, pname);
  code_routine_definitions (ex_names, pname, ptype);
  code_header_trailer (basename);
  close_output_files ();
}
