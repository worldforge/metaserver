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
#include <unistd.h>
#include <list>
#include "wrap.hh"
#include "protocol_instructions.h"

class Handshake
{
  public:
    Handshake() {}
    Handshake(uint32_t a, uint32_t h, time_t t)
      { set_values(a, h, t); }
    Handshake(const Handshake &rhs)
      { addr = rhs.addr; handshake = rhs.handshake; 
        timestamp = rhs.timestamp; }
    ~Handshake() {}
    void set_values(uint32_t a, uint32_t h, time_t t)
      { addr = a; handshake = h; timestamp = t; }
    void set_timestamp(time_t t)
      { timestamp = t; }
    uint32_t get_address() { return addr; }
    uint32_t get_handshake() { return handshake; }
    time_t get_timestamp() { return timestamp; }
  private:
    uint32_t addr;
    uint32_t handshake;
    time_t   timestamp;
};

enum list_type { SERVER_HANDSHAKE_LIST, CLIENT_HANDSHAKE_LIST, SERVER_LIST, 
                 CLIENT_LIST };

void recvfrom_int(int);
void reaper(int);
void reap_list(list_type);
char *pack_uint32(uint32_t, char *, unsigned int *);
char *unpack_uint32(uint32_t *, char *);
char *polished_presentation(char *);
template<class ForwardIterator, class T>
ForwardIterator find_by_hs(ForwardIterator, ForwardIterator, T&);
template<class ForwardIterator, class T>
ForwardIterator find_by_addr(ForwardIterator, ForwardIterator, T&);

int packet_count;
list<Handshake> active_servers;
list<Handshake> active_clients;
list<Handshake> server_handshakes;
list<Handshake> client_handshakes;
int peak_active_servers = 0;
int peak_active_clients = 0;
int peak_pending_server_handshakes = 0;
int peak_pending_client_handshakes = 0;

#ifndef DEBUG
#define REAP_INTERVAL 930 /* 15 minutes plus 30 seconds */
#else
#define REAP_INTERVAL 5
#endif

