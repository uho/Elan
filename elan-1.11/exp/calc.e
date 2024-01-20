
PROC push (REAL CONST r):
  FOR i FROM stack max DOWNTO 2
  REP stack [i] := stack [i - 1]
  ENDREP;
  stack [1] := r
ENDPROC push;

REAL PROC pop:
  REAL CONST top :: stack [1];
  FOR i FROM 1 UPTO stack max - 1
  REP stack [i] := stack [i + 1]
  ENDREP;
  stack [stack max] := 0.0;
  top
ENDPROC pop;

program:
  init calculator;
  REP
    get next command;
    process command
  UNTIL end
  ENDREP;
  page.

  init calculator:
    tell name;
    give menu;
    give command prompt;
    init stack.
  
    tell name:
      to header position;
      put ("Pocket calculator").
    
      to header position:
        cursor (30, 1).
      
    give menu:
      to menu position;
      put ("    Commands:");
      line (2);
      put ("<REAL denotation>");
      line;
      put ("+ - * /");
      line;
      put ("i       invert x");
      line;
      put ("x       x <--> y");
      line;
      put ("d       y <-x");
      line;
      put ("^       x ** y");
      line;
      put ("q       sqrt(x)");
      line;
      put ("l       ln(x)");
      line;
      put ("e       exp(x)");
      line;
      put ("p       pi");
      line;
      put ("s       sin(x)");
      line;
      put ("c       cos(x)");
      line;
      put ("a       arctan(x)");
      line;
      put ("m       max real");
      line (2);
      put ("$       end").
    
      to menu position:
        cursor (1, 3).
      
    give command prompt:
      cursor (33, 12);
      put ("Command:").
    
    init stack:
      LET stack max = 4;
      ROW stack max REAL VAR stack;
      INT VAR i;
      FOR i FROM 1 UPTO stack max
      REP stack [i] := 0.0
      ENDREP;
      show stack.
    
      show stack:
        FOR i FROM stack max DOWNTO 1
        REP
          cursor (40, 11 - i);
          put (""2"");
          IF i = 2
          THEN put ("y ")
          ELIF i = 1
          THEN put ("x ")
          ELSE put ("  ")
          FI;
          put (stack [i]);
          line
        ENDREP.
      
  get next command:
    TEXT VAR command;
    to command position;
    get (command).
  
    to command position:
      cursor (44, 12);
      put (""2"").
    
  process command:
    IF NOT end
    THEN
      TEXT CONST h :: HEAD command;
      IF digit (h) >= 0
      THEN process numeral
      ELSE
        REAL CONST top :: pop;
        SELECT pos ("+-*/ixd^qlepscam", h) OF
          CASE 1: add
          CASE 2: subtract
          CASE 3: multiply
          CASE 4: divide
          CASE 5: invert
          CASE 6: exchange
          CASE 7: double top
          CASE 8: power
          CASE 9: square root
          CASE 10: logarithm
          CASE 11: exponent
          CASE 12: load pi
          CASE 13: sinus
          CASE 14: cosinus
          CASE 15: arcus tangent
          CASE 16: load max real
          OTHERWISE
            push (top);
            illegal command
        ENDSELECT
      FI;
      show stack
    FI.
  
    end:
      HEAD command = "$".
    
    process numeral:
      REAL VAR r :: real (command);
      IF last conversion ok
      THEN push (r)
      ELSE illegal number
      FI.
    
      illegal number:
        to command position;
        put ("Illegal number!");
        sleep (3).
      
    add:
      push (top + pop).
    
    subtract:
      push (pop - top).
    
    multiply:
      push (pop * top).
    
    divide:
      push (pop / top).
    
    invert:
      push (- top).
    
    exchange:
      REAL CONST temp :: pop;
      push (top);
      push (temp).
    
    double top:
      push (top);
      push (top).
    
    power:
      push (top ** pop).
    
    square root:
      push (sqrt (top)).
    
    logarithm:
      push (ln (top)).
    
    exponent:
      push (exp (top)).
    
    load pi:
      push (top);
      push (pi).
    
    sinus:
      push (sin (top)).
    
    cosinus:
      push (cos (top)).
    
    arcus tangent:
      push (arctan (top)).
    
    load max real:
      push (top);
      push (maxreal).
    
    illegal command:
      to command position;
      put ("Illegal command!");
      sleep (3).
