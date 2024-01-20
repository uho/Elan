/*
   File: elan_graphics.c
   Defines a graphics library for elan

   CVS ID: "$Id: elan_graphics.c,v 1.4 2007/03/29 19:30:48 marcs Exp $"
*/

/* include config.h if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* global includes */
#include <stdio.h>
#include <string.h>

/* Check whether to include malloc or stdlib.h */
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#else
#include <stdlib.h>
#endif

/* liberts includes */
#include <rts_error.h>
#include <rts_alloc.h>
#include <rts_texts.h>

/* X11 includes */
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Simple.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#define DefaultDisplayName (char *)0

/* local includes */
#include "egr_objects.h"
#include "elan_graphics.h"
#include "Primitive.h"

/* Administration of widget classes */
enum widget_class_id
	{ EWC_SIMPLE,
	  EWC_LABEL,
	  EWC_COMMAND,
	  EWC_PRIMITIVE,
	  EWC_BOX,
	  EWC_FORM,
	  EWC_APPLICATION_SHELL,
	};
static WidgetClass *class_array[] =
	{ &simpleWidgetClass,
	  &labelWidgetClass,
	  &commandWidgetClass,
	  &primitiveWidgetClass,
	  &boxWidgetClass,
	  &formWidgetClass,
	  &applicationShellWidgetClass,
	};
#define NrWidgetClasses (sizeof(class_array)/sizeof (WidgetClass *))

int egr_simple_widget_class ()		  { return (EWC_SIMPLE); };
int egr_label_widget_class ()		  { return (EWC_LABEL); };
int egr_command_widget_class ()		  { return (EWC_COMMAND); };
int egr_primitive_widget_class ()	  { return (EWC_PRIMITIVE); };
int egr_box_widget_class ()		  { return (EWC_BOX); };
int egr_form_widget_class ()		  { return (EWC_FORM); };
int egr_application_shell_widget_class () { return (EWC_APPLICATION_SHELL); };

/*
   Allocate an application shell
*/
int egr_create_application_shell (char *name, char *class)
	{ Display *display;
	  Widget shell;
	  if (!egr_context_created ())
	    { /* graphics admin is like a virgin */
	      XtAppContext context;
	      int nul = 0;

	      /* Initialize the X toolkit, create application context and open display */
	      XtToolkitInitialize ();
	      context = XtCreateApplicationContext ();
	      display = XtOpenDisplay (context, DefaultDisplayName, name, class,
				       NULL, 0, &nul, NULL);
	      if (display == NULL)
		rts_panic ("Could not open default X Display");

	      /* Fill in admin */
	      (void) egr_new_context (context);
	      (void) egr_new_display (display);
	    }
	  else display = egr_root_display ();

	  /* Get a slot and fill it with an application shell Widget */
	  shell = XtAppCreateShell (name, class, applicationShellWidgetClass, display, NULL, 0);
	  return (egr_new_widget (shell));
	};

/*
   Construct an XtArgList from an elan_arg_list
*/
#define elan_args_list_null ((elan_arg_list) NULL)
#define elan_args_list_nil ((elan_arg_list) (1L))
static int make_xt_argument_list (elan_arg_list args, ArgList *ret_args)
	{ /* Determine argument validity */
	  elan_arg_list my_args = args;
	  ArgList xt_args;
	  int len = 0;
	  int idx = 0;

	  if (my_args == elan_args_list_null) return (-1);

	  /* Determine length of argument list */
	  while (my_args != elan_args_list_nil)
	    { my_args = my_args -> rest;
	      len++;
	    };

	  /* Allocate structure and fill it */
	  xt_args = (ArgList) XtCalloc ((Cardinal) len, sizeof (Arg));
	  for (my_args = args, idx = 0; idx < len; my_args = my_args -> rest, idx++)
	    XtSetArg (xt_args[idx], my_args -> arg -> name,
				    ((XtArgVal) my_args -> arg -> value));

	  /* Done */
	  *ret_args = xt_args;
	  return (len);
	};

