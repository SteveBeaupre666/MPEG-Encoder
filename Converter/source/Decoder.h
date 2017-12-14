#pragma once
//----------------------------------------------------------------------//
#include <Windows.h>
#include <stdio.h>
//----------------------------------------------------------------------//
#include "ffmpeg.h"
//----------------------------------------------------------------------//
#include "Timer.h"
#include "Buffer.h"
#include "Encoder.h"
#include "SafeKill.h"
//----------------------------------------------------------------------//
#define INVALID_STREAM	-1
//----------------------------------------------------------------------//
#define MAX_TIMES_SAMPLES	50
//----------------------------------------------------------------------//
#define JOB_CANCELED	0x00000001
#define JOB_SUCCEDED	0x00000000
#define UNKNOW_ERROR	0xFFFFFFFF
//----------------------------------------------------------------------//
#define WM_UPDATE_FILE_PROGRESS		WM_USER + 101
#define WM_THREAD_TERMINATED		WM_USER + 103
//----------------------------------------------------------------------//

struct CProgressInfo {
	int   FrameNumber;
	int   FramesCount;
	int   FramesPerSeconds;
	float AvgTimePerFrames;
	float RemainingTime;
};

class CDecoder {
public:
	CDecoder();
	~CDecoder();
private:
	CTimer Timer;
	void CalcRemainingTime();
private:
	CEncoder  *pEncoder;
	CRenderer *pRenderer;
private:
	HWND hMainWnd;
private:
    AVPacket        *packet; 
	AVFrame         *src_frame;
    AVFrame         *dst_frame;
	AVCodec         *codec;
    AVCodecContext  *codec_ctx;
    SwsContext      *convert_ctx;
    AVFormatContext *format_ctx;
private:
	BOOL Abort;
	BOOL Converting;
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

	int decoded;
	int got_frame;

	int frame;
	int frames_count;
	int frames_per_seconds;
	
	float RemainingTime;
	float AvgTimePerFrames;
	float ElapsedTime[MAX_TIMES_SAMPLES];

	CBuffer FrameBuffer;
private:
	void Reset();
public:
	bool OpenInputFile(char *fname);

	bool AllocFormatContext();
	void GetConvertContext();

	bool FindStreamInfo();
	bool FindVideoStream();

	bool FindDecoder();
	bool OpenCodec();
	
	int  GetFramesCount();

	void SetupDecoder();
	void SetupEncoder(CEncoderSettings *pSettings);
	
	bool AllocFrames();
	bool AllocPacket();

	bool AllocFrameBuffer();
	void SetupFrameBuffer();

	void InitPacket();
	void FreePacket();

	bool ReadFrame();
	void ScaleFrame();
	void RenderFrame(bool clear = false);
	void ProcessFrame();

	bool DecodeChunk();
	bool IsVideoStream();

	void SetWindow(HWND hWnd);
	void UpdateProgress(int cur_frame, int frames_count);
	
	//bool Aborted();
	bool IsConverting();

	void AbortConversion();
	void ProcessMessages();

	bool InitDecoder(char *in, char *out);
	bool InitEncoder(char *out);
	bool InitRenderer();

	UINT DecodeVideo(char *in, char *out, CEncoder *encoder, CRenderer *renderer, HWND hWnd);

	void CleanupDecoder();
	void CleanupEncoder();
	void CleanupRenderer();

	void Cleanup();
};

