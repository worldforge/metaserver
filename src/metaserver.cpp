/*
    Generic Game Metaserver

    Copyright (C) 2000-2001 Dragon Master

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

    This software was implemented based on the following article:

    Half-Life and TeamFortress Networking: Closing the Loop on Scalable 
      Network Gaming Backend Services
    By Yahn Bernier 
    Gamasutra
    May 11, 2000
    URL: http://www.gamasutra.com/features/20000511/bernier_01.htm

    Thanks to Yahn Bernier and Gamasutra.  The article saved having to 
    go back and correct what would have been at least one major blunder.
*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clientshakemsg.hh"
#include "handshake.hh"
#include "handshakemsg.hh"
#include "listrequestmsg.hh"
#include "listresponsemsg.hh"
#include "metaserver.hh"
#include "metaserver_version.h"
#include "handshakemsg.hh"
#include "serverkeepalivemsg.hh"
#include "servershakemsg.hh"
#include "sigalrmhandler.hh"
#include "siginthandler.hh"
#include "signalmanager.hh"
#include "terminatemsg.hh"

#include <arpa/inet.h>        // for inet_pton()
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include <stdlib.h>     // Solaris only?
#endif
#include <iostream>
#include <list>
#include <netdb.h>            // for gethostbyname(), hostent
#include <signal.h>           // for SIGHUP, SIG_IGN
#include <syslog.h>           // for syslog(), openlog() and LOG_PID
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>           // for setsid()

#define DEFAULT_LISTEN_PORT           8453

extern int daemon_proc;


//=============================================================================
Metaserver::Metaserver() : mDaemon(false),
			   mListenPort(DEFAULT_LISTEN_PORT),
			   mListenSocket(0),
			   mPacketCount(0),
			   mPeakActiveServers(0),
			   mPeakActiveClients(0),
			   mPeakPendingServerHandshakes(0),
			   mPeakPendingClientHandshakes(0),
			   mSigalrmHandler(0),
			   mSigintHandler(0)
{
  memset(&mBindAddress, 0, sizeof(mBindAddress));
  mExecute[0] = '\0';
}


//=============================================================================
Metaserver::~Metaserver()
{
  delete mSigintHandler;
  delete mSigalrmHandler;
}


//=============================================================================
bool Metaserver::Initialize(char *binaryName)
{
  struct sockaddr_in servaddr;

  if(mDaemon)
    Daemonize(binaryName, LOG_DAEMON);

  mSigalrmHandler = new SIGALRMHandler;
  mSigintHandler = new SIGINTHandler;

  SignalManager::GetInstance().RegisterHandler(SIGINT, mSigintHandler);

  mListenSocket = socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  memcpy(&servaddr.sin_addr, &mBindAddress, sizeof(struct in_addr));
  servaddr.sin_port        = htons(mListenPort);

  if(bind(mListenSocket, (SA *)&servaddr, sizeof(servaddr)))
    return false;
  else
    return true;
}


//=============================================================================
int Metaserver::ParseCommandline(int argc, char * argv[])
{
  int c;
  bool quiet = false;
  bool server_ip_specified = false;
  char server_ip[128];

  while (1)
  {
    int option_index = 0;
#ifndef sun      /* TODO: replace with config.h check */
    static struct option long_options[] =
    {
      // name, has_arg, return 0?, val
      {"help", 0, 0, 'h'},
      {"port", 1, 0, 'p'},
      {"quiet", 0, 0, 'q'},
      {"server-ip", 1, 0, 's'},
      {"version", 0, 0, 'v'},
      {"daemon", 0, 0, 'd'},
      {"execute", 1, 0, 'x'},
      {0, 0, 0, 0}
    };

    c = getopt_long (argc, argv, "hp:qs:vdx:", long_options, &option_index);
#else
    c = getopt (argc, argv, "hp:qs:vdx:");
