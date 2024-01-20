USE std;

LET max queens = 8;

PROC print schaakbord:
  INT VAR k;
  FOR k UPTO max queens
  REP print regel
  ENDREP.

  print regel:
    INT VAR l;
    FOR l UPTO max queens
    REP print positie
    ENDREP;
    line.

  print positie:
    IF l = kolom koningin [k]
    THEN put ("Q")
    ELSE put (".")
    FI.

ENDPROC print schaakbord;

PROC plaats koninginnen vanaf (INT deze rij):
  IF deze rij > max queens
  THEN meld succes
  ELSE
    INT VAR i;
    FOR i 
    UPTO max queens
    REP
      IF plaats i is onbedreigd
      THEN
        pak plaats i;
        plaats koninginnen vanaf (deze rij + 1);
        geef plaats i vrij
      FI
    ENDREP
  FI.

  meld succes:
    print schaakbord;
    aantal oplossingen INCR 1;
    put (aantal oplossingen);
    line;
    line.

  plaats i is onbedreigd:
    INT VAR j;
    FOR j UPTO deze rij - 1
    REP
      IF jde koningin bedreigt i
      THEN LEAVE plaats i is onbedreigd WITH false
      FI
    ENDREP;
    true.

  jde koningin bedreigt i:
    INT die kolom :: kolom koningin [j];
    IF zelfde kolom
    THEN true
    ELIF rechtsboven bezet
    THEN true
    ELSE linksboven bezet
    FI.

  zelfde kolom:
    die kolom = i.

  rechtsboven bezet:
    die kolom - i = deze rij - j.

  linksboven bezet:
    die kolom - i = j - deze rij.

  pak plaats i:
    kolom koningin [deze rij] := i.

  geef plaats i vrij:.

ENDPROC plaats koninginnen vanaf;

program:
  hoeveel koninginnen;
  start telling;
  plaats koninginnen vanaf (1);
  meer oplossingen zijn er niet.

hoeveel koninginnen:
  put ("N Queens problem");
  ROW max queens INT VAR kolom koningin.

start telling:
  INT VAR aantal oplossingen :: 0.

meer oplossingen zijn er niet:
  put ("Those are all solutions.\n").
