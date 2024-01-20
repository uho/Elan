{ semantics check2 }
main:
  losse flodder;
  vraag flodder;
  flotsam;
  print flodder.
  
losse flodder:
  INT VAR jan;
  0.

vraag flodder:
  put ("geef invoer: ");
  get (jan).

flotsam:
  put (text (losse flodder) + "\n").
  
print flodder:
  put (text (jan) + "\n").
