kwadrateren:
   WHILE er is invoer
   REP
      kwadrateer invoer;
      druk kwadraat af
   ENDREP.

er is invoer:
   INT VAR invoer;
   put ("Geef een getal: ");
   get (invoer);
   invoer <> 0.

kwadrateer invoer:
   INT kwadraat :: invoer * invoer.

druk kwadraat af:
   put ("Het kwadraat van ");
   put (text (invoer));
   put (" is ");
   put (text (kwadraat));
   put ("\n").
