/*
   File: elan_socket.c
   Defines a socket library for elan

   CVS ID: "$Id: elan_socket.c,v 1.2 2004/02/14 13:00:22 marcs Exp $"
*/

/* global includes */
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/* net stuff includes */
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netdb.h>
#include <netinet/in.h>

/* liberts includes */
#include <rts_error.h>
#include <rts_alloc.h>
#include <rts_texts.h>

/* local includes */
#include <elan_socket.h>

#define REUSE 1
#define NONBLOCK 2
#define BLOCK 3

/* PROC server socket (TEXT addr, INT port, INT queue size, SOCKET VAR result) */
/* Perhaps we need something better than reporting the error and then quit */
void esock_server_socket (char *addr, int port, int queue_size, int *result)
	{ /* lookup network address of host */
	  struct hostent *host = gethostbyname (addr);
	  struct sockaddr_in sa_in;
	  int fd;

	  /* Check if we have resolved into a network address */
	  if (host == NULL)
	     rts_error ("Could not locate host %s, reason: %s", addr, strerror (errno));

	  /* Copy the hosts network address into sa_in */
	  bzero (&sa_in, sizeof (sa_in));
	  bcopy (host -> h_addr, &sa_in.sin_addr, host -> h_length);
	  sa_in.sin_family = AF_INET;
	  sa_in.sin_port = port;

	  /* create an INET socket */
	  fd = socket (AF_INET, SOCK_STREAM, 0);
	  if (fd < 0)
	     rts_error ("Could not setup IP socket, reason: %s", strerror (errno));

	  /* bind the socket to the network address */
	  if (bind (fd, (struct sockaddr *) &sa_in, sizeof (sa_in)) < 0)
	     rts_error ("Could not bind socket, reason: %s", strerror (errno));

	  /* set the socket to listen */
	  if (listen (fd, queue_size) < 0)
	     rts_error ("Could not listen on socket, reason: %s", strerror (errno));

	  /* Server socket is setup and listening */
	  *result = fd;
	};

/*
   PROC client socket (TEXT addr, INT port, SOCKET VAR result)
*/
void esock_client_socket (char *addr, int port, int *result)
	{ /* lookup network address of host */
	  struct hostent *host = gethostbyname (addr);
	  struct sockaddr_in sa_in;
	  int fd;

	  /* Check if we have resolved into a network address */
	  if (host == NULL)
	     rts_error ("Could not locate host %s, reason: %s", addr, strerror (errno));

	  /* Copy the hosts network address into sa_in */
	  bzero (&sa_in, sizeof (sa_in));
	  bcopy (host -> h_addr, &sa_in.sin_addr, host -> h_length);
	  sa_in.sin_family = AF_INET;
	  sa_in.sin_port = port;

	  /* create an INET socket */
	  fd = socket (AF_INET, SOCK_STREAM, 0);
	  if (fd < 0)
	     rts_error ("Could not setup IP socket, reason: %s", strerror (errno));

	  /* try and connect to the host */
	  if (connect (fd, (struct sockaddr *) &sa_in, sizeof (sa_in)) < 0)
	     rts_error ("Could not connect socket to host, reason: %s", strerror (errno));

	  /* Client socket is setup and connected to host */
	  *result = fd;
	};

/* BOOL PROC input on (SOCKET s, INT wait) */
int esock_input_on (int s, int wait_ms)
	{ struct timeval time_out;
	  fd_set read_fds;
	  time_out.tv_usec = wait_ms % 1000;
	  time_out.tv_sec = wait_ms / 1000;
	  FD_ZERO (&read_fds);
	  FD_SET (s, &read_fds);
	  select (s + 1, &read_fds, NULL, NULL, &time_out);
	  if (FD_ISSET (s, &read_fds)) return (1);
	  return (0);
	};

/* PROC accept (SOCKET s, SOCKET VAR fd) */
void esock_accept (int s, int *fd)
	{ struct sockaddr addr_buf;
	  int size = sizeof (addr_buf);
	  int result = accept (s, &addr_buf, &size);
	  if (result >= 0) *fd = result;
	  else if (errno != EWOULDBLOCK)
	     rts_error ("accept failed, reason: %s", strerror (errno));
	  else *fd = -1;
	};

/* PROC close (SOCKET s) */
void esock_close (int s)
	{ if (close (s) < 0)
	     rts_error ("close failed, reason: %s", strerror (errno));
	};

/* PROC put (SOCKET s, TEXT b) */
void esock_put (int s, char *b)
	{ if (write (s, b, strlen (b)) < 0)
	     rts_error ("write on socket failed, reason: %s", strerror (errno));
	};

/* PROC get (SOCKET s, TEXT VAR b, INT len) */
void esock_get (int s, char **b, int len)
	{ char *new = rts_malloc (len + 1);
	  int act_read = read (s, new, len);
	  if (act_read == -1)
	     rts_error ("read from socket failed, reason: %s", strerror (errno));
	  new[act_read] = '\0';
	  rts_detach (b);
	  *b = new;
	};

/* PROC set socket option (SOCKET s, OPTION opt) */
void esock_set_opt (int s, int opt)
	{ int flags = fcntl (s, F_GETFL, 0);
	  int stat = 0;
	  int optval;
	  if (flags == -1)
	     rts_error ("get flags from socket failed, reason: %s", strerror (errno));
	  switch (opt)
	     { case REUSE:
		  stat = setsockopt (s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval));
		  break;
	       case NONBLOCK:
		  stat = fcntl (s, F_SETFL, flags | O_NONBLOCK);
		  break;
	       case BLOCK:
		  stat = fcntl (s, F_SETFL, flags & ~O_NONBLOCK);
		  break;
	       default: rts_warning ("set socket option called with unknown option");
	     };
	  if (stat == -1)
	     rts_error ("set socket option failed, reason: %s", strerror (errno));
	};

/* TEXT PROC hostname */
char *esock_hostname ()
	{ char buf[MAXHOSTNAMELEN + 1];
	  if (gethostname (buf, MAXHOSTNAMELEN) < 0)
	     rts_error ("gethostname failed, reason: %s", strerror (errno));
	  return (rts_new_text (buf));       
	};
