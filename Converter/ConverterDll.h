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
//----------------------------------------------------------------------//
#include "Decoder.h"
//----------------------------------------------------------------------//

struct JobDataStruct {
	int   NumFiles;
	char *InputFiles;
	char *OutputFiles;
};

//----------------------------------------------------------------------//
// Internal Functions
//----------------------------------------------------------------------//
void InitDll();
void ShutDownDll();

//----------------------------------------------------------------------//
// Exported Functions
//----------------------------------------------------------------------//
void EXP_FUNC _SetHandle(HWND hWnd);

BOOL EXP_FUNC _InitializeOpenGL(HWND hWnd);
void EXP_FUNC _CleanupOpenGL();

void EXP_FUNC _Render();
void EXP_FUNC _SetClearColor(float r, float g, float b);

void EXP_FUNC _StartJob(int files_count, char *input_files, char *output_files);
BOOL EXP_FUNC _IsJobRunning();
void EXP_FUNC _CancelJob();

UINT EXP_FUNC _ConvertVideo(char *input_fname, char *output_fname);

//----------------------------------------------------------------------//
// Globals Functions
//----------------------------------------------------------------------//
void PostJobDoneMsg(bool canceled);
int  GetFileNameLen(char *fname);

//void UpdateProgress(int frame, int frames_count);
//void PostThreadExitedMsg(bool canceled);
