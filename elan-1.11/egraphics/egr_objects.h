/*
   File: egr_objects.h
   Provides storage admin for graphic objects

   CVS ID: "$Id: egr_objects.h,v 1.1 2007/03/03 12:10:02 marcs Exp $"
*/
#ifndef IncEgrObjects
#define IncEgrObjects

/* X11 includes */
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

/* Local includes */
#include "elan_graphics.h"

/* Initialization */
int egr_context_created ();
void egr_init_graphics ();

/* Object tags */
typedef enum
	{ EGR_FREE_OBJECT,
	  EGR_CONTEXT,
	  EGR_DISPLAY,
	  EGR_WIDGET,
	  EGR_STRING,
	  EGR_LONG,
	  EGR_INT,
	  EGR_CALLBACK
	} egr_object_tag;

/* Index and tag Verification */
int  egr_test_index   (int index);
void egr_verify_index (int index, char *elan_routine, char *error_msg);
int  egr_test_tag     (int index, egr_object_tag tag);
void egr_verify_tag   (int index, egr_object_tag tag, char *elan_routine, char *error_msg);

/* Allocation routines */
int egr_new_context (XtAppContext context);
int egr_new_display (Display *display);
int egr_new_widget  (Widget widget);
int egr_new_callback (elan_callback callback, int widget_index, int client_data);

/* Access Routines */
XtAppContext egr_context ();
Display *egr_root_display ();
Widget egr_widget_from_index (int index);
void egr_get_callback_data (int index, elan_callback *callback,
			    int *widget_index, int *client_data);

#endif /* IncEgrObjects */
