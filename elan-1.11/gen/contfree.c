/*
   File: contfree.c
   Defines the context free analysis

   This module is also responsible for loading
   and or parsing of the packets used by the main packet.

   CVS ID: "$Id: contfree.c,v 1.15 2011/10/05 10:50:38 marcs Exp $"
*/

/* global includes */
#include <stdio.h>   
#include <string.h>
#include <sys/param.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>   
#include <dcg_binfile.h>
#include <dcg_plist.h>
#include <dcg_plist_ops.h>
               
/* local includes */ 
#include "options.h"
#include "elan_ast.h"
#include "lexer.h"
#include "parser.h"
#include "warshall.h"
#include "contfree.h"

/* The name of the standard packet is standard.e */
#define STANDARD_PACKET_NAME "standard"

static int try_locate (char *fname, int main)
{ FILE *fd = fopen (fname, "r");
  if (fd == NULL) return (0);
  if (main) set_actual_source (fname);
  init_lexer (fd, fname, main);
  return (1);
}

static void locate_and_init_packet (int main)
{ char buf[MAXPATHLEN+1];
  sprintf (buf, "%s.elan", source_name);
  if (try_locate (buf, main)) return;
  sprintf (buf, "%s.eln", source_name);
  if (try_locate (buf, main)) return;
  sprintf (buf, "%s.e", source_name);
  if (try_locate (buf, main)) return;
  sprintf (buf, "%s", source_name);
  if (try_locate (buf, main)) return;
  dcg_panic ("could not open source file '%s'", source_name);
}

static void add_uses_to_pnames (string_list uses, string_list pnames)
{ int ix;
  for (ix = 0; ix < uses -> size; ix++)
    { string pnm = uses -> array[ix];
      if (streq (pnm, STANDARD_PACKET_NAME)) continue;
      add_uniquely_to_string_list (pnames, pnm);
    };
}

/*
   For each directory in the packet_search_path, try to locate and load
   a preparsed version of the packet. If not available try to load a
   source version of the packet and parse it. In both cases the packets
   AST is returned.
*/
#define PREPARSE_SUFFIX "ppe"
static void load_or_parse_packet (string pname, packet *pkt)
{ int ix;
  for (ix = 0; ix < packet_search_path -> size; ix++)
    { string pkt_dir = packet_search_path -> array[ix];
      char buf[MAXPATHLEN+1];
      BinFile bf;
      FILE *fd;

      /* try load preparsed version */
      sprintf (buf, "%s/%s.%s", pkt_dir, pname, PREPARSE_SUFFIX);
      bf = dcg_bin_fopen (buf, "rb", 0);
      if (bf != NULL)
	{ dcg_warning (0, "   loading packet %s...", pname);
	  dcg_bin_verify_header (bf, "ELAN", "preparsed packet", VERSION);
	  load_packet (bf, pkt);
	  dcg_bin_verify_eof (bf);
	  if (!streq (pname, (*pkt) -> pname))
	    dcg_panic ("   internal packet name %s and packet filename %s.%s do not match",
		      (*pkt) -> pname, pname, PREPARSE_SUFFIX);
	  dcg_bin_fclose (&bf);
	  return;
	};

      /* try parse packet */
      sprintf (buf, "%s/%s.e", pkt_dir, pname);
      fd = fopen (buf, "r");
      if (fd != NULL)
	{ dcg_warning (0, "   parsing packet %s...", pname);
	  init_lexer (fd, buf, 0);
	  should_be_packet (pname, pkt);
	  dcg_panic_if_errors ();
          return;
	};
    };
  dcg_panic ("   could not locate packet %s", pname);
}
/*
   Parse and save the packet in a binary format
*/
void parse_and_save_source ()
{ char buf[MAXPATHLEN+1];
  BinFile bf;
  packet pkt;

  /* Parse source packet */
  locate_and_init_packet (0);
  should_be_packet (basename, &pkt);
  dcg_panic_if_errors ();
  if (dump_parser)
    { dcg_wlog ("   Dump of packet parse tree after parsing:");
      pp_packet (stderr, pkt);
      dcg_wlog ("");
    };

  /* Save packet */
  dcg_warning (0, "   saving packet %s", basename);
  sprintf (buf, "%s.%s", basename, PREPARSE_SUFFIX);
  bf = dcg_bin_fopen (buf, "wb", 1);
  dcg_bin_save_header (bf, "ELAN", "preparsed packet", VERSION);
  save_packet (bf, pkt);
  dcg_bin_save_eof (bf);
  dcg_bin_fclose (&bf);
}

/*
   Sorting of packets by use
*/
static int lookup_packet_name (packet_list pks, string pname)
{ int ix;
  for (ix = 0; ix < pks -> size; ix++)
    if (streq (pks -> array[ix] -> pname, pname))
      return (ix);
  dcg_internal_error ("lookup_packet_name");
  return (0);
}

static char *build_use_relation (packet_list pks)
{ int size = pks -> size;
  char *rel = dcg_calloc (size * size, sizeof (char));
  int ix;

  /* Iterate over the packages */
  for (ix = 0; ix < size; ix++)
    { string_list puses = pks -> array[ix] -> puses;
      int iy;

      /* Iterate over its uses */
      for (iy = 0; iy < puses -> size; iy++)
	{ /* Mark appropriately in the use relation */
	  int index = lookup_packet_name (pks, puses -> array[iy]);
	  rel[ix * size + index] = 1;
	};
    };

  return (rel);
}

