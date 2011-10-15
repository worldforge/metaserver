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
#ifndef WRAPUNIX_HH
#define WRAPUNIX_HH

#define _USE_POSIX
#define _USE_GNU
#ifndef __FreeBSD__
//#include <features.h>
#endif
#include <stdarg.h>	        // for var args
#include <stdio.h>
//#include <sys/types.h>
#include <sys/socket.h>         // for sockaddr and socklen_t
#include <netinet/in.h>         // for in_addr


//=============================================================================
/* Convenience defines */
#define SA      struct sockaddr
#define SAIN    struct sockaddr_in
/* 578 byte UDP MTU/MRU minus slack for packet header */
#define MAXLINE 570


//=============================================================================
class WrapUnix
{
public:
  typedef void    Sigfunc(int);   /* for signal handlers */
public:
  WrapUnix();
  ~WrapUnix();

  int bind(int, const struct sockaddr *, socklen_t);
  char *fgets(char *, int, FILE *);
  pid_t fork();
  void fputs(const char *, FILE *);
  void inet_aton(const char *, struct in_addr *);
  int inet_pton(int, const char *, void *);
  void *malloc(size_t);
  ssize_t recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);
  void sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
  void setsockopt(int, int, int, const void *, socklen_t);
  char *sock_ntop(const struct sockaddr *, socklen_t); /* NOT THREAD SAFE! */
  int socket(int, int, int);
  Sigfunc *Signal(int, Sigfunc *);

  // !!FIXME!! move to logging class and cleanup. -DM 11/17/01
  void err_quit(const char *, ...);
  void err_sys(const char *, ...);
  void debug_msg(const char *, ...);
  void notice_msg(const char *, ...);
  void warning_msg(const char *, ...);

private:
  Sigfunc *signal(int, Sigfunc *);

  void err_doit(int errnoflag, int level, const char *fmt, va_list ap);
};


#endif
