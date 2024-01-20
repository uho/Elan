tt:
  INT VAR i,j;
  ROW 4 ROW 4 TEXT VAR arr;
  FOR i UPTO 4
  REP
    FOR j UPTO 4
    REP
      put ("filling array [" + text (i) + "][" + text (j) + "]\n");
      arr [i][j] := text (i + j)
    ENDREP
  ENDREP;
  FOR i UPTO 4
  REP
    FOR j UPTO 4
    REP
      put ("array [" + text (i) + "][" + text (j) + "] = " + arr[i][j] + "\n")
    ENDREP
  ENDREP.
