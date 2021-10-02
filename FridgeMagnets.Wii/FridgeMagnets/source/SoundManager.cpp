#include "SoundManager.h"

#include "malloc.h"
#include "WiiFile.h"
#include "debug.h"
#include "string.h"

#include <asndlib.h>

#define ENDIAN32(Value) (Value = ((Value&0xff000000)>>24) |  ((Value&0x00ff0000)>>8) | ((Value&0x0000ff00)<<8) |  ((Value&0x000000ff)<<24))
#define ENDIAN16(Value) (Value = ((Value&0x00ff)<<8) |  ((Value&0xff00)>>8))


void RawSample::Play(u8 Chan, u8 VolumeLeft, u8 VolumeRight)
{
	//if (ASND_StatusVoice(Chan) == SND_UNUSED)
	{
		ASND_SetVoice( Chan, m_NumberOfChannels,m_SampleRate,0, m_RawData, m_RawDataLength , VolumeLeft, VolumeRight, NULL);
	}
}	

SoundManager::SoundManager( )
{ 
	Init();
}

SoundManager::~SoundManager( )
{ 
	UnInit();
}


void SoundManager::Init( )
{ 
	return;

	//AUDIO_Init(NULL);   // is this init needed?
	ASND_Init();
	ASND_Pause(0);  //unpause
}


void SoundManager::UnInit( )
{ 
	ASND_Pause(1);  //pause
	ASND_End();
}


RawSample* SoundManager::GetSound( int Value )
{
	return m_SoundsContainer[Value];
}

RawSample* SoundManager::LoadSound( std::string FileName )
{
	FILE* WAVFile = WiiFile::FileOpenForRead(FileName.c_str());
	
	//printf("     %s", FileName.c_str());
	//-------------------------------------------------------------
	// "RIFF" chunk discriptor
	RIFFChunk RIFFChunkData;
	fread(&RIFFChunkData, sizeof(RIFFChunk), 1, WAVFile);
	if ( strncmp ( RIFFChunkData.RIFF, "RIFF", 4 ) != 0 )
		ExitPrintf("'RIFF' check failed %c%c%c%c" ,RIFFChunkData.RIFF[0],RIFFChunkData.RIFF[1],RIFFChunkData.RIFF[2],RIFFChunkData.RIFF[3] );
	if ( strncmp ( RIFFChunkData.RIFFType, "WAVE", 4 ) != 0 )
		ExitPrintf("'WAVE' check failed %c%c%c%c",RIFFChunkData.RIFFType[0],RIFFChunkData.RIFFType[1],RIFFChunkData.RIFFType[2],RIFFChunkData.RIFFType[3]);
	//-------------------------------------------------------------
	// "fmt" sub-chunk
	fmtChunk fmtChunkData;
	fread(&fmtChunkData, sizeof(fmtChunk), 1, WAVFile);
	if ( strncmp ( fmtChunkData.fmt, "fmt ", 4 ) != 0 )
		ExitPrintf("fmt' check failed %c%c%c%c" , fmtChunkData.fmt[0],fmtChunkData.fmt[1],fmtChunkData.fmt[2],fmtChunkData.fmt[3]);

	fmtChunkData.Channels = ENDIAN16(fmtChunkData.Channels);
	fmtChunkData.SampleRate = ENDIAN32(fmtChunkData.SampleRate);
	//-------------------------------------------------------------
	// "data" sub-chunk
	dataChunk dataChunkData;
	fread(&dataChunkData, sizeof(dataChunk), 1, WAVFile);
	if ( strncmp ( dataChunkData.data, "data", 4 ) != 0 )
		ExitPrintf("'data' check failed %c%c%c%c",  dataChunkData.data[0],dataChunkData.data[1],dataChunkData.data[2],dataChunkData.data[3]);

	dataChunkData.dataLength = ENDIAN32(dataChunkData.dataLength);
	//-------------------------------------------------------------
	// Raw sound data

	RawSample* pRawSample( new RawSample );

	u8* pData = (u8*)memalign(32, (dataChunkData.dataLength*4)/4 );  //just incase, end on 4 bytes
	memset(pData,0,dataChunkData.dataLength);
	fread(pData, 1, dataChunkData.dataLength, WAVFile);

	fmtChunkData.BitResolution = ENDIAN16(fmtChunkData.BitResolution);

	pRawSample->SetRawData(pData);
	pRawSample->SetRawDataLength(dataChunkData.dataLength);
	pRawSample->SetNumberOfChannels(fmtChunkData.Channels);
	pRawSample->SetSampleRate(fmtChunkData.SampleRate);
	pRawSample->SetBitsPerSample(fmtChunkData.BitResolution);
	//-------------------------------------------------------------
	
	u16* pData16 = (u16*)pData;
			
	// 8 or 16 bit samples - anything other than 16 is just seen as 8 bit
	if (fmtChunkData.BitResolution == 16) 
	{
		for (u32 i(0); i<dataChunkData.dataLength / (fmtChunkData.BitResolution/8); i++)
		{
			pData16[i] = ENDIAN16(pData16[i]);
		}
	}

	fclose ( WAVFile );


	//ExitPrintf("         %p %d %d %d %d", pRawSample->m_RawData,
	//pRawSample->m_RawDataLength,
	//pRawSample->m_NumberOfChannels,
	//pRawSample->m_SampleRate,
	//pRawSample->m_BitsPerSample); 


	m_SoundsContainer.push_back(pRawSample);

	return pRawSample;
}
