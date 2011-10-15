#ifndef SIGNALMANAGER_HH
#define SIGNALMANAGER_HH

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
#include <csignal>                  // for NSIG

//=============================================================================
class SignalHandler;


//=============================================================================
class SignalManager
{
public:
  static SignalManager &GetInstance();

  SignalHandler *RegisterHandler(int signum, SignalHandler *sh);

  SignalHandler *RemoveHandler(int signum);

private:

  SignalManager();

  static void Dispatcher(int signum);

  static SignalHandler *mSignalHandlers[NSIG];
};

#endif
