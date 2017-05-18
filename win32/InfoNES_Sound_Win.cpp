/*===================================================================*/
/*                                                                   */
/*  InfoNES_Sound_Win.cpp : Implementation of DIRSOUND Class         */
/*                                                                   */
/*  2000/05/18  InfoNES Project                                      */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include <windows.h>
#include <mmsystem.h>
#include "dsound.h"
#include "InfoNES_Sound_Win.h"
#include "../InfoNES_System.h"

/*-------------------------------------------------------------------*/
/*  Constructor                                                      */
/*-------------------------------------------------------------------*/
DIRSOUND::DIRSOUND(HWND hwnd)
{
	DWORD ret;
	WORD x;

	// init variables
	iCnt = Loops * 3 / 4; // loops:20 iCnt:20*3/4 = 15

	for ( x = 0;x < ds_NUMCHANNELS; x++ ) // ds_NUMCHANNELS = 8
	{
		lpdsb[x] = NULL; // DirectSoundBuffer lpdsb[ds_NUMCHANNELS]; 8个 初始化为 NULL
	}

	// init DirectSound 创建一个 DirectSound 里面有 8个 DirectSoundBuffer
	ret = DirectSoundCreate(NULL, &lpdirsnd, NULL);

	if (ret != DS_OK)
	{
		InfoNES_MessageBox( "Sound Card is needed to execute this application." );
		exit(-1);
	}

  // set cooperative level
#if 1
	//设置属性不重要
	ret = lpdirsnd->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
#else
	ret = lpdirsnd->SetCooperativeLevel( hwnd, DSSCL_NORMAL );
#endif

	if ( ret != DS_OK )
	{
    InfoNES_MessageBox( "SetCooperativeLevel() Failed." );
		exit(-1);
	}
}

/*-------------------------------------------------------------------*/
/*  Destructor                                                       */
/*-------------------------------------------------------------------*/
DIRSOUND::~DIRSOUND()
{
	WORD x;

	for ( x=0; x < ds_NUMCHANNELS; x++ )
	{
		if ( lpdsb[x] != NULL )
		{
			DestroyBuffer( x );

			if (sound[x] != NULL)
			{
				delete sound[x];
			}
		}
	}
	(void)lpdirsnd->Release();	/* Nothing to do, if it errors */
}

/*-------------------------------------------------------------------*/
/*  FillBuffer() : Fill Sound Buffer                                 */
/*-------------------------------------------------------------------*/
void DIRSOUND::FillBuffer( WORD channel )
{
	LPVOID write1;
	DWORD length1;
	LPVOID write2;
	DWORD length2;
	HRESULT hr;

	hr = lpdsb[channel]->Lock( iCnt * len[channel], len[channel], &write1, &length1, &write2, &length2, 0 );

	//如果返回DSERR_BUFFERLOST，释放并重试锁定
	if (hr == DSERR_BUFFERLOST)
	{
		lpdsb[channel]->Restore();

		hr = lpdsb[channel]->Lock( iCnt * len[channel], len[channel], &write1, &length1, &write2, &length2, 0 );
	}

	if (hr != DS_OK)
	{
		InfoNES_MessageBox( "Lock() Failed." );
		exit(-1);
	}

	CopyMemory( write1, sound[channel], length1 );

	if (write2 != NULL)
	{
		CopyMemory(write2, sound[channel] + length1, length2);
	}

	hr = lpdsb[channel]->Unlock(write1, length1, write2, length2);

	if (hr != DS_OK)
	{
		InfoNES_MessageBox( "Unlock() Failed." );
		exit(-1);
	}
}

/*-------------------------------------------------------------------*/
/*  CreateBuffer() : Create IDirectSoundBuffer                       */
/*-------------------------------------------------------------------*/
void DIRSOUND::CreateBuffer(WORD channel)
{
	DSBUFFERDESC dsbdesc; //SoundBuffer 描述
	PCMWAVEFORMAT pcmwf;  //wav fmt 格式描述
	HRESULT hr;

	//清0
	memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT));
	//pcm 格式
	pcmwf.wf.wFormatTag 		 = WAVE_FORMAT_PCM;
	//1个声道
	pcmwf.wf.nChannels			 = ds_CHANSPERSAMPLE;
	//采样率 44100
	pcmwf.wf.nSamplesPerSec  = ds_SAMPLERATE;
	//对齐 采样率 / 8 * 声道数 = 44100 / 8 * 1 = 5512.5
	pcmwf.wf.nBlockAlign		 = ds_CHANSPERSAMPLE * ds_BITSPERSAMPLE / 8;
	//缓存区大小 44100*5512.5 = 243101250
	pcmwf.wf.nAvgBytesPerSec = pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign;
	//8位 声音
	pcmwf.wBitsPerSample		 = ds_BITSPERSAMPLE;

	//清0
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize				= sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags 			= 0;
	//缓存大小 735 * 15 = 11025
	dsbdesc.dwBufferBytes = len[channel]*Loops;
	dsbdesc.lpwfxFormat 	= (LPWAVEFORMATEX)&pcmwf;

	hr = lpdirsnd->CreateSoundBuffer(&dsbdesc, &lpdsb[channel], NULL);

	if (hr != DS_OK)
	{
		InfoNES_MessageBox( "CreateSoundBuffer() Failed." );
		exit(-1);
	}
}

