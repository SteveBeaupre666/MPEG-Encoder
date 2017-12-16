#include "Decoder.h"

CDecoder::CDecoder()
{
	Reset();
}

CDecoder::~CDecoder()
{
	Cleanup();
}

void CDecoder::Reset()
{
	hMainWnd     = NULL;
	pEncoder     = NULL;
	pRenderer    = NULL;

    packet       = NULL; 
	src_frame    = NULL;
    dst_frame    = NULL;
	codec        = NULL;
    codec_ctx    = NULL;
    convert_ctx  = NULL;
    format_ctx   = NULL;
			    
	Abort        = FALSE;
	Converting   = FALSE;

	pY           = NULL;
	pU           = NULL;
	pV           = NULL;

	y_size       = 0;
	u_size       = 0;
	v_size       = 0;  

	src_width    = 0;
	src_height   = 0;

	dst_width    = 0;
	dst_height   = 0;

	num_pixels   = 0;
			     
	frame        = 0;
	frames_count = 0;

	decoded      = 0;
	got_frame    = 0;

	src_format   = AV_PIX_FMT_NONE;
	dst_format   = AV_PIX_FMT_NONE;

	video_stream = NULL;
	video_stream_indx = INVALID_STREAM;		     
}

void CDecoder::Cleanup()
{
	CleanupDecoder();
	CleanupEncoder();
	Reset();
}

void CDecoder::CleanupDecoder()
{
	if(src_frame){
		av_frame_free(&src_frame);
		src_frame = NULL;
	}

	if(dst_frame){
		av_frame_free(&dst_frame);
		dst_frame = NULL;
	}

	if(packet){
		free(packet);
		packet = NULL;
	}

	if(codec_ctx){
		avcodec_close(codec_ctx);
		codec_ctx = NULL;
		codec = NULL;
	}

	if(format_ctx){
		avformat_close_input(&format_ctx);
		format_ctx = NULL;
	}

	if(convert_ctx){
		sws_freeContext(convert_ctx);
		convert_ctx = NULL;
	}

	FrameBuffer.Free();
}

void CDecoder::CleanupEncoder()
{
	if(pEncoder){
		pEncoder->CloseOutputFile();
		pEncoder->Cleanup();
		pEncoder = NULL;
	}
}

void CDecoder::SetWindow(HWND hWnd)
{
	hMainWnd = hWnd;
}

void CDecoder::AbortConversion()
{
	Abort = TRUE;
}

//bool CDecoder::Aborted()
//{
//	return Abort != FALSE;
//}

bool CDecoder::IsConverting()
{
	return Converting != FALSE;
}

bool CDecoder::OpenInputFile(char *fname)
{
	return avformat_open_input(&format_ctx, fname, NULL, NULL) == 0;
}

bool CDecoder::AllocFormatContext()
{
	format_ctx = avformat_alloc_context();
	return format_ctx != NULL;

}

void CDecoder::GetConvertContext()
{
	convert_ctx = sws_getContext(src_width, src_height, src_format, dst_width, dst_height, dst_format, SWS_BICUBIC, 0, 0, 0);
}

bool CDecoder::FindStreamInfo()
{
	return avformat_find_stream_info(format_ctx, NULL) >= 0;
}

bool CDecoder::FindVideoStream()
{
	video_stream_indx = INVALID_STREAM;

	for(UINT i = 0; i < format_ctx->nb_streams; i++){
		if(format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			video_stream_indx = (int)i;
			video_stream = format_ctx->streams[i];
			return true;
		}
	}

	return false;
}

bool CDecoder::FindDecoder()
{
	if(video_stream_indx < 0)
		return false;

	int i = video_stream_indx;
	video_stream = format_ctx->streams[i];

	codec_ctx = video_stream->codec;
	if(codec_ctx == NULL)
		return false;

	codec = avcodec_find_decoder(codec_ctx->codec_id);
	if(codec == NULL)
		return false;

	return true;
}

