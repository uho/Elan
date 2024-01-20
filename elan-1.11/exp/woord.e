

INT PROC strpos ( TEXT waar we in zoeken, TEXT gezochte letter ):
  INT VAR teller;
  INT VAR lengte;
  teller:= 1;
  lengte := LENGTH waar we in zoeken;

 
 REP
	 IF subtext(waar we in zoeken,teller,teller)=gezochte letter THEN
	    LEAVE strpos WITH teller
	 FI;

	 IF teller = lengte THEN
	   LEAVE strpos WITH 0
	 FI;
	 teller := teller + 1
 ENDREP;
 0
ENDPROC strpos;

INT PROC strsop ( TEXT waar we in zoeken, TEXT gezochte letter ):
  INT VAR teller;
  INT VAR lengte;
  lengte := LENGTH waar we in zoeken;

  teller:= lengte;
 
 REP
	 IF subtext(waar we in zoeken,teller,teller)=gezochte letter THEN
	    LEAVE strsop WITH teller
	 FI;

	 IF teller = 1 THEN
	   LEAVE strsop WITH 0
	 FI;
	 teller := teller - 1
 ENDREP;
 0
ENDPROC strsop;


woord:

  BOOL VAR doorgaan :: TRUE;
  TEXT VAR hooiberg;
  TEXT VAR naald;
  TEXT VAR type;

  INT VAR plek;

REP
put ("Als je het woord van voor naar achter wilt, typ dan 'voor'. Anders als je het woord van achter naar voor wilt, typ dan 'achter':");
line;
line;
put ("Als je het screen wilt clearen typ dan 'page' en om te stoppen typ dan 'stop'");
line;
line;
get(type);
line;
line;
line;

IF type="voor" OR type="Voor" OR type="VOOR" THEN
  put("Typ je woord in:");
  line;
  get (hooiberg);
  line;
  line;
  put("Typ welke letter je wilt hebben:");
  line;
  get (naald);
  line;
  line;
  plek := strpos(hooiberg,naald);
  put(plek); line; line; line


ELIF type="achter" OR type="ACHTER" OR type="Achter" THEN
  put("Typ je woord in:");
  line;
  get (hooiberg);
  line;
  line;
  put("Typ welke letter je wilt hebben:");
  line;
  get (naald);
  line;
  line;
  plek := strsop(hooiberg,naald);
  put(plek); line; line; line

ELIF type="page" THEN
page

ELIF type="stop" THEN
doorgaan:= FALSE

FI
UNTIL doorgaan=FALSE
ENDREP.
