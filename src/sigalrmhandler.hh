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
#ifndef SIGALRMHANDLER_HH
#define SIGALRMHANDLER_HH

#include <csignal>                // for sig_atomic_t, SIGINT

#include "signalhandler.hh"


//=============================================================================
class SIGALRMHandler : public SignalHandler
{
public:
  SIGALRMHandler();

  virtual int HandleSignal(int signum);

  void         SetAlarm(sig_atomic_t alarm);

  sig_atomic_t GetAlarm() const;
private:
  sig_atomic_t mAlarm;
};

//=============================================================================
inline void SIGALRMHandler::SetAlarm(sig_atomic_t alarm)
{
  mAlarm = alarm;
}
//=============================================================================
inline sig_atomic_t SIGALRMHandler::GetAlarm() const
{
  return mAlarm;
}


#endif
