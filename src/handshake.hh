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
#ifndef HANDSHAKE_HH
#define HANDSHAKE_HH

#include <sys/types.h>


//=============================================================================
class Handshake
{
public:
  Handshake();
  Handshake(unsigned a, unsigned h, time_t t);
  Handshake(const Handshake &rhs);
  ~Handshake();

  void SetValues(unsigned a, unsigned h, time_t t);
  void SetTimestamp(time_t t);
  unsigned GetAddress() const;
  unsigned GetHandshake() const;
  time_t GetTimestamp() const;
private:
  unsigned mAddr;
  unsigned mHandshake;
  time_t mTimestamp;
};

//=============================================================================
inline Handshake::Handshake()
{
}
//=============================================================================
inline Handshake::Handshake(unsigned a, unsigned h, time_t t)
{
  SetValues(a, h, t);
}
//=============================================================================
inline Handshake::Handshake(const Handshake &rhs)
{
  SetValues(rhs.mAddr, rhs.mHandshake, rhs.mTimestamp);
}
//=============================================================================
inline Handshake::~Handshake()
{
}
//=============================================================================
inline void Handshake::SetValues(unsigned a, unsigned h, time_t t)
{
  mAddr = a;
  mHandshake = h;
  mTimestamp = t;
}
//=============================================================================
inline void Handshake::SetTimestamp(time_t t)
{
  mTimestamp = t;
}
//=============================================================================
inline unsigned Handshake::GetAddress() const
{
  return mAddr;
}
//=============================================================================
inline unsigned Handshake::GetHandshake() const
{
  return mHandshake;
}
//=============================================================================
inline time_t Handshake::GetTimestamp() const
{
  return mTimestamp;
}

#endif
