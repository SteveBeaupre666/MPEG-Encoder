#pragma once
//----------------------------------------------------------------------//
#include <Windows.h>
#include <stdio.h>
//----------------------------------------------------------------------//
#include "ffmpeg.h"
#include "Buffer.h"
//----------------------------------------------------------------------//
#include "Encoder.h"
#include "GLEngine.h"
//----------------------------------------------------------------------//
#define INVALID_STREAM	-1
//----------------------------------------------------------------------//

extern HWND main_wnd;
extern HWND render_wnd;

extern BOOL Abort;
extern BOOL Converting;

class CDecoder {
public:
	CDecoder();
	~CDecoder();
private:
	CEncoder  *pEncoder;
	CGLEngine *pGLEngine;
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

	AVPixelFormat src_format;
	AVPixelFormat dst_format;

	AVStream* video_stream;
	AVStream* audio_stream;
	
	int video_stream_indx;
	int audio_stream_indx;

	int frame;
	int frames_count;

	int decoded;
	int got_frame;

	CBuffer FrameBuffer;
private:
	void Reset();
	void Cleanup();
public:
	bool InitOpenGL();

	bool OpenInputFile(char *fname);

	bool AllocFormatContext();
	void GetConvertContext();

	bool FindStreamInfo();
	bool FindVideoStream();

	bool FindDecoder();
	bool OpenCodec();
	
	int  GetFramesCount();

	void SetupDecoder();
	void SetupEncoder();
	
	bool AllocFrames();
	bool AllocPacket();

	bool AllocFrameBuffer();
	void SetupFrameBuffer();

	void InitPacket();
	void FreePacket();

	bool ReadFrame();
	void ScaleFrame();
	void RenderFrame();
	void ProcessFrame();

	bool DecodeChunk();
	bool IsVideoStream();
public:
	int GetFrameWidth();
	int GetFrameHeight();
	AVPixelFormat GetPixelFormat();
public:
	bool InitDecoder(char *fname, CEncoder *encoder, CGLEngine *engine);
	bool DecodeVideo();
	void CloseDecoder();
public:
	//virtual void OnFrameDecoded(int frame_nb, AVPacket *packet, AVFrame* frame, BYTE *pY, BYTE *pU, BYTE *pV){}
};

