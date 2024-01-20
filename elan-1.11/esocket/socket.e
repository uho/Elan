{ ------------ Jaap Boenders Socket library for ELAN ------------ }
{ -------------- Rewrite by Marc Seutter for ELAN3 -------------- }
PACKET socket:

USES_LIBRARY "esocket";

TYPE SOCKET = INT;
TYPE OPTION = INT;

OPTION PROC reuse:	OPTION: 1 ENDPROC reuse;
OPTION PROC nonblock:	OPTION: 2 ENDPROC nonblock;
OPTION PROC block:	OPTION: 3 ENDPROC block;

{ ------------------- Code in socket library -------------------- }
PROC server socket (TEXT addr, INT port, INT queue size, SOCKET VAR result):
   EXTERNAL "esock_server_socket"
ENDPROC;

PROC client socket (TEXT addr, INT port, SOCKET VAR result):
   EXTERNAL "esock_client_socket"
ENDPROC;

BOOL PROC input on (SOCKET s, INT wait):	EXTERNAL "esock_input_on" ENDPROC;
PROC accept (SOCKET s, SOCKET VAR fd):		EXTERNAL "esock_accept" ENDPROC;
PROC close  (SOCKET s):				EXTERNAL "esock_close" ENDPROC;
PROC put    (SOCKET s, TEXT b):			EXTERNAL "esock_put" ENDPROC;
PROC get    (SOCKET s, TEXT VAR b, INT len):	EXTERNAL "esock_get" ENDPROC;
PROC set socket option (SOCKET s, OPTION opt):	EXTERNAL "esock_set_opt" ENDPROC;
TEXT PROC hostname:				EXTERNAL "esock_hostname" ENDPROC;

{ ---------------------- ELAN coded part ------------------------ }
PROC line (SOCKET s): put (s, "\n") ENDPROC;

SOCKET PROC server socket (TEXT addr, INT port, INT queue size):
  SOCKET VAR fd;
  server socket (addr, port, queue size, fd);
  fd
ENDPROC server socket;

SOCKET PROC client socket (TEXT addr, INT port):
  SOCKET VAR fd;
  client socket (addr, port, fd);
  fd
ENDPROC client socket;

SOCKET PROC accept (SOCKET s):
  SOCKET VAR fd;
  accept (s, fd);
  fd
ENDPROC accept;

BOOL PROC valid socket (SOCKET s):
  CONCR s >= 0
ENDPROC valid socket;

PROC get line (SOCKET s, TEXT VAR sline):
   TEXT VAR char;
   sline := "";
   get (s, char, 1);
   WHILE char <> "\n"
   REP
      sline := sline + char;
      get (s, char, 1)
   ENDREP
ENDPROC get line;

ENDPACKET socket;
