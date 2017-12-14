#pragma once
//----------------------------------------------------------------------//
#include <Windows.h>
#include <stdio.h>
//----------------------------------------------------------------------//
#include "ffmpeg.h"
//----------------------------------------------------------------------//
#include "FileIO.h"
#include "Renderer.h"
//----------------------------------------------------------------------//

struct CEncoderSettings
{	
	int Bitrate;
	int FrameWidth;
	int FrameHeight;
	
	AVRational    Framerate;
	AVPixelFormat PixelFormat;

	int GopSize;
	int Max_B_Frames;
};

//----------------------------------------------------------------------//
	
AVRational MakeRatio(int num, int den);

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
	int encoded;
	int got_output;
private:
	CFileIO Output;
	CEncoderSettings Settings;
private:
	void Reset();
public:
	bool CreateOutputFile(char *fname);
	void CloseOutputFile();

	bool AllocCodecContext();
	bool FindEncoder();
	void SetCodecContext();
	bool OpenCodec();

	void InitSettings();

	bool AllocPacket();
	void InitPacket();
	void FreePacket();

	bool WriteFrame();

	void SetupEncoder(int width, int height, int bitrate, AVRational framerate);
	bool InitEncoder(char *fname);
	
	bool EncodeFrame(AVFrame* frame);
	bool WriteEndCode();
	void Cleanup();
};

