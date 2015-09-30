#pragma once

/*
 *      Copyright (C) 2010-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <list>

#include "system.h"
#include "DVDAudioCodec.h"
#include "DVDStreamInfo.h"
#include "cores/AudioEngine/Utils/AEAudioFormat.h"
#include "cores/AudioEngine/Utils/AEChannelInfo.h"

class CDVDAudioCodecPassthroughRaw : public CDVDAudioCodec
{
public:
  CDVDAudioCodecPassthroughRaw();
  virtual ~CDVDAudioCodecPassthroughRaw();

  virtual bool Open(CDVDStreamInfo &hints, CDVDCodecOptions &options);
  virtual void Dispose();
  virtual int  Decode(uint8_t* pData, int iSize);
  virtual int  GetData(uint8_t** dst);
  virtual void Reset();
  virtual int  GetChannels               ();
  virtual CAEChannelInfo GetChannelMap       ();
  virtual int  GetSampleRate             ();
  virtual enum AEDataFormat GetDataFormat();
  virtual bool NeedPassthrough           () { return true;          }
  virtual const char* GetName            () { return "passthroughraw"; }
  virtual int  GetBufferSize();
private:
  CDVDStreamInfo     m_hints;
  uint8_t*           m_outBuf;
  unsigned int       m_bufSize;
  unsigned int       m_outBufSize;
};

