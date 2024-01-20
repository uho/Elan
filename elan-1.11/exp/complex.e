PACKET complex

DEFINES COMPLEX,
	complex, RE, IM,
	+, -, *, /,
	conj, abs,
	text:

  TYPE COMPLEX = STRUCT (REAL x, y);

  { Constructor and access routines }
  COMPLEX PROC complex (REAL CONST x, y): COMPLEX: [x, y] ENDPROC complex;
  REAL OP RE (COMPLEX a): CONCR a.x ENDOP RE;
  REAL OP IM (COMPLEX a): CONCR a.y ENDOP IM;

  { Basic arithmetic }
  COMPLEX PROC conj (COMPLEX a): complex (RE a, -IM a) ENDPROC;
  COMPLEX OP + (COMPLEX a, b): complex (RE a + RE b, IM a + IM b) ENDOP +;
  COMPLEX OP - (COMPLEX a, b): complex (RE a - RE b, IM a - IM b) ENDOP -;
  COMPLEX OP - (COMPLEX a): complex (-RE a, -IM a) ENDOP -;
  COMPLEX OP + (COMPLEX a): a ENDOP +;

  COMPLEX OP * (COMPLEX a, b):
    complex (RE a * RE b - IM a * IM b, RE a * IM b + IM a * RE b)
  ENDOP *;

  COMPLEX OP / (COMPLEX a, b):
    REAL quot :: RE b ** 2 + IM b ** 2;
    complex ((RE a * RE b + IM a * IM b)/quot, (IM a * RE b - RE a * IM b)/quot)
  ENDOP /;

  REAL PROC abs (COMPLEX a):
    sqrt (RE a ** 2 + IM a ** 2)
  ENDPROC abs;

  { Text conversion }
  TEXT PROC text (COMPLEX z):
    "(" + text (RE z) + ", " + text (IM z) + ")"
  ENDPROC text;

ENDPACKET complex;
