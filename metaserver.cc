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
#include <netinet/in.h>
#include <iostream>
#include <getopt.h>
#include "wrap.hh"
#include "mainloop.hh"

#define DEFAULT_SERV_PORT           8453


const char *long_version = "Generic Game Metaserver v1.1";
const char *short_version = "1.1";
const char *copyright_license_no_warranty =
"Copyright (C) 2000 Dragon Master\n"
"This software is licensed under the GNU GPL and comes with ABSOLUTELY NO\n"
"WARRANTY.  See the accompanying License file for details.";


int parse_commandline(int, char **, int *, struct in_addr *);
void version_and_license();
void help();

int main(int argc, char **argv)
{
  int sockfd;
  struct sockaddr_in servaddr, cliaddr;
  int listen_port = DEFAULT_SERV_PORT;
  struct in_addr addr;

  memset(&addr, 0, sizeof(addr));

  if(!parse_commandline(argc, argv, &listen_port, &addr))
    return 1;

  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  memcpy(&servaddr.sin_addr, &addr, sizeof(struct in_addr));
  servaddr.sin_port        = htons(listen_port);

  Bind(sockfd, (SA *)&servaddr, sizeof(servaddr));

  main_loop(sockfd, (SA *)&cliaddr, sizeof(cliaddr));

  return 0;
}

int parse_commandline(int argc, char * argv[], int *port, struct in_addr *addr)
{
  int c;
  bool quiet = false;
  bool server_ip_specified = false;
  char server_ip[128];

  while (1)
  {
    int option_index = 0;
    static struct option long_options[] =
    {
      // name, has_arg, return 0?, val
      {"help", 0, 0, 'h'},
      {"port", 1, 0, 'p'},
      {"quiet", 0, 0, 'q'},
      {"server-ip", 1, 0, 's'},
      {"version", 0, 0, 'v'},
      {0, 0, 0, 0}
    };

    c = getopt_long (argc, argv, "hp:qs:v", long_options, &option_index);
    if (c == -1)
      break;

    switch (c)
    {
      case 0:
        cout << "Unsupported long option " << long_options[option_index].name;
        if (optarg)
          cout << " with arg " << optarg;
        cout << endl;
        break;
      case 'h':
        version_and_license();
        cout << endl;
        help();
        exit(0);
        break;
      case 'p':
        sscanf(optarg, "%i", port);
        break;
      case 'q':
        quiet = true;
        break;
      case 's':
        strncpy(server_ip, optarg, 128);
        Inet_aton(optarg, addr);
        server_ip_specified = true;
        break;
      case 'v':
        version_and_license();
        exit(0);
        break;
      case '?':
        break;
      default:
        cerr << "?? getopt returned character code 0x" 
             << hex << c << dec << " ??" << endl;
    }
  }

  if(!server_ip_specified)
    addr->s_addr = INADDR_ANY;

  if(!quiet)
  {
    if (optind < argc)
    {
      cout << "Unrecognized crap on the commandline: ";
      while (optind < argc)
        cout << argv[optind++];
      cout << endl;
    }
    if(*port != DEFAULT_SERV_PORT)
      cout << "Using server listen port " << *port << endl;
    else
      cout << "Using default listen port " << *port << endl;
    if(server_ip_specified)
      cout << "Binding to `" << server_ip << "'" << endl;
    else
      cout << "Binding to any IP." << endl;
  }

  return 1;
}

void version_and_license()
{
  cout << long_version << "  "
       << copyright_license_no_warranty << endl;
}

void help()
{
  cout << "-v   --version    Version" << endl
       << "-h   --help       Help" << endl
       << "-s   --server-ip  Server IP" << endl
       << "-p   --port       Server Listen Port" << endl;
}
