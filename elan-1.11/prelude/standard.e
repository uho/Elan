PACKET standard 

DEFINES { General }	NILTYPE, ISNIL, isnil,
	{ INT }		maxint, +, -, *, DIV, MOD, **, INCR, DECR, 
			=, <>, <, <=, >, >=,
			SIGN, sign, ABS, abs, min, max, even, odd,
	{ REAL }	pi, e, maxreal, smallreal, /, 
	{ libm }	sqrt, ln, exp, sin, cos, tan,
			arcsin, arccos, arctan, log2, log10,
	{ BOOL }	true, false, NOT, AND, OR, XOR,
	{ TEXTS }	digits, niltext, blank, quote, line sep,	{ line sep? }
			LENGTH, length, SUB, pos, subtext,
			ascii, code, HEAD, TAIL, CAT,
			text, replace, change, change all, compress,
	{ Conversions } real, trunc, round, int,
	{ Files }	TRANSPUTDIRECTION, FILE,
			input, output, from command, to command,
			sequential file, close, erase, opened, eof,
			put, put line, get, get line, line,
	{ Screen IO }	screen width, screen length, get cursor, cursor,
			page, beep, flush, inchar, incharety,
	{ Edit }	edit,
	{ Coded IO }	yes, no,
	{ Random }	initialize random, random,
	{ System IF }	argument count, argument, system,
			exectime, stop, error stop, sleep,
			assert, system ok, system assert:

{---------------------------------------------------------- General }
LET NILTYPE = INTERNAL "niltype";
BOOL OP ISNIL (NILTYPE a):   INTERNAL "is_nil" ENDOP;
BOOL PROC isnil (NILTYPE a): INTERNAL "is_nil" ENDPROC;

{---------------------------------------------------------- INT }
INT PROC maxint: EXTERNAL "rts_maxint" ENDPROC;

INT OP + (INT a):     INTERNAL "copy_int" ENDOP;
INT OP - (INT a):     INTERNAL "negate_int" ENDOP;

INT OP + (INT a,b):   INTERNAL "int_plus_int" ENDOP;
INT OP - (INT a,b):   INTERNAL "int_minus_int" ENDOP;
INT OP * (INT a,b):   INTERNAL "int_times_int" ENDOP;
INT OP DIV (INT a,b): INTERNAL "int_div_int" ENDOP;
INT OP MOD (INT a,b): INTERNAL "int_mod_int" ENDOP;
INT OP ** (INT a, b): INTERNAL "int_pow_int" ENDOP;

OP INCR (INT VAR a, INT b): a := a + b ENDOP;
OP DECR (INT VAR a, INT b): a := a - b ENDOP;

BOOL OP =  (INT a,b): INTERNAL "int_equal_int" ENDOP;
BOOL OP <> (INT a,b): INTERNAL "int_notequal_int" ENDOP;
BOOL OP <  (INT a,b): INTERNAL "int_less_than_int" ENDOP;
BOOL OP <= (INT a,b): INTERNAL "int_less_equal_int" ENDOP;
BOOL OP >  (INT a,b): INTERNAL "int_greater_than_int" ENDOP;
BOOL OP >= (INT a,b): INTERNAL "int_greater_equal_int" ENDOP;

{---------------------------------------------------------- INT Coded }
INT OP SIGN (INT CONST n):
  IF n > 0
  THEN 1
  ELIF n = 0
  THEN 0
  ELSE - 1
  FI
ENDOP SIGN;

INT PROC sign (INT CONST n):
  SIGN n
ENDPROC sign;

INT OP ABS (INT CONST a):
  IF a >= 0
  THEN a
  ELSE - a
  FI
ENDOP ABS;

INT PROC abs (INT CONST a):
  ABS a
ENDPROC abs;

INT PROC min (INT CONST a, b):
  IF a > b
  THEN b
  ELSE a
  FI
ENDPROC min;

INT PROC max (INT CONST a, b):
  IF a > b
  THEN a
  ELSE b
  FI
ENDPROC max;

BOOL PROC even (INT CONST n): n MOD 2 = 0 ENDPROC;
BOOL PROC odd (INT CONST n):  n MOD 2 = 1 ENDPROC;

{---------------------------------------------------------- REAL }
LET pi = 3.141592653589793;
LET e = 2.71828182845904;

