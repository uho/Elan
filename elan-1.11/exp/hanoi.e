LET delta = 2;

PROC down:
  indentation := indentation - delta
ENDPROC down;

PROC indent:
  line;
  spaces (indentation)
ENDPROC indent;

PROC move piece ( INT number, TEXT from, TEXT to ):
  indent;
  put ("Move piece from ");
  put (from);
  put (" to ");
  put (to)
ENDPROC move piece;

PROC move tower ( INT height, TEXT from, TEXT to, TEXT via ):
  IF height > 0
  THEN
    up;
    move tower (height - 1, from, via, to);
    move piece (height, from, to);
    move tower (height - 1, via, to, from);
    down
  FI
ENDPROC move tower;

PROC spaces ( INT n ):
  put (n * " ")
ENDPROC spaces;

PROC up:
  indentation := indentation + delta
ENDPROC up;

program:
  INT VAR indentation :: 0;
  move tower (5, "bronze", "golden", "silver");
  line;
  line.
