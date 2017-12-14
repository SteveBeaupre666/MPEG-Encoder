#include "Encoder.h"

CEncoder::CEncoder()
{
	Reset();
}

CEncoder::~CEncoder()
{
	Cleanup();
}

void CEncoder::Reset()
{
	codec      = NULL;
    packet     = NULL; 
	codec_ctx  = NULL;

	encoded    = 0;
	got_output = 0;

	InitSettings();
}

void CEncoder::Cleanup()
{
	Output.Close();

	if(packet){
		free(packet);
		packet = NULL;
	}

	if(codec_ctx){
		avcodec_close(codec_ctx);
		//av_free(&codec_ctx);     // <--- This SHOULD BE HERE, BUT CRASH ???
		//avcodec_free_context(&codec_ctx);
		codec_ctx = NULL;
	}

	Reset();
}

AVRational MakeRatio(int num, int den)
{
	AVRational ratio;
	ratio.num = num;
	ratio.den = den;
	return ratio;
}

bool CEncoder::CreateOutputFile(char *fname)
{
	return Output.Create(fname);
}

void CEncoder::CloseOutputFile()
{
	Output.Close();
}

bool CEncoder::FindEncoder()
{
	codec = avcodec_find_encoder(CODEC_ID_MPEG1VIDEO);
	
	return codec != NULL;
}

bool CEncoder::AllocCodecContext()
{
	codec_ctx = avcodec_alloc_context3(codec);
	av_free(&codec_ctx);

	return codec_ctx != NULL;
}

void CEncoder::SetCodecContext()
{
	codec_ctx->width        = Settings.FrameWidth;
	codec_ctx->height       = Settings.FrameHeight;
	codec_ctx->pix_fmt      = Settings.PixelFormat;
	codec_ctx->bit_rate     = Settings.Bitrate;
	codec_ctx->time_base    = Settings.Framerate;
	codec_ctx->gop_size     = Settings.GopSize;
	codec_ctx->max_b_frames = Settings.Max_B_Frames;
}

bool CEncoder::OpenCodec()
{
	SetCodecContext();
	return avcodec_open2(codec_ctx, codec, NULL) >= 0;
}

bool CEncoder::AllocPacket()
{
	packet = (AVPacket*)malloc(sizeof(AVPacket));
	
	return packet != NULL;
}

void CEncoder::InitPacket()
{
    av_init_packet(packet);
	packet->size = 0;
	packet->data = NULL;
}

void CEncoder::FreePacket()
{
	av_free_packet(packet);
}

bool CEncoder::WriteFrame()
{
	if(Output.IsOpened()){
		Output.Write(packet->data, packet->size);
		return true;
	}

	return false;
}

bool CEncoder::WriteEndCode()
{
	if(Output.IsOpened()){
		DWORD EndCode = 0x000001B7;
		Output.Write(&EndCode, sizeof(DWORD));
		return true;
	}
	
	return false;
}

void CEncoder::InitSettings()
{
	static const int gop_size    = 10;
	static const int max_b_frame = 1;

	ZeroMemory(&Settings, sizeof(CEncoderSettings));

	Settings.GopSize      = gop_size;
	Settings.Max_B_Frames = max_b_frame;

	Settings.PixelFormat  = AV_PIX_FMT_YUV420P;
}

void CEncoder::SetupEncoder(int width, int height, int bitrate, AVRational framerate)
{
	InitSettings();

	Settings.FrameWidth  = width;
	Settings.FrameHeight = height;

	Settings.Bitrate   = bitrate;
	Settings.Framerate = framerate;
}

bool CEncoder::InitEncoder(char *fname)
{
	if(!FindEncoder())
		return false;

	if(!AllocCodecContext())
		return false;

	if(!OpenCodec())
		return false;

	if(!CreateOutputFile(fname))
		return false;

	if(!AllocPacket())
		return false;

	return true;
}

bool CEncoder::EncodeFrame(AVFrame* frame)
{
	InitPacket();

	got_output = 0;
	encoded = avcodec_encode_video2(codec_ctx, packet, frame, &got_output);
	if(encoded < 0){
		Cleanup();
		return false;
	}

	if(got_output){
		
		if(!WriteFrame()){
			Cleanup();
			return false;
		}

		FreePacket();
	}

	return true;
}