REAL PROC maxreal:   EXTERNAL "rts_maxreal" ENDPROC;
REAL PROC smallreal: EXTERNAL "rts_smallreal" ENDPROC;

REAL OP + (REAL a):   INTERNAL "copy_real" ENDOP;
REAL OP - (REAL a):   INTERNAL "negate_real" ENDOP;

REAL OP + (REAL a,b): INTERNAL "real_plus_real" ENDOP;
REAL OP - (REAL a,b): INTERNAL "real_minus_real" ENDOP;
REAL OP * (REAL a,b): INTERNAL "real_times_real" ENDOP;
REAL OP / (REAL a,b): INTERNAL "real_divide_real" ENDOP;
REAL OP ** (REAL a, INT b): INTERNAL "real_pow_int" ENDOP;

OP INCR (REAL VAR a, REAL b): a := a + b ENDOP;
OP DECR (REAL VAR a, REAL b): a := a - b ENDOP;

BOOL OP =  (REAL a,b): INTERNAL "real_equal_real" ENDOP;
BOOL OP <> (REAL a,b): INTERNAL "real_notequal_real" ENDOP;
BOOL OP <  (REAL a,b): INTERNAL "real_less_than_real" ENDOP;
BOOL OP <= (REAL a,b): INTERNAL "real_less_equal_real" ENDOP;
BOOL OP >  (REAL a,b): INTERNAL "real_greater_than_real" ENDOP;
BOOL OP >= (REAL a,b): INTERNAL "real_greater_equal_real" ENDOP;

{---------------------------------------------------------- Usual stuff from libm }
REAL PROC sqrt   (REAL a): EXTERNAL "sqrt" ENDPROC;
REAL PROC ln     (REAL a): EXTERNAL "log"  ENDPROC;
REAL PROC exp    (REAL a): EXTERNAL "exp"  ENDPROC;
REAL PROC sin    (REAL a): EXTERNAL "sin"  ENDPROC;
REAL PROC cos    (REAL a): EXTERNAL "cos"  ENDPROC;
REAL PROC tan    (REAL a): EXTERNAL "tan"  ENDPROC;
REAL PROC arcsin (REAL a): EXTERNAL "asin" ENDPROC;
REAL PROC arccos (REAL a): EXTERNAL "acos" ENDPROC;
REAL PROC arctan (REAL a): EXTERNAL "atan" ENDPROC;

{---------------------------------------------------------- REAL coded }
REAL OP / (INT a,b):
  real (a) / real (b)
ENDOP /;

REAL OP ** (REAL a,b):
  IF a > 0.0
  THEN exp (b * ln (a))
  ELIF a < 0.0
  THEN error stop ("OP ** (REAL a,b) called with negative base"); 0.0
  ELIF a = 0.0 AND b = 0.0 THEN 1.0
  ELSE 0.0
  FI
ENDOP **;

INT OP SIGN (REAL a):
  IF a > 0.0
  THEN 1
  ELIF a = 0.0
  THEN 0
  ELSE - 1
  FI
ENDOP SIGN;

INT PROC sign (REAL a):
  SIGN a
ENDPROC sign;

REAL OP ABS (REAL a):
  IF a >= 0.0
  THEN a
  ELSE - a
  FI
ENDOP ABS;

REAL PROC abs (REAL a):
  ABS a
ENDPROC abs;

REAL PROC min (REAL a,b):
  IF a > b
  THEN b
  ELSE a
  FI
ENDPROC min;

REAL PROC max (REAL a,b):
  IF a > b
  THEN a
  ELSE b
  FI
ENDPROC max;

REAL PROC log2 (REAL a):
  ln (a) / ln (2.0)
ENDPROC log2;

REAL PROC log10 (REAL a):
  ln (a) / ln (10.0)
ENDPROC log10;

{---------------------------------------------------------- BOOL }
LET true = TRUE;
LET false = FALSE;

BOOL OP NOT (BOOL a):   INTERNAL "not_bool" ENDOP;
BOOL OP =   (BOOL a,b): INTERNAL "int_equal_int" ENDOP;
BOOL OP <>  (BOOL a,b): INTERNAL "int_notequal_int" ENDOP;
BOOL OP AND (BOOL a,b): INTERNAL "bool_and_bool" ENDOP;
BOOL OP OR  (BOOL a,b): INTERNAL "bool_or_bool" ENDOP;
BOOL OP XOR (BOOL a,b): INTERNAL "bool_xor_bool" ENDOP;

