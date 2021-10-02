#ifndef SoundManager_H_
#define SoundManager_H_

#include <gccore.h>
#include <vector>
#include <string>


class RawSample
{
public:

	void Play(u8 Chan=0, u8 VolumeLeft=200, u8 VolumeRight=200);

	void SetRawData(u8* pData) {m_RawData = pData;}

	void SetRawDataLength(u32 Data) {m_RawDataLength = Data;}
	void SetNumberOfChannels(u8 Data) {m_NumberOfChannels = Data;}
	void SetSampleRate(u32 Data) {m_SampleRate = Data;}
	void SetBitsPerSample(u32 Data) { m_BitsPerSample = Data; }
private:
	u8* m_RawData;
	u32	m_RawDataLength;
	u8  m_NumberOfChannels; // uses SND_SetVoice format as defined in 'asndlib.h'
	u32 m_SampleRate;		// pitch frequency (in Hz)
	u32 m_BitsPerSample ;	// 8bits 16 bits
// u8	m_LeftVolume
// u8	m_RightVolume	
};


class SoundManager
{
public:

	SoundManager();
	~SoundManager();
		
	RawSample* LoadSound( std::string FileName );
	RawSample* GetSound( int Value );

private:

	void Init();
	void UnInit();

	//A WAV header consits of several chunks:
	struct RIFFChunk
	{
		char RIFF[4];		// "RIFF"  - big-endian form
		u32 NextChunkSize;	// 36 + SubChunk2Size, or more precisely:
                            //  4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
                            // This is the size of the rest of the chunk 
                            // following this number.  This is the size of the 
                            // entire file in bytes minus 8 bytes for the
                            // two fields not included in this count:
                            // ChunkID and ChunkSize.

		char RIFFType[4];	// Contains the letters "WAVE"  - big-endian form
	};


	struct fmtChunk
	{
		char  fmt[4];		// "fmt "  - big-endian form
		u32  fmtLength;		// 16 for PCM.  This is the size of the rest of the Subchunk which follows this number.
		u16 WaveType;		//PCM = 1 (i.e. Linear quantization) Values other than 1 indicate some form of compression.
		u16 Channels;		//Mono 1, Stereo 2
		u32 SampleRate;		//8000, 44100, etc.
		u32 BytesPerSecond; // == SampleRate * NumChannels * BitsPerSample/8
		u16 BlockAlignment; // == NumChannels * BitsPerSample/8   The number of bytes for one sample including
                            // all channels. I wonder what happens when this number isn't an integer?
		u16 BitResolution;  // 8 bits, 16 bits

	};

	struct dataChunk
	{
		char data[4];    // "data"  -  big-endian form
		u32 dataLength;  // sound data length
	};

	
	std::vector<RawSample*> m_SoundsContainer;

};


#endif