static void check_recursive_package_use (packet_list pks, char *use_closure)
{ int size = pks -> size;
  int recursive = 0;
  int had_first = 0;
  int ix;

  /* check on the diagonal of the closure */
  for (ix = 0; ix < size; ix++)
    recursive = recursive || use_closure[ix * size + ix];
  if (!recursive) return;
  
  /* Complain and be gone */
  dcg_wlog ("   packets ");
  for (ix = 0; ix < pks -> size; ix++)
    { dcg_wlog ("%s%s", (had_first)?",":"", pks -> array[ix] -> pname);
      had_first = 1;
    };
  dcg_panic (" are used recursively");
}

static int can_add_packet_to_ordered_list (int ix, int_list order, int size, char *use_closure)
{ int iy;
  for (iy = 0; iy < size; iy++)
    if (use_closure [ix * size + iy] && !is_on_int_list (order, iy))
      return (0);
  return (1);
}

static void add_packet_to_ordered_list (int_list order, int size, char *use_closure)
{ int ix;

  /* Search order is a heuristic */
  for (ix = size - 1; 0 <= ix; ix--)
    if (is_on_int_list (order, ix)) continue;
    else if (can_add_packet_to_ordered_list (ix, order, size, use_closure))
      { app_int_list (order, ix);
	return;
      };

  dcg_internal_error ("add_packet_to_ordered_list");
}

static int_list decide_compilation_order (int size, char *use_closure)
{ int_list order = init_int_list (size);
  int pass;
  for (pass = 0; pass < size; pass++)
    add_packet_to_ordered_list (order, size, use_closure);
  return (order);
}

static packet_list make_sorted_packet_list (packet_list pks, int_list order)
{ packet_list new_pks = init_packet_list (pks -> size);
  int ix;
  for (ix = 0; ix < order -> size; ix++)
    { packet p = pks -> array[order -> array[ix]];
      app_packet_list (new_pks, attach_packet (p));
    };
  return (new_pks);
}

static void report_compilation_order (packet_list pks)
{ int ix;
  if (!verbose) return;
  dcg_wlog ("   packets will be analyzed in the following order:");
  dcg_eprint ("      standard");
  for (ix = 0; ix < pks -> size; ix++)
    dcg_eprint (", %s", pks -> array[ix] -> pname);
  dcg_wlog ("");
}

static void sort_packages_by_use (packet_list *pks)
{ char *use_relation, *use_closure;
  packet_list unsorted_pks = *pks;
  int size = unsorted_pks -> size;
  packet_list sorted_pks;
  int_list order;

  dcg_warning (0, "   sorting packets by use...");
  use_relation = build_use_relation (unsorted_pks);
  use_closure = warshall (size, use_relation);
  check_recursive_package_use (unsorted_pks, use_closure);
  order = decide_compilation_order (size, use_closure);
  sorted_pks = make_sorted_packet_list (unsorted_pks, order);
  report_compilation_order (sorted_pks);

  *pks = sorted_pks;
  detach_packet_list (&unsorted_pks);
  detach_int_list (&order);
}

/*
   Insertion of used libraries
*/
static void insert_used_libraries (packet_list pks)
{ int ix, iy;
  for (ix = 0; ix < pks -> size; ix++)
    { string_list used_libs = pks -> array[ix] -> puselibs;
      for (iy = 0; iy < used_libs -> size; iy++)
	{ string used_lib = used_libs -> array[iy];
	  if (!is_on_string_list (link_library_names, used_lib))
	    ins_string_list (link_library_names, 0, used_lib);
	};
    };
}

/* set default location of X11 library path */
#ifndef X11LIBDIR
#define X11LIBDIR "/usr/X11R6/lib"
#endif
static void try_add_X11_search_path ()
{ int ix;
  for (ix = 0; ix < link_library_names -> size; ix++)
    if (streq (link_library_names -> array[ix], "X11"))
      { ins_string_list (library_search_path, 0, new_string (X11LIBDIR));
	return;
      };
}

/*
   The actual context free analysis
   as well as the loading of preparsed packets
*/
void context_free_analysis (packet_list *all_pks)
{ string_list pnames = new_string_list ();
  packet_list pks = new_packet_list ();
  packet pkt, mainp;
  int ix = 0;

  /* parse the main packet */
  dcg_warning (0, "   parsing the program...");
  locate_and_init_packet (1);
  should_be_main_packet (&mainp);
  if (dump_parser)
    { dcg_wlog ("   Dump of main packet parse tree after parsing:");
      pp_packet (stderr, mainp);
      dcg_wlog ("");
    };
  dcg_panic_if_errors ();

  /* if we're only parsing, we can stop now */
  if (parse_only) return;

  /* try and load needed packets */
  add_uses_to_pnames (mainp -> puses, pnames);
  while (ix < pnames -> size)
    { load_or_parse_packet (pnames -> array[ix], &pkt);
      add_uses_to_pnames (pkt -> puses, pnames);
      ins_packet_list (pks, 0, pkt);
      ix++;
    };

  /* Check if we have to sort the packets */
  if (pnames -> size > 1)
     sort_packages_by_use (&pks);
  insert_used_libraries (pks);
  try_add_X11_search_path (pks);
  
  /* try and load the standard prelude and insert at start of list */
  load_or_parse_packet (new_string (STANDARD_PACKET_NAME), &pkt);
  ins_packet_list (pks, 0, pkt);

  /* add main packet at end of list */
  app_packet_list (pks, mainp);
  *all_pks = pks;
};
