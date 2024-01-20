PACKET kareltje

DEFINES start kareltje,
	stop kareltje,
	stap,
	linksom,
	rechtsom,
	muur voor,
	muur rechts,
	maak trap,
	maak huis,
	maak doolhof,
	steen op,
	piepbal op,
	karel op,
	pak piepbal,
	leg piepbal,
	piepbal,
	slowness:

LET breedte = 29;

LET hoogte = 21;

LET xstap = 2;

LET ystap = 1;

LET leeg = 0;

LET bal = 1;

LET steen = 2;

LET karel = 3;

PROC slowness (REAL a):
  sleep time := a
ENDPROC;

BOOL PROC noordwaarts:
  dy = 1
ENDPROC noordwaarts;

BOOL PROC oostwaarts:
  dx = 1
ENDPROC oostwaarts;

BOOL PROC zuidwaarts:
  dy = -1
ENDPROC zuidwaarts;

BOOL PROC westwaarts:
  dx = -1
ENDPROC westwaarts;

PROC zet karel op veld:
  veld [xpos] [ypos] INCR karel;
  teken karel
ENDPROC zet karel op veld;

PROC teken karel:
  set cursor;
  IF noordwaarts
  THEN put ("^")
  ELIF oostwaarts
  THEN put (">")
  ELIF zuidwaarts
  THEN put ("v")
  ELSE put ("<")
  FI;
  sleep (sleep time);
  reset cursor
ENDPROC teken karel;

PROC stop kareltje:
  ga naar onderkant scherm;
  put ("Kareltje heeft zijn opdracht beeindigd.");
  sleep (2);
  haal karel van veld;
  ga naar onderkant scherm
ENDPROC stop kareltje;

PROC stap:
  haal karel van veld;
  probeer een stap;
  zet karel op veld.

  probeer een stap:
    IF muur voor
    THEN
      ga naar onderkant scherm;
      IF gaat buiten veld
      THEN put (" Kareltje is uit het zicht verdwenen.")
      ELSE put (" Kareltje is tegen de muur gelopen.")
      FI;
      assert (karel fout)
    ELSE
      xpos INCR dx;
      ypos INCR dy
    FI.
ENDPROC;

PROC set cursor:
  cursor (xstap * xpos, ystap * (hoogte + 1 - ypos))
ENDPROC set cursor;

PROC reset cursor:
  cursor (1, ystap * (hoogte + 1 - ypos))
ENDPROC reset cursor;

PROC rechtsom:
  dx INCR dy;
  dy DECR dx;
  dx INCR dy;
  teken karel
ENDPROC rechtsom;

BOOL PROC muur voor:
  IF gaat buiten veld
  THEN true
  ELSE veld [xpos + dx] [ypos + dy] = steen
  FI
ENDPROC muur voor;

BOOL PROC muur rechts:
  rechtsom;
  BOOL VAR muurtje :: muur voor;
  linksom;
  muurtje
ENDPROC muur rechts;

PROC linksom:
  dy INCR dx;
  dx DECR dy;
  dy INCR dx;
  teken karel
ENDPROC linksom;

BOOL PROC piepbal:
  veld [xpos] [ypos] MOD karel = bal
ENDPROC piepbal;

PROC pak piepbal:
  IF piepbal
  THEN
    veld [xpos] [ypos] DECR bal
  ELSE
    ga naar onderkant scherm;
    put (" Hier ligt geen piepbal.");
    assert (karel fout)
  FI
ENDPROC pak piepbal;

PROC leg piepbal:
  IF piepbal
  THEN
    ga naar onderkant scherm;
    put (" Hier ligt al een piepbal.");
    assert (karel fout)
  ELSE
    veld [xpos] [ypos] INCR bal
  FI
ENDPROC leg piepbal;

PROC haal karel van veld:
  veld [xpos] [ypos] DECR karel;
  set cursor;
  IF piepbal
  THEN put ("o")
  ELSE put (" ")
  FI;
  reset cursor
ENDPROC haal karel van veld;

PROC ga naar onderkant scherm:
  cursor (1, ystap * (hoogte + 1))
ENDPROC ga naar onderkant scherm;

