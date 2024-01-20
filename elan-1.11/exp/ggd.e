INT PROC ggd (INT a, b):
   IF a > b
   THEN ggd (b, a)
   ELIF a = 0
   THEN b
   ELSE ggd (b MOD a, a)
   FI
ENDPROC ggd;

program:
   put (ggd (567, 345)).