#endif
    if (c == -1)
      break;

    switch (c)
    {
      case 0:
#ifdef sun
        cout << "Unsupported option due to Solaris stupidity...\n";
#else
        cout << "Unsupported long option " << long_options[option_index].name;
#endif
        if (optarg)
          cout << " with arg " << optarg;
        cout << endl;
        break;
      case 'h':
        VersionAndLicense();
        cout << endl;
        Help();
        exit(0);
        break;
      case 'p':
        sscanf(optarg, "%i", &mListenPort);
        break;
      case 'q':
        quiet = true;
        break;
      case 's':
        strncpy(server_ip, optarg, 128);
        if(LookupHost(optarg, &mBindAddress))
        {
          cout << "No such host. (" << optarg << ")" << endl;
          exit(1);
        }
        server_ip_specified = true;
        break;
      case 'v':
        VersionAndLicense();
        exit(0);
        break;
      case 'd':
        mDaemon = true;
        break;
      case 'x':
        strncpy(mExecute, optarg, 128);
      case '?':
        break;
      default:
        cerr << "?? getopt returned character code 0x" 
             << hex << c << dec << " ??" << endl;
    }
  }

  if(!server_ip_specified)
    mBindAddress.s_addr = INADDR_ANY;

  if(!quiet)
  {
    if (optind < argc)
    {
      cout << "Unrecognized crap on the commandline: ";
      while (optind < argc)
        cout << argv[optind++];
      cout << endl;
    }
    if(mListenPort != DEFAULT_LISTEN_PORT)
      cout << "Using server listen port " << mListenPort << endl;
    else
      cout << "Using default listen port " << mListenPort << endl;
    if(server_ip_specified)
      cout << "Binding to `" << server_ip << "'" << endl;
    else
      cout << "Binding to any IP." << endl;
  }

  return 1;
}


//=============================================================================
void Metaserver::VersionAndLicense()
{
  cout << long_version << "  "
       << copyright_license_no_warranty << endl;
}


//=============================================================================
void Metaserver::Help()
{
  cout << "-v   --version    Version" << endl
       << "-h   --help       Help" << endl
       << "-s   --server-ip  Server IP" << endl
       << "-p   --port       Server Listen Port" << endl
       << "-d   --daemon     Move Into Background" << endl
       << "-x   --execute    Execute Specified Command on Server List/Delist" << endl;
}


//=============================================================================
void Metaserver::Daemonize(const char *pname, int facility)
{
  int   i;
  int   tablesize=0;

  pid_t pid;

  if((pid = fork()) != 0)
    exit(0);         /* parent terminates */

  /* 1st child continues */
  setsid();          /* become session leader */

  Signal(SIGHUP, SIG_IGN);
  if((pid = fork()) != 0)
    exit(0);         /* 1st child terminates */

  /* 2nd child continues */
  daemon_proc = 1;   /* for our err_XXX() functions */

  chdir("/");        /* change working directory */

  umask(0);          /* clear our file mode creation mask */

  tablesize = getdtablesize();
  if(tablesize == -1)
    tablesize = 256; /* Default OPEN_MAX for Linux */

  for(i = 0; i < tablesize; i++)
    close(i);

  openlog(pname, LOG_PID, facility);
}


//=============================================================================
int Metaserver::LookupHost(const char *hostname, struct in_addr *inp)
{
  struct hostent *hoste;
  if ((hoste = gethostbyname(hostname)) != NULL)
  {
    inp = (struct in_addr *) *hoste->h_addr_list;
    return 0;
  }
  return 1;
}


//=============================================================================
void Metaserver::Process()
{
  SAIN                      clientAddr;
  socklen_t                 clientAddrLen = sizeof(clientAddr);
  unsigned char             mesg[MAXLINE];
  int                       bytes;

  SignalManager::GetInstance().RegisterHandler(SIGALRM, mSigalrmHandler);
  alarm(REAP_INTERVAL);
  for( ; ; )
  {
    if(mSigintHandler->GetGracefulQuit())
      Interrupted();
    if(mSigalrmHandler->GetAlarm())
    {
      mSigalrmHandler->SetAlarm(0);
      Reaper();
    }

    if((bytes = recvfrom(mListenSocket, mesg, MAXLINE, 0, (SA *)&clientAddr, 
			 &clientAddrLen)) < 0)
      continue;

    NetMsg netmsg((const unsigned char *)mesg, bytes);

    switch(netmsg.GetType())
    {
      case NMT_SERVERKEEPALIVE:
      {
	HandleServerKeepalive((SA *)&clientAddr, clientAddrLen);
        break;
      }
      case NMT_CLIENTKEEPALIVE:
      {
	HandleClientKeepalive((SA *)&clientAddr, clientAddrLen);
        break;
      }
      case NMT_SERVERSHAKE:
      {
	ServerShakeMsg msg(mesg, bytes);
	
	HandleServerShake((SA *)&clientAddr, msg);
        break;
      }
      case NMT_CLIENTSHAKE:
      {
	ClientShakeMsg msg(mesg, bytes);

	HandleClientShake((SA *)&clientAddr, msg);
        break;
      }
      case NMT_TERMINATE:
      {
	TerminateMsg msg(mesg, bytes);

	if(msg.Good())
	{
	  HandleTerminate((SA *)&clientAddr);
	}
        break;
      }
      case NMT_LISTREQ:
      {
	ListRequestMsg msg(mesg, bytes);

	if(msg.Good())
        {
	  HandleListRequest((SA *)&clientAddr, clientAddrLen, msg);
	}
        break;
      }
      case NMT_NULL:
      case NMT_HANDSHAKE:
      case NMT_LISTRESP:
      case NMT_PROTO_ERANGE:
      default:
#ifdef DEBUG
	printf("Ignoring unknown message type: %d\n", netmsg.GetType());
#endif
	break;
    }
    mPacketCount++;
  }
}


