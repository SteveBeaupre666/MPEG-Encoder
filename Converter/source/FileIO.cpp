#include "FileIO.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

CFileIO::CFileIO()
{
	Initialize();
}
 
////////////////////////////////////////////////////////////////

CFileIO::~CFileIO()
{
	Close(); 
}
 
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void CFileIO::Initialize()
{
	h = INVALID_HANDLE_VALUE;
	InitializeEx();
}
 
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

UI64 CFileIO::Pack64(void *hi, void *lo)
{
	DWORD *HiPart = (DWORD*)hi;
	DWORD *LoPart = (DWORD*)lo;

	return (((UI64)(*HiPart) << 32) | (UI64)(*LoPart));
}

////////////////////////////////////////////////////////////////

void CFileIO::Unpack64(UI64 n, void *hi, void *lo)
{
	DWORD *HiPart = (DWORD*)hi;
	DWORD *LoPart = (DWORD*)lo;

	*HiPart = (DWORD)(n >> 32);
	*LoPart = (DWORD)(n);
}
 
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

bool CFileIO::IsOpened()
{
	return h != INVALID_HANDLE_VALUE;
}
 
////////////////////////////////////////////////////////////////

bool CFileIO::Open(char *fname, bool ReadOnly)
{
	Close();

	DWORD CreationMode  = OPEN_EXISTING;
	DWORD DesiredAccess = ReadOnly ? GENERIC_READ : (GENERIC_READ | GENERIC_WRITE);

	h = CreateFileA(fname, DesiredAccess, 0, NULL, CreationMode, FILE_ATTRIBUTE_NORMAL, NULL);

	return IsOpened();
}
 
////////////////////////////////////////////////////////////////

bool CFileIO::Create(char *fname)
{
	Close();

	DWORD CreationMode  = CREATE_ALWAYS;
	DWORD DesiredAccess = GENERIC_WRITE | GENERIC_READ;

	h = CreateFileA(fname, DesiredAccess, 0, NULL, CreationMode, FILE_ATTRIBUTE_NORMAL, NULL);

	return IsOpened();
}
 
////////////////////////////////////////////////////////////////

void CFileIO::Close()
{
	if(IsOpened()){
		CloseHandle(h);
		Initialize();
	}
}
 
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

UI64 CFileIO::GetSize()
{
	if(!IsOpened())
		return 0;

	DWORD Hi = 0;
	DWORD Lo = GetFileSize(h, &Hi);

	if(Lo == INVALID_FILE_SIZE && GetLastError() != NO_ERROR)
		return 0;

	return Pack64(&Hi, &Lo);
}
 
////////////////////////////////////////////////////////////////

UI64 CFileIO::Tell()
{
	if(!IsOpened())
		return 0;

	DWORD Hi = 0;
	DWORD Lo = 0;

	Lo = SetFilePointer(h, Lo, (long*)&Hi, FILE_CURRENT);
	if(Lo == INVALID_FILE_SIZE && GetLastError() != NO_ERROR)
		return 0;

	return Pack64(&Hi, &Lo);
}
 
////////////////////////////////////////////////////////////////

bool CFileIO::Seek(UI64 Offset, UINT MoveMethod)
{
	if(!IsOpened())
		return false;

	DWORD Hi, Lo;
	Unpack64(Offset, &Hi, &Lo);

	Lo = SetFilePointer(h, Lo, (long*)&Hi, MoveMethod);
	if(Lo == INVALID_FILE_SIZE && GetLastError() != NO_ERROR)
		return false;

	return true;
}
 
////////////////////////////////////////////////////////////////

void CFileIO::Flush()
{
	if(!IsOpened())
		return;

	FlushFileBuffers(h); 
}
 
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

bool CFileIO::Read(void *buffer, UINT size, UINT *num_read)
{
	if(!IsOpened() || size == 0)
		return false;

	DWORD NumBytesRead = 0;
	ReadFile(h, buffer, size, &NumBytesRead, NULL);
	if(num_read)
		*num_read = NumBytesRead;

	return NumBytesRead == size;
}
 
////////////////////////////////////////////////////////////////

bool CFileIO::Write(void *buffer, UINT size, UINT *num_written)
{
	if(!IsOpened() || size == 0)
		return false;

	DWORD NumBytesWritten = 0;
	WriteFile(h, buffer, size, &NumBytesWritten, NULL);
	if(num_written)
		*num_written = NumBytesWritten;

	return NumBytesWritten == size;
}
 
////////////////////////////////////////////////////////////////

bool CFileIO::ReadBool(bool *b)
{
	return Read(b, sizeof(bool));
}

////////////////////////////////////////////////////////////////

bool CFileIO::WriteBool(bool *b)
{
	return Write(b, sizeof(bool));
}

////////////////////////////////////////////////////////////////

bool CFileIO::ReadInt(int *n, UINT size)
{
	return Read(n, size);
}

////////////////////////////////////////////////////////////////

bool CFileIO::WriteInt(int *n, UINT size)
{
	return Write(n, size);
}

////////////////////////////////////////////////////////////////

bool CFileIO::ReadString(char *s, UINT max_size)
{
	UINT i = 0;

	while(1){
	
		if(i >= max_size)
			return false;

		char c = 0;	
		bool res = Read(&c, 1);
		if(!res)
			return false;

		if(c == 0)
			return true;

		s[i++] = c;
	}

	return false;
}

////////////////////////////////////////////////////////////////

bool CFileIO::WriteString(char *s)
{
	UINT i = 0;

	while(1){
	
		char c = s[i++];
		if(c == NULL)
			return true;

		bool res = Write(&c, 1);
		if(!res)
			return false;
	}

	return false;
}

////////////////////////////////////////////////////////////////
