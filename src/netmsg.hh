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
#ifndef NETMSG_HH
#define NETMSG_HH

#define MAX_MSG_LENGTH 570 // something less than default MTU

#include "protocol_instructions.h"

// Subclasses use memcpy, so we include <string> here
#include <string>

class NetMsg
{
public:
                         NetMsg();
                         NetMsg(const unsigned char *message, 
				const unsigned int messageLength);
  virtual               ~NetMsg();

          NetMsgType     GetType() const;

  virtual bool           Good();

    const char          *GetPackedBuffer();
          unsigned int   GetPackedBufferLength();

protected:
          void           PackType();
          void           PackChar(char);
          void           PackUChar(unsigned char);
          void           PackInt(int);
          void           PackUInt(unsigned int);
          void           PackString(char *, unsigned int);
          void           PackShort(short);
          void           PackUShort(unsigned short);
          void           PackFloat(float);
          void           PackDouble(double);
          void           PackLongLong(long long);

          NetMsgType     UnpackType();
          char           UnpackChar();
          unsigned char  UnpackUChar();
          int            UnpackInt();
          unsigned int   UnpackUInt();
          unsigned int   UnpackString(char *);
          short          UnpackShort();
          unsigned short UnpackUShort();
          float          UnpackFloat();
          double         UnpackDouble();
          long long      UnpackLongLong();

protected:
  virtual void           Pack();
  virtual void           Unpack();
  virtual bool           VerifyType(NetMsgType type);

          char           mBuffer[MAX_MSG_LENGTH];
          bool           mGood;
          bool           mPacked;
          NetMsgType     mType;
private:
          unsigned int   mLength;
          unsigned int   mPlace;
};

#endif