//=============================================================================
void Metaserver::Interrupted()
{
  notice_msg("Interrupted.  Shutting down.");
  notice_msg("Received %d datagrams", mPacketCount);
  notice_msg("There were %d active servers.", mActiveServers.size());
  notice_msg("There were %d pending server handshakes.", 
             mServerHandshakes.size());
  notice_msg("There were %d pending client handshakes.",
             mClientHandshakes.size());
  notice_msg("There were %d peak active servers", mPeakActiveServers);
  notice_msg("There were %d peak active clients", mPeakActiveClients);
  notice_msg("There were %d peak pending server handshakes", 
             mPeakPendingServerHandshakes);
  notice_msg("There were %d peak pending client handshakes", 
             mPeakPendingClientHandshakes);
  exit(0);
}


//=============================================================================
template<class ForwardIterator, class T>
ForwardIterator Metaserver::FindByHS(ForwardIterator first, 
				       ForwardIterator last, 
				       T& value)
{
  while(first != last)
  {
    if((*first).GetHandshake() == value.GetHandshake())
      return first;
    first++;
  }
  return last;
}


//=============================================================================
template<class ForwardIterator, class T>
ForwardIterator Metaserver::FindByAddr(ForwardIterator first, 
				       ForwardIterator last, 
				       T& value)
{
  while(first != last)
  {
    if((*first).GetAddress() == value.GetAddress())
      return first;
    first++;
  }
  return last;
}


//=============================================================================
void Metaserver::Reaper()
{
  ReapList(SERVER_LIST);
  ReapList(CLIENT_LIST);
  ReapList(SERVER_HANDSHAKE_LIST);
  ReapList(CLIENT_HANDSHAKE_LIST);
  alarm(REAP_INTERVAL);
}


//=============================================================================
void Metaserver::ReapList(ListType which)
{
  list<Handshake>::iterator lead, last;
  list<Handshake> *list_ptr;
  int active, *peak;

  switch(which)
  {
    case SERVER_LIST:
      peak = &mPeakActiveServers;
      list_ptr = &mActiveServers;
      break;
    case CLIENT_LIST:
      peak = &mPeakActiveClients;
      list_ptr = &mActiveClients;
      break;
    case SERVER_HANDSHAKE_LIST:
      peak = &mPeakPendingServerHandshakes;
      list_ptr = &mServerHandshakes;
      break;
    case CLIENT_HANDSHAKE_LIST:
      peak = &mPeakPendingClientHandshakes;
      list_ptr = &mClientHandshakes;
      break;
  }
  if((active = (*list_ptr).size()) > *peak)
        *peak = active;
  lead = (*list_ptr).begin();
  last = (*list_ptr).end();

  while(lead != last)
  {
    if(time(NULL) > (*lead).GetTimestamp() + REAP_INTERVAL)
    {
#ifdef DEBUG
      char str[128];
      struct in_addr temp;
      temp.s_addr = (*lead).GetAddress();
      inet_ntop(AF_INET, &temp, str, sizeof(str));
      switch(which)
      {
        case SERVER_LIST:
          if(mExecute[0] != '\0')
	    system(mExecute);
          debug_msg("Reaping stale server: %s", str);
          break;
        case CLIENT_LIST:
          debug_msg("Reaping stale client: %s", str);
          break;
        case SERVER_HANDSHAKE_LIST:
          debug_msg("Reaping stale server handshake: %s", str);
          break;
        case CLIENT_HANDSHAKE_LIST:
          debug_msg("Reaping stale client handshake: %s", str);
          break;
      }
#endif
      lead = (*list_ptr).erase(lead);
    }
    else
      lead++;
  }
}


