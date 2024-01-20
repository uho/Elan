{ tests the ex2 example }
prog:
   INT VAR a :: 1;
   INT VAR b;
   INT VAR c;
   WHILE a <> 0
   REP
      put ("Enter a: ");
      get (a);
      put ("Enter b: ");
      get (b);
      put ("Enter c: ");
      get (c);
      put ("(a+b) ** 2 + c DIV 5 - 3 = ");
      put (text ((a+b) ** 2 + c DIV 5 - 3));
      put ("\n")
   ENDREP.
