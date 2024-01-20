PACKET mandelbrot
DEFINES iterate:

USES complex;

LET max iterations = 256;

INT PROC iterate (COMPLEX c):
   INT VAR nr iterations :: 0;
   COMPLEX VAR z :: complex (0.0, 0.0);
   REP
      nr iterations INCR 1;
      z := z * z + c
   UNTIL abs (z) > 2.0 OR nr iterations = max iterations
   ENDREP;
   nr iterations
ENDPROC;

ENDPACKET mandelbrot;
