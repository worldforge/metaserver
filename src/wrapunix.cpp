/*
    Generic Game Metaserver

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
/*
    The code in this file is adapted from various source files distributed 
    with Unix Network Programming Volume 1 by W. Richard Stevens
*/
#include "wrapunix.hh"

#include <arpa/inet.h>          // for inet_aton(), inet_pton()
#include <csignal>              // for SIG_ERR, SIGALRM, sigaction(), 
                                // sigemptyset()
#include <errno.h>              // for errno
#ifdef	HAVE_SOCKADDR_DL_STRUCT
# include	<net/if_dl.h>
#endif
#include <stdlib.h>             // for malloc()
#include <string.h>             // for strerror(), strcat()
#include <syslog.h>             // for syslog(), LOG_ERR, LOG_DEBUG, 
                                // LOG_NOTICE, LOG_WARNING
#include <sys/un.h>             // for struct sockaddr_un
#include <unistd.h>             // for fork()

int daemon_proc;   /* set nonzero by daemonize() */

#define HAVE_VSNPRINTF

//=============================================================================
WrapUnix::WrapUnix()
{
}


//=============================================================================
WrapUnix::~WrapUnix()
{
}


//=============================================================================
void WrapUnix::err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
  int   errno_save, n;
  char  buf[MAXLINE];

  errno_save = errno;	        /* value caller might want printed */
#ifdef	HAVE_VSNPRINTF
  vsnprintf(buf, sizeof(buf), fmt, ap);	/* this is safe */
#else
  vsprintf(buf, fmt, ap);               /* this is not safe */
#endif
  n = strlen(buf);
  if(errnoflag)
    snprintf(buf+n, sizeof(buf)-n, ": %s", strerror(errno_save));
  strcat(buf, "\n");

  if(daemon_proc)
    syslog(level, buf);
  else
  {
    fflush(stdout);             /* in case stdout and stderr are the same */
    fputs(buf, stderr);
    fflush(stderr);
  }
  return;
}

//=============================================================================
/* Fatal error related to a system call.
 * Print a message and terminate. */
void WrapUnix::err_sys(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(1, LOG_ERR, fmt, ap);
  va_end(ap);
  exit(1);
}

//=============================================================================
/* Fatal error unrelated to a system call.
 * Print a message and terminate. */
void WrapUnix::err_quit(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(0, LOG_ERR, fmt, ap);
  va_end(ap);
  exit(1);
}

//=============================================================================
/* Debug message.  Print a message. */
void WrapUnix::debug_msg(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(0, LOG_DEBUG, fmt, ap);
  va_end(ap);
}

/* Notice message. Print a message. */

//=============================================================================
void WrapUnix::notice_msg(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(0, LOG_NOTICE, fmt, ap);
  va_end(ap);
}

/* Warning message. Print a warning. */

//=============================================================================
void WrapUnix::warning_msg(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(0, LOG_WARNING, fmt, ap);
  va_end(ap);
}

//=============================================================================
int WrapUnix::bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
  int rc;

  if((rc = ::bind(fd, sa, salen)) < 0)
    err_sys("bind error");
  return rc;
}


//=============================================================================
char *WrapUnix::fgets(char *ptr, int n, FILE *stream)
{
  char	*rptr;

  if((rptr = ::fgets(ptr, n, stream)) == NULL && ferror(stream))
    err_sys("fgets error");

  return (rptr);
}


//=============================================================================
pid_t WrapUnix::fork(void)
{
  pid_t	pid;

  if((pid = ::fork()) == -1)
    err_sys("fork error");
  return(pid);
}


//=============================================================================
void WrapUnix::fputs(const char *ptr, FILE *stream)
{
  if(::fputs(ptr, stream) == EOF)
    err_sys("fputs error");
}


//=============================================================================
void WrapUnix::inet_aton(const char *cp, struct in_addr *inp)
{
  int n;

  if(!(n = ::inet_aton(cp, inp)))
    err_quit("'%s' is not a valid decimal-and-dots IP address", cp);
}


//=============================================================================
int WrapUnix::inet_pton(int family, const char *strptr, void *addrptr)
{
  int n;

  if((n = ::inet_pton(family, strptr, addrptr)) < 0)
    err_sys("inet_pton error for %s", strptr);	/* errno set */
  else if (n == 0)
    err_quit("inet_pton error for %s", strptr);	/* errno not set */

  return n;
}


