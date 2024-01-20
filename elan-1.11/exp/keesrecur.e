
 PROC p (INT i):
   IF i MOD 100 = 0
   THEN put (".")
   FI;
   p (i + 1)
 ENDPROC p;

 program: p (0).

