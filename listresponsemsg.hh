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
#ifndef LISTRESPONSEMSG_HH
#define LISTRESPONSEMSG_HH

#include "netmsg.hh"

#include <list>

typedef int           Address;
typedef list<Address> AddressList;

//=============================================================================
class ListResponseMsg : public NetMsg
{
public:
                       ListResponseMsg();
                       ListResponseMsg(const unsigned char *message,
				       const unsigned int messageLength);

          bool         AddServer(int address);
          void         SetTotalServers(unsigned total);

    const AddressList &GetServers() const;
    const unsigned     GetTotalServers() const;
private:
  virtual void         Pack();
  virtual void         Unpack();

  AddressList          mList;
  unsigned             mTotal;
  unsigned             mMax;
};

//=============================================================================
inline ListResponseMsg::ListResponseMsg() : NetMsg(),
					    mTotal(0)
{
  mType = NMT_LISTRESP;
#ifdef DEBUG
  mMax = 5;
#else
  mMax = (MAX_MSG_LENGTH - sizeof(mType) - sizeof(mTotal) - sizeof(int)) 
    / sizeof(Address);
#endif
}
//=============================================================================
inline void ListResponseMsg::SetTotalServers(unsigned total)
{
  mTotal = total;
}
//=============================================================================
inline const AddressList &ListResponseMsg::GetServers() const
{
  return mList;
}
//=============================================================================
inline const unsigned ListResponseMsg::GetTotalServers() const
{
  return mTotal;
}


#endif
