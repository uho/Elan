OP PLUSAB (REAL VAR a, REAL b): a := a + b ENDOP;

prog:
   put ("hello world with reals\n");
   REAL VAR a :: 1.4;
   REAL b :: 0.13;
   put ("a = " + text (a) +"\n");
   put ("b = " + text (b) +"\n");
   a PLUSAB b;
   put ("a = " + text (a) +"\n");
   put (text (a) + " + " + text (b) + " = " + text (a + b) + "\n");
   put (text (a) + " - " + text (b) + " = " + text (a - b) + "\n");
   put (text (a) + " * " + text (b) + " = " + text (a * b) + "\n");
   put (text (a) + " / " + text (b) + " = " + text (a / b) + "\n");
   put (text (a) + " ** 2 "         + " = " + text (a ** 2) + "\n");
   put (           " - " + text (b) + " = " + text (- b) + "\n");
   put (text (a) + " = " + text (b) + " = " + text (a = b) + "\n");
   put (text (a) + " <> " + text (b) + " = " + text (a <> b) + "\n");
   put (text (a) + " < " + text (b) + " = " + text (a < b) + "\n");
   put (text (a) + " <= " + text (b) + " = " + text (a <= b) + "\n");
   put (text (a) + " > " + text (b) + " = " + text (a > b) + "\n");
   put (text (a) + " >= " + text (b) + " = " + text (a >= b) + "\n").
