
 PROC showstack (ROW 3 INT s):
   page;
   INT VAR i, j;
   FOR i FROM 1 UPTO 3
   REP
     place numbers;
     place disks;
     place pilar sticks
   ENDREP.
  
   place numbers: place (i, 0, text (i, 1)).
  
   place disks:
     FOR j FROM 1 UPTO stack [i]
     REP place (i, j, disk (depth - j + 1))
     ENDREP.
  
   place pilar sticks:
     FOR j FROM stack [i] + 1 UPTO depth + 1
     REP place (i, j, nodisk (depth))
     ENDREP.
  
 ENDPROC showstack;

 PROC place (INT a, b, TEXT t):
   cursor (36 + 2 * (a - 2) * depth + a, 20 - b);
   put (t);
   line
 ENDPROC place;

 TEXT PROC nodisk (INT width):
   TEXT disk flank :: width * " ", disk move :: width * left;
   disk move + disk flank + pilar + disk flank + disk move + left
 ENDPROC nodisk;

 PROC move tower (INT height, from, to, via):
   IF height > 0
   THEN
     move tower (height - 1, from, via, to);
     move piece (height, from, to);
     move tower (height - 1, via, to, from)
   FI
 ENDPROC move tower;

 PROC move piece (INT number, from, to):
   lift piece from tower;
   move it to the other;
   drop it there.
  
   lift piece from tower:
     INT VAR i;
     TEXT n :: nodisk (number), d :: disk (number);
     TEXT down disk :: n + down + d, up disk :: n + up + d;
     FOR i FROM stack [from] UPTO depth
     REP place (from, i, up disk)
     ENDREP;
     stack [from] DECR 1.
  
   move it to the other:
     place (from, depth + 1, n);
     place (to, depth + 1, d).
  
   drop it there:
     stack [to] INCR 1;
     FOR i FROM depth + 1 DOWNTO stack [to] + 1
     REP place (to, i, down disk)
     ENDREP.
  
 ENDPROC move piece;

 TEXT PROC disk (INT width):
   INT flank :: width - 1;
   TEXT disk flank :: flank * disk elem, disk move :: flank * left;
   disk move + disk flank + disk center + disk flank + disk move + left
 ENDPROC disk;

 program:
   LET depth = 10, delay = 0;
   LET left = ""6"", up = ""8"", down = ""9"";
   LET disk elem = "#", disk center = "#", pilar = "|";
   ROW 3 INT VAR stack :: [depth, 0, 0];
   showstack (stack);
   move tower (depth, 1, 2, 3);
   line.

