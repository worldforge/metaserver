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
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The author can be reached via e-mail to dragonm@hypercubepc.com
*/
#include <unistd.h>
#include <sys/timeb.h>
#include <time.h>
#include "wrap.h"
#include "protocol_instructions.h"

void metaserver_listreq(int, const SA *, socklen_t);
void writehtml(const char *);
char *pack_uint32(uint32_t, char *, unsigned int *);
char *unpack_uint32(uint32_t *, char *);

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
  printf("Sending server list request...\n");
  metaserver_listreq(sockfd, pservaddr, servlen);
}

void metaserver_listreq(int sockfd, const SA *servaddr, socklen_t servlen)
{
  char           mesg[MAXLINE];
  char          *mesg_ptr;
  uint32_t       handshake=0, command=0, total=1, packed, from=0;
  SA             addr;
  socklen_t      addrlen;
  unsigned int   packet_size;
  int            count;
  char           str[128];
  char          *html;
  int            htmllen = 1024;
  int            htmlwhere = 0;
  struct in_addr temp;

  packet_size = 0;
  mesg_ptr = pack_uint32(CKEEP_ALIVE, mesg, &packet_size);
  Sendto(sockfd, mesg, packet_size, 0, servaddr, servlen);

  addrlen = sizeof(addr);
  Recvfrom(sockfd, mesg, MAXLINE, 0, &addr, &addrlen);
  mesg_ptr = unpack_uint32(&command, mesg);
  if(command == HANDSHAKE)
  {
    mesg_ptr = unpack_uint32(&handshake, mesg_ptr);
    printf("Server contacted successfully.\n");

    packet_size = 0;
    mesg_ptr = pack_uint32(CLIENTSHAKE, mesg, &packet_size);
    mesg_ptr = pack_uint32(handshake, mesg_ptr, &packet_size);
    Sendto(sockfd, mesg, packet_size, 0, servaddr, servlen);
  }

  html = (char *)malloc(htmllen);
  while(1)
  {
    if(from > total || total == 0)
      break;
    packet_size = 0;
    mesg_ptr = pack_uint32(LIST_REQ, mesg, &packet_size);
    mesg_ptr = pack_uint32(from, mesg_ptr, &packet_size);
    Sendto(sockfd, mesg, packet_size, 0, servaddr, servlen);

    addrlen = sizeof(addr);
    Recvfrom(sockfd, mesg, MAXLINE, 0, &addr, &addrlen);
    mesg_ptr = unpack_uint32(&command, mesg);
    if(command == PROTO_ERANGE)
    {
      printf("Server said Range Error.\n");
      break;
    }
    else if(command == LIST_RESP)
    {
      printf("Received server list packet.\n");
      mesg_ptr = unpack_uint32(&total, mesg_ptr);
      mesg_ptr = unpack_uint32(&packed, mesg_ptr);
      printf("Received %u servers of %u total.\n", packed, total);
      for(count = packed; count > 0; count--)
      {
	mesg_ptr = unpack_uint32(&temp.s_addr, mesg_ptr);
	inet_ntop(AF_INET, &temp, str, sizeof(str));
	printf("Server at: %s\n", str);
        if(htmllen < htmlwhere + 24)
        {
          htmllen += 1024;
          html = (char *)realloc(html, htmllen);
        }
        htmlwhere += sprintf(html + htmlwhere, "<h3>%s</h3>\n", str);
      }
      from += packed;
    }
    else
    {
      printf("Received unexpected packet: %d\n", command);
      break;
    }
  }
  writehtml(html);
  free(html);
}

void writehtml(const char *html)
{
  FILE          *htmlout;
  struct timeb   currenttime;
  char          *timestr;

  htmlout = fopen("metaserver.txt", "w");
  fputs(html, htmlout);

  tzset();
  ftime(&currenttime);
  timestr = ctime(&(currenttime.time));
  timestr[strlen(timestr)-1] = '\0';
  fprintf(htmlout, "<p>\n%s %s</p>", timestr, daylight ? tzname[1] : tzname[0]);
  fclose(htmlout);
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
