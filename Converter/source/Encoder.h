#pragma once
//----------------------------------------------------------------------//
#include <Windows.h>
#include <stdio.h>
//----------------------------------------------------------------------//
#include "ffmpeg.h"
#include "FileIO.h"
//----------------------------------------------------------------------//

struct CEncoderSettings
{	
	int FrameWidth;
	int FrameHeight;
	AVPixelFormat PixelFormat;
	
	int Bitrate;
	AVRational Framerate;

	int GopSize;
	int Max_B_Frames;
};

//----------------------------------------------------------------------//

class CEncoder {
public:
	CEncoder();
	~CEncoder();
private:
	AVCodec         *codec;
    AVPacket        *packet; 
    AVCodecContext  *codec_ctx;
private:
	int frame_width;
	int frame_height;
	
	AVPixelFormat pixel_format;

	int encoded;
	int got_output;

	CFileIO Output;
private:
	void Reset();
	void Cleanup();
public:
	bool CreateOutputFile(char *fname);

	bool AllocCodecContext();
	bool FindEncoder();

	void SetupEncoder(CEncoderSettings *pSettings);
	bool OpenCodec();

	bool AllocPacket();
	void InitPacket();
	void FreePacket();

	bool WriteFrame();
public:
	bool InitEncoder(char *fname, CEncoderSettings *pSettings);
	bool EncodeFrame(AVFrame* frame);
	bool WriteEndCode();
	void CloseEncoder();
};