void main_loop(int sockfd, SA *pcliaddr, socklen_t clilen)
{
  socklen_t	            len;
  char		            mesg[MAXLINE];
  char                     *mesg_ptr;
  uint32_t                  handshake=0, command=0, from=0;
  Handshake                 hs;
  Handshake                 search_for;
  SAIN                     *sa;
  char                     *presentation;
  list<Handshake>::iterator i;
  int                       active;
  unsigned int              buff_len;
  unsigned int              packet_size;

  Signal(SIGINT, recvfrom_int);
  Signal(SIGALRM, reaper);
  alarm(REAP_INTERVAL);
  for( ; ; )
  {
    len = clilen;
    if(Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len) < 0)
      continue;

    sa = (SAIN *)pcliaddr;

    mesg_ptr = unpack_uint32(&command, mesg);
    switch(command)
    {
      case SKEEP_ALIVE:
        handshake = random();

        hs.set_values(sa->sin_addr.s_addr, handshake, time(NULL));
        server_handshakes.push_front(hs);
        if((active = server_handshakes.size()) > 
           peak_pending_server_handshakes)
          peak_pending_server_handshakes = active;

        packet_size = 0;
        mesg_ptr = pack_uint32(HANDSHAKE, mesg, &packet_size);
        mesg_ptr = pack_uint32(handshake, mesg_ptr, &packet_size);
        Sendto(sockfd, mesg, packet_size, 0, pcliaddr, len);
#ifdef DEBUG
        presentation = Sock_ntop(pcliaddr, sizeof(SA));
        cout << "Received server keepalive from " 
             << polished_presentation(presentation) 
             << endl;
#endif
        break;
      case CKEEP_ALIVE:
        handshake = random();

        hs.set_values(sa->sin_addr.s_addr, handshake, time(NULL));
        client_handshakes.push_front(hs);
        if((active = client_handshakes.size()) > 
           peak_pending_client_handshakes)
          peak_pending_client_handshakes = active;

        packet_size = 0;
        mesg_ptr = pack_uint32(HANDSHAKE, mesg, &packet_size);
        mesg_ptr = pack_uint32(handshake, mesg_ptr, &packet_size);
        Sendto(sockfd, mesg, packet_size, 0, pcliaddr, len);
#ifdef DEBUG
        presentation = Sock_ntop(pcliaddr, sizeof(SA));
        cout << "Received client keepalive from "
             << polished_presentation(presentation)
             << endl;
#endif
        break;
      case SERVERSHAKE:
        mesg_ptr = unpack_uint32(&handshake, mesg_ptr);
        search_for.set_values(sa->sin_addr.s_addr, handshake, 0);
        i = find_by_hs(server_handshakes.begin(), server_handshakes.end(), 
                       search_for);
        if(i != server_handshakes.end())
        {
#ifdef DEBUG
          presentation = Sock_ntop(pcliaddr, sizeof(SA));
          cout << "Received valid server handshake from " 
               << polished_presentation(presentation) 
               << endl;
#endif
          server_handshakes.erase(i);
          i = find_by_addr(active_servers.begin(), active_servers.end(), 
                           search_for);
          if(i == active_servers.end())
          {
            search_for.set_values(sa->sin_addr.s_addr, 0, time(NULL));
            active_servers.push_front(search_for);

            if((active = active_servers.size()) > peak_active_servers)
              peak_active_servers = active;
#ifdef DEBUG
            presentation = Sock_ntop(pcliaddr, sizeof(SA));
            cout << "Added new active server at "
                 << polished_presentation(presentation) 
                 << endl;
#endif
          }
          else
          {
            (*i).set_timestamp(time(NULL));
#ifdef DEBUG
            presentation = Sock_ntop(pcliaddr, sizeof(SA));
            cout << "Updated existing active server at "
                 << polished_presentation(presentation)
                 << endl;
#endif
          }
        }
        else
        {
          presentation = Sock_ntop(pcliaddr, sizeof(SA));
          cerr << "Bogus SERVERSHAKE packet received from: " 
               << polished_presentation(presentation)
               << endl;
        }
        break;
      case CLIENTSHAKE:
        mesg_ptr = unpack_uint32(&handshake, mesg_ptr);
        search_for.set_values(sa->sin_addr.s_addr, handshake, 0);
        i = find_by_hs(client_handshakes.begin(), client_handshakes.end(), 
                       search_for);
        if(i != client_handshakes.end())
        {
#ifdef DEBUG
          presentation = Sock_ntop(pcliaddr, sizeof(SA));
          cout << "Received valid handshake from " 
               << polished_presentation(presentation) 
               << endl;
#endif
          client_handshakes.erase(i);
          i = find_by_addr(active_clients.begin(), active_clients.end(), 
                             search_for);
          if(i == active_clients.end())
          {
            search_for.set_values(sa->sin_addr.s_addr, 0, time(NULL));
            active_clients.push_front(search_for);

            if((active = active_clients.size()) > peak_active_clients)
              peak_active_clients = active;
#ifdef DEBUG
            presentation = Sock_ntop(pcliaddr, sizeof(SA));
            cout << "Added new active client at "
                 << polished_presentation(presentation)
                 << endl;
#endif
          }
          else
          {
            (*i).set_timestamp(time(NULL));
#ifdef DEBUG
            presentation = Sock_ntop(pcliaddr, sizeof(SA));
            cout << "Updated existing client at "
                 << polished_presentation(presentation)
                 << endl;
#endif
          }
        }
        else
        {
          presentation = Sock_ntop(pcliaddr, sizeof(SA));
          cerr << "Bogus CLIENTSHAKE packet received from: " 
               << polished_presentation(presentation)
               << endl;
        }
        break;
      case TERMINATE:
        search_for.set_values(sa->sin_addr.s_addr, handshake, 0);
        i = find_by_addr(active_servers.begin(), active_servers.end(), 
                         search_for);
        if(i != active_servers.end())
        {
#ifdef DEBUG
          presentation = Sock_ntop(pcliaddr, sizeof(SA));
          cout << "Received termination from server at "
               << polished_presentation(presentation) 
               << endl;
#endif
          active_servers.erase(i);
        }
        else
        {
          presentation = Sock_ntop(pcliaddr, sizeof(SA));
          cerr << "Received TERMINATION packet for unlisted server from: "
               << polished_presentation(presentation)
               << endl;
        }
        break;
      case LIST_REQ:
        mesg_ptr = unpack_uint32(&from, mesg_ptr);

        i = active_servers.begin();
        if(from)
          while(i != active_servers.end())
            if((*i).get_address() == from)
              break;

        packet_size = 0;
        if(i == active_servers.end())
        {
          if(active_servers.size() == 0)
          {
            mesg_ptr = pack_uint32(LIST_RESP, mesg, &packet_size);
            mesg_ptr = pack_uint32(active_servers.size(), mesg_ptr, 
                                   &packet_size);
            mesg_ptr = pack_uint32(0, mesg_ptr, &packet_size);
          }
          else
            mesg_ptr = pack_uint32(PROTO_ERANGE, mesg, &packet_size);
        }
        else
        {
          mesg_ptr = pack_uint32(LIST_RESP, mesg, &packet_size);
          mesg_ptr = pack_uint32(active_servers.size(), mesg_ptr, 
                                 &packet_size);
          mesg_ptr += sizeof(uint32_t);
          buff_len = MAXLINE;
          while(i != active_servers.end() && buff_len > sizeof(uint32_t))
          {
            mesg_ptr = pack_uint32((*i).get_address(), mesg_ptr, &packet_size);
            i++;
            buff_len -= sizeof(uint32_t);
          }
          mesg_ptr = mesg + sizeof(uint32_t)*2;
          pack_uint32(distance(active_servers.begin(), i), mesg_ptr, 
                    &packet_size);
        }
        Sendto(sockfd, mesg, packet_size, 0, pcliaddr, len);

        break;
    }
    packet_count++;
  }
}

