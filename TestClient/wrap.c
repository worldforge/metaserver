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
#include "wrap.h"

/* Print a message and return to caller.
 * Caller specifies "errnoflag" and "level". */

#define HAVE_VSNPRINTF

void err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
  int   errno_save, n;
  char  buf[MAXLINE];

  errno_save = errno;	        /* value caller might want printed */
#ifdef	HAVE_VSNPRINTF
  vsnprintf(buf, sizeof(buf), fmt, ap);	/* this is safe */
#else
  vsprintf(buf, fmt, ap);		     /* this is not safe */
#endif
  n = strlen(buf);
  if(errnoflag)
    snprintf(buf+n, sizeof(buf)-n, ": %s", strerror(errno_save));
  strcat(buf, "\n");

  fflush(stdout);	        /* in case stdout and stderr are the same */
  fputs(buf, stderr);
  fflush(stderr);

  return;
}

/* Fatal error related to a system call.
 * Print a message and terminate. */

void err_sys(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(1, LOG_ERR, fmt, ap);
  va_end(ap);
  exit(1);
}

/* Fatal error unrelated to a system call.
 * Print a message and terminate. */

void err_quit(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(0, LOG_ERR, fmt, ap);
  va_end(ap);
  exit(1);
}

void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
  if(bind(fd, sa, salen) < 0)
    err_sys("bind error");
}

char *Fgets(char *ptr, int n, FILE *stream)
{
  char	*rptr;

  if((rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
    err_sys("fgets error");

  return (rptr);
}

void Fputs(const char *ptr, FILE *stream)
{
  if(fputs(ptr, stream) == EOF)
    err_sys("fputs error");
}

void Inet_pton(int family, const char *strptr, void *addrptr)
{
  int n;

  if((n = inet_pton(family, strptr, addrptr)) < 0)
    err_sys("inet_pton error for %s", strptr);	/* errno set */
  else if (n == 0)
    err_quit("inet_pton error for %s", strptr);	/* errno not set */
}

void *Malloc(size_t size)
{
  void	*ptr;

  if((ptr = malloc(size)) == NULL)
    err_sys("malloc error");
  return(ptr);
}

ssize_t Recvfrom(int fd, void *ptr, size_t nbytes, int flags,
                 struct sockaddr *sa, socklen_t *salenptr)
{
  ssize_t n;

  if((n = recvfrom(fd, ptr, nbytes, flags, sa, salenptr)) < 0)
    err_sys("recvfrom error");
  return(n);
}

void Sendto(int fd, const void *ptr, size_t nbytes, int flags,
            const struct sockaddr *sa, socklen_t salen)
{
  if(sendto(fd, ptr, nbytes, flags, sa, salen) != nbytes)
    err_sys("sendto error");
}

void Setsockopt(int fd, int level, int optname, const void *optval, 
                socklen_t optlen)
{
  if(setsockopt(fd, level, optname, optval, optlen) < 0)
    err_sys("setsockopt error");
}

int Socket(int family, int type, int protocol)
{
  int n;

  if((n = socket(family, type, protocol)) < 0)
    err_sys("socket error");
  return(n);
}

Sigfunc *signal(int signo, Sigfunc *func)
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
/* end signal */

Sigfunc *Signal(int signo, Sigfunc *func)      /* for our signal() function */
{
  Sigfunc *sigfunc;

  if((sigfunc = signal(signo, func)) == SIG_ERR)
    err_sys("signal error");
  return(sigfunc);
}

