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
    This software was implemented based on the following articles:

    Applying Design Patterns to Simplify Signal Handling
    By Douglas C. Schmidt
    C++ Report
    April 1998
    URL: http://www.cs.wustl.edu/~schmidt/signal-patterns.html

    Design Patterns
    Singleton | C++ demos
    By Vince Huston
    Published on the Web
    URL: http://rampages.onramp.net/~huston/dp/patterns.html

    The second was of course based on:

    Design Patterns: Elements of Reusable Object-Oriented Software
    By Erich Gamma, Richard Helm, Ralph Johnson, and John Vlissides
*/
#include "sigalrmhandler.hh"

#include <assert.h>

//=============================================================================
SIGALRMHandler::SIGALRMHandler() : mAlarm(0)
{
}


//=============================================================================
int SIGALRMHandler::HandleSignal(int signum)
{
  assert(signum == SIGALRM);
  mAlarm = 1;
  return 0;
}
