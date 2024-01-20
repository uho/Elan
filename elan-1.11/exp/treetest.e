
 TYPE TREE = STRUCT (TREE left, right, TEXT info);

 PROC put (TREE t):
   IF NOT ISNIL t
   THEN
     put (CONCR t.left);
     put (CONCR t.info);
     line;
     put (CONCR t.right)
   FI
 ENDPROC put;

 PROC enter (TREE VAR t, TEXT txt):
   IF ISNIL t
   THEN t := TREE: [NIL, NIL, txt]
   ELIF txt < CONCR t.info
   THEN enter (CONCR t.left, txt)
   ELIF txt > CONCR t.info
   THEN enter (CONCR t.right, txt)
   FI
 ENDPROC enter;

 program:
   UPTO 100
   REP
     TREE VAR tree :: NIL;
     TEXT VAR t :: "";
     UPTO 5
     REP
       t CAT "a";
       enter (tree, t)
     ENDREP
   ENDREP;
   put (exectime).