//=============================================================================
void Metaserver::HandleServerKeepalive(SA *pcliaddr, socklen_t socketlen)
{
  unsigned int     handshake = random();
  int              active;
  Handshake        hs;
  char            *presentation;
  SAIN            *sa = (SAIN *)pcliaddr;

  hs.SetValues(sa->sin_addr.s_addr, handshake, time(NULL));
  mServerHandshakes.push_back(hs);
  if((active = mServerHandshakes.size()) > 
     mPeakPendingServerHandshakes)
    mPeakPendingServerHandshakes = active;

  HandshakeMsg msg(handshake);
  sendto(mListenSocket, msg.GetPackedBuffer(), msg.GetPackedBufferLength(), 
	 0, pcliaddr, socketlen);
#ifdef DEBUG
  presentation = sock_ntop(pcliaddr, sizeof(SA));
  debug_msg("Received server keepalive from %s", 
	    PolishedPresentation(presentation));
#endif
}


//=============================================================================
void Metaserver::HandleClientKeepalive(SA *pcliaddr, socklen_t socketlen)
{
  unsigned int     handshake = random();
  int              active;
  Handshake        hs;
  char            *presentation;
  SAIN            *sa = (SAIN *)pcliaddr;

  hs.SetValues(sa->sin_addr.s_addr, handshake, time(NULL));
  mClientHandshakes.push_back(hs);
  if((active = mClientHandshakes.size()) > 
     mPeakPendingClientHandshakes)
    mPeakPendingClientHandshakes = active;

  HandshakeMsg msg(handshake);
  sendto(mListenSocket, msg.GetPackedBuffer(), msg.GetPackedBufferLength(), 
	 0, pcliaddr, socketlen);
#ifdef DEBUG
  presentation = sock_ntop(pcliaddr, sizeof(SA));
  debug_msg("Received client keepalive from %s",
	    PolishedPresentation(presentation));
#endif
}


//=============================================================================
void Metaserver::HandleServerShake(SA *pcliaddr, const ServerShakeMsg &msg)
{
  int                       active;
  unsigned int              handshake;
  list<Handshake>::iterator i;
  char                     *presentation;
  SAIN                     *sa;
  Handshake                 search_for;

  sa = (SAIN *)pcliaddr;
  handshake = msg.GetHandshake();
  search_for.SetValues(sa->sin_addr.s_addr, handshake, 0);
  i = FindByHS(mServerHandshakes.begin(), mServerHandshakes.end(), 
	       search_for);
  if(i != mServerHandshakes.end())
  {
#ifdef DEBUG
    presentation = sock_ntop(pcliaddr, sizeof(SA));
    debug_msg("Received valid server handshake from %s", 
	      PolishedPresentation(presentation));
#endif
    mServerHandshakes.erase(i);
    i = FindByAddr(mActiveServers.begin(), mActiveServers.end(), 
		   search_for);
    if(i == mActiveServers.end())
    {
      search_for.SetValues(sa->sin_addr.s_addr, 0, time(NULL));
      mActiveServers.push_back(search_for);

      if(mExecute[0] != '\0')
        system(mExecute);

      if((active = mActiveServers.size()) > mPeakActiveServers)
	mPeakActiveServers = active;
#ifdef DEBUG
      presentation = sock_ntop(pcliaddr, sizeof(SA));
      debug_msg("Added new active server at %s",
		PolishedPresentation(presentation));
#endif
    }
    else
    {
      (*i).SetTimestamp(time(NULL));
#ifdef DEBUG
      presentation = sock_ntop(pcliaddr, sizeof(SA));
      debug_msg("Updated existing active server at %s",
		PolishedPresentation(presentation));
#endif
    }
  }
  else
  {
    presentation = sock_ntop(pcliaddr, sizeof(SA));
    warning_msg("Bogus SERVERSHAKE packet received from: %s", 
		PolishedPresentation(presentation));
  }
}


