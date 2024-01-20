tt:
  INT VAR i;
  ROW 4 INT VAR arr1, arr2;
  FOR i UPTO 4
  REP
     IF i = 2
     THEN arr1
     ELSE arr2
     FI [i] := 0
  ENDREP;
  put ("filled with zeroes\n").
