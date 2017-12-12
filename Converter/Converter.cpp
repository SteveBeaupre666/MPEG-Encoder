#include "Converter.h"

HWND main_wnd   = NULL;
HWND render_wnd = NULL;
HDC  render_hdc = NULL;

CThread Job;
JobDataStruct JobData;
DWORD WINAPI JobThread(void *params);

CBuffer InputFiles;
CBuffer OutputFiles;

BOOL Converting = FALSE;
BOOL AbortConvertion = FALSE;

//CGLEngine GLEngine;

///////////////////////////////////////////////////
// Dll entry point...
///////////////////////////////////////////////////
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwreason, LPVOID lpReserved)
{
    switch(fdwreason)
	{
    case DLL_PROCESS_ATTACH: InitDll();     break;
    case DLL_PROCESS_DETACH: ShutDownDll(); break; 
    case DLL_THREAD_ATTACH:  break;
    case DLL_THREAD_DETACH:  break;
    }
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void InitDll()
{
	av_register_all();
	avformat_network_init();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void ShutDownDll()
{
	InputFiles.Free();
	OutputFiles.Free();

	//_CleanupOpenGL();
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void EXP_FUNC _SetHandles(HWND hMainWnd, HWND hRenderWnd, HDC hRenderDC)
{
	main_wnd   = hMainWnd;
	render_wnd = hRenderWnd;
	render_hdc = hRenderDC;
}

///////////////////////////////////////////////////////////////////////////////////////////////

BOOL EXP_FUNC _InitializeOpenGL(HWND hWnd)
{
	//return GLEngine.Initialize(hWnd, false) == true;
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void EXP_FUNC _CleanupOpenGL()
{
	//GLEngine.MakeCurrentContext();
	//GLEngine.Shutdown();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void EXP_FUNC _StartJob(int files_count, char *input_files, char *output_files)
{
	#ifdef MULTI_THREADED
	if(!Job.IsRunning()){
		wglMakeCurrent(NULL, NULL);

		int in_len  = strlen(input_files);
		int out_len = strlen(output_files);

		InputFiles.Allocate(in_len+1);
		OutputFiles.Allocate(out_len+1);

		char *in  = InputFiles.String();
		char *out = OutputFiles.String();

		memcpy(in,  input_files,  in_len);
		memcpy(out, output_files, out_len);

		JobData.InputFiles  = in;
		JobData.OutputFiles = out;
		JobData.ErrorMsg    = NULL;
		JobData.NumFiles    = files_count;

		Job.Start(JobThread, &JobData);
	}
	#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

BOOL EXP_FUNC _IsJobRunning()
{
	#ifdef MULTI_THREADED
	return Job.IsRunning();
	#else
	return Converting == TRUE;
	#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

void EXP_FUNC _CancelJob()
{
	#ifdef MULTI_THREADED
	if(Job.IsRunning())
		Job.Abort();
	#else
	AbortConvertion = TRUE;
	#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI JobThread(void *params)
{
	#ifdef MULTI_THREADED
	JobDataStruct *pJobData = (JobDataStruct*)params;

	wglMakeCurrent(NULL, NULL);
	//GLEngine.MakeCurrentContext();

	const int ErrorMsgSize = 1024;
	char ErrorMsg[ErrorMsgSize];

	int n = pJobData->NumFiles;

	char *in  = pJobData->InputFiles;
	char *out = pJobData->OutputFiles;

	for(int i = 0; i < n; i++){
	
		if(Job.Aborted())
			break;

		ZeroMemory(ErrorMsg, ErrorMsgSize);

		int in_len  = GetFileNameLen(in);
		int out_len = GetFileNameLen(out);

		CBuffer InputName(in_len+1);
		CBuffer OutputName(out_len+1);

		memcpy(InputName.Get(),  &in[1],  in_len);
		memcpy(OutputName.Get(), &out[1], out_len);
		
		in  += in_len  + 3;
		out += out_len + 3;

		char *input_name  = InputName.String();
		char *output_name = OutputName.String();

		_ConvertVideo(input_name, output_name, ErrorMsg);
	}

	PostThreadExitedMsg(Job.Aborted());
	#endif

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////

DWORD EXP_FUNC _ConvertVideo(char *input_fname, char *output_fname, char *error_msg)
{
	DWORD res = 0;

	#ifndef MULTI_THREADED
	AbortConvertion = FALSE;
	Converting = TRUE;
	#endif

	CGLEngine GLEngine;

	CBuffer FrameBuffer;
	CFileIO OutputFile;

	ffmpegStruct ffmpeg;
	ZeroMemory(&ffmpeg, sizeof(ffmpeg));

    ///////////////////////////////////////////////////////////////////////////////////////

	ffmpeg.format_ctx = avformat_alloc_context();
	if(!ffmpeg.format_ctx){
        sprintf(error_msg, "Unable to allocate format context.\n");
		goto cleanup;
	}

	if(avformat_open_input(&ffmpeg.format_ctx, input_fname, NULL, NULL) != 0){
        sprintf(error_msg, "Unable to open %s.\n", input_fname);
		goto cleanup;
	}

    ///////////////////////////////////////////////////////////////////////////////////////

	if(avformat_find_stream_info(ffmpeg.format_ctx, NULL) < 0){
		sprintf(error_msg, "Unable to find stream information.\n");
		goto cleanup;
	}

	// Try to find a video stream
	int video_stream = -1;
	for(int i = 0; i < (int)ffmpeg.format_ctx->nb_streams; i++){
		if(ffmpeg.format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			video_stream = i;
			break;
		}
	}

	// Check for error...
	if(video_stream == -1){
		sprintf(error_msg, "Unable to find video stream.\n");
		goto cleanup;
	}

    ///////////////////////////////////////////////////////////////////////////////////////

	// Get codec from the stream
	AVStream* stream = ffmpeg.format_ctx->streams[video_stream];
	ffmpeg.dec_codec_ctx = stream->codec;
	ffmpeg.dec_codec = avcodec_find_decoder(ffmpeg.dec_codec_ctx->codec_id);

    // open it
    if(avcodec_open2(ffmpeg.dec_codec_ctx, ffmpeg.dec_codec, NULL) < 0){
        sprintf(error_msg, "Unable to open decoder codec.\n");
		goto cleanup;
    }

    ffmpeg.src_frame = av_frame_alloc();
    ffmpeg.dst_frame = av_frame_alloc();

    if(!ffmpeg.src_frame || !ffmpeg.dst_frame){
        sprintf(error_msg, "Unable to allocate video frame.\n");
		goto cleanup;
    }

    ///////////////////////////////////////////////////////////////////////////////////////

	// find the mpeg1 video encoder
	ffmpeg.enc_codec = avcodec_find_encoder(CODEC_ID_MPEG1VIDEO);
	if(!ffmpeg.enc_codec){
        sprintf(error_msg, "Unable to find encoder codec.\n");
		goto cleanup;
	}

	ffmpeg.enc_codec_ctx = avcodec_alloc_context3(ffmpeg.enc_codec);
	if(!ffmpeg.enc_codec_ctx){
        sprintf(error_msg, "Unable to open encoder codec.\n");
		goto cleanup;
	}

    ///////////////////////////////////////////////////////////////////////////////////////

	int src_width  = ffmpeg.dec_codec_ctx->width;
	int src_height = ffmpeg.dec_codec_ctx->height;

	int dst_width  = src_width;
	int dst_height = src_height;
	
	if(src_width > 1024){
		dst_width  = src_width  / 2;
		dst_height = src_height / 2;
	}

	const int align = 16;
	int width_gap  = dst_width  % align;
	int height_gap = dst_height % align;
	if(width_gap  != 0){dst_width  += align - width_gap;}
	if(height_gap != 0){dst_height += align - height_gap;}

	int num_pixels = dst_width * dst_height;
	int y_size = num_pixels;
	int u_size = num_pixels / 4;
	int v_size = num_pixels / 4;

	AVPixelFormat dst_format = AV_PIX_FMT_YUV420P;
	AVPixelFormat src_format = ffmpeg.dec_codec_ctx->pix_fmt;

	///////////////////////////////////////////////////////////////////////////////////////

	AVRational avRatio;
	avRatio.num = 1;
	avRatio.den = 25;

	ffmpeg.enc_codec_ctx->width  = dst_width;
	ffmpeg.enc_codec_ctx->height = dst_height;
	ffmpeg.enc_codec_ctx->bit_rate = 2000000;

	ffmpeg.enc_codec_ctx->time_base = avRatio;
	ffmpeg.enc_codec_ctx->gop_size = 10;
	ffmpeg.enc_codec_ctx->max_b_frames = 1;
	ffmpeg.enc_codec_ctx->pix_fmt = dst_format;

	// open it
	if(avcodec_open2(ffmpeg.enc_codec_ctx, ffmpeg.enc_codec, NULL) < 0){
		sprintf(error_msg, "Unable to open encoder codec.\n");
		goto cleanup;
	}

	///////////////////////////////////////////////////////////////////////////////////////

	FrameBuffer.Allocate(y_size + u_size + v_size);
	BYTE *frame_buffer = FrameBuffer.Get();

	av_image_fill_arrays(ffmpeg.dst_frame->data, ffmpeg.dst_frame->linesize, frame_buffer, dst_format, dst_width, dst_height, 1);
	
    ///////////////////////////////////////////////////////////////////////////////////////

	ffmpeg.convert_ctx = sws_getContext(src_width, src_height, src_format, dst_width, dst_height, dst_format, SWS_BICUBIC, 0, 0, 0);

    ///////////////////////////////////////////////////////////////////////////////////////

	ffmpeg.dec_packet = (AVPacket*)malloc(sizeof(AVPacket));
	ffmpeg.enc_packet = (AVPacket*)malloc(sizeof(AVPacket));

    av_init_packet(ffmpeg.dec_packet);
	ffmpeg.dec_packet->size = 0;
	ffmpeg.dec_packet->data = NULL;
	
	///////////////////////////////////////////////////////////////////////////////////////

	// Initialize OpenGL
	//bool vsync = false;
	GLEngine.Initialize(render_wnd, render_hdc);
	GLEngine.CreateTexture(dst_width, dst_height, 4);

	BYTE *pY = ffmpeg.dst_frame->data[0];
	BYTE *pU = ffmpeg.dst_frame->data[1];
	BYTE *pV = ffmpeg.dst_frame->data[2];

	///////////////////////////////////////////////////////////////////////////////////////

	if(!OutputFile.Create(output_fname)){
        sprintf(error_msg, "Unable to create %s.\n", output_fname);
		goto cleanup;
    }

	///////////////////////////////////////////////////////////////////////////////////////

	int got_frame  = 0;
	int got_output = 0;

	int frame = 1;
	int frames_count = (int)stream->nb_frames;

	UpdateProgress(0, frames_count);

	///////////////////////////////////////////////////////////////////////////////////////

	while(av_read_frame(ffmpeg.format_ctx, ffmpeg.dec_packet) >= 0){
		if(ffmpeg.dec_packet->stream_index == video_stream){
			
			#ifdef MULTI_THREADED
				if(Job.Aborted()){
					sprintf(error_msg, "Job Canceled\n");
					goto cleanup;
				}
			#else
				if(AbortConvertion){
					sprintf(error_msg, "Job Canceled\n");
					goto cleanup;
				}
			#endif


			got_frame = 0;
			int decoded = avcodec_decode_video2(ffmpeg.dec_codec_ctx, ffmpeg.src_frame, &got_frame, ffmpeg.dec_packet);			
			if(decoded < 0){
				sprintf(error_msg, "Error decoding frame %d\n", frame);
				goto cleanup;
			}

			if(got_frame){
				
				sws_scale(ffmpeg.convert_ctx, ffmpeg.src_frame->data, ffmpeg.src_frame->linesize, 0, src_height, ffmpeg.dst_frame->data, ffmpeg.dst_frame->linesize);

				GLEngine.UpdateTexture(pY, pU, pV);
				GLEngine.Render();

				av_init_packet(ffmpeg.enc_packet);
				ffmpeg.enc_packet->size = 0;
				ffmpeg.enc_packet->data = NULL;

				got_output = 0;
				int encoded = avcodec_encode_video2(ffmpeg.enc_codec_ctx, ffmpeg.enc_packet, ffmpeg.dst_frame, &got_output);
				if(encoded < 0){
					sprintf(error_msg, "Error encoding frame %d\n", frame);
					goto cleanup;
				}

				if(got_output){
					OutputFile.Write(ffmpeg.enc_packet->data, ffmpeg.enc_packet->size);
					UpdateProgress(++frame, frames_count);
					av_free_packet(ffmpeg.enc_packet);
				}
			}
		}

		av_free_packet(ffmpeg.dec_packet);
    }

    ///////////////////////////////////////////////////////////////////////////////////////

	while(1){

		#ifdef MULTI_THREADED
			if(Job.Aborted()){
				sprintf(error_msg, "Job Canceled\n");
				goto cleanup;
			}
		#else
			if(AbortConvertion){
				sprintf(error_msg, "Job Canceled\n");
				goto cleanup;
			}
		#endif

		got_frame = 0;
		int decoded = avcodec_decode_video2(ffmpeg.dec_codec_ctx, ffmpeg.src_frame, &got_frame, ffmpeg.dec_packet);
		if(decoded < 0){
			sprintf(error_msg, "Error decoding frame %d\n", frame);
			goto cleanup;
		}

		if(!got_frame)
			break;

		sws_scale(ffmpeg.convert_ctx, ffmpeg.src_frame->data, ffmpeg.src_frame->linesize, 0, src_height, ffmpeg.dst_frame->data, ffmpeg.dst_frame->linesize);

		GLEngine.UpdateTexture(pY, pU, pV);
		GLEngine.Render();

		got_output = 0;                                                    /* i think this should NOT be commented... test last */
		int encoded = avcodec_encode_video2(ffmpeg.enc_codec_ctx, ffmpeg.enc_packet, ffmpeg.dst_frame, &got_output);
		if(encoded < 0){
			sprintf(error_msg, "Error encoding frame %d\n", frame);
			goto cleanup;
		}

		if(got_output){
			OutputFile.Write(ffmpeg.enc_packet->data, ffmpeg.enc_packet->size);
			UpdateProgress(++frame, frames_count);
			av_free_packet(ffmpeg.enc_packet);
		}

		av_free_packet(ffmpeg.dec_packet);
	}
	
	res = SUCCESS;
	
	//GLEngine.Render(NULL, NULL, NULL);

cleanup:
	//GLEngine.DeleteTexture();
	GLEngine.Shutdown();

	// Write 32 bits "End code" and close the file...
	WriteEndCode(OutputFile);

	// Cleanup everything
	Cleanup(ffmpeg, FrameBuffer, OutputFile);

	#ifndef MULTI_THREADED
	Converting = FALSE;
	#endif

	return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void UpdateProgress(int frame, int frames_count)
{
	if(main_wnd){
		
		if(frames_count <= 0 || frame > frames_count)
			frame = frames_count = 0;

		#ifdef MULTI_THREADED
			PostMessage(main_wnd, WM_UPDATE_FILE_PROGRESS, frame, frames_count);
		#else
			SendMessage(main_wnd, WM_UPDATE_FILE_PROGRESS, frame, frames_count);
		#endif
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void PostThreadExitedMsg(bool canceled)
{
	#ifdef MULTI_THREADED
	if(main_wnd)
		PostMessage(main_wnd, WM_THREAD_TERMINATED, 0, canceled ? 1 : 0);
	#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

void WriteEndCode(CFileIO &OutputFile)
{
	if(OutputFile.IsOpened()){
		DWORD EndCode = 0x000001B7;
		OutputFile.Write(&EndCode, sizeof(DWORD));
		OutputFile.Close();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void FreeFrame(AVFrame** frame)
{
	if(*frame){
		av_frame_free(&(*frame));
		*frame = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void FreePacket(AVPacket** packet)
{
	if(*packet){
		free(*packet);
		*packet = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void FreeCodecCtx(AVCodecContext** codec_ctx, AVCodec** codec, bool free_ctx)
{
	if(*codec_ctx){
		avcodec_close(*codec_ctx);
		if(free_ctx)
			av_free(&(*codec_ctx));
		*codec_ctx = NULL;
	}
	*codec = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void FreeFormatCtx(AVFormatContext** format_ctx)
{
	if(*format_ctx){
		avformat_close_input(&(*format_ctx));
		*format_ctx = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void FreeConvertCtx(SwsContext** convert_ctx)
{
	if(*convert_ctx){
		sws_freeContext(*convert_ctx);
		*convert_ctx = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void Cleanup(ffmpegStruct &ffmpeg, CBuffer &FrameBuffer, CFileIO &OutputFile)
{
	FrameBuffer.Free();

	FreeFrame(&ffmpeg.src_frame);
	FreeFrame(&ffmpeg.dst_frame);
	FreePacket(&ffmpeg.dec_packet);
	FreePacket(&ffmpeg.enc_packet);	
	FreeCodecCtx(&ffmpeg.dec_codec_ctx, &ffmpeg.dec_codec, 0);
	FreeCodecCtx(&ffmpeg.enc_codec_ctx, &ffmpeg.enc_codec, 1);	
	FreeFormatCtx(&ffmpeg.format_ctx);
	FreeConvertCtx(&ffmpeg.convert_ctx);
}

///////////////////////////////////////////////////////////////////////////////////////////////

int GetFileNameLen(char *fname)
{
	int i = 0;
	int len = 0;

	while(1){
		
		char c = fname[i++];

		if(c == NULL)
			break;

		if(c == 0x22){
			if(len == 0){
				continue;
			} else {
				break;
			}
		}

		len++;		
	}

	return len;
}

