ottt:
  TEXT VAR a :: "a", b :: "b", c :: "";
  put ("abcdef SUB 2 = " + ("abcdef" SUB 2) + "\n");
  put ("abcdef SUB 0 = " + ("abcdef" SUB 0) + "\n");
  put ("empty SUB 1 = '" + ("" SUB 1) + "'\n");
  put ("5 * abc = '" + 5 * "abc" + "'\n");
  put ("subtext (12345,3,6) = " + subtext ("12345", 3, 6) + "\n");
  WHILE length (a) < 60
  REP
     c := a;
     a CAT b;
     b := c;
     put ("fib is " + a + "\n")
  ENDREP.
