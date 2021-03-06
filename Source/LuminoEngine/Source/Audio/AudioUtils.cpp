﻿
#include "Internal.h"
#include "AudioStream.h"
#include "AudioUtils.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_AUDIO_BEGIN
namespace detail
{

//==============================================================================
// AudioUtils
//==============================================================================

#if defined(_WIN32)

//------------------------------------------------------------------------------
void AudioUtils::convertLNWaveFormatToWAVEFORMATEX(const WaveFormat& lnFmt, WAVEFORMATEX* wavFmt)
{
    wavFmt->wFormatTag       = lnFmt.formatTag;
    wavFmt->nChannels        = lnFmt.channels;
    wavFmt->nSamplesPerSec   = lnFmt.samplesPerSec;
    wavFmt->nAvgBytesPerSec  = lnFmt.avgBytesPerSec;
    wavFmt->nBlockAlign      = lnFmt.blockAlign;
    wavFmt->wBitsPerSample   = lnFmt.bitsPerSample;
    wavFmt->cbSize           = lnFmt.exSize;
}

//------------------------------------------------------------------------------
void AudioUtils::convertWAVEFORMATEXToLNWaveFormat(const WAVEFORMATEX& wavFmt, WaveFormat* lnFmt)
{
    lnFmt->formatTag      = wavFmt.wFormatTag;
    lnFmt->channels       = wavFmt.nChannels;
    lnFmt->samplesPerSec  = wavFmt.nSamplesPerSec;
    lnFmt->avgBytesPerSec = wavFmt.nAvgBytesPerSec;
    lnFmt->blockAlign     = wavFmt.nBlockAlign;
    lnFmt->bitsPerSample  = wavFmt.wBitsPerSample;
    lnFmt->exSize         = wavFmt.cbSize;
}   

//------------------------------------------------------------------------------
void AudioUtils::printWAVEFORMATEX(const WAVEFORMATEX& wavFmt, const char* str)
{
    if ( str )
	{
		printf( "%s\n", str );
	}
	else
	{
        printf( "AudioUtils::dumpWAVEFORMATEX\n" );
	}
	printf( "    wFormatTag                %hu\n", wavFmt.wFormatTag );
	printf( "    nChannels                 %hu\n", wavFmt.nChannels );
	printf( "    nSamplesPerSec            %lu\n", wavFmt.nSamplesPerSec );
	printf( "    nAvgBytesPerSec(Byte/sec) %lu\n", wavFmt.nAvgBytesPerSec );
	printf( "    nBlockAlign               %hu\n", wavFmt.nBlockAlign );
	printf( "    wBitsPerSample            %hu\n", wavFmt.wBitsPerSample );
	printf( "    cbSize                    %hu\n", wavFmt.cbSize );
}

#endif

//------------------------------------------------------------------------------
StreamFormat AudioUtils::checkFormat(Stream* stream)
{
	if (!stream || stream->getLength() < 4) {
		return StreamFormat_Unknown;
	}

	StreamFormat format = StreamFormat_Unknown;

	byte_t head[4];
	stream->read(head, 4);
	if (memcmp(head, "RIFF", 4) == 0)
	{
		stream->seek(4, SeekOrigin_Current);
		byte_t wave[4];
		stream->read(wave, 4);
		if (memcmp(wave, "WAVE", 4) == 0)
		{
			format = StreamFormat_Wave;
		}
	}
	else if (memcmp(head, "OggS", 4) == 0)
	{
		format = StreamFormat_Ogg;
	}
	else if (memcmp(head, "MThd", 4) == 0)
	{
		format = StreamFormat_Midi;
	}
	else if (memcmp(head, "ID3", 3) == 0)
	{
		format = StreamFormat_Mp3;
	}

	if (format == StreamFormat_Unknown)
	{
		// mp3 の場合はヘッダが ID3 だったり後ろの方に TAG があったり
		// 同期ヘッダだけあったりするので全部チェックする
		if (head[0] == 0xff && head[1] >> 5 == 0x07)
		{
			format = StreamFormat_Mp3;
		}
		else
		{
			stream->seek(-128, SeekOrigin_End);
			char data[3];
			stream->read(data, 3);
			if (data[0] == 'T' && data[1] == 'A' && data[2] == 'G')
			{
				format = StreamFormat_Mp3;
			}
		}
	}

	stream->seek(0, SeekOrigin_Begin);
	return format;
}

//------------------------------------------------------------------------------
SoundPlayingMode AudioUtils::checkAudioPlayType(SoundPlayingMode type, AudioStream* audioStream, uint32_t limitSize)
{
	// 作成するオーディオプレイヤーの種類を決めていく
	if (audioStream->getDecoder()->getSourceFormat() == StreamFormat_Midi)
	{
		type = SoundPlayingMode::Midi;
	}
	else
	{
		if (type == SoundPlayingMode::Unknown)
		{
			if (audioStream->getDecoder()->getOnmemoryPCMBufferSize() > limitSize)
			{
				type = SoundPlayingMode::Streaming;
			}
			else
			{
				type = SoundPlayingMode::OnMemory;
			}
		}
	}

    return type;
}

} // namespace detail
LN_NAMESPACE_AUDIO_END
LN_NAMESPACE_END