bool CDecoder::OpenCodec()
{
    return avcodec_open2(codec_ctx, codec, NULL) >= 0;
}

int CDecoder::GetFramesCount()
{
	return (int)video_stream->nb_frames;
}

void CDecoder::SetupDecoder()
{
	src_width  = codec_ctx->width;
	src_height = codec_ctx->height;

	src_format = codec_ctx->pix_fmt;
	dst_format = AV_PIX_FMT_YUV420P;

	dst_width  = src_width;
	dst_height = src_height;
	
	if(src_width > 1024){
		dst_width  = src_width  / 2;
		dst_height = src_height / 2;
	}

	const int align = 16;
	int width_gap  = dst_width  % align;
	int height_gap = dst_height % align;
	if(width_gap  != 0){dst_width  += align - width_gap;}
	if(height_gap != 0){dst_height += align - height_gap;}

	num_pixels = dst_width * dst_height;
	
	y_size = num_pixels;
	u_size = num_pixels / 4;
	v_size = num_pixels / 4;

	if(pEncoder)
		pEncoder->SetupEncoder(dst_width, dst_height, 2000000, MakeRatio(1, 25));
}

bool CDecoder::AllocFrames()
{
    src_frame = av_frame_alloc();
    dst_frame = av_frame_alloc();

	return src_frame != NULL && dst_frame != NULL;
}

bool CDecoder::AllocPacket()
{
	packet = (AVPacket*)malloc(sizeof(AVPacket));
	
	return packet != NULL;
}

bool CDecoder::AllocFrameBuffer()
{
	int size = y_size + u_size + v_size;
	return FrameBuffer.Allocate(size);
}

void CDecoder::SetupFrameBuffer()
{
	av_image_fill_arrays(dst_frame->data, dst_frame->linesize, FrameBuffer.Get(), dst_format, dst_width, dst_height, 1);
	
	pY = dst_frame->data[0];
	pU = dst_frame->data[1];
	pV = dst_frame->data[2];
}

void CDecoder::InitPacket()
{
    av_init_packet(packet);
	packet->size = 0;
	packet->data = NULL;
}

void CDecoder::FreePacket()
{
	av_free_packet(packet);
}

bool CDecoder::ReadFrame()
{
	return av_read_frame(format_ctx, packet) >= 0;
}

bool CDecoder::IsVideoStream()
{
	return packet->stream_index == video_stream_indx;
}

bool CDecoder::DecodeChunk()
{
	got_frame = 0;
	decoded = avcodec_decode_video2(codec_ctx, src_frame, &got_frame, packet);			
	
	return decoded >= 0;
}

void CDecoder::ScaleFrame()
{
	sws_scale(convert_ctx, src_frame->data, src_frame->linesize, 0, src_height, dst_frame->data, dst_frame->linesize);
}

void CDecoder::RenderFrame()
{
	if(pRenderer){
		pRenderer->UpdateTexture(pY, pU, pV);
		pRenderer->Render();
	}
}

void CDecoder::CalcRemainingTime()
{
	int RemainingFrames = frames_count - frame;
	if(RemainingFrames < 0)
		return;

	//frames_per_seconds = ;
	AvgTimePerFrames = 0.0f;

	float dt = Timer.Tick();

	int NumSamples = frame < MAX_TIMES_SAMPLES ? frame : MAX_TIMES_SAMPLES - 1;
	for(int i = 0; i < NumSamples; i++){
		float t = ElapsedTime[i];
		ElapsedTime[i+1] = t;
		AvgTimePerFrames += t;
	}
	
	ElapsedTime[0]   = dt;
	AvgTimePerFrames += dt;
	AvgTimePerFrames /= (float)NumSamples;
	
	RemainingTime = AvgTimePerFrames * RemainingFrames;
}

void CDecoder::ProcessFrame()
{
	frame++;

	ScaleFrame();
	RenderFrame();

	CalcRemainingTime();
	UpdateProgress(frame, frames_count);
	
	if(pEncoder)
		pEncoder->EncodeFrame(dst_frame);
}

