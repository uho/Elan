{|nonreentrant|}
test integers:
   put ("hello world with ints\n");
   INT VAR a :: 1, b;
   WHILE a <> 0
   REP
      put ("Enter a: ");
      get (a);
      put ("Enter b: ");
      get (b);
      put (text (a) + " + " + text (b) + " = " + text (a + b) + "\n");
      put (text (a) + " - " + text (b) + " = " + text (a - b) + "\n");
      put (text (a) + " * " + text (b) + " = " + text (a * b) + "\n");
      put (text (a) + " DIV " + text (b) + " = " + text (a DIV b) + "\n");
      put (text (a) + " MOD " + text (b) + " = " + text (a MOD b) + "\n");
      put (text (a) + " ** " + text (b) + " = " + text (a ** b) + "\n");
      put (             " - " + text (b) + " = " + text (-b)    + "\n");
      put (text (a) + " = " +  text (b) + " = " + text (a = b)  + "\n");
      put (text (a) + " <> " + text (b) + " = " + text (a <> b) + "\n");
      put (text (a) + " < " +  text (b) + " = " + text (a < b)  + "\n");
      put (text (a) + " <= " + text (b) + " = " + text (a <= b) + "\n");
      put (text (a) + " > " +  text (b) + " = " + text (a > b)  + "\n");
      put (text (a) + " >= " + text (b) + " = " + text (a >= b) + "\n")
   ENDREP.
