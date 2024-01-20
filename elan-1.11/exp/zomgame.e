USE elan0;

 program:
   start zombiegame;
   play zombiegame;
   finish zombiegame.

   start zombiegame:
     LET empty = 0;
     LET player = 1;
     LET zombie = 2;
     LET dying zombie = 3;
     LET black hole = 4;
     LET height = 20;
     LET width = 20;
     LET max zombies = 200;
     LET max random = 128;
     LET xstep = 2;
     INT size :: height * width;
     INT extra player :: size DIV 10 * 2;
     INT VAR score :: 0;
     INT VAR bonus :: 0;
     INT VAR players :: 1;
     INT VAR used black holes :: size DIV 7 + 10;
     INT VAR used zombies :: size DIV 7;
     INT VAR living zombies;
     INT VAR zombies on screen;
     INT VAR x;
     INT VAR y;
     INT VAR plx;
     INT VAR ply;
     INT VAR dx;
     INT VAR dy;
     INT VAR top;
     INT VAR distance;
     INT VAR i;
     INT VAR carry;
     TEXT VAR getal :: "5";
     ROW width ROW height INT VAR field;
     ROW max zombies INT VAR zombiefield;
     ROW 5 INT VAR freefield.
  
   play zombiegame:
     give statusline;
     WHILE still players
     REP
       create field;
       give data;
       play round;
       create data
     ENDREP.
  
     give statusline:
       cursor (xstep, 1);
       put ("score:         zombies:        black holes:");
       UPTO 2
       REP add player
       ENDREP.
    
       add player:
         cursor (xstep * players, height + 3);
         draw player;
         players INCR 1.
      
         draw player: put ("*").
        
     still players: players > 0.
    
     create field:
       home;
       place field;
       place player;
       place zombies;
       place black holes.
    
       home: cursor (1, 2).
      
       place field:
         FOR y FROM 1 UPTO height
         REP
           field [1] [y] := empty;
           UPTO width
           REP put (" .")
           ENDREP;
           line
         ENDREP;
         FOR x FROM 2 UPTO width
         REP field [x] := field [1]
         ENDREP.
      
       place player:
         REP generate xy random
         UNTIL acceptable playerfield
         ENDREP;
         code xy player;
         place player on field.
      
         generate xy random:
           x := random (1, width);
           y := random (1, height).
        
         acceptable playerfield:
           IF nothing here
           THEN x > width DIV 4 AND x < width - width DIV 4 AND
		y > height DIV 4 AND y < height - height DIV 4
           ELSE false
           FI.
        
           nothing here: field [x] [y] = empty.
          
         code xy player:
           plx := x;
           ply := y.
        
         place player on field:
           field [x] [y] := player;
           set cursor;
           draw player.
        
           set cursor: cursor (xstep * x, y + 1).
          
       place zombies:
         living zombies := used zombies;
         zombies on screen := used zombies;
         FOR i FROM 1 UPTO used zombies
         REP
           REP generate xy random
           UNTIL acceptable zombiefield
           ENDREP;
           code xy zombie;
           place zombie on field
         ENDREP.
      
         acceptable zombiefield: nothing here AND absolute distance > 3.
        
           absolute distance:
             determinate dx dy;
             IF dx < 0
             THEN dx := - dx
             FI;
             IF dy < 0
             THEN dy := - dy
             FI;
             IF dx > dy
             THEN dx
             ELSE dy
             FI.
          
             determinate dx dy:
               dx := x - plx;
               dy := ply - y.
            
         code xy zombie: zombiefield [i] := position.
        
           position: x + (y - 1) * width.
          
         place zombie on field:
           field [x] [y] := zombie;
           set cursor;
           draw zombie.
        
           draw zombie: put ("z").
          
       place black holes:
         UPTO 4
         REP
           REP
             x := random (0, 7);
             y := random (0, 7);
             x INCR plx - 3;
             y INCR ply - 3
           UNTIL nothing here
           ENDREP;
           place black hole on field
         ENDREP;
         UPTO used black holes - 4
         REP
           REP generate xy random
           UNTIL nothing here
           ENDREP;
           place black hole on field
         ENDREP.
      
         place black hole on field:
           field [x] [y] := black hole;
           set cursor;
           draw black hole.
        
           draw black hole: put ("o").
          
     give data:
       give zombies;
       give black holes;
       commentline;
       INT VAR direction;
       put ("direction?").
    
       give zombies:
         cursor (25, 1);
         put (text (zombies on screen, 5)).
      
       give black holes:
         cursor (45, 1);
         put (text (used black holes, 4)).
      
       commentline: cursor (xstep, height + 2).
      
     play round:
       WHILE still zombies
       REP
         move player;
         move zombies
       ENDREP.
    
       still zombies: living zombies > 0.
      
       move player:
         ask direction player;
         decode xy player;
         empty field;
         move player to new field;
         try put player on field;
         code xy player.
      
         ask direction player:
           REP
             cursor (12, height + 2);
             get (direction);
             cursor (12, height + 2);
             put (5 * " ")
           UNTIL 1 <= direction AND direction <= 9
           ENDREP.
        
         decode xy player:
           x := plx;
           y := ply.
        
         empty field:
           field [x] [y] := empty;
           set cursor;
           draw empty field.
        
           draw empty field: put (".").
          
         move player to new field:
           IF direction > 6
           THEN
             IF y > 1
             THEN north
             FI
           ELIF direction < 4
           THEN
             IF y < height
             THEN south
             FI
           FI;
           IF direction MOD 3 = 0
           THEN
             IF x < width
             THEN east
             FI
           ELIF direction MOD 3 = 1
           THEN
             IF x > 1
             THEN west
             FI
           FI.
        
           north: y DECR 1.
          
           south: y INCR 1.
          
           east: x INCR 1.
          
           west: x DECR 1.
          
         try put player on field:
           IF nothing here
           THEN place player on field
           ELSE
             commentline;
             IF zombie here
             THEN put ("you stepped on a zombie.\n")
             ELSE put ("you disapeared in a black hole.\n")
             FI;
             LEAVE play round
           FI.
        
           zombie here: field [x] [y] = zombie.
          
       move zombies:
         move living zombies;
         remove dying zombies.
      
         move living zombies:
           FOR i FROM living zombies DOWNTO 1
           REP
             decode xy zombie;
             IF zombie here
             THEN
               empty field;
               move zombie to new field;
               try put zombie on field
             ELSE remove this zombiefield
             FI
           ENDREP.
        
           decode xy zombie:
             x := (zombiefield [i] - 1) MOD width + 1;
             y := (zombiefield [i] - 1) DIV width + 1.
          
           move zombie to new field:
             distance := absolute distance;
             IF keep zombie alive
             THEN move zombie alive
             ELSE move zombie towards player
             FI.
          
             keep zombie alive: choose128 > f d + f z + 43.
            
               f d:
                 IF distance < 8
                 THEN 77 - 11 * distance
                 ELIF distance > 11
                 THEN 11 * distance - 121
                 ELSE 0
                 FI.
              
               f z:
                 IF zombies on screen > 17
                 THEN 0
                 ELSE 119 - 7 * zombies on screen
                 FI.
              
             move zombie alive:
               top := 0;
               determinate dx dy;
               IF dy > dx
               THEN try nw half
               ELIF dy < dx
               THEN try se half
               ELIF dx > 0
               THEN
                 try w;
                 try s;
                 try e
               ELSE
                 try e;
                 try n;
                 try w
               FI;
               give xy field.
            
               try nw half:
                 IF dy < - dx
                 THEN try w part
                 ELIF dy > - dx
                 THEN try n part
                 ELSE
                   try s;
                   try e;
                   try n
                 FI.
              
                 try w part:
                   try e;
                   IF y < height
                   THEN
                     try s;
                     try w;
                     north;
                     east
                   FI;
                   IF y > 1
                   THEN
                     try n;
                     try w
                   FI.
                
                   try e:
                     east;
                     try field.
                  
                     try field:
                       IF nothing here
                       THEN
                         top INCR 1;
                         freefield [top] := position
                       FI.
                    
                   try s:
                     south;
                     try field.
                  
                   try w:
                     west;
                     try field.
                  
                   try n:
                     north;
                     try field.
                  
                 try n part:
                   try s;
                   IF x < width
                   THEN
                     try e;
                     try n;
                     west;
                     south
                   FI;
                   IF x > 1
                   THEN
                     try w;
                     try n
                   FI.
                
               try se half:
                 IF dy < - dx
                 THEN try s part
                 ELIF dy > - dx
                 THEN try e part
                 ELSE
                   try w;
                   try n;
                   try e
                 FI.
              
                 try s part:
                   try n;
                   IF x < width
                   THEN
                     try e;
                     try s;
                     west;
                     north
                   FI;
                   IF x > 1
                   THEN
                     try w;
                     try s
                   FI.
                
                 try e part:
                   try w;
                   IF y < height
                   THEN
                     try s;
                     try e;
                     north;
                     west
                   FI;
                   IF y > 1
                   THEN
                     try n;
                     try e
                   FI.
                
               give xy field:
                 IF top > 0
                 THEN zombiefield [i] := freefield [random (1, top)]
                 FI;
                 decode xy zombie.
              
             move zombie towards player:
               determinate dx dy;
               IF dy > dx
               THEN move zombie in nw half
               ELIF dy < dx
               THEN move zombie in se half
               ELIF dy > 0
               THEN
                 south;
                 west
               ELSE
                 north;
                 east
               FI.
            
               move zombie in nw half:
                 IF dy > - dx
                 THEN move zombie in n part
                 ELIF dy < - dx
                 THEN move zombie in w part
                 ELSE
                   south;
                   east
                 FI.
              
                 move zombie in n part:
                   south;
                   maybe ew.
                
                   maybe ew:
                     IF random (0, 1) = 0
                     THEN
                       IF dx > 0
                       THEN west
                       ELIF dx < 0
                       THEN east
                       FI
                     FI.
                  
                 move zombie in w part:
                   east;
                   maybe ns.
                
                   maybe ns:
                     IF random (0, 1) = 0
                     THEN
                       IF dy > 0
                       THEN south
                       ELIF dy < 0
                       THEN north
                       FI
                     FI.
                  
               move zombie in se half:
                 IF dy > - dx
                 THEN move zombie in e part
                 ELIF dy < - dx
                 THEN move zombie in s part
                 ELSE
                   north;
                   west
                 FI.
              
                 move zombie in e part:
                   west;
                   maybe ns.
                
                 move zombie in s part:
                   north;
                   maybe ew.
                
           try put zombie on field:
             code xy zombie;
             IF nothing here
             THEN place zombie on field
             ELIF player here
             THEN killed by zombie
             ELSE kill zombie
             FI.
          
             player here: field [x] [y] = player.
            
             killed by zombie:
               place zombie on field;
               commentline;
               put ("you are killed by a zombie.\n");
               LEAVE play round.
            
             kill zombie:
               IF zombie here
               THEN
                 empty field;
                 field [x] [y] := dying zombie;
                 two zombies removed
               ELSE
                 remove this zombiefield;
                 one zombie removed
               FI.
            
               two zombies removed:
                 zombies on screen DECR 2;
                 score INCR 1;
                 try new player;
                 give score;
                 give zombies.
              
                 try new player:
                   IF score MOD extra player = 0
                   THEN add player
                   FI.
                
                 give score:
                   cursor (8, 1);
                   put (text (score, 6));
                   put (getal).
                
               remove this zombiefield:
                 carry := zombiefield [i];
                 zombiefield [i] := zombiefield [living zombies];
                 zombiefield [living zombies] := carry;
                 living zombies DECR 1.
              
               one zombie removed:
                 zombies on screen DECR 1;
                 IF getal = "0"
                 THEN getal := "5"
                 ELSE
                   getal := "0";
                   score INCR 1;
                   try new player
                 FI;
                 give score;
                 give zombies.
              
         remove dying zombies:
           top := living zombies;
           i := living zombies + 1;
           UPTO top - zombies on screen
           REP
             REP
               i DECR 1;
               decode xy zombie
             UNTIL field [x] [y] = dying zombie
             ENDREP;
             remove this zombiefield
           ENDREP;
           FOR i FROM living zombies + 1 UPTO top
           REP
             decode xy zombie;
             field [x] [y] := empty
           ENDREP.
        
     create data:
       IF still zombies
       THEN create easier round
       ELSE create harder round
       FI;
       clear commentline.
    
       create easier round:
         sleep (2);
         players DECR 1;
         cursor (xstep * players, height + 3);
         put (" ");
         used zombies DECR round deviation.
      
         round deviation: random (1, size DIV 50).
        
       create harder round:
         bonus INCR size DIV 70;
         score INCR bonus;
         commentline;
         put ("Bonus for completing round:");
         put (bonus);
         put ("0");
         give score;
         sleep (2);
         IF (score - bonus) MOD extra player + bonus >= extra player
         THEN add player
         FI;
         used black holes DECR round deviation + 1;
         used zombies INCR round deviation;
         IF used zombies > max zombies
         THEN used zombies := max zombies
         FI.
      
       clear commentline:
         commentline;
         put (36 * " ").
      
   finish zombiegame: cursor (1, height + 1).
  