//=============================================================================
void *WrapUnix::malloc(size_t size)
{
  void	*ptr;

  if((ptr = ::malloc(size)) == NULL)
    err_sys("malloc error");
  return(ptr);
}


//=============================================================================
ssize_t WrapUnix::recvfrom(int fd, void *ptr, size_t nbytes, int flags,
			   struct sockaddr *sa, socklen_t *salenptr)
{
  ssize_t n;

  if((n = ::recvfrom(fd, ptr, nbytes, flags, sa, salenptr)) < 0)
    if(errno != EINTR)
      err_sys("recvfrom error");
  return(n);
}


//=============================================================================
void WrapUnix::sendto(int fd, const void *ptr, size_t nbytes, int flags,
		      const struct sockaddr *sa, socklen_t salen)
{
  if(::sendto(fd, ptr, nbytes, flags, sa, salen) != (int)nbytes)
    err_sys("sendto error");
}


//=============================================================================
void WrapUnix::setsockopt(int fd, int level, int optname, const void *optval, 
			  socklen_t optlen)
{
  if(::setsockopt(fd, level, optname, optval, optlen) < 0)
    err_sys("setsockopt error");
}


//=============================================================================
char *WrapUnix::sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
  char        portstr[7];
  static char str[128];		/* Unix domain is largest */

  switch (sa->sa_family)
  {
    case AF_INET:
    {
      struct sockaddr_in *sin = (struct sockaddr_in *) sa;

      if(inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
        return(NULL);
      if(ntohs(sin->sin_port) != 0)
      {
        snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
        strcat(str, portstr);
      }
      return(str);
    }
    /* end sock_ntop */

#ifdef	IPV6
    case AF_INET6:
    {
      struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;

      if(inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL)
        return(NULL);
      if(ntohs(sin6->sin6_port) != 0)
      {
        snprintf(portstr, sizeof(portstr), ".%d", ntohs(sin6->sin6_port));
        strcat(str, portstr);
      }
      return(str);
    }
#endif

#ifdef	AF_UNIX
    case AF_UNIX:
    {
      struct sockaddr_un *unp = (struct sockaddr_un *) sa;

      /* OK to have no pathname bound to the socket: happens on
         every connect() unless client calls bind() first. */
      if(unp->sun_path[0] == 0)
        strcpy(str, "(no pathname bound)");
      else
        snprintf(str, sizeof(str), "%s", unp->sun_path);
      return(str);
    }
#endif

#ifdef	HAVE_SOCKADDR_DL_STRUCT
    case AF_LINK:
    {
      struct sockaddr_dl *sdl = (struct sockaddr_dl *) sa;

      if(sdl->sdl_nlen > 0)
        snprintf(str, sizeof(str), "%*s",
                 sdl->sdl_nlen, &sdl->sdl_data[0]);
      else
        snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
      return(str);
    }
#endif
    default:
      snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d",
               sa->sa_family, salen);
      return(str);
  }
  return (NULL);
}

//============================================================================
int WrapUnix::socket(int family, int type, int protocol)
{
  int n;

  if((n = ::socket(family, type, protocol)) < 0)
    err_sys("socket error");
  return(n);
}


//=============================================================================
WrapUnix::Sigfunc *WrapUnix::Signal(int signo, WrapUnix::Sigfunc *func)      /* for our signal() function */
{
  Sigfunc *sigfunc;

  if((sigfunc = signal(signo, func)) == SIG_ERR)
    err_sys("signal error");
  return(sigfunc);
}


//=============================================================================
WrapUnix::Sigfunc *WrapUnix::signal(int signo, WrapUnix::Sigfunc *func)
{
  struct sigaction act, oact;

  act.sa_handler = func;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if(signo == SIGALRM)
  {
#ifdef  SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;   /* SunOS 4.x */
#endif
  }
  else
  {
#ifdef  SA_RESTART
    act.sa_flags |= SA_RESTART;             /* SVR4, 44BSD */
#endif
  }
  if(sigaction(signo, &act, &oact) < 0)
    return(SIG_ERR);
  return(oact.sa_handler);
}
