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
#ifdef __FreeBSD__
//#include <db.h>
#endif
//#include <netinet/in.h>

//#include "mainloop.hh"

#include "metaserver.hh"


int main(int argc, char **argv)
{
  Metaserver metaserver;

  if(!metaserver.ParseCommandline(argc, argv))
    return 1;

  if(!metaserver.Initialize(argv[0]))
    return 2;

  metaserver.Process();

  return 0;
}
