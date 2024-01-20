wazig:
   TEXT VAR blup;
   TEXT VAR blop;
   INT VAR bort;
   BOOL VAR puk;
   put ("hello world\n");
   get (blup);
   put (blup);
   blop := blup;
   put ("\n");
   put (blop);
   put ("\n");
   put ("give number:\n");
   WHILE NOT TRUE REP ENDREP;
   get (bort);
   put ("bort is ");
   put (text (bort));
   put ("\n");
   puk := bort = 5;
   put ("puk is ");
   put (text (puk));
   put ("\n");
   WHILE NOT TRUE REP ENDREP.