/*-------------------------------------------------------------------*/
/*  DestoryBuffer() : Destory IDirectSoundBuffer                     */
/*-------------------------------------------------------------------*/
void DIRSOUND::DestroyBuffer(WORD channel)
{
	DWORD hr;

	hr = lpdsb[channel]->Release();

	if (hr != DS_OK)
	{
    InfoNES_MessageBox( "Release() Failed." );
		exit(-1);
	}

	lpdsb[channel] = NULL;
}

/*-------------------------------------------------------------------*/
/*  AllocChannel() : Allocate channel for IDirectSoundBuffer         */
/*-------------------------------------------------------------------*/
WORD DIRSOUND::AllocChannel(void)
{
	WORD x;
	
	//判断 lpdsb 找到一个 为空的 这里应该返回0
	for (x=0;x<ds_NUMCHANNELS;x++)
	{
		if (lpdsb[x] == NULL)
		{
			break;
		}
	}

	if ( x == ds_NUMCHANNELS )
	{
    /* No available channel */
    InfoNES_MessageBox( "AllocChannel() Failed." );
		exit(-1); 		
	}

	return (x);
}

/*-------------------------------------------------------------------*/
/*  Play() : Play Sound                                              */
/*-------------------------------------------------------------------*/
void DIRSOUND::Start(WORD channel, BOOL looping)
{
	HRESULT hr;

	hr = lpdsb[channel]->Play( 0, 0, looping == TRUE ? DSBPLAY_LOOPING : 0 );

	if ( hr != DS_OK )
	{
    InfoNES_MessageBox( "Play() Failed." );
		exit(-1);
	}
}

/*-------------------------------------------------------------------*/
/*  Stop() : Stop Sound                                              */
/*-------------------------------------------------------------------*/
void DIRSOUND::Stop(WORD channel)
{
	lpdsb[channel]->Stop();
}

/*-------------------------------------------------------------------*/
/*  UnLoadWave() : Destory Channel for IDirectSoundBuffer            */
/*-------------------------------------------------------------------*/
void DIRSOUND::UnLoadWave(WORD channel)
{
	DestroyBuffer(channel);

	if ( sound[channel] != NULL )
	{
		delete sound[channel];
	}
}

/*-------------------------------------------------------------------*/
/*  SoundOpen() : Open Devices for Sound Emulation                   */
/*-------------------------------------------------------------------*/
//samples_per_sync = 735 sample_rate = 44100
BOOL DIRSOUND::SoundOpen(int samples_per_sync, int sample_rate)
{
	//ch 1 WORD unsigned short 类型 , 创建一个 通道 , 返回 第0 个 SoundBuffer
	ch1 = AllocChannel();
	
	/** 
	*   参数定义
	*	BYTE 	*sound[ds_NUMCHANNELS];
	*	DWORD   len[ds_NUMCHANNELS];
	*/
	//申请了一个 735 大小的 Byte
	sound[ch1] = new BYTE[ samples_per_sync ];
	//记录了 大小 735
	len[ch1]	 = samples_per_sync;

	if ( sound[ch1] == NULL )
	{
		InfoNES_MessageBox( "new BYTE[] Failed." );
		exit(-1);
	}
	
	//创建缓存区
	CreateBuffer( ch1 );

	/* Clear buffer */
	FillMemory( sound[ch1], len[ch1], 0 ); 
	//执行15次
	for ( int i = 0; i < Loops; i++ )
		SoundOutput( len[ch1], sound[ch1] ); 

	/* Begin to play sound */
	Start( ch1, TRUE );

  return TRUE;
}

/*-------------------------------------------------------------------*/
/*  SoundClose() : Close Devices for Sound Emulation                 */
/*-------------------------------------------------------------------*/
void DIRSOUND::SoundClose( void )
{
  /* Stop to play sound */
  Stop( ch1 );

  UnLoadWave(ch1);
}

/*-------------------------------------------------------------------*/
/*  SoundOutput() : Output Sound to Device for Sound Emulation       */
/*-------------------------------------------------------------------*/
//初始化时 执行 samples:735 wave:NULL
BOOL DIRSOUND::SoundOutput(int samples, BYTE *wave)
{
	/* Buffering sound data */
	//将 wave 复制到 sound
	CopyMemory( sound[ ch1 ], wave, samples );  

	/* Copying to sound data buffer */
	FillBuffer( ch1 );  

	/* Play if Counter reaches buffer edge */
	//初始化时 iCnt:15 Loops:20
	if ( Loops == ++iCnt )
	{
		iCnt = 0;
	}
	//这里 iCnt = 16
	return TRUE;
}

/*-------------------------------------------------------------------*/
/*  SoundMute() : Sound Mute for Sound Emulation                     */
/*-------------------------------------------------------------------*/
BOOL DIRSOUND::SoundMute( BOOL flag )
{
  if ( flag )
  {
    /* Mute on : Stop to play sound */
    Stop( ch1 );
  } 
  else 
  {
#if 0
    /* Clear buffer */
    FillMemory( sound[ch1], len[ch1], 0 ); 
    for ( int i = 0; i < Loops; i++ )
      SoundOutput( len[ch1], sound[ch1] );
#endif

    /* Mute off : Begin to play sound */    
    iCnt = Loops * 3 / 4;
    Start( ch1, TRUE );
  }
  return TRUE;
}

// End of InfoNES_Sound_Win.cpp