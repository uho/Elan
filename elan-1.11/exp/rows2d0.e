progr:
   LET n = 5;
   ROW n ROW n TEXT VAR mat;
   INT VAR i,j;
   FOR j UPTO n
   REP
      FOR i UPTO n
      REP mat[i][j] := text (i * j, 3)
      ENDREP
   ENDREP;

   FOR j UPTO n
   REP
      FOR i UPTO n
      REP put (mat[i][j])
      ENDREP;
      put ("\n")
   ENDREP.
