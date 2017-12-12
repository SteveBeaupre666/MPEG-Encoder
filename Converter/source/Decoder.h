#pragma once
//----------------------------------------------------------------------//
#include <Windows.h>
#include <stdio.h>
//----------------------------------------------------------------------//
#include "ffmpeg.h"
#include "Buffer.h"
//----------------------------------------------------------------------//

#define INVALID_STREAM	-1

class CDecoder {
public:
	CDecoder();
	~CDecoder();
private:
    AVPacket        *packet; 
	AVFrame         *src_frame;
    AVFrame         *dst_frame;
	AVCodec         *codec;
    AVCodecContext  *codec_ctx;
    SwsContext      *convert_ctx;
    AVFormatContext *format_ctx;
private:

	BYTE *pY;
	BYTE *pU;
	BYTE *pV;

	int y_size;
	int u_size;
	int v_size;

	int src_width;
	int src_height;
	int dst_width;
	int dst_height;
	int num_pixels;

	int frame;
	int frames_count;
	int video_stream;

	int decoded;
	int got_frame;

	AVPixelFormat src_format;
	AVPixelFormat dst_format;

	CBuffer FrameBuffer;
private:
	void Reset();
	void Cleanup();
public:
	bool OpenInputFile(char *fname);
	void CloseInputFile();

	bool AllocFormatContext();
	void GetConvertContext();

	bool FindStreamInfo();
	bool FindVideoStream();

	bool FindDecoder();
	bool OpenCodec();
	
	void SetupDecoder();
	
	bool AllocFrames();
	bool AllocPacket();

	bool AllocFrameBuffer();
	void SetupFrameBuffer();

	void InitPacket();
	void FreePacket();

	bool ReadFrame();
	void ScaleFrame();
	void DecodeFrame();

	bool DecodeVideo();
	bool IsVideoStream();

	bool InitDecoder(char *fname);
public:
	bool Decode(char *fname);
public:
	virtual void OnFrameDecoded(int frame_nb, AVPacket *packet, AVFrame* frame, BYTE *pY, BYTE *pU, BYTE *pV){}
};

