USES threads;

PROC thread (ARG arg):
   INT my nr :: int (arg);
   REP
      sleep (real (random (1,100)) * 0.005);
      put ("Thread " + text (my nr) + " prints this\n")
   ENDREP
ENDPROC;

main:
   INT VAR i;
   FOR i UPTO 10
   REP
      create thread (thread, thread arg (i))
   ENDREP;
   REP
      sleep (real (random (1,100)) * 0.01);
      put ("The main program prints this\n")
   ENDREP.
