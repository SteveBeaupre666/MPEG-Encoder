#include "Texture.h"

CTexture::CTexture()
{
	Initialize();
}

CTexture::~CTexture()
{
	Delete();
}

void CTexture::Initialize()
{
	TexID  = 0;
	Width  = 0;
	Height = 0;
	Format = 0;
}

bool CTexture::Create(UINT w, UINT h, UINT bpp)
{
	if(!Buffer.Allocate(w, h, bpp))
		return false;

	glGenTextures(1, &TexID);
	if(!TexID)
		return false;
		
	glBindTexture(GL_TEXTURE_2D, TexID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, Format, Buffer.GetWidth(), Buffer.GetHeight(), 0, Format, GL_UNSIGNED_BYTE, Buffer.Get());

	return true;
}

void CTexture::Update(BYTE *Y, BYTE *U, BYTE *V)
{
	//yuv420p_to_rgb(pY, pU, pV, Buffer, TexWidth, TexHeight, BufferWidth, BufferBPP);
	
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Buffer.GetWidth(), Buffer.GetHeight(), Format, GL_UNSIGNED_BYTE, Buffer.Get());
}

void CTexture::Delete()
{
	if(TexID > 0)
		glDeleteTextures(1, &TexID);

	Buffer.Free();
	Initialize();
}