{---------------------------------------------------------- TEXT }
LET digits = "0123456789";
LET niltext = "";
LET blank = " ";
LET quote = "\"";
LET line sep = "\r";

INT  OP LENGTH (TEXT a): EXTERNAL "rts_length_text" ENDOP;
INT  PROC length (TEXT a): EXTERNAL "rts_length_text" ENDPROC;

TEXT OP + (TEXT a,b): EXTERNAL "rts_text_plus_text" ENDOP;
TEXT OP * (INT a, TEXT b): EXTERNAL "rts_int_times_text" ENDOP;

BOOL OP =  (TEXT a,b): INTERNAL "text_equal_text" ENDOP;
BOOL OP <> (TEXT a,b): INTERNAL "text_notequal_text" ENDOP;
BOOL OP <  (TEXT a,b): INTERNAL "text_less_than_text" ENDOP;
BOOL OP <= (TEXT a,b): INTERNAL "text_less_equal_text" ENDOP;
BOOL OP >  (TEXT a,b): INTERNAL "text_greater_than_text" ENDOP;
BOOL OP >= (TEXT a,b): INTERNAL "text_greater_equal_text" ENDOP;

TEXT OP SUB (TEXT a, INT b): EXTERNAL "rts_text_sub_int" ENDOP;
INT PROC pos (TEXT t, pat): EXTERNAL "rts_pos_text_text" ENDPROC;
TEXT PROC subtext (TEXT a, INT from, to):
   EXTERNAL "rts_subtext_text_int_int"
ENDPROC;

TEXT PROC ascii (INT a): EXTERNAL "rts_ascii_int" ENDPROC;
TEXT PROC code (INT a):  EXTERNAL "rts_ascii_int" ENDPROC;
INT PROC ascii (TEXT a): EXTERNAL "rts_ascii_text" ENDPROC;
INT PROC code (TEXT a):  EXTERNAL "rts_ascii_text" ENDPROC;

TEXT OP HEAD (TEXT t): t SUB 1 ENDOP;
TEXT OP TAIL (TEXT t): subtext (t, 2, length (t)) ENDOP;
OP CAT (TEXT VAR a, TEXT b): a := a + b ENDOP;

TEXT PROC subtext (TEXT t, INT from):
   subtext (t, from, LENGTH t)
ENDPROC;

INT PROC pos (TEXT t, pat, INT from):
  INT i :: pos (subtext (t, from), pat);
  IF i = 0
  THEN 0
  ELSE i + from - 1
  FI
ENDPROC;

TEXT PROC text (TEXT t, INT length):
   IF LENGTH t >= length
   THEN subtext (t, 1, length)
   ELSE t + (length - LENGTH t) * blank
   FI
ENDPROC;

TEXT PROC text (TEXT t, INT length, from):
   text (subtext (t, from), length)
ENDPROC;

INT PROC pos (TEXT CONST t, pat, INT CONST from, to):
   pos (text (t, to), pat, from)
ENDPROC;

PROC replace (TEXT VAR t, INT n, TEXT new):
   IF n > 0
   THEN t := text (t, n - 1) + new + subtext (t, n + LENGTH new)
   FI
ENDPROC;

PROC change (TEXT VAR t, TEXT old, new):
  INT index :: pos (t, old);
  IF index > 0
  THEN t := text (t, index - 1) + new + subtext (t, index + LENGTH old)
  FI
ENDPROC;

PROC change all (TEXT VAR t, TEXT old, new):
   INT loc :: pos (t, old);
   IF loc > 0
   THEN
      TEXT VAR temp :: subtext (t, loc + LENGTH old);
      change all (temp, old, new);
      t := text (t, loc - 1) + new + temp
   FI
ENDPROC;

TEXT PROC compress (TEXT CONST t):
   INT VAR l :: length (t), k :: 1;
   WHILE (t SUB l) = blank
   REP l DECR 1
   ENDREP;
   WHILE (t SUB k) = blank
   REP k INCR 1
   ENDREP;
   subtext (t, k, l)
ENDPROC;

