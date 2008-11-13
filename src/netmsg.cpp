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
#include "netmsg.hh"

#include <assert.h>           // for assert()
#include <netinet/in.h>       // for htonl() and ntohl()
#include <string.h>           // for memset()

//=============================================================================
NetMsg::NetMsg(): mGood(true),
		  mPacked(false),
		  mType(NMT_NULL),
		  mLength(0),
		  mPlace(0)
{
  memset(mBuffer, 0, MAX_MSG_LENGTH);
}


//=============================================================================
NetMsg::NetMsg(const unsigned char *message, const unsigned int messageLength)
{
  if(messageLength < MAX_MSG_LENGTH)
    memcpy(mBuffer, message, sizeof(NetMsgType)); // Intentionally copy only type

  mGood = false;
  mPacked = true;
  mLength = messageLength;
  mPlace = 0;

  Unpack();
}


//=============================================================================
NetMsg::~NetMsg()
{
}


//=============================================================================
NetMsgType NetMsg::GetType() const
{
  return mType;
}


//=============================================================================
bool NetMsg::Good()
{
  if(mType > NMT_NULL && mType < NMT_LAST)
    return true;
  return false;
}


//=============================================================================
const char *NetMsg::GetPackedBuffer()
{
  if(!mPacked)
    Pack();
  return mBuffer;
}


//=============================================================================
unsigned int NetMsg::GetPackedBufferLength()
{
  if(!mPacked)
    Pack();
  return mLength;
}


//=============================================================================
void NetMsg::PackType()
{
  unsigned netorder = htonl(mType);

  memcpy(mBuffer+mLength, (void *)&netorder, sizeof(netorder));
  mLength += sizeof(mType);
}


//=============================================================================
void NetMsg::PackChar(char byte)
{
  memcpy(mBuffer+mLength, (void *)&byte, sizeof(byte));
  mLength += sizeof(byte);
}


//=============================================================================
void NetMsg::PackUChar(unsigned char byte)
{
  memcpy(mBuffer+mLength, (void *)&byte, sizeof(byte));
  mLength += sizeof(byte);
}


//=============================================================================
void NetMsg::PackInt(int data)
{
  int netorder = htonl(data);

  memcpy(mBuffer+mLength, (void *)&netorder, sizeof(netorder));
  mLength += sizeof(netorder);
}


//=============================================================================
void NetMsg::PackUInt(unsigned int data)
{
  unsigned int netorder = htonl(data);

  memcpy(mBuffer+mLength, (void *)&netorder, sizeof(netorder));
  mLength += sizeof(netorder);
}


//=============================================================================
void NetMsg::PackString(char *str, unsigned int length)
{
  assert(length < MAX_MSG_LENGTH - sizeof(int) - sizeof(NetMsgType));

  PackUInt(length);
  memcpy(mBuffer+mLength, (void *)str, length);
  mLength += length;
}


//=============================================================================
void NetMsg::PackShort(short data)
{
  short netorder = htons(data);

  memcpy(mBuffer+mLength, (void *)&netorder, sizeof(netorder));
  mLength += sizeof(netorder);
}


//=============================================================================
void NetMsg::PackUShort(unsigned short data)
{
  unsigned short netorder = htons(data);

  memcpy(mBuffer+mLength, (void *)&netorder, sizeof(netorder));
  mLength += sizeof(netorder);
}


//=============================================================================
void NetMsg::PackFloat(float data)
{
  memcpy(mBuffer+mLength, (void *)&data, sizeof(data));
  mLength += sizeof(data);
}


//=============================================================================
void NetMsg::PackDouble(double data)
{
  memcpy(mBuffer+mLength, (void *)&data, sizeof(data));
  mLength += sizeof(data);
}


//=============================================================================
void NetMsg::PackLongLong(long long data)
{
  memcpy(mBuffer+mLength, (void *)&data, sizeof(data));
  mLength += sizeof(data);
}


//=============================================================================
NetMsgType NetMsg::UnpackType()
{
  unsigned netorder;
  NetMsgType type;

  memcpy((void *)&netorder, mBuffer+mPlace, sizeof(netorder));
  mPlace += sizeof(type);
  type = (NetMsgType)ntohl(netorder);
  return type;
}


//=============================================================================
char NetMsg::UnpackChar()
{
  char data;
  memcpy((void *)&data, mBuffer+mPlace, sizeof(data));
  mPlace += sizeof(data);
  return data;
}


//=============================================================================
unsigned char NetMsg::UnpackUChar()
{
  unsigned char data;
  memcpy((void *)&data, mBuffer+mPlace, sizeof(data));
  mPlace += sizeof(data);
  return data;
}


//=============================================================================
int NetMsg::UnpackInt()
{
  int netorder, data;
  memcpy((void *)&netorder, mBuffer+mPlace, sizeof(netorder));
  mPlace += sizeof(netorder);
  data = ntohl(netorder);
  return data;
}


//=============================================================================
unsigned int NetMsg::UnpackUInt()
{
  unsigned int netorder, data;
  memcpy((void *)&netorder, mBuffer+mPlace, sizeof(netorder));
  mPlace += sizeof(netorder);
  data = ntohl(netorder);
  return data;
}


//=============================================================================
unsigned int NetMsg::UnpackString(char *data)
{
  unsigned int length = UnpackInt();
  memcpy((void *)data, mBuffer+mPlace, length);
  mPlace += length;
  return length;
}


//=============================================================================
short NetMsg::UnpackShort()
{
  short netorder, data;
  memcpy((void *)&netorder, mBuffer+mPlace, sizeof(netorder));
  mPlace += sizeof(netorder);
  data = ntohs(netorder);
  return data;
}


//=============================================================================
unsigned short NetMsg::UnpackUShort()
{
  unsigned short netorder, data;
  memcpy((void *)&netorder, mBuffer+mPlace, sizeof(netorder));
  mPlace += sizeof(netorder);
  data = ntohs(netorder);
  return data;
}


//=============================================================================
float NetMsg::UnpackFloat()
{
  float data;
  memcpy((void *)&data, mBuffer+mPlace, sizeof(data));
  mPlace += sizeof(data);
  return data;
}


//=============================================================================
double NetMsg::UnpackDouble()
{
  double data;
  memcpy((void *)&data, mBuffer+mPlace, sizeof(data));
  mPlace += sizeof(data);
  return data;
}


//=============================================================================
long long NetMsg::UnpackLongLong()
{
  long long data;
  memcpy((void *)&data, mBuffer+mPlace, sizeof(data));
  mPlace += sizeof(data);
  return data;
}


//=============================================================================
bool NetMsg::VerifyType(NetMsgType type)
{
  NetMsg::Unpack();
  return mType == type;
}


//=============================================================================
void NetMsg::Pack()
{
  PackType();
  mPacked = true;
}


//=============================================================================
void NetMsg::Unpack()
{
  mType = UnpackType();
}
