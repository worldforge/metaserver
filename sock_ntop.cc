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
#ifdef __FreeBSD__
#include <db.h>
#endif
#include <sys/un.h>
#include "wrap.hh"

#ifdef	HAVE_SOCKADDR_DL_STRUCT
# include	<net/if_dl.h>
#endif

char *sock_ntop(const struct sockaddr *sa, socklen_t salen)
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

char *Sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
  char	*ptr;

  if((ptr = sock_ntop(sa, salen)) == NULL)
    err_sys("sock_ntop error");	/* inet_ntop() sets errno */
  return(ptr);
}
