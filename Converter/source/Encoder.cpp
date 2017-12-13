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
	codec        = NULL;
    packet       = NULL; 
	codec_ctx    = NULL;

	frame_width  = 0;
	frame_height = 0;
	pixel_format = AV_PIX_FMT_NONE;

	encoded      = 0;
	got_output   = 0;
}

void CEncoder::Cleanup()
{
	Output.Close();

	if(packet)
		free(packet);

	if(codec_ctx){
		avcodec_close(codec_ctx);
		//av_free(&codec_ctx);     // <--- This SHOULD BE HERE, BUT CRASH ???
		//avcodec_free_context(&codec_ctx); 	
	}

	Reset();
}

bool CEncoder::CreateOutputFile(char *fname)
{
	return Output.Create(fname);
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

void CEncoder::SetupEncoder(CEncoderSettings *pSettings)
{
	codec_ctx->width        = pSettings->FrameWidth;
	codec_ctx->height       = pSettings->FrameHeight;
	codec_ctx->pix_fmt      = pSettings->PixelFormat;
	codec_ctx->bit_rate     = pSettings->Bitrate;
	codec_ctx->time_base    = pSettings->Framerate;
	codec_ctx->gop_size     = pSettings->GopSize;
	codec_ctx->max_b_frames = pSettings->Max_B_Frames;

	frame_width  = codec_ctx->width;
	frame_height = codec_ctx->height;
	pixel_format = codec_ctx->pix_fmt;
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

bool CEncoder::InitEncoder(char *fname, CEncoderSettings *pSettings)
{
	if(!FindEncoder())
		return false;

	if(!AllocCodecContext())
		return false;

	SetupEncoder(pSettings);

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
	Cleanup();
}