template<class ForwardIterator, class T>
ForwardIterator find_by_hs(ForwardIterator first, ForwardIterator last, 
                           T& value)
{
  while(first != last)
  {
    if((*first).get_handshake() == value.get_handshake())
      return first;
    first++;
  }
  return last;
}

template<class ForwardIterator, class T>
ForwardIterator find_by_addr(ForwardIterator first, ForwardIterator last, 
                           T& value)
{
  while(first != last)
  {
    if((*first).get_address() == value.get_address())
      return first;
    first++;
  }
  return last;
}

void recvfrom_int(int signo)
{
  cout << "Interrupted.  Shutting down." << endl;
  cout << "Received " << packet_count << " datagrams" << endl;
  cout << "There were " << active_servers.size() << " active servers." << endl;
  cout << "There were " << server_handshakes.size() 
       << " pending server handshakes." << endl;
  cout << "There were " << client_handshakes.size() 
       << " pending client handshakes." << endl;
  cout << "There were " << peak_active_servers << " peak active servers" 
       << endl;
  cout << "There were " << peak_active_clients << " peak active clients" 
       << endl;
  cout << "There were " << peak_pending_server_handshakes 
       << " peak pending server handshakes" << endl;
  cout << "There were " << peak_pending_client_handshakes 
       << " peak pending client handshakes" << endl;
  exit(0);
}

void reaper(int signo)
{
  reap_list(SERVER_LIST);
  reap_list(CLIENT_LIST);
  reap_list(SERVER_HANDSHAKE_LIST);
  reap_list(CLIENT_HANDSHAKE_LIST);
  alarm(REAP_INTERVAL);
}

void reap_list(list_type which)
{
  list<Handshake>::iterator lead, last;
  list<Handshake> *list_ptr;
  int active, *peak;

  switch(which)
  {
    case SERVER_LIST:
      peak = &peak_active_servers;
      list_ptr = &active_servers;
      break;
    case CLIENT_LIST:
      peak = &peak_active_clients;
      list_ptr = &active_clients;
      break;
    case SERVER_HANDSHAKE_LIST:
      peak = &peak_pending_server_handshakes;
      list_ptr = &server_handshakes;
    case CLIENT_HANDSHAKE_LIST:
      peak = &peak_pending_client_handshakes;
      list_ptr = &client_handshakes;
  }
  if((active = (*list_ptr).size()) > *peak)
        *peak = active;
  lead = (*list_ptr).begin();
  last = (*list_ptr).end();

  while(lead != last)
  {
    if(time(NULL) > (*lead).get_timestamp() + REAP_INTERVAL)
    {
#ifdef DEBUG
      char str[128];
      struct in_addr temp;
      temp.s_addr = (*lead).get_address();
      inet_ntop(AF_INET, &temp, str, sizeof(str));
      switch(which)
      {
        case SERVER_LIST:
          cout << "Reaping stale server: " << str << endl;
          break;
        case CLIENT_LIST:
          cout << "Reaping stale client: " << str << endl;
          break;
        case SERVER_HANDSHAKE_LIST:
          cout << "Reaping stale server handshake: " << str << endl;
        case CLIENT_HANDSHAKE_LIST:
          cout << "Reaping stale client handshake: " << str << endl;
      }
#endif
      lead = (*list_ptr).erase(lead);
    }
    else
      lead++;
  }
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

char *polished_presentation(char *presentation)
{
  char           *walk;

  for(walk = presentation+strlen(presentation); walk != presentation; walk--)
    if(*walk == ':')
    {
      *walk = '\0';
      break;
    }
  return presentation;
}
