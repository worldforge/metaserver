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
#include <unistd.h>
#include "wrap.h"
#include "protocol_instructions.h"

void metaserver_keepalive(int, const SA *, socklen_t);
void metaserver_terminate(int, const SA *, socklen_t);
char *pack_uint32(uint32_t, char *, unsigned int *);
char *unpack_uint32(uint32_t *, char *);

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
  int i;

  printf("Sending 5 server keep alive msgs, spaced 5 seconds apart...\n");
  for(i=0; i < 5; i++)
  {
    metaserver_keepalive(sockfd, pservaddr, servlen);
    sleep(5);
  }
  printf("Waiting for 2X debug reap interval...\n");
  sleep(10);
  printf("Sending 1 server keep alive msg...\n");
  metaserver_keepalive(sockfd, pservaddr, servlen);
  printf("Sending 1 server terminate msg...\n");
  metaserver_terminate(sockfd, pservaddr, servlen);
  printf("Waiting for 2X debug reap interval...\n");
  sleep(10);
  printf("Sending 1 server terminate msg for unlisted server...\n");
  metaserver_terminate(sockfd, pservaddr, servlen);
}

void metaserver_keepalive(int sockfd, const SA *servaddr, socklen_t servlen)
{
  char         mesg[MAXLINE];
  char        *mesg_ptr;
  uint32_t     handshake=0, command=0;
  SA           addr;
  socklen_t    addrlen;
  unsigned int packet_size;

  packet_size = 0;
  mesg_ptr = pack_uint32(SKEEP_ALIVE, mesg, &packet_size);
  Sendto(sockfd, mesg, packet_size, 0, servaddr, servlen);

  addrlen = sizeof(addr);
  Recvfrom(sockfd, mesg, MAXLINE, 0, &addr, &addrlen);
  mesg_ptr = unpack_uint32(&command, mesg);

  if(command == HANDSHAKE)
  {
    mesg_ptr = unpack_uint32(&handshake, mesg_ptr);
    printf("Server contacted successfully.\n");

    packet_size = 0;
    mesg_ptr = pack_uint32(SERVERSHAKE, mesg, &packet_size);
    mesg_ptr = pack_uint32(handshake, mesg_ptr, &packet_size);

    Sendto(sockfd, mesg, packet_size, 0, servaddr, servlen);
  }
}

void metaserver_terminate(int sockfd, const SA *servaddr, socklen_t servlen)
{
  char         mesg[MAXLINE];
  unsigned int packet_size=0;

  pack_uint32(TERMINATE, mesg, &packet_size);
  Sendto(sockfd, mesg, packet_size, 0, servaddr, servlen);
}

char *pack_uint32(uint32_t data, char *buffer, unsigned int *size)
{
  uint32_t netorder;

  netorder = htonl(data);
  memcpy(buffer, &netorder, sizeof(uint32_t));
  *size += sizeof(uint32_t);
  return buffer+sizeof(uint32_t);
}

char *unpack_uint32(uint32_t *dest, char *buffer)
{
  uint32_t netorder;

  memcpy(&netorder, buffer, sizeof(uint32_t));
  *dest = ntohl(netorder);
  return buffer+sizeof(uint32_t);
}
