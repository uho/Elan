tt:
  INT VAR i;
  ROW 4 INT VAR arr;
  FOR i UPTO 4
  REP
     put ("filling array index " + text (i) + "\n");
     arr [i] := i
  ENDREP;
  FOR i UPTO 4
  REP
     put ("array [" + text (i) + "] = " + text (arr[i]) + "\n")
  ENDREP.
