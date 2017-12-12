#pragma once
//----------------------------------------------------------------------//
#include <Windows.h>
#include <stdio.h>
//----------------------------------------------------------------------//
#include "ffmpeg.h"
#include "FileIO.h"
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
	AVFrame* frame;
	AVPixelFormat format;

	int width;
	int height;

	int encoded;
	int got_output;

	CFileIO Output;
private:
	void Reset();
	void Cleanup();
public:
	bool CreateOutputFile(char *fname);
	void CloseOutputFile();

	bool AllocCodecContext();
	bool OpenCodec();

	bool FindEncoder();
	void SetupEncoder(int w, int h, int bitrate, AVRational framerate, AVPixelFormat pix_fmt, AVFrame* frm);

	bool AllocPacket();
	void InitPacket();
	void FreePacket();

	bool WriteFrame();
	bool WriteEndCode();
public:
	bool InitEncoder(char *fname, int w, int h, int bitrate, AVRational framerate, AVPixelFormat pix_fmt, AVFrame* frm);
	bool EncodeFrame();
	void CloseEncoder();
};

