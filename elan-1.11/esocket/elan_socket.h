/*
   File: elan_socket.h
   Provides basic socket interfacing

   CVS ID: "$Id: elan_socket.h,v 1.1 2003/11/29 13:17:51 marcs Exp $"
*/
#ifndef IncElanSocket
#define IncElanSocket

/* Exported routines */
extern void  esock_server_socket (char *addr, int port, int queue_size, int *result);
extern void  esock_client_socket (char *addr, int port, int *result);
extern int   esock_input_on (int s, int wait);
extern void  esock_accept (int s, int *fd);
extern void  esock_close (int s);
extern void  esock_put (int s, char *b);
extern void  esock_get (int s, char **b, int len);
extern void  esock_set_opt (int s, int opt);
extern char *esock_hostname ();
 
#endif /* IncElanSocket */
