USE elan0;

program:
  tell program name;
  ask old file name;
  ask new file name;
  copy from old to new.

  tell program name:
    put ("Copy file");
    line.
  
  ask old file name:
    TEXT VAR x;
    put ("Old file: ");
    get (x);
    old file (x).
  
  ask new file name:
    put ("New file: ");
    get (x);
    new file (x).
  
  copy from old to new:
    REP
      read line (x);
      write line (x)
    UNTIL file ended
    ENDREP;
    close file.
