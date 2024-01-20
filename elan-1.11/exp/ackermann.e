INT PROC ackermann (INT p, INT q):
  assert (p >= 0 AND q >= 0);
  IF p = 0
  THEN 2 * q
  ELIF q = 0
  THEN 0
  ELIF q = 1
  THEN 2
  ELSE ackermann (p - 1, ackermann (p, q - 1))
  FI
ENDPROC ackermann;

INT PROC ackermann (INT p, INT q, INT depth):
  assert (p >= 0 AND q >= 0);
  put (p);
  put (q);
  put (depth);
  line;
  IF p = 0
  THEN 2 * q
  ELIF q = 0
  THEN 0
  ELIF q = 1
  THEN 2
  ELSE ackermann (p - 1, ackermann (p, q - 1, depth + 1), depth + 1)
  FI
ENDPROC ackermann;

program:
  put (ackermann (4, 3, 1)).
