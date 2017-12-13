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
	pEncoder     = NULL;
	pGLEngine    = NULL;

    packet       = NULL; 
	src_frame    = NULL;
    dst_frame    = NULL;
	codec        = NULL;
    codec_ctx    = NULL;
    convert_ctx  = NULL;
    format_ctx   = NULL;
			    
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
	pGLEngine->Shutdown();

	FrameBuffer.Free();

	if(src_frame)
		av_frame_free(&src_frame);

	if(dst_frame)
		av_frame_free(&dst_frame);

	if(packet)
		free(packet);

	if(codec_ctx)
		avcodec_close(codec_ctx);

	if(format_ctx)
		avformat_close_input(&format_ctx);

	if(convert_ctx)
		sws_freeContext(convert_ctx);

	Reset();
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
	if(pGLEngine){
		pGLEngine->UpdateTexture(pY, pU, pV);
		pGLEngine->Render();
	}
}

void CDecoder::ProcessFrame()
{
	frame++;

	ScaleFrame();
	RenderFrame();

	if(pEncoder)
		pEncoder->EncodeFrame(dst_frame);
}

int CDecoder::GetFrameWidth()
{
	return dst_width;
}

int CDecoder::GetFrameHeight()
{
	return dst_height;
}

AVPixelFormat CDecoder::GetPixelFormat()
{
	return dst_format;
}

bool CDecoder::InitOpenGL()
{	
	if(!pGLEngine)
		return false;

	if(!pGLEngine->Initialize(render_wnd))
		return false;

	if(!pGLEngine->CreateTexture(dst_width, dst_height, 4))
		return false;

	return true;
}

bool CDecoder::InitDecoder(char *fname, CEncoder *encoder, CGLEngine *engine)
{
	pEncoder  = encoder;
	pGLEngine = engine;

	if(!AllocFormatContext())
		return false;

	if(!OpenInputFile(fname))
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

	if(!InitOpenGL())
		return false;

	return true;
}

bool CDecoder::DecodeVideo()
{
	bool res = false;

	InitPacket();

	while(ReadFrame()){

		if(IsVideoStream()){

			if(Abort)
				goto cleanup;

			if(!DecodeChunk())
				goto cleanup;

			if(got_frame)
				ProcessFrame();
		}
		
		FreePacket();
	}

	while(1){

		if(Abort)
			goto cleanup;

		if(!DecodeChunk())
			goto cleanup;

		if(!got_frame)
			break;

		ProcessFrame();

		FreePacket();
	}

	res = true;

cleanup:

	if(pEncoder)
		pEncoder->WriteEndCode();

	CloseDecoder();

	return res;
}

void CDecoder::CloseDecoder()
{
	Cleanup();
}