//=============================================================================
void Metaserver::HandleClientShake(SA *pcliaddr, ClientShakeMsg &msg)
{
  int                       active;
  unsigned int              handshake;
  list<Handshake>::iterator i;
  char                     *presentation;
  SAIN                     *sa;
  Handshake                 search_for;

  sa = (SAIN *)pcliaddr;
  handshake = msg.GetHandshake();
  search_for.SetValues(sa->sin_addr.s_addr, handshake, 0);
  i = FindByHS(mClientHandshakes.begin(), mClientHandshakes.end(), 
	       search_for);
  if(i != mClientHandshakes.end())
  {
#ifdef DEBUG
    presentation = sock_ntop(pcliaddr, sizeof(SA));
    debug_msg("Received valid handshake from %s", 
	      PolishedPresentation(presentation));
#endif
    mClientHandshakes.erase(i);
    i = FindByAddr(mActiveClients.begin(), mActiveClients.end(), 
		   search_for);
    if(i == mActiveClients.end())
    {
      search_for.SetValues(sa->sin_addr.s_addr, 0, time(NULL));
      mActiveClients.push_back(search_for);

      if((active = mActiveClients.size()) > mPeakActiveClients)
	mPeakActiveClients = active;
#ifdef DEBUG
      presentation = sock_ntop(pcliaddr, sizeof(SA));
      debug_msg("Added new active client at %s",
		PolishedPresentation(presentation));
#endif
    }
    else
    {
      (*i).SetTimestamp(time(NULL));
#ifdef DEBUG
      presentation = sock_ntop(pcliaddr, sizeof(SA));
      debug_msg("Updated existing client at %s",
		PolishedPresentation(presentation));
#endif
    }
  }
  else
  {
    presentation = sock_ntop(pcliaddr, sizeof(SA));
    warning_msg("Bogus CLIENTSHAKE packet received from: %s", 
		PolishedPresentation(presentation));
  }
}


//=============================================================================
void Metaserver::HandleTerminate(SA *pcliaddr)
{
  list<Handshake>::iterator i;
  char                     *presentation;
  SAIN                     *sa;
  Handshake                 search_for;

  sa = (SAIN *)pcliaddr;
  search_for.SetValues(sa->sin_addr.s_addr, 0, 0);
  i = FindByAddr(mActiveServers.begin(), mActiveServers.end(), 
		 search_for);
  if(i != mActiveServers.end())
  {
#ifdef DEBUG
    presentation = sock_ntop(pcliaddr, sizeof(SA));
    debug_msg("Received termination from server at %s",
	      PolishedPresentation(presentation));
#endif
    mActiveServers.erase(i);
    if(mExecute[0] != '\0')
      system(mExecute);
  }
  else
  {
    presentation = sock_ntop(pcliaddr, sizeof(SA));
    warning_msg("Received TERMINATION packet for unlisted server from: "
		"%s", PolishedPresentation(presentation));
  }
}


//=============================================================================
void Metaserver::HandleListRequest(SA *pcliaddr, socklen_t socketlen, 
				   ListRequestMsg &msg)
{
  list<Handshake>::iterator i;
  unsigned int j = 0, from = 0;

  from = msg.GetBeginningFrom();

  // Move iterator to requested start from location
  // Note: This will get new servers which have registered in between
  //       client block requests, since new servers are added at the end
  //       of the list.
  i = mActiveServers.begin();
  if(from)
    for(j = 0; j < from; ++j, ++i);

  ListResponseMsg response;

  if(i == mActiveServers.end())
  {
    if(mActiveServers.size() == 0)
    {
      //mesg_ptr = pack_uint32(mActiveServers.size(), mesg_ptr, 
      //                       &packet_size);
      //mesg_ptr = pack_uint32(0, mesg_ptr, &packet_size);
    }
    //else
    //mesg_ptr = pack_uint32(NMT_PROTO_ERANGE, mesg, &packet_size);
  }
  else
  {
    response.SetTotalServers(mActiveServers.size());
    while(i != mActiveServers.end())
    {
      if(!response.AddServer((*i).GetAddress()))
	break;
      i++;
    }
  }
  sendto(mListenSocket, response.GetPackedBuffer(), 
	 response.GetPackedBufferLength(), 0, pcliaddr, socketlen);
}


//=============================================================================
char *Metaserver::PolishedPresentation(char *presentation)
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
