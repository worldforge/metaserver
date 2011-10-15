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
#include "dgclibig.h"

#define DEFAULT_SERV_PORT           8453

int main(int argc, char **argv)
{
  int                metaserver_port = DEFAULT_SERV_PORT;
  int                sockfd;
  struct sockaddr_in sa;

  if(argc < 2 || argc >  3)
    err_quit("usage: testclient <hostname> [<port>]");

  if(argc == 3)
    sscanf(argv[2], "%d", &metaserver_port);

  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(metaserver_port);
  Inet_pton(AF_INET, argv[1], &sa.sin_addr);

  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  dg_cli(stdin, sockfd, (SA *)&sa, sizeof(sa));

  exit(0);
}
