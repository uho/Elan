USE graphics;

PROC callback (INT widget, INT data):
  put ("One bell system, it rings: " + text (data) + "\n")
ENDPROC;

program:
  WIDGET app shell :: create application shell ("xtest", "Xtest");
  WIDGET box    :: create managed widget ("box", app shell, box widget class);
  WIDGET cmd1 :: create managed widget ("cmd1", box, command widget class);
  WIDGET cmd2 :: create managed widget ("cmd2", box, command widget class);
  add callback (cmd1, "callback", callback, 1);
  add callback (cmd2, "callback", callback, 2);
  realize widget (app shell);
  main event loop.
