#include "ConverterDll.h"

HWND hMainWnd = NULL;

CThread Job;
JobDataStruct JobData;
DWORD WINAPI JobThread(void *params);

CBuffer InputFiles;
CBuffer OutputFiles;

CDecoder *pDecoder = NULL;
CEncoder *pEncoder = NULL;

CRenderer Renderer;

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

	_CleanupOpenGL();
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void EXP_FUNC _SetHandle(HWND hWnd)
{
	hMainWnd = hWnd;
}

///////////////////////////////////////////////////////////////////////////////////////////////

BOOL EXP_FUNC _InitializeOpenGL(HWND hWnd)
{
	Renderer.SetWindow(hWnd);
	Renderer.CreatePrimaryContext();

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void EXP_FUNC _CleanupOpenGL()
{
	Renderer.DeletePrimaryContext();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void EXP_FUNC _Render()
{
	Renderer.SelectContext(PRIMARY_THREAD);
	Renderer.Render();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void EXP_FUNC _SetBgColor(float r, float g, float b)
{
	Renderer.SetBackgroundColor(r,g,b);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void EXP_FUNC _StartJob(int files_count, char *input_files, char *output_files)
{
	#ifdef MULTI_THREADED
	if(!Job.IsRunning()){
		
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
	return pDecoder != NULL && pDecoder->IsConverting() != FALSE;
	#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

void EXP_FUNC _CancelJob()
{
	#ifdef MULTI_THREADED
	if(Job.IsRunning())
		Job.Abort();
	#else
	if(pDecoder)
		pDecoder->AbortConversion();
	#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI JobThread(void *params)
{
	#ifdef MULTI_THREADED
	UINT res = UNKNOW_ERROR;

	JobDataStruct *pJobData = (JobDataStruct*)params;

	Renderer.CreateSecondaryContext();

	char *in  = pJobData->InputFiles;
	char *out = pJobData->OutputFiles;

	int n = pJobData->NumFiles;

	bool JobCanceled = false;

	for(int i = 0; i < n; i++){
	
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

		res = _ConvertVideo(input_name, output_name);
		if(res != JOB_SUCCEDED){
			JobCanceled = res == JOB_CANCELED;
			break;
		}
	}

	Renderer.DeleteSecondaryContext();

	PostJobDoneMsg(JobCanceled);
	#endif

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////

UINT EXP_FUNC _ConvertVideo(char *input_fname, char *output_fname)
{
	pEncoder = new CEncoder();
	pDecoder = new CDecoder();

	UINT res = UNKNOW_ERROR;

	try {
		res = pDecoder->DecodeVideo(input_fname, output_fname, hMainWnd, pEncoder, &Renderer, &Job);
	} catch(...){
		SAFE_DELETE_OBJECT(pEncoder);
		SAFE_DELETE_OBJECT(pDecoder);
		return false;
	}
	
	SAFE_DELETE_OBJECT(pEncoder);
	SAFE_DELETE_OBJECT(pDecoder);
		
	return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void PostJobDoneMsg(bool canceled)
{
	#ifdef MULTI_THREADED
	if(hMainWnd)
		PostMessage(hMainWnd, WM_THREAD_TERMINATED, 0, canceled ? 1 : 0);
	#endif
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