BOOL PROC gaat buiten veld :
  xpos + dx < 1 OR xpos + dx > breedte OR (ypos + dy < 1 OR ypos + dy > hoogte)
ENDPROC gaat buiten veld;

PROC start kareltje:
  FOR ypos FROM 1
  UPTO hoogte
  REP veld [1] [ypos] := leeg
  ENDREP;
  FOR xpos FROM 2
  UPTO breedte
  REP veld [xpos] := veld [1]
  ENDREP;
  xpos := 1;
  ypos := 1;
  zet karel op veld
ENDPROC start kareltje;

PROC steen op (INT x, STRUCT (INT start, INT eind) y):
  INT VAR i;
  FOR i FROM y.start
  UPTO y.eind
  REP steen op (x, i)
  ENDREP
ENDPROC steen op;

PROC steen op (STRUCT (INT start, INT eind) x, INT y):
  INT VAR i;
  FOR i FROM x.start
  UPTO x.eind
  REP steen op (i, y)
  ENDREP
ENDPROC steen op;

PROC steen op (INT x, INT y):
  xpos := x;
  ypos := y;
  veld [xpos] [ypos] := steen;
  set cursor;
  put ("X")
ENDPROC steen op;

PROC piepbal op (INT x, INT y):
  xpos := x;
  ypos := y;
  veld [xpos] [ypos] := bal;
  set cursor;
  put ("o")
ENDPROC piepbal op;

PROC karel op (INT x, INT y):
  haal karel van veld;
  xpos := x;
  ypos := y;
  zet karel op veld
ENDPROC karel op;

{ ---- Obstakel constructie ---- }
PROC start obstakel:
  page;
  haal karel van veld
ENDPROC start obstakel;

PROC maak trap:
  start obstakel;
  steen op ([7, 9], 1);
  steen op (9, 2);
  steen op (10, [2, 4]);
  steen op ([11, 13], 4);
  steen op (13, [5, 6]);
  steen op ([14, 16], 6);
  steen op ([16, 18], 5);
  steen op (18, 4);
  steen op ([18, 21], 3);
  steen op (21, 2);
  steen op ([21, 24], 1);
  piepbal op (23, 2);
  karel op (1, 1)
ENDPROC maak trap;

PROC maak huis:
  start obstakel;
  steen op ([11, 16], 5);
  steen op (16, [6, 15]);
  steen op ([11, 15], 15);
  steen op (11, [12, 14]);
  steen op ([9, 10], 12);
  steen op ([9, 11], 10);
  steen op (11, [5, 9]);
  piepbal op (10, 8);
  karel op (12, 6);
  linksom
ENDPROC maak huis;

PROC maak doolhof:
  start obstakel;
  steen op ([8, 10], 3);
  steen op ([3, 6], 5);
  steen op ([10, 15], 5);
  steen op ([7, 10], 7);
  steen op ([5, 10], 9);
  steen op ([12, 13], 9);
  steen op ([12, 13], 10);
  steen op ([9, 10], 11);
  steen op ([6, 7], 12);
  steen op ([9, 10], 12);
  steen op ([12, 13], 12);
  steen op ([4, 8], 14);
  steen op ([10, 14], 14);
  steen op ([3, 8], 16);
  steen op ([10, 15], 16);
  steen op (3, [6, 9]);
  steen op (3, [11, 15]);
  steen op (5, 6);
  steen op (5, 8);
  steen op (5, [11, 12]);
  steen op (7, [3, 5]);
  steen op (7, [10, 11]);
  steen op (8, 5);
  steen op (8, [15, 17]);
  steen op (10, 6);
  steen op (10, 13);
  steen op (10, 17);
  steen op (11, [3, 4]);
  steen op (12, [6, 7]);
  steen op (13, 11);
  steen op (14, 7);
  steen op (15, [6, 15]);
  piepbal op (10, 18);
  karel op (9, 4)
ENDPROC;

{ ---- pakket initialisatie ---- }
kareltjes administratie:
  INT VAR xpos, ypos;
  INT VAR dx :: 0, dy :: 1;
  REAL VAR sleep time :: 0.2;
  ROW breedte ROW hoogte INT VAR veld;
  BOOL karel fout :: FALSE.

ENDPACKET kareltje;
