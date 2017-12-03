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
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
//----------------------------------------------------------------------//
extern "C" {
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libswscale/swscale.h"
	#include "libavutil/imgutils.h"
};
//----------------------------------------------------------------------//
#define WM_UPDATE_FILE_PROGRESS	WM_USER + 101
//----------------------------------------------------------------------//
#define SUCCESS		0
//#define ERROR_		
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
// Internal Functions
//----------------------------------------------------------------------//
void InitDll();
void ShutDownDll(){}

//----------------------------------------------------------------------//
// Exported Functions
//----------------------------------------------------------------------//
void  EXP_FUNC _SetHandles(HWND hMainWnd, HWND hRenderWnd);
DWORD EXP_FUNC _ConvertVideo(char *input_fname, char *output_fname, char *error_msg);
void  EXP_FUNC _AbortJob();

//----------------------------------------------------------------------//
// Globals Functions
//----------------------------------------------------------------------//
void WriteEndCode(CFileIO &OutputFile);
void UpdateProgress(int frame, int frames_count);

void FreeFrame(AVFrame** frame);
void FreePacket(AVPacket** packet);
void FreeConvertCtx(SwsContext** convert_ctx);
void FreeFormatCtx(AVFormatContext** format_ctx);
void FreeCodecCtx(AVCodecContext** codec_ctx, AVCodec** codec, bool free_ctx = false);
void Cleanup(ffmpegStruct &ffmpeg, CBuffer &FrameBuffer, CFileIO &OutputFile, CGLEngine &GLEngine);

