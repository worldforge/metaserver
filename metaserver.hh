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
#ifndef METASERVER_HH
#define METASERVER_HH

#include "wrapunix.hh"

// Forward declare it later.
#include <list>

#ifndef DEBUG
#define REAP_INTERVAL 930 /* 15 minutes plus 30 seconds */
#else
#define REAP_INTERVAL 7   /* 5 seconds plus 2 slack */
#endif


//=============================================================================
class ClientShakeMsg;
class Handshake;
class ListRequestMsg;
class SIGALRMHandler;
class SIGINTHandler;
class ServerShakeMsg;


//=============================================================================
class Metaserver : public WrapUnix
{
public:
  Metaserver();
  ~Metaserver();

  bool Initialize(char *binaryName);
  int  ParseCommandline(int, char **);
  void Process();
public:
  enum ListType
  {
    SERVER_HANDSHAKE_LIST,
    CLIENT_HANDSHAKE_LIST,
    SERVER_LIST, 
    CLIENT_LIST
  };

private:
  void Daemonize(const char *, int);
  void VersionAndLicense();
  void Help();
  int  LookupHost(const char *, struct in_addr *);

  void Interrupted();
  void Reaper();

  void HandleServerKeepalive(SA *pcliaddr, socklen_t socketlen);
  void HandleClientKeepalive(SA *pcliaddr, socklen_t socketlen);
  void HandleServerShake(SA *pcliaddr, const ServerShakeMsg &msg);
  void HandleClientShake(SA *pcliaddr, ClientShakeMsg &msg);
  void HandleTerminate(SA *pcliaddr);
  void HandleListRequest(SA *pcliaddr, socklen_t socketlen, 
			 ListRequestMsg &msg);

  template<class ForwardIterator, class T>
  ForwardIterator FindByHS(ForwardIterator first, ForwardIterator last, 
			     T& value);
  template<class ForwardIterator, class T>
  ForwardIterator FindByAddr(ForwardIterator first, ForwardIterator last, 
			     T& value);
  void ReapList(ListType which);
  char *PolishedPresentation(char *presentation);

  bool            mDaemon;
  struct in_addr  mBindAddress;
  int             mListenPort;
  int             mListenSocket;
  int             mPacketCount;
  list<Handshake> mActiveServers;
  list<Handshake> mActiveClients;
  list<Handshake> mServerHandshakes;
  list<Handshake> mClientHandshakes;
  int             mPeakActiveServers;
  int             mPeakActiveClients;
  int             mPeakPendingServerHandshakes;
  int             mPeakPendingClientHandshakes;
  SIGALRMHandler *mSigalrmHandler;
  SIGINTHandler  *mSigintHandler;
};

#endif
