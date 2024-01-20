{ -------------- ELAN thread library -------------- }
PACKET threads:

USES_LIBRARY "ethread";

TYPE PID = INTERNAL "addr";
TYPE ARG = INTERNAL "addr";

BOOL OP = (PID pid1, pid2):
   EXTERNAL "eth_equal_pid_pid"
ENDOP =;

ARG PROC thread arg (INT i):
   EXTERNAL "eth_thread_arg_int"
ENDPROC thread arg;

INT PROC int (ARG arg):
   EXTERNAL "eth_int_arg"
ENDPROC int;

PID PROC create thread (PROC code):
   EXTERNAL "eth_create_thread_proc"
ENDPROC create thread;

PID PROC create thread (PROC (ARG) code, ARG arg):
   EXTERNAL "eth_create_thread_proc_arg"
ENDPROC create thread;

PROC finish thread:
   EXTERNAL "eth_finish_thread"
ENDPROC finish thread;

ENDPACKET threads;
