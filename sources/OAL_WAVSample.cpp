/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of OALWrapper
 *
 * For conditions of distribution and use, see copyright notice in LICENSE
 */
/**
	@file OAL_WAVSample.cpp
	@author Luis Rodero
	@date 2006-10-02
	@version 0.1
	Derived class for containing WAV Sample data 
*/

#include "OALWrapper/OAL_WAVSample.h"
#include "OALWrapper/OAL_Buffer.h"
#include "OALWrapper/OAL_Helper.h"
#include "OALWrapper/OAL_Device.h"

// Pull in for OAL Types
#include <ogg/ogg.h>

//------------------------------------------------------------------

///////////////////////////////////////////////////////////
//	void Load ( const string &asFilename )
//	-	Loads sample data from a WAV file
///////////////////////////////////////////////////////////

//------------------------------------------------------------------

bool cOAL_WAVSample::CreateFromFile(const wstring &asFilename)
{
	FILE *fileHandle = OpenFileW(asFilename, L"rb");
	fseek(fileHandle, 0, SEEK_END);
	size_t pos = ftell(fileHandle);
	fseek(fileHandle, 0, SEEK_SET);

	void *buffer = malloc(pos);
	
	fread(buffer, pos, 1, fileHandle);
	fclose(fileHandle);
	bool result = CreateFromBuffer(buffer, pos);
	free(buffer);

	return result;
}

struct RIFF_Header {
	char chunkID[4];
	ogg_int32_t chunkSize;
	char format[4];
};

struct WAVE_Format {
	char subChunkID[4];
	ogg_int32_t subChunkSize;
	ogg_int16_t audioFormat;
	ogg_int16_t numChannels;
	ogg_int32_t sampleRate;
	ogg_int32_t byteRate;
	ogg_int16_t blockAlign;
	ogg_int16_t bitsPerSample;
};

struct WAVE_Data {
  char subChunkID[4]; //should contain the word data
  ogg_int32_t subChunkSize; //Stores the size of the data block
};

#define readStruct(dest, src, s) memcpy(&dest, src, sizeof(s)); src += sizeof(s)

bool cOAL_WAVSample::CreateFromBuffer(const void* apBuffer, size_t aSize)
{
	const char* start = (const char*)apBuffer;
	const char* end = start + aSize;
	const char* ptr = start;
	RIFF_Header riff_header;
	WAVE_Format wave_format;
	WAVE_Data wave_data;

	readStruct(riff_header, ptr, RIFF_Header);
	if (riff_header.chunkID[0] != 'R' ||
		riff_header.chunkID[1] != 'I' ||
		riff_header.chunkID[2] != 'F' ||
		riff_header.chunkID[3] != 'F' ||
		riff_header.format[0] != 'W' ||
		riff_header.format[1] != 'A' ||
		riff_header.format[2] != 'V' ||
		riff_header.format[3] != 'E')
	{
		return false;
	}
	readStruct(wave_format, ptr, WAVE_Format);
	if (wave_format.subChunkID[0] != 'f' ||
		wave_format.subChunkID[1] != 'm' ||
		wave_format.subChunkID[2] != 't' ||
		wave_format.subChunkID[3] != ' ')
	{
		return false;
	}
	
	if (wave_format.audioFormat != 1) {
		return false;
	}

	if (wave_format.subChunkSize > 16)
	{
		ptr += wave_format.subChunkSize - 16;
	}

	bool found = false;

	while (!found)
	{
		if (ptr > end) return false;

		readStruct(wave_data, ptr, WAVE_Data);
		if (wave_data.subChunkID[0] == 'd' &&
			wave_data.subChunkID[1] == 'a' &&
			wave_data.subChunkID[2] == 't' &&
			wave_data.subChunkID[3] == 'a')
		{
			found = true;
		} else {
			ptr += wave_data.subChunkSize;
		}
	}

	size_t size = wave_data.subChunkSize;
	if (size > (end - ptr)) {
		return false;
	}

	mlChannels = wave_format.numChannels;
	if (mlChannels == 2)
	{
		if (wave_format.bitsPerSample == 8)
		{
			mFormat = AL_FORMAT_STEREO8;
			mlSamples = size / 2;
		}
		else //if (wave_format.bitsPerSample == 16)
		{
			mlSamples = size / 4;
			mFormat = AL_FORMAT_STEREO16;
		}
	} else //if (mlChannels == 1)
	{
		if (wave_format.bitsPerSample == 8)
		{
			mlSamples = size;
			mFormat = AL_FORMAT_MONO8;
		}
		else //if (wave_format.bitsPerSample == 16)
		{
			mlSamples = size / 2;
			mFormat = AL_FORMAT_MONO16;
		}
	}
	mlFrequency = wave_format.sampleRate;
	mfTotalTime = float(mlSamples) / float(mlFrequency);
	
	cOAL_Buffer* pBuffer = mvBuffers[0];
	mbStatus = pBuffer->Feed((ALvoid*)ptr, size);

	return true;
}

//------------------------------------------------------------------


