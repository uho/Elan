/*
   File: elan_graphics.h
   Provides basic graphics

   CVS ID: "$Id: elan_graphics.h,v 1.3 2007/03/03 12:10:02 marcs Exp $"
*/
#ifndef IncElanGraphics
#define IncElanGraphics

/* Providing the widget class abstraction */
int egr_simple_widget_class ();
int egr_label_widget_class ();
int egr_command_widget_class ();
int egr_box_widget_class ();
int egr_form_widget_class ();
int egr_application_shell_widget_class ();

/* define the types for callbacks and argument lists */
typedef void (*elan_callback)(int, int);
typedef struct elan_arg_rec
	{ char *name;
	  int value;
	} *elan_arg;
typedef struct elan_arg_list_rec
	{ elan_arg arg;
	  struct elan_arg_list_rec *rest;
	} *elan_arg_list;

/* Xt Convenience routines */
int egr_create_managed_widget (char *name, int father, int class);
int egr_create_managed_widget_with_args (char *name, int father, int class, elan_arg_list args);
int egr_create_application_shell (char *name, char *class);
int egr_create_command_button (char *name, int father);
void egr_add_callback (int widget, char *name, void (*callback)(int,int), int client_data);
void egr_realize_widget (int widget);
void egr_main_event_loop ();
 
#endif /* IncElanGraphics */
