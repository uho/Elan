faculteit:
   INT VAR i;
   put ("give number:\n");
   get (i);
   INT VAR fac :: 1;
   INT VAR counter1 :: 1;
   WHILE counter1 <= i
   REP
      INT VAR sum :: 0, counter2 :: 0;
      WHILE counter2 < counter1
      REP
	 sum := sum + fac;
	 counter2 := counter2 + 1
      ENDREP;
      fac := sum;
      counter1 := counter1 + 1
   ENDREP;
   put ("fac (" + text (i) + ") = " + text (fac) + "\n").

