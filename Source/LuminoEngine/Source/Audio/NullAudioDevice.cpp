﻿
#include "Internal.h"
#include "NullAudioDevice.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_AUDIO_BEGIN
namespace detail
{

//==============================================================================
// NullAudioPlayer
//==============================================================================

//------------------------------------------------------------------------------
NullAudioPlayer::NullAudioPlayer(AudioDevice* device)
	: AudioPlayer(device)
{
}

//------------------------------------------------------------------------------
NullAudioPlayer::~NullAudioPlayer()
{
}


//==============================================================================
// NullAudioDevice
//==============================================================================

//------------------------------------------------------------------------------
NullAudioDevice::NullAudioDevice()
{
}

//------------------------------------------------------------------------------
NullAudioDevice::~NullAudioDevice()
{
}

//------------------------------------------------------------------------------
AudioPlayer* NullAudioDevice::createAudioPlayer(AudioStream* source, bool enable3d, SoundPlayingMode mode)
{
	Ref<NullAudioPlayer> player(LN_NEW NullAudioPlayer(this), false);
	player->initialize(source, enable3d);
	return player.detachMove();
}

//------------------------------------------------------------------------------
void NullAudioDevice::update()
{
}

//------------------------------------------------------------------------------
void NullAudioDevice::setMetreUnitDistance(float d)
{
}

} // namespace detail
LN_NAMESPACE_AUDIO_END
LN_NAMESPACE_END

