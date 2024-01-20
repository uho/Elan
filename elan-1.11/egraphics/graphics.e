{ -------------- ELAN graphics library -------------- }
PACKET graphics

  DEFINES WIDGET,
	  WIDGET_CLASS,
	  ARG,
	  ARG_LIST,
	  xtn orientation,
	  orient horizontal,
	  orient vertical,
	  simple widget class,
	  label widget class,
	  command widget class,
	  primitive widget class,
	  box widget class,
	  application shell widget class,
	  create application shell,
	  create managed widget,
	  add callback,
	  realize widget,
	  main event loop:

  USES_LIBRARY "egraphics";
  USES_LIBRARY "Xaw";
  USES_LIBRARY "Xpm";
  USES_LIBRARY "Xmu";
  USES_LIBRARY "Xt";
  USES_LIBRARY "X11";
  USES_LIBRARY "Xext";
  USES_LIBRARY "SM";
  USES_LIBRARY "ICE";

  { Introduce types for abstraction }
  TYPE WIDGET = INT;
  TYPE WIDGET_CLASS = INT;
  TYPE ARG = STRUCT (TEXT name, INT value);
  TYPE ARG_LIST = STRUCT (ARG arg, ARG_LIST rest);

  PROC add to argument list (ARG arg, ARG_LIST VAR arg list):
    arg list := ARG_LIST: [ arg, arg list ]
  ENDPROC add to argument list;

  { Abstract the widget classes through a proc interface }
  WIDGET_CLASS PROC simple widget class:  	EXTERNAL "egr_simple_widget_class"	ENDPROC;
  WIDGET_CLASS PROC label widget class:   	EXTERNAL "egr_label_widget_class"	ENDPROC;
  WIDGET_CLASS PROC command widget class:	EXTERNAL "egr_command_widget_class"	ENDPROC;
  WIDGET_CLASS PROC primitive widget class:	EXTERNAL "egr_primitive_widget_class"	ENDPROC;
  WIDGET_CLASS PROC box widget class:     	EXTERNAL "egr_box_widget_class"		ENDPROC;
  WIDGET_CLASS PROC form widget class:		EXTERNAL "egt_form_widget_class"	ENDPROC;

  WIDGET_CLASS PROC application shell widget class:
    EXTERNAL "egr_application_shell_widget_class"
  ENDPROC;

  WIDGET PROC create application shell (TEXT name, TEXT class):
    EXTERNAL "egr_create_application_shell" 
  ENDPROC create application shell;

  WIDGET PROC create managed widget (TEXT name, WIDGET father, WIDGET_CLASS class):
    EXTERNAL "egr_create_managed_widget"
  ENDPROC create managed widget;

  WIDGET PROC create managed widget (TEXT name, WIDGET father, WIDGET_CLASS class, ARG_LIST args):
    EXTERNAL "egr_create_managed_widget_with_args"
  ENDPROC create managed widget;

  PROC add callback (WIDGET w, TEXT name, PROC (INT, INT) callback, INT client data):
    EXTERNAL "egr_add_callback"
  ENDPROC add callback;

  PROC realize widget (WIDGET widget):
    EXTERNAL "egr_realize_widget"
  ENDPROC realize widget;

  PROC main event loop:
    EXTERNAL "egr_main_event_loop"
  ENDPROC main event loop;

  { Resource, ResourceClass names }
  LET xtn orientation = "orientation";
  LET orient horizontal = 0;
  LET orient vertical = 1;

  { Not exported but necessary for initialization }
  PROC init graphics:
    EXTERNAL "egr_init_graphics"
  ENDPROC init graphics;

  graphics initialization:
    init graphics.   

ENDPACKET graphics;
