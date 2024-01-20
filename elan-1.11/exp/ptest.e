PROC proc test (INT a, PROC (INT) p, PROC (INT) q):
   IF a <> 0
   THEN p
   ELSE q
   FI (a - 1)
ENDPROC;

main:
   proc test (10, put, line).
