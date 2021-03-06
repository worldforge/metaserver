/*
    Generic Game Metaserver Test Client

    Copyright (C) 2000-2004 Dragon Master

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
    02110-1301 USA

    The author can be reached via e-mail to dragonm@hypercubepc.com
*/
#ifndef WRAP_H
#define WRAP_H

/* Convenience defines */
#define SA      struct sockaddr
#define SAIN    struct sockaddr_in
/* 1500 byte ethernet MTU/MRU minus slack for packet header */
#define MAXLINE 1450

typedef void    Sigfunc(int);   /* for signal handlers */

#define _USE_POSIX
#define _USE_GNU

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* HAVE_FEATURES_H defined / undefined in config.h */
#ifdef HAVE_FEATURES_H
#warning "Including features.h"
#include <features.h>
#else
#warning "Not including features.h"
#endif

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>	        /* for var args */
#include <string.h>
#include <errno.h>
#include <syslog.h>             /* for syslog() */
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void Bind(int, const struct sockaddr *, socklen_t);
char *Fgets(char *, int, FILE *);
pid_t Fork(void);
void Fputs(const char *, FILE *);
int LookupHost(const char *, struct in_addr *);
void Inet_aton(const char *, struct in_addr *);
int Inet_pton(int, const char *, void *);
void *Malloc(size_t);
ssize_t Recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);
void Sendto(int, const void *, size_t, int, const struct sockaddr *, 
            socklen_t);
void Setsockopt(int, int, int, const void *, socklen_t);
char *Sock_ntop(const struct sockaddr *, socklen_t); /* NOT THREAD SAFE! */
int Socket(int, int, int);
Sigfunc *Signal(int, Sigfunc *);
void err_quit(const char *, ...);
void err_sys(const char *, ...);
void debug_msg(const char *, ...);
void notice_msg(const char *, ...);
void warning_msg(const char *, ...);

#endif