{---------------------------------------------------------- Conversions }
REAL PROC real (INT a): INTERNAL "int_to_real" ENDPROC;
INT PROC trunc (REAL a): EXTERNAL "rts_trunc_real_to_int" ENDPROC;
INT PROC round (REAL a): EXTERNAL "rts_round_real_to_int" ENDPROC;
TEXT PROC text (REAL a): EXTERNAL "rts_real_to_text" ENDPROC;
REAL PROC real (TEXT t, BOOL VAR conv ok): EXTERNAL "rts_text_to_real" ENDPROC;

TEXT PROC digit (INT CONST i): digits SUB i + 1 ENDPROC;
INT PROC digit (TEXT CONST t): pos (digits, t) - 1 ENDPROC;

TEXT PROC text (BOOL b):
  IF b THEN "TRUE" ELSE "FALSE" FI
ENDPROC;

TEXT PROC text (INT n):
  IF n = -maxint - 1
  THEN "-" + text (maxint DIV 10) + digit (maxint MOD 10 + 1)
  ELIF n < 0
  THEN "-" + text (- n)
  ELIF n >= 10
  THEN text (n DIV 10) + digit (n MOD 10)
  ELSE digit (n)
  FI
ENDPROC;

TEXT PROC text (INT n, width):
  TEXT t :: text (n);
  INT l :: LENGTH t;
  IF l > width
  THEN width * "*"
  ELSE (width - l) * blank + t
  FI
ENDPROC;

TEXT PROC text (REAL n, INT width):
  IF n < 0.0
  THEN add sign (text (- n, width - 1))
  ELSE
    INT VAR exp;
    TEXT VAR mant;
    get mant and exp (n, mant, exp);
    IF LENGTH mant < width
    THEN mant CAT (width - LENGTH mant) * "0"
    FI;
    IF exp <= 0
    THEN text ("0." + -exp * "0" + mant, width)
    ELIF exp >= width
    THEN width * "*"
    ELSE text (text (mant, exp) + "." + subtext (mant, exp + 1), width)
    FI
  FI
ENDPROC text;

TEXT PROC text (REAL n, INT width, after period):
  INT CONST pre period :: width - after period - 1;
  IF n < 0.0
  THEN add sign (text (- n, width - 1, after period))
  ELIF pre period < 1
  THEN width * "*"
  ELSE
    INT VAR exp;
    TEXT VAR mant;
    get mant and exp (n, mant, exp);
    INT CONST mant length :: LENGTH mant;
    IF LENGTH mant < width
    THEN mant CAT (width - mant length) * "0"
    FI;
    IF exp <= 0
    THEN text ((pre period - 1) * blank + "0." + - exp * "0" + mant, width)
    ELIF exp > pre period
    THEN pre period * "*" + "." + after period * "*"
    ELSE text ((pre period - exp) * blank + text (mant, exp) + "."
               + subtext (mant, exp + 1), width)
    FI
  FI
ENDPROC text;

{ Not exported }
PROC get mant and exp (REAL n, TEXT VAR mant, INT VAR exp):
  TEXT CONST t :: compress (text (n));
  INT CONST epos :: pos (t, "e"), dot pos :: pos (t, ".");
  IF epos = 0
  THEN
    mant := text (t, dot pos - 1) + subtext (t, dot pos + 1);
    exp := dot pos - 1
  ELSE
    mant := text (t, dot pos - 1) + subtext (t, dot pos + 1, epos - 1);
    exp := int (subtext (t, epos + 1)) + 1
  FI
ENDPROC get mant and exp;

{ Not exported }
TEXT PROC add sign (TEXT t):
  IF t = niltext
  THEN "*"
  ELIF (t SUB 1) = "*"
  THEN "*" + t
  ELSE
    INT VAR non space :: 1;
    WHILE (t SUB non space) = blank
    REP non space INCR 1
    ENDREP;
    (non space - 1) * blank + "-" + subtext (t, non space)
  FI
ENDPROC add sign;

REAL PROC real (TEXT t):
  BOOL VAR conv ok;
  real (t, conv ok)
ENDPROC real;

INT PROC int (REAL a):
  INT CONST b :: trunc (a);
  IF a >= 0.0
  THEN b
  ELIF a = real (b)
  THEN b
  ELSE b - 1
  FI
ENDPROC int;

