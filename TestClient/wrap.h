/*
    Generic Game Metaserver Test Client

    Copyright (C) 2000 Dragon Master

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as
    published by the Free Software Foundation.  A copy of this license is
    included in this package.  If there is any discrepancy between this copy
    and the version published by the Free Software Foundation, the included
    version shall apply.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The author can be reached via e-mail to dragonm@leech.org
*/
#ifndef WRAP_H
#define WRAP_H

#define SA      struct sockaddr
#define MAXLINE 4096

typedef void    Sigfunc(int);   /* for signal handlers */

#define _USE_POSIX
#define _USE_GNU

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_FEATURES_H
#include <features.h>
#endif
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
#include <arpa/inet.h>          /* for inet_ntop() */

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
