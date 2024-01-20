PACKET elan0

DEFINES { Elan0 files }	new file, old file, close file, file ended,
			write, write line, read, read line,
	{ Elan0 random}	choose128:

{---------------------------------------------------------- ELAN0 File io }
PROC new file (TEXT f):
  elan0 output file := sequential file (output, f);
  assert (opened (elan0 output file), "can't open file: " + f)
ENDPROC;

PROC old file (TEXT f):
  elan0 input file := sequential file (input, f);
  assert (opened (elan0 input file), "can't open file: " + f)
ENDPROC;

PROC close file:
  close (elan0 input file);
  close (elan0 output file)
ENDPROC;

BOOL PROC file ended:
  eof (elan0 input file)
ENDPROC;

PROC write (TEXT t):
  put (elan0 output file, t)
ENDPROC;

PROC write (INT i):
  put (elan0 output file, i)
ENDPROC;

PROC write (REAL r):
  put (elan0 output file, r)
ENDPROC;

PROC write line (TEXT t):
  put line (elan0 output file, t)
ENDPROC;

PROC write line:
  line (elan0 output file)
ENDPROC;

PROC read (INT VAR i):
  get (elan0 input file, i)
ENDPROC;

PROC read (REAL VAR r):
  get (elan0 input file, r)
ENDPROC;

PROC read (TEXT VAR t):
  get (elan0 input file, t)
ENDPROC;

PROC read line (TEXT VAR t):
  get line (elan0 input file, t)
ENDPROC;

{---------------------------------------------------------- ELAN0 Random }
INT PROC choose128:
  random (0, 127)
ENDPROC choose128;

{---------------------------------------------------------- Initialization }
elan0 initialization:
  FILE VAR elan0 input file, elan0 output file.

ENDPACKET elan0;