INT PROC int (TEXT CONST t, BOOL VAR conv ok):
  INT l :: LENGTH t;
  INT VAR value :: 0, i;
  conv ok := TRUE;
  IF l = 0
  THEN conv ok := FALSE
  ELIF subtext (t, 1, 1) = "-"
  THEN value := - int (subtext (t, 2), conv ok)
  ELIF subtext (t, 1, 1) = "+"
  THEN value := int (subtext (t, 2), conv ok)
  ELSE
    FOR i UPTO l
    REP
      INT CONST dig :: pos (digits, t SUB i) - 1;
      IF dig < 0
      THEN conv ok := FALSE
      ELIF value > maxint DIV 10 
      THEN conv ok := FALSE
      ELIF value * 10 > maxint - dig
      THEN conv ok := FALSE
      ELSE value := value * 10 + dig
      FI
    ENDREP
  FI;
  value
ENDPROC int;

INT PROC int (TEXT CONST t):
  BOOL VAR conv ok;
  int (t, conv ok)
ENDPROC int;

{---------------------------------------------------------- FILES }
TYPE TRANSPUTDIRECTION = INT;
TRANSPUTDIRECTION PROC input:		TRANSPUTDIRECTION: 1 ENDPROC;
TRANSPUTDIRECTION PROC output:		TRANSPUTDIRECTION: 2 ENDPROC;
TRANSPUTDIRECTION PROC from command:	TRANSPUTDIRECTION: 3 ENDPROC;
TRANSPUTDIRECTION PROC to command:	TRANSPUTDIRECTION: 4 ENDPROC;

LET FILE = INTERNAL "file";
FILE PROC sequential file (TRANSPUTDIRECTION dir, TEXT name):
					EXTERNAL "rts_sequential_file" ENDPROC;
PROC close (FILE f):			EXTERNAL "rts_close_file" ENDPROC;
PROC erase (FILE f):			EXTERNAL "rts_erase_file" ENDPROC;
BOOL PROC opened (FILE f):		EXTERNAL "rts_opened_file" ENDPROC;
BOOL PROC eof (FILE f):			EXTERNAL "rts_eof_file" ENDPROC;

PROC put (FILE f, TEXT t):		EXTERNAL "rts_put_file_text" ENDPROC;
PROC get (FILE f, TEXT VAR t):		EXTERNAL "rts_get_file_text" ENDPROC;
PROC get line (FILE f, TEXT VAR t):	EXTERNAL "rts_get_line_file_text" ENDPROC;

PROC put (FILE f, INT i):
  put (f, text (i, int width))
ENDPROC;

PROC put (FILE f, REAL r):
  put (f, text (r))
ENDPROC;

PROC put line (FILE f, TEXT t):
  put (f, t);
  put (f, "\n")
ENDPROC;

PROC line (FILE f):
  put (f, "\n")
ENDPROC;

PROC line (FILE f, INT n):
  UPTO n
  REP put (f, "\n")
  ENDREP
ENDPROC;

PROC get (FILE f, INT VAR i):
  TEXT VAR t;
  get (f, t);
  i := int (t)
ENDPROC;

PROC get (FILE f, REAL VAR r):
  TEXT VAR t;
  get (f, t);
  r := real (t)
ENDPROC;

{---------------------------------------------------------- Screen IO }
LET int width = 11;
INT PROC screen width:	EXTERNAL "rts_screen_width" ENDPROC;
INT PROC screen length:	EXTERNAL "rts_screen_length" ENDPROC;

PROC get cursor (INT VAR column, line): EXTERNAL "rts_get_cursor" ENDPROC;
PROC cursor (INT column, line):		EXTERNAL "rts_cursor" ENDPROC;

PROC put (TEXT a):	EXTERNAL "rts_put_text" ENDPROC;
PROC line:		put ("\n") ENDPROC;
PROC page:		put (""1"") ENDPROC;
PROC beep:		put (""7"") ENDPROC;
PROC flush:		put (""12"") ENDPROC;
PROC put line:		put ("\n") ENDPROC;
PROC put line (TEXT a): put (a); put ("\n") ENDPROC;

PROC line (INT n):
  UPTO n
  REP put ("\n") ENDREP
ENDPROC;

TEXT PROC inchar:	EXTERNAL "rts_inchar" ENDPROC;
TEXT PROC incharety:	EXTERNAL "rts_incharety" ENDPROC;
PROC inchar (TEXT VAR char): char := inchar ENDPROC;

