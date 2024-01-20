/*
   File: egr_objects.c
   Defines a graphics library for elan

   CVS ID: "$Id: egr_objects.c,v 1.1 2007/03/03 12:10:02 marcs Exp $"
*/

/* include config.h if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* global includes */
#include <stdio.h>
#include <string.h>

/* liberts includes */
#include <rts_error.h>

/* Check whether to include malloc or stdlib.h */
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#else
#include <stdlib.h>
#endif

/* X11 includes */
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

/* local includes */
#include "elan_graphics.h"
#include "egr_objects.h"

/*
   Setup an administration for graphic objects
*/
struct graph_object_rec
	{ egr_object_tag tag;
	  union object_union
	    { XtAppContext context;
	      Display *display;
	      Widget widget;
	      char *c_str;
	      long lval;
	      int ival;
	      struct 
		{ elan_callback callback;
		  int widget_index;
		  int client_data;
		} cback;
	    } u;
	}; 

typedef struct graph_adm_rec
	{ int room;
	  int size;
	  struct graph_object_rec *array;
	} *graph_adm;

/*
   Initialization of the admin
*/
#define INITIAL_ALLOC 256
static graph_adm my_admin = NULL;
void egr_init_graphics ()
	{ struct graph_object_rec *array = (struct graph_object_rec *)
			    calloc (INITIAL_ALLOC, sizeof (struct graph_object_rec));
	  my_admin = (struct graph_adm_rec *) malloc (sizeof (struct graph_adm_rec));
	  my_admin -> room = INITIAL_ALLOC;
	  my_admin -> size = 0;
	  my_admin -> array = array;
	};

/*
   Check if something was allocated
*/
int egr_context_created ()
	{ if (my_admin == NULL) return (0);
	  return (my_admin -> size);
	};

/*
   Check if we have been called by the graphics packet body
*/
static void check_init_graphics ()
	{ if (my_admin == NULL)
	    rts_panic ("graphical object accessed without initializing the graphics packet");
	  if (!my_admin -> size)
	    rts_panic ("graphical object accessed without creating the root widget first");
	};

/*
   Allocate a slot for a new object
*/
static int new_admin_slot ()
	{ /* Check if we have not yet been initialized properly */
	  int ix;
	  check_init_graphics ();

	  /* Search for a free slot */
	  for (ix = 0; ix < my_admin -> size; ix++)
	    if (my_admin -> array[ix].tag == EGR_FREE_OBJECT)
	      return (ix);
	  
	  /* If all slots used, we have to reallocate */
	  if (my_admin -> size == my_admin -> room)
	    { my_admin -> room *= 2;
	      my_admin -> array = (struct graph_object_rec *) realloc (my_admin -> array,
					my_admin -> room * sizeof (struct graph_object_rec));
	    }; 

	  /* Return the index */
	  my_admin -> size++;
	  return (my_admin -> size - 1);
	};

/*
   Index and tag Verification
*/
int egr_test_index (int index)
	{ check_init_graphics ();
	  return ((0 <= index) && (index < my_admin -> size));
	};

void egr_verify_index (int index, char *elan_routine, char *error_msg)
	{ if (egr_test_index (index)) return;
	  rts_panic ("%s: %s", elan_routine, error_msg);
	}

int egr_test_tag (int index, egr_object_tag tag)
	{ check_init_graphics ();
	  return (my_admin -> array[index].tag == tag);
	};

void egr_verify_tag (int index, egr_object_tag tag, char *elan_routine, char *error_msg)
	{ if (egr_test_tag (index, tag)) return;
	  rts_panic ("%s: %s", elan_routine, error_msg);
	};

/*
   Allocation routines
*/
int egr_new_context (XtAppContext context)
	{ /* We are the only one that does not check for the graphics initialization */
	  if (egr_context_created ())
	    rts_panic ("Can not create application context twice");

	  /* Fill in the arguments and mark the first one */
	  my_admin -> array[0].tag = EGR_CONTEXT;
	  my_admin -> array[0].u.context = context;
	  my_admin -> size = 1;
	  return (0);
	}

int egr_new_display (Display *display)
	{ int new_index = new_admin_slot ();
	  my_admin -> array[new_index].tag = EGR_DISPLAY;
	  my_admin -> array[new_index].u.display = display;
	  return (new_index);
	};

int egr_new_widget (Widget widget)
	{ int new_index = new_admin_slot ();
	  my_admin -> array[new_index].tag = EGR_WIDGET;
	  my_admin -> array[new_index].u.widget = widget;
	  return (new_index);
	};

int egr_new_callback (elan_callback callback, int widget_index, int client_data)
	{ int new_index = new_admin_slot ();
	  my_admin -> array[new_index].tag = EGR_CALLBACK;
	  my_admin -> array[new_index].u.cback.callback = callback;
	  my_admin -> array[new_index].u.cback.widget_index = widget_index;
	  my_admin -> array[new_index].u.cback.client_data = client_data;
	  return (new_index);
	};

/*
   Access routines
*/
XtAppContext egr_context ()
	{ check_init_graphics ();
	  return (my_admin -> array[0].u.context);
	};

Display *egr_root_display ()
	{ check_init_graphics ();
	  return (my_admin -> array[1].u.display);
	};

Widget egr_widget_from_index (int index)
	{ check_init_graphics ();
	  return (my_admin -> array[index].u.widget);
	};

void egr_get_callback_data (int index, elan_callback *callback, int *widget_index, int *client_data)
	{ check_init_graphics ();
	  *callback = my_admin -> array[index].u.cback.callback;
	  *widget_index = my_admin -> array[index].u.cback.widget_index;
	  *client_data = my_admin -> array[index].u.cback.client_data;
	};
