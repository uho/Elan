LET january = 1;

TEXT PROC month from int (INT i):
   SELECT i OF
      CASE january: "January"
      CASE 2: "February"
      CASE 3: "March"
      CASE 4: "April"
      CASE 5: "May"
      CASE 6: "June"
      CASE 7: "July"
      CASE 8: "August"
      CASE 9: "September"
      CASE 10: "October"
      CASE 11: "November"
      CASE 12: "December"
   OTHERWISE ""
   ENDSELECT
ENDPROC;

program:
   INT VAR i;
   FOR i UPTO 12
   REP put ("Month " + text (i) + " is " + month from int (i) + "\n")
   ENDREP.

