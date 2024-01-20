 { Demonstration of Conway's life in ELAN by Bert Windau }
 { The program is also a good test of data structures in ELAN }

 TYPE LIJST = STRUCT (INT x, y, LIJST rest);

 PROC push (LIJST VAR stack, INT x, y): stack := LIJST: [x, y, stack]
 ENDPROC push;

 PROC pop (LIJST VAR stack, INT VAR x, y):
   x := CONCR stack.x;
   y := CONCR stack.y;
   stack := CONCR stack.rest
 ENDPROC pop;

 BOOL PROC is not empty (LIJST stack): NOT ISNIL stack
 ENDPROC is not empty;

 PROC determ (INT x, y):
   buffer [y] [x].op stack := FALSE;
   IF IF buffer [y] [x].levend
   THEN buffer [y] [x].buren < 2 OR buffer [y] [x].buren > 3
   ELSE buffer [y] [x].buren = 3
   FI
   THEN push (stack2, x, y)
   FI
 ENDPROC determ;

 PROC change (INT x, y):
   INT VAR xx, yy, v;
   cursor (x, y);
   IF buffer [y] [x].levend
   THEN
     put (" ");
     v := - 1;
     buffer [y] [x].levend := FALSE
   ELSE
     put ("*");
     v := + 1;
     buffer [y] [x].levend := TRUE
   FI;
   FOR yy FROM y - 1 UPTO y + 1
   REP
     FOR xx FROM x - 1 UPTO x + 1
     REP
       IF NOT (xx < 1 OR xx > breedte OR yy < 1 OR yy > lengte OR x = xx AND y = yy)
       THEN
         buffer [yy] [xx].buren := buffer [yy] [xx].buren + v;
         IF NOT buffer [yy] [xx].op stack
         THEN
           buffer [yy] [xx].op stack := TRUE;
           push (stack1, xx, yy)
         FI
       FI
     ENDREP
   ENDREP
 ENDPROC change;

 program:
   definieer het lifeveld;
   definieer de eerste generatie;
   bereken de volgende generaties.

 definieer het lifeveld:
   LET lengte = 20;
   LET breedte = 75;
   LIJST VAR stack1 :: NIL, stack2 :: NIL;
   ROW lengte ROW breedte STRUCT (BOOL levend, op stack, INT buren) VAR buffer;
   INT VAR x, y;
   FOR y UPTO lengte
   REP
     FOR x UPTO breedte
     REP
       buffer [y] [x].levend := FALSE;
       buffer [y] [x].op stack := FALSE;
       buffer [y] [x].buren := 0
     ENDREP
   ENDREP;
   page.

 definieer de eerste generatie:
   FOR y FROM 2 UPTO lengte - 1
   REP
     FOR x FROM 2 UPTO breedte - 1
     REP
       IF x > y + lengte
       THEN push (stack2, x, y)
       FI
     ENDREP
   ENDREP.

 bereken de volgende generaties:
   INT VAR generatie;
   FOR generatie
   REP
     cursor (1, lengte + 1);
     put ("generatie: ");
     put (generatie);
     WHILE is not empty (stack1)
     REP
       pop (stack1, x, y);
       determ (x, y)
     ENDREP;
     WHILE is not empty (stack2)
     REP
       pop (stack2, x, y);
       change (x, y)
     ENDREP
   UNTIL NOT is not empty (stack1)
   ENDREP.

