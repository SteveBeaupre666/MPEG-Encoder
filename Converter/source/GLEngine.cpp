#include "GLEngine.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CGLEngine::CGLEngine()
{
	Reset();
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CGLEngine::~CGLEngine()
{
	Shutdown();
}

//-----------------------------------------------------------------------------
// Reset member variables
//-----------------------------------------------------------------------------
void CGLEngine::Reset()
{
	hDC = NULL;
	hRC = NULL;

	hWnd  = NULL;
	VSync = NULL;
	
	WndWidth  = 0;
	WndHeight = 0;

	ResetTextureData();

	IsOpenGLInitialized = false;
}

//-----------------------------------------------------------------------------
// Reset texture variables
//-----------------------------------------------------------------------------
void CGLEngine::ResetTextureData()
{
	TexID     = 0;
	TexWidth  = 0;
	TexHeight = 0;
    TexFormat = 0;

	BufBPP    = 0;
	BufWidth  = 0;
	BufHeight = 0;

	buf = NULL;
}

//-----------------------------------------------------------------------------
// Set the pixel format
//-----------------------------------------------------------------------------
bool CGLEngine::SetupPixelFormatDescriptor(HDC hdc)
{ 
	static const int pfd_size = sizeof(PIXELFORMATDESCRIPTOR);

    PIXELFORMATDESCRIPTOR pfd; 
	ZeroMemory(&pfd,  pfd_size);
	pfd.nSize       = pfd_size;
    pfd.dwFlags     = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.iPixelType  = PFD_TYPE_RGBA;
    pfd.dwLayerMask = PFD_MAIN_PLANE;
    pfd.cColorBits  = 32;
    pfd.cDepthBits  = 24; // 32 for no alpha bits
    pfd.cAlphaBits  = 8;
    pfd.nVersion    = 1;
	
	// This gets us a pixel format that best matches the one passed in from the device
    int pixelformat = ChoosePixelFormat(hdc, &pfd);
	if(pixelformat == 0)
		return false;

	// This sets the pixel format that we extracted from above
	if(SetPixelFormat(hdc, pixelformat, &pfd) == 0)
        return false; 
 
    return true;
}

//-----------------------------------------------------------------------------
// Return true if OpenGL is initialized
//-----------------------------------------------------------------------------
bool CGLEngine::IsInitialized()
{ 
	return IsOpenGLInitialized;
}

//-----------------------------------------------------------------------------
// Initialize OpenGL
//-----------------------------------------------------------------------------
bool CGLEngine::Initialize(HWND hwnd)
{
	if(IsInitialized())
		return false;

	//hDC  = hdc;
	hWnd = hwnd;
	hDC  = GetDC(hWnd);

	if(!SetupPixelFormatDescriptor(hDC)){
		ReleaseDC(hWnd, hDC);
		Reset();
		return false;
	}

	hRC = wglCreateContext(hDC);
	BOOL res = wglMakeCurrent(hDC, hRC);	                        

	///////////////////////////////////////////////////////////////////////////////////////////
	static const float BgCol = 0.0;
	glColor3f(1.0f, 1.0f, 1.0f);                          // Current Color
	glClearColor(BgCol, BgCol, BgCol, 0.0f);              // Black Background
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear stencil buffer
	glClearDepth(1.0f);									  // Depth Buffer Setup
	///////////////////////////////////////////////////////////////////////////////////////////
	glEnable(GL_LINE_SMOOTH);						      // Enables Depth Testing
	glShadeModel(GL_SMOOTH);                              // Enable Smooth Shading
	glDepthFunc(GL_LEQUAL);							      // The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);    // Really Nice Perspective Calculations
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);			  // Really Nice Perspective Calculations
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);				  // Really Nice Perspective Calculations
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);			  // Really Nice Perspective Calculations
	///////////////////////////////////////////////////////////////////////////////////////////
	glDisable(GL_LIGHTING);                               // Disable depth testing
	glDisable(GL_DEPTH_TEST);                             // Disable lighting
	glEnable(GL_TEXTURE_2D);                              // Enable texture mapping
	///////////////////////////////////////////////////////////////////////////////////////////
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);                // Set the texture alligment
	///////////////////////////////////////////////////////////////////////////////////////////

	if(CheckExtension("WGL_EXT_swap_control")){
		VSync = (PFVSYNC)wglGetProcAddress("wglSwapIntervalEXT");
		if(VSync)
			VSync(!VSync ? 0 : 1);
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	IsOpenGLInitialized = true;
	return IsOpenGLInitialized;
}

//-----------------------------------------------------------------------------
// Shutdown OpenGL
//-----------------------------------------------------------------------------
void CGLEngine::Shutdown()
{
	if(!IsInitialized())
		return;

	DeleteTexture();
	TextureBuffer.Free();

	if(hRC){
		//wglMakeCurrent(NULL, NULL);	                        
		wglDeleteContext(hRC);
		hRC = NULL;
	}

	if(hDC){
		ReleaseDC(hWnd, hDC);
		hDC = NULL;
	}
	
	Reset();
}

