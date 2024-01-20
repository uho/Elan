test2:
   TEXT aap :: "aap";
   TEXT noot :: "noot";
   TEXT VAR alles;
   put ("geef alles: ");
   get (alles);
   put ("alles is " + alles + "\n");
   alles := aap + noot;
   put (alles + "\n").
