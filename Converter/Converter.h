//----------------------------------------------------------------------//
#define EXP_FUNC __stdcall//use the default windows api calling convention
//----------------------------------------------------------------------//
#define WIN32_LEAN_AND_MEAN // Trim libraies.
#define VC_LEANMEAN         // Trim even further.
//----------------------------------------------------------------------//
#include <Windows.h>
#include <stdio.h>
//----------------------------------------------------------------------//
#include "Buffer.h"
#include "FileIO.h"
#include "Thread.h"
//----------------------------------------------------------------------//
#include "Window.h"
#include "GLEngine.h"
//----------------------------------------------------------------------//
#include "Decoder.h"
//#include "Encoder.h"
//----------------------------------------------------------------------//
//#define MULTI_THREADED
//----------------------------------------------------------------------//
#define SUCCESS		0
//----------------------------------------------------------------------//
#define WM_UPDATE_FILE_PROGRESS		WM_USER + 101
#define WM_THREAD_TERMINATED		WM_USER + 102
//----------------------------------------------------------------------//

struct ffmpegStruct {

    AVFrame         *src_frame;
    AVFrame         *dst_frame;

    AVPacket        *dec_packet;
    AVPacket        *enc_packet;

    AVCodec         *dec_codec;
    AVCodec         *enc_codec;

    AVCodecContext  *dec_codec_ctx;
    AVCodecContext  *enc_codec_ctx;

    AVFormatContext *format_ctx;
    SwsContext      *convert_ctx;
};

//----------------------------------------------------------------------//

struct JobDataStruct {
	int   NumFiles;
	char *InputFiles;
	char *OutputFiles;
	char *ErrorMsg;
};

//----------------------------------------------------------------------//
// Internal Functions
//----------------------------------------------------------------------//
void InitDll();
void ShutDownDll();

//----------------------------------------------------------------------//
// Exported Functions
//----------------------------------------------------------------------//
void  EXP_FUNC _SetHandles(HWND hMainWnd, HWND hRenderWnd, HDC hRenderDC);

DWORD EXP_FUNC _ConvertVideo(char *input_fname, char *output_fname, char *error_msg);

void EXP_FUNC _StartJob(int files_count, char *input_files, char *output_files);
BOOL EXP_FUNC _IsJobRunning();
void EXP_FUNC _CancelJob();

BOOL EXP_FUNC _InitializeOpenGL(HWND hWnd);
void EXP_FUNC _CleanupOpenGL();

//----------------------------------------------------------------------//
// Globals Functions
//----------------------------------------------------------------------//
void WriteEndCode(CFileIO &OutputFile);

void UpdateProgress(int frame, int frames_count);
void PostThreadExitedMsg(bool canceled);

void FreeFrame(AVFrame** frame);
void FreePacket(AVPacket** packet);
void FreeConvertCtx(SwsContext** convert_ctx);
void FreeFormatCtx(AVFormatContext** format_ctx);
void FreeCodecCtx(AVCodecContext** codec_ctx, AVCodec** codec, bool free_ctx = false);
void Cleanup(ffmpegStruct &ffmpeg, CBuffer &FrameBuffer, CFileIO &OutputFile);

int GetFileNameLen(char *fname);