//-----------------------------------------------------------------------------
// Return the opengl texture format from the number of bytes per pixels
//-----------------------------------------------------------------------------
UINT CGLEngine::GetTextureFormat(UINT bpp)
{
	switch(bpp)
	{
	case 1: return GL_LUMINANCE;
	case 3: return GL_RGB;
	case 4: return GL_RGBA;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Return true if the number of bytes per pixels is valid
//-----------------------------------------------------------------------------
bool CGLEngine::IsTextureFormatValid(UINT bpp)
{
	return GetTextureFormat(bpp) != 0;
}

//-----------------------------------------------------------------------------
// Create the main texture
//-----------------------------------------------------------------------------
bool CGLEngine::CreateTexture(UINT w, UINT h, UINT bpp)
{
	if(!IsInitialized())
		return false;

	DeleteTexture();

	if(!IsTextureFormatValid(bpp))
		return false;

	if(!TextureBuffer.Allocate(w, h, bpp))
		return false;

	//////////////////////////////////////////////////

	TexWidth  = w;
	TexHeight = h;
	TexFormat = GetTextureFormat(bpp);

	buf       = TextureBuffer.Get();
	BufBPP    = TextureBuffer.GetBPP();
	BufWidth  = TextureBuffer.GetWidth();
	BufHeight = TextureBuffer.GetHeight();

	//////////////////////////////////////////////////

	glGenTextures(1, &TexID);
	if(!TexID){
		ResetTextureData();
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, TexID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, TexFormat, BufWidth, BufHeight, 0, TexFormat, GL_UNSIGNED_BYTE, buf);

	return true;
}

//-----------------------------------------------------------------------------
// Update the main texture
//-----------------------------------------------------------------------------
void CGLEngine::UpdateTexture(BYTE *pY, BYTE *pU, BYTE *pV)
{
	if(!IsInitialized())
		return;

	yuv420p_to_rgb(pY, pU, pV, buf, TexWidth, TexHeight, BufWidth, BufBPP);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, BufWidth, BufHeight, TexFormat, GL_UNSIGNED_BYTE, buf);
}

//-----------------------------------------------------------------------------
// Delete the main texture
//-----------------------------------------------------------------------------
void CGLEngine::DeleteTexture()
{
	if(!IsInitialized())
		return;

	TextureBuffer.Free();

	if(TexID)
		glDeleteTextures(1, &TexID);

	ResetTextureData();
}

//-----------------------------------------------------------------------------
// Calculate the rendering window size
//-----------------------------------------------------------------------------
void CGLEngine::CalcWindowSize()
{
	if(hWnd){
		RECT r;
		GetClientRect(hWnd, &r);
		WndWidth  = r.right  - r.left;
		WndHeight = r.bottom - r.top;
	}
}

//-----------------------------------------------------------------------------
// Setup OpenGL to draw in 2D
//-----------------------------------------------------------------------------
void CGLEngine::Set2DMode()
{
	CalcWindowSize();
	glViewport(0, 0, WndWidth, WndHeight);
	glMatrixMode(GL_PROJECTION);

	float l = 0.0f;
	float t = 0.0f;
	float w = (float)WndWidth;
	float h = (float)WndHeight;

	glLoadIdentity();
	gluOrtho2D(l, w, t, -h);
	glMatrixMode(GL_MODELVIEW);
}

//-----------------------------------------------------------------------------
// Draw a textured quad on screen
//-----------------------------------------------------------------------------
void CGLEngine::DrawQuad()
{
	float ww = (float)WndWidth;
	float wh = (float)WndHeight;
	float tw = (float)TexWidth;
	float th = (float)TexHeight;
	float bw = (float)BufWidth;
	float bh = (float)BufHeight;

	float wnd_ratio = ww / wh;
	float tex_ratio = tw / th;

	float s = 1.0f;
	float x = 0.0f;
	float y = 0.0f;
	
	if(tex_ratio > wnd_ratio){
		s = ww / tw;
		y = (wh - (th * s)) / 2.0f;
	} else if(tex_ratio < wnd_ratio){
		s = wh / th;
		x = (ww - (tw * s)) / 2.0f;
	}

	/////////////////////////////////////////////////////////////////

	CRect<float> UVRect(0.0f, 1.0f, 1.0f, 0.0f);
	CRect<float> QuadRect(0.0f, 0.0f, bw, bh);
	CRect<float> ClipRect(0.0f, 0.0f, tw, th);

	QuadRect.Scale(s);
	ClipRect.Scale(s);

	ClipRect.Translate(x, y);
	QuadRect.Translate(x, y - wh);

	/////////////////////////////////////////////////////////////////

	float tl,tt,tr,tb;
	float vl,vt,vr,vb;

	UVRect.GetRect(tl,tb,tr,tt);
	QuadRect.GetRect(vl,vb,vr,vt);
	
	int cx = (int)ClipRect.GetLeft();
	int cy = WndHeight - (int)(ClipRect.GetHeight() + ClipRect.GetTop());
	int cw = (int)ClipRect.GetWidth();
	int ch = (int)ClipRect.GetHeight();

	/////////////////////////////////////////////////////////////////

	glPushMatrix();

		glEnable(GL_SCISSOR_TEST);
		glScissor(cx, cy, cw, ch);

		glBegin(GL_QUADS);
			glTexCoord2f(tl, tt); glVertex2f(vl, vt);
			glTexCoord2f(tr, tt); glVertex2f(vr, vt);
			glTexCoord2f(tr, tb); glVertex2f(vr, vb);
			glTexCoord2f(tl, tb); glVertex2f(vl, vb);
		glEnd();

		glDisable(GL_SCISSOR_TEST);

	glPopMatrix();
}

//-----------------------------------------------------------------------------
// Render without shaders
//-----------------------------------------------------------------------------
void CGLEngine::Render()
{
	if(!IsInitialized())
		return;

	glClear(GL_CLEAR_FLAGS);
	glColor3f(1.0f, 1.0f, 1.0f);

	Set2DMode();
	DrawQuad();

	SwapBuffers(hDC);
}