void CDecoder::UpdateProgress(int cur_frame, int num_frames)
{
	if(!hMainWnd)
		return;
		
	if(num_frames <= 0 || cur_frame > num_frames)
		cur_frame = num_frames = 0;

	static CProgressInfo Info;
	Info.FrameNumber      = cur_frame;
	Info.FramesCount      = num_frames;
	Info.RemainingTime    = RemainingTime;
	Info.AvgTimePerFrames = AvgTimePerFrames;
	Info.FramesPerSeconds = frames_per_seconds;

	//SendMessage(hMainWnd, WM_UPDATE_FILE_PROGRESS, (WPARAM)(DWORD)&Info, 0);
	PostMessage(hMainWnd, WM_UPDATE_FILE_PROGRESS, (WPARAM)(DWORD)&Info, 0);
}

bool CDecoder::InitDecoder(char *in, char *out)
{
	if(!AllocFormatContext())
		return false;

	if(!OpenInputFile(in))
		return false;

	if(!FindStreamInfo())
		return false;

	if(!FindVideoStream())
		return false;

	if(!FindDecoder())
		return false;

	if(!OpenCodec())
		return false;

	SetupDecoder();

	if(!AllocFrames())
		return false;

	if(!AllocPacket())
		return false;

	if(!AllocFrameBuffer())
		return false;

	SetupFrameBuffer();
	GetConvertContext();
	InitPacket();

	return true;
}

bool CDecoder::InitEncoder(char *out)
{
	if(pEncoder){
		if(!pEncoder->InitEncoder(out))
			return false;
	}

	return true;
}

void CDecoder::ProcessMessages()
{
	if(hMainWnd){

		static MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		while(PeekMessage(&msg, hMainWnd, NULL, NULL, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

UINT CDecoder::DecodeVideo(char *in, char *out, HWND hWnd, CEncoder *encoder, CRenderer *renderer, CThread *thread)
{
	Converting = TRUE;
	Abort      = FALSE;

	Cleanup();

	SetWindow(hWnd);

	pEncoder  = encoder;
	pRenderer = renderer;

	UINT res = UNKNOW_ERROR;
	if(!InitDecoder(in, out))
		goto cleanup;

	if(!InitEncoder(out))
		goto cleanup;

	if(pRenderer){
		if(!pRenderer->CreateTexture(dst_width, dst_height, 4))
			goto cleanup;
	}

	frame = 0;
	frames_count = GetFramesCount();

	Timer.Reset();
	ZeroMemory(&ElapsedTime[0], sizeof(float) * MAX_TIMES_SAMPLES);

	while(ReadFrame()){

		if(IsVideoStream()){

			#ifndef MULTI_THREADED
			if(Abort){
				res = JOB_CANCELED;
				goto cleanup;			
			}
			#else
			if(thread && thread->Aborted()){
				res = JOB_CANCELED;
				goto cleanup;			
			}
			#endif

			if(!DecodeChunk())
				goto cleanup;

			if(got_frame){
				ProcessFrame();
				//Sleep(1000);
				//goto cleanup;
			}
		}
		
		FreePacket();
	}

	while(1){

		#ifndef MULTI_THREADED
		if(Abort){
			res = JOB_CANCELED;
			goto cleanup;			
		}
		#else
		if(thread && thread->Aborted()){
			res = JOB_CANCELED;
			goto cleanup;			
		}
		#endif

		if(!DecodeChunk())
			goto cleanup;

		if(!got_frame)
			break;

		ProcessFrame();

		FreePacket();
	}

	res = JOB_SUCCEDED;

cleanup:

	if(pRenderer){
		pRenderer->DeleteTexture();
		pRenderer->Render();
	}

	if(pEncoder)
		pEncoder->WriteEndCode();

	Cleanup();

	Converting = FALSE;

	return res;
}
