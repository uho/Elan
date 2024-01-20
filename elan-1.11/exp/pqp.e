wat leuker:
   INT VAR a :: 1;
   INT VAR b;
   TEXT VAR aggr :: "Q";
   TEXT VAR ehh :: "P";
   TEXT VAR null :: "";
   WHILE a <> 0
   REP
      put ("Enter a: ");
      get (a);
      put ("Enter b: ");
      get (b);
      IF a < 0
      THEN aggr
      ELIF a > 0
      THEN ehh
      ELSE null
      FI := 
      IF b < 0
      THEN aggr CAT ehh; aggr
      ELIF b > 0
      THEN ehh CAT aggr; ehh
      ELSE null CAT "p"; null
      FI;
      put ("aggr = " + aggr + ", ehh = " + ehh + ", null = " + null + "\n")
   ENDREP.
   
