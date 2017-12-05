#include "GLExtentions.h"

//-----------------------------------------------------------------------------
// Return true if the requested extention is available
//-----------------------------------------------------------------------------
bool CheckExtension(char *extName)
{
	char *extList = (char*) glGetString(GL_EXTENSIONS);
	if(!extName || !extList)
		return false;

	while(*extList){

		UINT ExtLen = (int)strcspn(extList, " ");
		if(strlen(extName) == ExtLen && strncmp(extName, extList, ExtLen) == 0)
			return true;

		extList += ExtLen + 1;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Return true if the given version is greater than current OpenGL version
//-----------------------------------------------------------------------------
bool CheckVersion(int MajVer, int MinVer)
{
	char *Ver = (char*) glGetString(GL_VERSION);
	return (Ver[0]-48 > MajVer || (Ver[0]-48 == MajVer && Ver[2]-48 >= MinVer));
}

