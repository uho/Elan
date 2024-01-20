TYPE TREE = STRUCT (TREE left, right, TEXT info);

PROC put (TREE t, INT depth):
  IF NOT ISNIL t
  THEN
    put (CONCR t.left, depth + 1);
    put (depth * " " + CONCR t.info + "\n");
    put (CONCR t.right, depth + 1)
  FI
ENDPROC;

PROC put (TREE t):
  put (t, 0)
ENDPROC;

PROC enter (TREE VAR t, TEXT txt):
  IF ISNIL t
  THEN t := TREE: [NIL, NIL, txt]
  ELIF txt < CONCR t.info
  THEN enter (CONCR t.left, txt)
  ELIF txt > CONCR t.info
  THEN enter (CONCR t.right, txt)
  FI
ENDPROC;

program:
   ROW 10 TREE VAR history;
   INT VAR ptr :: 1, ptr2 :: 1;
   TREE VAR root :: NIL;
   history [ptr] := root; ptr INCR 1;
   put ("Before enter petra\n");
   enter (root, "petra");
   put (root);
   history [ptr] := root; ptr INCR 1;
   put ("\nBefore enter marie\n");
   enter (root, "marie");
   put (root);
   history [ptr] := root; ptr INCR 1;
   put ("\nBefore enter sientje\n");
   enter (root, "sientje");
   put (root);
   history [ptr] := root; ptr INCR 1;
   put ("\nBefore enter roosje\n");
   enter (root, "roosje");
   put (root);
   history [ptr] := root; ptr INCR 1;
   put ("\nBefore enter nathalie\n");
   enter (root, "nathalie");
   put (root);
   history [ptr] := root; ptr INCR 1;
   put ("\nBefore enter anneke\n");
   enter (root, "anneke");
   history [ptr] := root; ptr INCR 1;
   put (root);
   put ("\nBefore enter yvette\n");
   enter (root, "yvette");
   history [ptr] := root; ptr INCR 1;
   put (root);
   FOR ptr2 UPTO ptr - 1
   REP
      put ("Iteration " + text (ptr2 - 1) + "\n");
      put (history [ptr2]);
      put ("\n")
   ENDREP.

