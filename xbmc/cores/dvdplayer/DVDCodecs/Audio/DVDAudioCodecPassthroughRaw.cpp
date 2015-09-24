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

#include "DVDAudioCodecPassthroughRaw.h"
#include "DVDCodecs/DVDCodecs.h"
#include "cores/AudioEngine/AEFactory.h"
#include "utils/log.h"

#include <algorithm>

static enum AEChannel OutputMaps[2][9] = {
  {AE_CH_RAW, AE_CH_RAW, AE_CH_NULL},
  {AE_CH_RAW, AE_CH_RAW, AE_CH_RAW, AE_CH_RAW, AE_CH_RAW, AE_CH_RAW, AE_CH_RAW, AE_CH_RAW, AE_CH_NULL}
};

CDVDAudioCodecPassthroughRaw::CDVDAudioCodecPassthroughRaw(void) :
  m_outBuf(NULL),
  m_outBufSize(0),
  m_bufSize(0)
{
}

CDVDAudioCodecPassthroughRaw::~CDVDAudioCodecPassthroughRaw(void)
{
  Dispose();
}

bool CDVDAudioCodecPassthroughRaw::Open(CDVDStreamInfo &hints, CDVDCodecOptions &options)
{
  m_hints = hints;

  bool bSupportsAC3Out    = CAEFactory::SupportsRaw(AE_FMT_AC3, m_hints.samplerate);
  bool bSupportsEAC3Out   = CAEFactory::SupportsRaw(AE_FMT_EAC3, 192000);
  bool bSupportsDTSOut    = CAEFactory::SupportsRaw(AE_FMT_DTS, m_hints.samplerate);
  bool bSupportsTrueHDOut = CAEFactory::SupportsRaw(AE_FMT_TRUEHD, 192000);
  bool bSupportsDTSHDOut  = CAEFactory::SupportsRaw(AE_FMT_DTSHD, 192000);

  /* 32kHz E-AC-3 passthrough requires 128kHz IEC 60958 stream
   * which HDMI does not support, and IEC 61937 does not mention
   * reduced sample rate support, so support only 44.1 and 48 */
  if ((m_hints.codec == AV_CODEC_ID_AC3 && bSupportsAC3Out) ||
      (m_hints.codec == AV_CODEC_ID_EAC3 && bSupportsEAC3Out && (m_hints.samplerate == 44100 || m_hints.samplerate == 48000)) ||
      (m_hints.codec == AV_CODEC_ID_DTS && bSupportsDTSOut) ||
      (m_hints.codec == AV_CODEC_ID_TRUEHD && bSupportsTrueHDOut))
  {
    CLog::Log(LOGDEBUG, "raw pt: sr:%d ch:%d br:%d", m_hints.samplerate, m_hints.channels, m_hints.bitrate);
    return true;
  }

  return false;
}

int CDVDAudioCodecPassthroughRaw::GetSampleRate()
{
  return m_hints.samplerate;
}

enum AEDataFormat CDVDAudioCodecPassthroughRaw::GetDataFormat()
{
  switch(m_hints.codec)
  {
    case AV_CODEC_ID_AC3:
      return AE_FMT_AC3;

    case AV_CODEC_ID_DTS:
      return AE_FMT_DTS;

    case AV_CODEC_ID_EAC3:
      return AE_FMT_EAC3;

    case AV_CODEC_ID_TRUEHD:
      return AE_FMT_TRUEHD;

    default:
      return AE_FMT_INVALID; //Unknown stream type
  }
}

int CDVDAudioCodecPassthroughRaw::GetChannels()
{
  switch (m_hints.codec)
  {
    case AV_CODEC_ID_AC3:
    case AV_CODEC_ID_DTS:
      return 2;

    default:
      return 8;
  }
}

CAEChannelInfo CDVDAudioCodecPassthroughRaw::GetChannelMap()
{
  switch (m_hints.codec)
  {
    case AV_CODEC_ID_AC3:
    case AV_CODEC_ID_DTS:
      return CAEChannelInfo(OutputMaps[0]);

    default:
      return CAEChannelInfo(OutputMaps[1]);
  }
}

void CDVDAudioCodecPassthroughRaw::Dispose()
{
  if (m_outBuf)
  {
    delete[] m_outBuf;
    m_outBuf = NULL;
  }
}

int CDVDAudioCodecPassthroughRaw::Decode(uint8_t* pData, int iSize)
{
  if (iSize <= 0) return 0;

  if (m_outBufSize < iSize)
  {
    delete[] m_outBuf;
    m_outBuf = new uint8_t[iSize];
    m_outBufSize = iSize;
  }
  memcpy(m_outBuf, pData, iSize);

  m_bufSize = iSize;
  return m_bufSize;
}

int CDVDAudioCodecPassthroughRaw::GetData(uint8_t** dst)
{
  int size = m_bufSize;
  *dst = m_outBuf;
  m_bufSize = 0;

  return size;
}

void CDVDAudioCodecPassthroughRaw::Reset()
{
}

int CDVDAudioCodecPassthroughRaw::GetBufferSize()
{
  return 0;
}
