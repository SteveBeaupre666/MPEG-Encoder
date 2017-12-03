#include "Texture.h"

CTextureObject::CTextureObject()
{
	Initialize();
}

CTextureObject::~CTextureObject()
{
	Free();
}

void CTextureObject::Initialize()
{
	TexID  = 0;
	Width  = 0;
	Height = 0;
	Format = 0;
}

UINT CTextureObject::GetOpenGLTextureFormat(UINT bpp)
{
	switch(bpp)
	{
	case 1: return GL_LUMINANCE;
	case 3: return GL_RGB;
	case 4: return GL_RGBA;
	}
	return 0;
}

bool CTextureObject::Create(UINT w, UINT h, UINT bpp)
{
	// Create the texture object
	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_2D, TexID);

	// Set filters mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Allocate the texture buffer
	Buffer.Allocate(w, h, bpp);

	// Fill the texture
	glTexImage2D(GL_TEXTURE_2D, 0, TexFormat, Buffer.GetWidth(), Buffer.GetHeight(), 0, TexFormat, GL_UNSIGNED_BYTE, Buffer.Get());
}

void CTextureObject::Update(BYTE *y, BYTE *u, BYTE *v)
{
	//yuv420p_to_rgb(pY, pU, pV, Buffer, TexWidth, TexHeight, BufferWidth, BufferBPP);
	
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, BufferWidth, BufferHeight, TexFormat, GL_UNSIGNED_BYTE, Buffer);
}

void CTextureObject::Free()
{
	//DeleteTextureBuffer();

	if(TexID > 0)
		glDeleteTextures(1, &TexID);

	Initialize();
}