{---------------------------------------------------------- Edit }
PROC edit (TEXT VAR t, INT CONST start, left, right,
           TEXT CONST end, TEXT VAR c):
  assert (1 <= left AND left <= start AND start <= right, "Invalid edit bounds");
  assert (end <> "", "no terminator text");
  INT VAR cur pos :: max (left, min (start, LENGTH t + 1));
  show head;
  show tail;
  WHILE non term char
  REP
    IF c >= " " AND c < ""127""
    THEN
      IF LENGTH t >= right
      THEN beep
      ELSE
        t := subtext (t, 1, cur pos - 1) + c + subtext (t, cur pos);
        cur pos INCR 1;
        put (c);
        show tail
      FI
    ELSE
      SELECT pos (""4""11""5""6"", c) OF
      CASE 1:
        IF cur pos <= left
        THEN beep
        ELSE
          t := subtext (t, 1, cur pos - 2) + subtext (t, cur pos);
          cur pos DECR 1;
          put (""6"");
          show tail
        FI
      CASE 2:
        IF cur pos > LENGTH t
        THEN beep
        ELSE
          t := subtext (t, 1, cur pos - 1) + subtext (t, cur pos + 1);
          show tail
        FI
      CASE 3:
        IF cur pos > LENGTH t
        THEN beep
        ELSE
          cur pos INCR 1;
          put (c)
        FI
      CASE 4:
        IF cur pos <= left
        THEN beep
        ELSE
          cur pos DECR 1;
          put (c)
        FI
      OTHERWISE beep
      ENDSELECT
    FI
  ENDREP.

  show head:
    put (subtext (t, 1, cur pos - 1)).

  show tail:
    INT VAR px, py;
    get cursor (px, py);
    put (subtext (t, cur pos) + " ");
    cursor (px, py).

  non term char:
    c := inchar;
    pos (end, c) = 0.
ENDPROC;

PROC edit (TEXT VAR t, INT CONST start, left, TEXT CONST end):
  TEXT VAR c;
  edit (t, start, left, 79, end, c)
ENDPROC;

PROC edit (TEXT VAR t, INT CONST start):
  TEXT VAR c;
  edit (t, start, start, 79, ""13"", c)
ENDPROC;

{---------------------------------------------------------- Coded IO }
{ Not exported }
PROC correct input (TEXT msg):
  INT VAR px, py;
  get cursor (px, py);
  cursor (my pos x, my pos y);
  put (msg);
  line;
  sleep (2);
  cursor (my pos x, my pos y);
  put (""2"");
  edit (my line buffer, 1, 1, ""13"");
  cursor (px, py)
ENDPROC;

{|nonreentrant|}
PROC get line (TEXT VAR t):
  get cursor (my pos x, my pos y);
  edit (my line buffer, 1, 1, ""13"");
  line;
  REP
    IF my line buffer <> ""
    THEN
      t := my line buffer;
      my line buffer := "";
      LEAVE get line
    ELSE correct input ("Text, please.")
    FI
  ENDREP
ENDPROC;

{ Not exported }
PROC cut off from input (INT end pos):
  my line buffer:= subtext (my line buffer, end pos + 1);
  my pos x INCR end pos
ENDPROC;

{ Not exported }
PROC provide input line:
  IF my line buffer = ""
  THEN
    get cursor (my pos x, my pos y);
    edit (my line buffer, 1, 1, ""13"");
    line
  FI;
  INT VAR t :: length (my line buffer);
  WHILE (my line buffer SUB t) = blank
  REP t DECR 1
  ENDREP;
  my line buffer := text (my line buffer, t)
ENDPROC;

{ Not exported }
PROC get next word (TEXT VAR t, INT VAR p, TEXT sep):
  WHILE pos (my line buffer, sep) = 1
  REP cut off from input (length (sep))
  ENDREP;
  p := pos (my line buffer, sep) - 1;
  IF p < 0
  THEN p := length (my line buffer)
  FI;
  t := text (my line buffer, p);
  p INCR length (sep)
ENDPROC;

{ Not exported }
PROC get next word (TEXT VAR t, INT VAR p):
  get next word (t, p, blank)
ENDPROC;

