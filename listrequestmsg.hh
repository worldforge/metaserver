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
#ifndef LISTREQUESTMSG_HH
#define LISTREQUESTMSG_HH

#include "netmsg.hh"


//=============================================================================
class ListRequestMsg : public NetMsg
{
public:
  ListRequestMsg(int);
  ListRequestMsg(const unsigned char *message,
		 const unsigned int messageLength);

  int GetBeginningFrom() const;

  void SetBeginningFrom(int);
private:
  virtual void Pack();
  virtual void Unpack();

  int mBeginningFrom;
};

//=============================================================================
inline ListRequestMsg::ListRequestMsg(int beginningfrom) 
                                      : mBeginningFrom(beginningfrom)
{
  mType = NMT_LISTREQ;
}
//=============================================================================
inline int ListRequestMsg::GetBeginningFrom() const
{
  return mBeginningFrom;
}
//=============================================================================
inline void ListRequestMsg::SetBeginningFrom(int beginningfrom)
{
  mBeginningFrom = beginningfrom;
}


#endif