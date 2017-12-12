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
	frame     = NULL;
	codec     = NULL;
    packet    = NULL; 
	codec_ctx = NULL;

	width  = 0;
	height = 0;
	format = AV_PIX_FMT_NONE;

	encoded    = 0;
	got_output = 0;
}

void CEncoder::Cleanup()
{
	CloseOutputFile();

	if(packet)
		free(packet);

	if(codec_ctx){
		avcodec_close(codec_ctx);
		av_free(&codec_ctx);
	}

	Reset();
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
	
	return codec_ctx != NULL;
}

void CEncoder::SetupEncoder(int w, int h, int bitrate, AVRational framerate, AVPixelFormat pix_fmt, AVFrame* frm)
{
	//AVRational avRatio;
	//avRatio.num = 1;
	//avRatio.den = 25;

	width  = w;
	height = h;
	format = pix_fmt;

	codec_ctx->width   = width;
	codec_ctx->height  = height;
	codec_ctx->pix_fmt = pix_fmt;

	codec_ctx->bit_rate  = bitrate;
	codec_ctx->time_base = framerate;

	codec_ctx->gop_size = 10;
	codec_ctx->max_b_frames = 1;
	
	frame = frm;
}

bool CEncoder::OpenCodec()
{
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

bool CEncoder::InitEncoder(char *fname, int w, int h, int bitrate, AVRational framerate, AVPixelFormat pix_fmt, AVFrame* frm)
{
	if(!FindEncoder())
		return false;

	if(!AllocCodecContext())
		return false;

	SetupEncoder(w, h, bitrate, framerate, pix_fmt, frm);

	if(!OpenCodec())
		return false;

	if(!CreateOutputFile(fname))
		return false;

	return true;
}

bool CEncoder::EncodeFrame()
{
	InitPacket();

	got_output = 0;
	encoded = avcodec_encode_video2(codec_ctx, packet, frame, &got_output);
	if(encoded < 0){
		CloseEncoder();
		return false;
	}

	if(got_output){
		
		if(!WriteFrame()){
			CloseEncoder();
			return false;
		}

		FreePacket();
	}

	return true;
}

void CEncoder::CloseEncoder()
{
	WriteEndCode();
	Cleanup();
}
