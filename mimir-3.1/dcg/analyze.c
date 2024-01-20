/*
   File: analyze.c
   Analyzes the datastructure definitions

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

   CVS ID: "$Id: analyze.c,v 1.8 2012/12/22 16:19:33 marcs Exp $"
*/

/* standard includes */
#include <stdio.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>

/* local includes */
#include "dcg_code.h"
#include "dcg_parser2.h"
#include "analyze.h"

static void check_no_double_decls (def_list defs)
{ int ix;
  for (ix = 0; ix < defs -> size; ix++)
    { def d = defs -> array[ix];
      int iy;
      for (iy = ix + 1; iy < defs -> size; iy++)
	if (streq (d -> lhs, defs -> array[iy] -> lhs))
	  dcg_error (1, "%s is multiply declared", d -> lhs);
    }; 
}

static def find_def (def_list defs, string dname)
{ int ix;
  for (ix = 0; ix < defs -> size; ix++)
    { def d = defs -> array[ix];
      if (streq (dname, d -> lhs)) return (d);
    };
  return (def_nil);
}

static void check_type (type t, int list_level, string lhs, def_list defs)
{ switch (t -> tag)
    { case TAGTname:
	{ string tname = t -> Tname.tname;
	  def d = find_def (defs, tname);
	  if (d == def_nil)
	    { if (streq (lhs, ">"))
		dcg_error (1, "could not find type %s in import", tname);
	      else if (streq (lhs, "?"))
		dcg_error (1, "could not find type %s in request list", tname);
	      else
		dcg_error (1, "could not find type %s in definition %s", tname, lhs);
	    }
	  else if (streq (lhs, ">"))
	    { if (list_level > d -> implsts)
	        d -> implsts = list_level;
	    }
	  else if (list_level > d -> nrlsts)
	    d -> nrlsts = list_level;
	};
	break;
      case TAGTlist:
	check_type (t -> Tlist.etyp, list_level + 1, lhs, defs);
	break;
      default: dcg_bad_tag (t -> tag, "check_type");
    };
}

static void check_type_list (type_list tl, string lhs, def_list defs)
{ int ix;
  for (ix = 0; ix < tl -> size; ix++)
    check_type (tl -> array[ix], 0, lhs, defs);
}

static void check_types_in_fields (field_list fl, string lhs, def_list defs)
{ int ix;
  for (ix = 0; ix < fl -> size; ix++)
    check_type (fl -> array[ix] -> ftype, 0, lhs, defs);
}

static void check_types_in_vfields (vfield_list vfl, string lhs, def_list defs)
{ int ix;
  for (ix = 0; ix < vfl -> size; ix++)
    check_types_in_fields (vfl -> array[ix] -> parts, lhs, defs);
}

static void check_types_in_def (def d, def_list defs)
{ string lhs = d -> lhs;
  switch (d -> tag)
    { case TAGRecord:
	{ check_types_in_fields (d -> Record.fixed, lhs, defs);
	  check_types_in_vfields (d -> Record.variant,lhs, defs);
	};
      case TAGEnum:
      case TAGPrimitive: break;
      default: dcg_bad_tag (d -> tag, "check_types_in_def");
    };
}

static void check_types_in_defs (def_list defs)
{ int ix;
  for (ix = 0; ix < defs -> size; ix++)
    check_types_in_def (defs -> array[ix], defs);
}

static void check_types_in_stat (stat s, def_list defs)
{ switch (s -> tag)
    { case TAGUse:
	check_type_list (s -> Use.utype, "?", defs);
      case TAGImport: break;
      default: dcg_bad_tag (s -> tag, "check_types_in_stat");
    };
}

static void check_types_in_stats (stat_list stats, def_list defs)
{ int ix;
  for (ix = 0; ix < stats -> size; ix++)
    check_types_in_stat (stats -> array[ix], defs);
}

static void check_imported_types (type_list types, def_list defs)
{ int ix;
  for (ix = 0; ix < types -> size; ix++)
    check_type (types -> array[ix], 0, ">", defs);
}

void analyze ()
{ dcg_hint ("analyzing definitions...");
  check_no_double_decls (all_defs);
  check_types_in_defs (all_defs);
  check_types_in_stats (all_stats, all_defs);
  check_imported_types (imp_types, all_defs);
  check_not_freed_def_list (all_defs);
  check_not_freed_stat_list (all_stats);
  check_not_freed_type_list (imp_types);
}

static void analyze_ex_name (def_list defs, string ex_name)
{ int ix;
  for (ix = 0; ix < defs -> size; ix++)
    { def d = defs -> array[ix];
      if (streq (ex_name, d -> lhs))
	{ if (d -> tag != TAGRecord)
	    dcg_panic ("%s is not a record type", ex_name);
	  return;	
	};
     };
  dcg_panic ("type %s does not exist", ex_name);
}

void analyze_ex_names (string_list ex_names)
{ int ix;
  for (ix = 0; ix < ex_names -> size; ix++)
     analyze_ex_name (all_defs, ex_names -> array[ix]);
}
