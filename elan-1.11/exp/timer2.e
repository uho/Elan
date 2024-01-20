
INT PROC plus (INT CONST a, b):
  a + b
ENDPROC plus;

program:
  INT VAR i, j;
  FOR i UPTO 10
  REP j := plus (i, 1);
      put (j)
  ENDREP.
