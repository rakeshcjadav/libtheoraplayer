/************************************************************************************
This source file is part of the Theora Video Playback Library
For latest info, see http://libtheoraplayer.googlecode.com
*************************************************************************************
Copyright (c) 2008-2014 Kresimir Spes (kspes@cateia.com)
This program is free software; you can redistribute it and/or modify it under
the terms of the BSD license: http://opensource.org/licenses/BSD-3-Clause
*************************************************************************************/
#include <xal/AudioManager.h>
#include <xal/Player.h>
#include <xal/xal.h>
#include "AudioVideoTimer.h"
#include <hltypes/hlog.h>
#include "VideoObject.h"

namespace aprilvideo
{
	AudioVideoTimer::AudioVideoTimer(VideoObject* object, xal::Player* player, float sync_offset) : VideoTimer(object)
	{
		mPrevTickCount = 0;
		mSyncOffset = sync_offset;
		mPrevTimePosition = -1;
		mAudioPosition = 0;
		mPlayer = player;
		mSyncDiff = mSyncDiffFactor = 0;
		mT = 0;
		static hstr audiosystem = xal::mgr->getName(); // XAL_AS_DISABLED audio system doesn't sync audio & video
		mDisabledAudio = (audiosystem == XAL_AS_DISABLED);
	}
	
	void AudioVideoTimer::update(float time_increase)
	{
		VideoTimer::update(time_increase);
		if (!mDisabledAudio)
		{
			bool paused = isPaused(), playerPaused = mPlayer->isPaused();
			// use our own time calculation because april's could be tampered with (speedup/slowdown)
			unsigned int tickCount = get_system_tick_count();
			if (mPrevTickCount == 0) mPrevTickCount = tickCount;
			time_increase = (tickCount - mPrevTickCount) / 1000.0f;
			if (paused) time_increase = 0;
			
			if (paused && !playerPaused && !mPlayer->isFadingOut())
			{
				mPlayer->pause();
			}
			else if (!paused && playerPaused)
			{
				mPlayer->play();
			}
			
			else if (time_increase > 0.1f) time_increase = 0.1f; // prevent long hiccups when defoucsing window

			mPrevTickCount = tickCount;
			if (mPlayer->isPlaying())
			{
				// on some platforms, getTimePosition() isn't accurate enough, so we need to manually update our timer and
				// use the audio position for syncing
#if defined(_DEBUG) && 0 // debug testing
				float timePosition = (int) mPlayer->getTimePosition();
#else
				float timePosition = mPlayer->getTimePosition();
#endif
				if (timePosition != mPrevTimePosition)
				{
					if (timePosition - mPrevTimePosition > 0.1f)
					{
						mSyncDiff = timePosition - mAudioPosition;
						mSyncDiffFactor = (float) fabs(mSyncDiff);
						mPrevTimePosition = timePosition;
#if defined(_DEBUG) && 0 // debug testing
						hlog::writef("aprilvideo_DEBUG", "sync diff: %.3f", mSyncDiff);
#endif
					}
					else
					{
						mAudioPosition = mPrevTimePosition = timePosition;
					}
				}
				else
				{
					mAudioPosition += time_increase;
				}
				if (mSyncDiff != 0)
				{
					float chunk = time_increase * mSyncDiffFactor;
					
					if (mSyncDiff > 0)
					{
						if (mSyncDiff - chunk < 0)
						{
							chunk = mSyncDiff;
							mSyncDiff = 0;
						}
						else
						{
							mSyncDiff -= chunk;
						}
						mAudioPosition += chunk;
					}
					else
					{
						if (mSyncDiff + chunk > 0)
						{
							chunk = -mSyncDiff;
							mSyncDiff = 0;
						}
						else
						{
							mSyncDiff += chunk;
						}
						mAudioPosition -= chunk;
					}
				}
				mTime = mAudioPosition - mSyncOffset;
			}
			else
			{
				mTime += time_increase;
			}
		}
		else
		{
			mT += time_increase;
			mTime = mT - mSyncOffset;
		}
	}
};