{|nonreentrant|}
PROC get (TEXT VAR t, TEXT CONST sep):
  INT VAR p;
  provide input line;
  get next word (t, p, sep);
  cut off from input (p)
ENDPROC;

{|nonreentrant|}
PROC get (TEXT VAR t, INT CONST l):
  provide input line;
  IF l < length (my line buffer)
  THEN
    t := text (my line buffer, l);
    cut off from input (l)
  ELSE
    t := my line buffer;
    my line buffer := ""
  FI
ENDPROC;

{|nonreentrant|}
PROC get (TEXT VAR t):
  INT VAR p;
  provide input line;
  REP
    get next word (t, p);
    IF t <> ""
    THEN
      cut off from input (p);
      LEAVE get
    ELSE correct input ("Text, please.")
    FI
  ENDREP
ENDPROC;

{|nonreentrant|}
PROC get (INT VAR a):
  TEXT VAR t;
  INT VAR p;
  BOOL VAR conv ok;
  provide input line;
  REP
    get next word (t, p);
    a := int (t, conv ok);
    IF conv ok
    THEN
      cut off from input (p);
      LEAVE get
    ELSE correct input ("Integer, please.")
    FI
  ENDREP
ENDPROC;

{|nonreentrant|}
PROC get (REAL VAR a):
  TEXT VAR t;
  INT VAR p;
  BOOL VAR conv ok;
  provide input line;
  REP
    get next word (t, p);
    a := real (t, conv ok);
    IF conv ok
    THEN
      cut off from input (p);
      LEAVE get
    ELSE correct input ("Real, please.")
    FI
  ENDREP
ENDPROC;

PROC put (INT a):
  put (text (a, int width))
ENDPROC;

PROC put (REAL a):
  put (text (a))
ENDPROC;

BOOL PROC yes (TEXT question):
  INT VAR l, c;
  get cursor (l, c);
  REP
    cursor (l, c);
    put (question + " "6"");
    TEXT VAR answer;
    get (answer);
    INT result :: pos ("jynJYN", answer SUB 1)
  UNTIL result <> 0
  ENDREP;
  result <> 3 AND result <> 6
ENDPROC yes;

BOOL PROC no (TEXT question):
  NOT yes (question)
ENDPROC no;

{---------------------------------------------------------- Random }
PROC initialize random (INT CONST a): EXTERNAL "rts_initialize_random" ENDPROC;
INT PROC random (INT CONST a,b): EXTERNAL "rts_random" ENDPROC;

PROC initialize random (REAL CONST a):
  initialize random (round (a * real (maxint)))
ENDPROC initialize random;

PROC random (INT VAR n):
  n := random (-max int, maxint)
ENDPROC random;

REAL PROC random:
  random (0, max int) / maxint
ENDPROC random;

{---------------------------------------------------------- System interface }
INT PROC argument count:	EXTERNAL "rts_argument_count" ENDPROC;
TEXT PROC argument (INT nr):	EXTERNAL "rts_argument" ENDPROC;
BOOL PROC system (TEXT cmd):	EXTERNAL "rts_system" ENDPROC;
INT PROC exectime:		EXTERNAL "rts_exectime" ENDPROC;
PROC stop:			EXTERNAL "rts_stop" ENDPROC;
PROC error stop:		EXTERNAL "rts_error_stop" ENDPROC;
PROC sleep (REAL s):		EXTERNAL "rts_rsleep" ENDPROC;
PROC sleep (INT n):		EXTERNAL "rts_sleep" ENDPROC;

PROC error stop (TEXT msg):
  put line (msg);
  errorstop
ENDPROC;

PROC assert (BOOL ok):
  IF NOT ok
  THEN error stop ("Assert failed")
  FI
ENDPROC;

PROC assert (BOOL ok, TEXT CONST t):
  IF NOT ok
  THEN error stop ("Assert failed: " + t)
  FI
ENDPROC;

PROC system ok (TEXT cmd):
  system (cmd)
ENDPROC;

PROC system assert (TEXT cmd):
  assert (system (cmd), "system (" + cmd + ")")
ENDPROC;

{---------------------------------------------------------- Initialization }
initialization:
  init line buffer.

  init line buffer:
    TEXT VAR my line buffer :: "";
    INT VAR my pos x, my pos y.

ENDPACKET standard;
