#ifndef WRAP_H
#define WRAP_H

#define SA      struct sockaddr
#define MAXLINE 4096

typedef void    Sigfunc(int);   /* for signal handlers */

#define _USE_POSIX
#define _USE_GNU
#include <features.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>	        /* ANSI C header file */
#include <string.h>
#include <errno.h>
#include <syslog.h>             /* for syslog() */
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void Bind(int, const struct sockaddr *, socklen_t);
char *Fgets(char *, int, FILE *);
void Fputs(const char *, FILE *);
void Inet_pton(int, const char *, void *);
void *Malloc(size_t);
ssize_t Recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);
void Sendto(int, const void *, size_t, int, const struct sockaddr *, 
            socklen_t);
void Setsockopt(int, int, int, const void *, socklen_t);
char *Sock_ntop(const struct sockaddr *, socklen_t);
int Socket(int, int, int);
Sigfunc *Signal(int, Sigfunc *);
void err_quit(const char *, ...);
void err_sys(const char *, ...);

#endif