/*
   Create a managed widget under a father
*/
#define proc_create_managed_widget "PROC create managed widget (TEXT name, WIDGET father,...)"
static void verify_father_widget_and_class (int father, int class, Widget *fw)
	{ /* Verify our arguments' validity */
	  egr_verify_index (father, proc_create_managed_widget, "illegal father argument");
	  egr_verify_tag (father, EGR_WIDGET, proc_create_managed_widget, "non widget father");

	  /* Check widget class */
	  if ((class < 0) || (class >= NrWidgetClasses))
	    rts_panic ("%s: %s", proc_create_managed_widget, "illegal widget class argument");
	  *fw = egr_widget_from_index (father);
	  if (!XtIsSubclass (*fw, compositeWidgetClass))
	    rts_panic ("%s: %s", proc_create_managed_widget, "non composite father widget");
	};


int egr_create_managed_widget (char *name, int father, int class)
	{ Widget fw, nw;
	  verify_father_widget_and_class (father, class, &fw);

	  /* All checked, act */
	  nw = XtCreateManagedWidget (name, *(class_array[class]), fw, NULL, 0);
	  return (egr_new_widget (nw));
	};

int egr_create_managed_widget_with_args (char *name, int father, int class, elan_arg_list args)
	{ Widget fw, nw;
	  ArgList xt_args;
	  int nr_args;
	  verify_father_widget_and_class (father, class, &fw);

	  /* Collect the argument list */
	  nr_args = make_xt_argument_list (args, &xt_args);
	  if (nr_args < 0)
	    rts_panic ("%s: %s", proc_create_managed_widget, "undefined argument list");

	  /* All checked, act */
	  nw = XtCreateManagedWidget (name, *(class_array[class]), fw, xt_args, nr_args);
	  XtFree ((char *) xt_args);
	  return (egr_new_widget (nw));
	};

/*
   Handling of Callbacks
*/
#define proc_my_callback "INTERNAL my_callback"
static void my_callback (Widget w, XtPointer client_data, XtPointer call_data)
	{ /* Intentional double cast */
	  int cback_index = (int) (long) client_data;
	  elan_callback callback;
	  int widget_index;
	  int client_index;

	  /* Verify the arguments' validity */
	  egr_verify_index (cback_index, proc_my_callback, "Inconsistent callback administration");
	  egr_verify_tag (cback_index, EGR_CALLBACK, proc_my_callback,
			  "Inconsistent callback administration");
	  egr_get_callback_data (cback_index, &callback, &widget_index, &client_index);
	  egr_verify_index (widget_index, proc_my_callback, "Inconsistent callback administration");
	  egr_verify_tag (widget_index, EGR_WIDGET, proc_my_callback,
			  "Inconsistent callback administration");
	  if (egr_widget_from_index (widget_index) != w)
	    rts_panic ("Inconsistent callback administration");
	  callback (widget_index, client_index);
	};

#define proc_add_callback "PROC add callback (WIDGET widget,...)"
void egr_add_callback (int widget, char *name, elan_callback callback, int client_data)
	{ /* Verify the arguments' validity */
	  int new_index;
	  Widget ow;
	  egr_verify_index (widget, proc_add_callback, "argument is invalid");
	  egr_verify_tag (widget, EGR_WIDGET, proc_add_callback, "argument is not a widget");

	  /* Now register the callback */
	  ow = egr_widget_from_index (widget);
	  new_index = egr_new_callback (callback, widget, client_data);
	  XtAddCallback (ow, name, my_callback, (XtPointer) (long) new_index);
	};

#define proc_realize_widget "PROC realize widget (Widget widget)"
void egr_realize_widget (int widget)
	{ egr_verify_index (widget, proc_realize_widget, "argument is invalid");
	  egr_verify_tag (widget, EGR_WIDGET, proc_realize_widget, "argument is not a widget");
	  XtRealizeWidget (egr_widget_from_index (widget));
	};

void egr_main_event_loop ()
	{ if (!egr_context_created ())
	    rts_panic ("PROC main event loop: uninitialized graphics packet");
	  XtAppMainLoop (egr_context ());
	};
