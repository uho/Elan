{ semantics check }
main:
  losse flodder;
  losse flodder;
  vraag flodder;
  doe nog wat;
  druk misschien af.

losse flodder:
  INT VAR jan.
  
vraag flodder:
  put ("geef invoer: ");
  get (jan).

druk misschien af:
  IF jan > 0
  THEN put (text (jan) + "\n")
  ELSE doe nog wat
  FI.

doe nog wat:
  druk misschien af.
