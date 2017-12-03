#pragma once
//----------------------------------------------------------------------//
#include <Windows.h>
#include <stdio.h>
//----------------------------------------------------------------------//
#define UI64 unsigned __int64
//----------------------------------------------------------------------//

class CFileIO {
public:
	CFileIO();
	~CFileIO();
protected:
	HANDLE h;

	void Initialize();
	virtual void InitializeEx(){}

	UI64 Pack64(void *hi, void *lo);
	void Unpack64(UI64 n, void *hi, void *lo);
public:
	bool IsOpened();
	void Close();

	virtual bool Create(char *fname);
	virtual bool Open(char *fname, bool ReadOnly = true);
	
	virtual UI64 GetSize();
	virtual void Flush();

	UI64 Tell();
	bool Seek(UI64 Offset, UINT MoveMethod);

	bool Read (void *buf, UINT size, UINT *num_read = NULL);
	bool Write(void *buf, UINT size, UINT *num_written = NULL);

	bool ReadBool(bool *b);
	bool ReadInt(int *n, UINT size = sizeof(int));
	bool ReadString(char *s, UINT buf_size);
	
	bool WriteBool(bool *b);
	bool WriteInt(int *n, UINT size = sizeof(int));
	bool WriteString(char *s);
};
