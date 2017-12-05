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
	hDC  = NULL;
	hRC  = NULL;
	hWnd = NULL;
	
	WndWidth  = 0;
	WndHeight = 0;

	Buffer       = NULL;
	BufferBPP    = 0;
	BufferSize   = 0;
	BufferWidth  = 0;
	BufferHeight = 0;

	TexID     = 0;
	TexWidth  = 0;
	TexHeight = 0;
    TexFormat = 0;

	IsOpenGLInitialized = false;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
UINT CGLEngine::GetNextPowerOfTwo(UINT n)
{
	UINT power = 1;
	while(power < n)
		power <<= 1;

	return power;
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
bool CGLEngine::Initialize(HWND hwnd, bool vsync)
{
	if(IsInitialized())
		return false;

	hWnd = hwnd;
	hDC  = GetDC(hWnd);

	if(!SetupPixelFormatDescriptor(hDC)){
		ReleaseDC(hWnd, hDC);
		Reset();
		return false;
	}

	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);	                        

	///////////////////////////////////////////////////////////////////////////////////////////
	static const float BgCol = 0.25;
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
	DeleteTextureBuffer();

	if(hRC){wglDeleteContext(hRC);}
	if(hDC){ReleaseDC(hWnd, hDC);}
	
	Reset();
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CGLEngine::CreateTextureBuffer(UINT w, UINT h)
{
	DeleteTextureBuffer();

	BufferBPP = 0;
	switch(TexFormat)
	{
	case GL_RGB:       BufferBPP = 3; break;
	case GL_RGBA:      BufferBPP = 4; break;
	case GL_LUMINANCE: BufferBPP = 1; break;
	}

	BufferWidth  = GetNextPowerOfTwo(w);
	BufferHeight = GetNextPowerOfTwo(h);
	BufferSize   = BufferWidth * BufferHeight * BufferBPP;

	if(!Buffer){
		Buffer = new BYTE[BufferSize];
		EraseTextureBuffer();
	}
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CGLEngine::EraseTextureBuffer()
{
	if(Buffer)
		ZeroMemory(Buffer, BufferSize);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CGLEngine::DeleteTextureBuffer()
{
	if(Buffer){

		delete [] Buffer;
		Buffer = NULL;

		BufferBPP     = 0;
		BufferSize    = 0;
		BufferWidth   = 0;
		BufferHeight  = 0;
	}
}

//-----------------------------------------------------------------------------
// Create the main texture
//-----------------------------------------------------------------------------
void CGLEngine::CreateTexture(UINT w, UINT h, UINT bpp)
{
	if(!IsInitialized())
		return;

	DeleteTexture();

	TexWidth  = w;
	TexHeight = h;

	switch(bpp)
	{
	case 3: TexFormat = GL_RGB;       break;
	case 4: TexFormat = GL_RGBA;      break;
	case 1: TexFormat = GL_LUMINANCE; break;
	}

	if(!TexFormat){
		Reset();
		return;
	}

	CreateTextureBuffer(w, h);

	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_2D, TexID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, TexFormat, BufferWidth, BufferHeight, 0, TexFormat, GL_UNSIGNED_BYTE, Buffer);
}

//-----------------------------------------------------------------------------
// Update the main texture
//-----------------------------------------------------------------------------
void CGLEngine::UpdateTexture(BYTE *pY, BYTE *pU, BYTE *pV)
{
	if(!IsInitialized() || !TexID)
		return;

	yuv420p_to_rgb(pY, pU, pV, Buffer, TexWidth, TexHeight, BufferWidth, BufferBPP);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, BufferWidth, BufferHeight, TexFormat, GL_UNSIGNED_BYTE, Buffer);
}

//-----------------------------------------------------------------------------
// Delete the main texture
//-----------------------------------------------------------------------------
void CGLEngine::DeleteTexture()
{
	if(!IsInitialized())
		return;

	DeleteTextureBuffer();

	if(TexID){

		glDeleteTextures(1, &TexID);

		TexID     = 0;
		TexWidth  = 0;
		TexHeight = 0;
		TexFormat = 0;
	}
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

#ifdef USE_NEW_CODE
//-----------------------------------------------------------------------------
// Draw a textured quad on screen
//-----------------------------------------------------------------------------
void CGLEngine::DrawQuad()
{
	float ww = (float)WndWidth;
	float wh = (float)WndHeight;
	float tw = (float)TexWidth;
	float th = (float)TexHeight;
	float bw = (float)BufferWidth;
	float bh = (float)BufferHeight;

	float wnd_ratio = ww / wh;
	float tex_ratio = tw / th;

	float s = 1.0f;
	float x = 0.0f;
	float y = 0.0f;
	
	if(tex_ratio > wnd_ratio){
		s = ww / tw;
		y = (wh - th) / 2.0f;
	} else if(tex_ratio < wnd_ratio){
		s = wh / th;
		x = (ww - tw) / 2.0f;
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
#else
//-----------------------------------------------------------------------------
// Draw a textured quad on screen
//-----------------------------------------------------------------------------
void CGLEngine::DrawQuad()
{
	static const float ul = 0.0f;
	static const float ur = 1.0f;
	static const float vt = 1.0f;
	static const float vb = 0.0f;

	//////////////////////////////////////////////////////////

	float ww = (float)WndWidth;
	float wh = (float)WndHeight;
	float tw = (float)TexWidth;
	float th = (float)TexHeight;
	float bw = (float)BufferWidth;
	float bh = (float)BufferHeight;

	float x_offset = 0.0f;
	float y_offset = 0.0f;
	float xy_scale = 1.0f;
	
	float wr = ww / wh;
	float tr = tw / th;

	//int clip_l = 0;
	//int clip_r = 0;
	//int clip_t = 0;
	//int clip_b = 0;

	// If the texture and window size don't match...
	if(tr > wr){
		// Scale texture width to window size
		xy_scale = ww / tw;
		// Move smaller side to center
		//y_offset = (wh - th) / 2.0f;

		//clip_l = 0;
		//clip_t = (int)y_offset;
		//clip_r = WndWidth;
		//clip_b = WndHeight - (int)y_offset;
	} else if(tr < wr){
		// Scale texture height to window size
		xy_scale = wh / th;
		// Move smaller side to center
		//x_offset = (ww - tw) / 2.0f;

		//clip_l = (int)x_offset;
		//clip_t = 0;
		//clip_r = WndWidth - (int)x_offset;
		//clip_b = WndHeight;
	}


	//y_offset -= wh;

	float l = x_offset;
	float b = y_offset;
	float r = x_offset + (bw * xy_scale);
	float t = y_offset + (bh * xy_scale);
	

	//int cx = clip_l;
	//int cy = clip_t;
	//int cw = clip_r - clip_l;
	//int ch = clip_b - clip_t;

	//glScissor(cx, cy, cw, ch);
	//glEnable(GL_SCISSOR_TEST);

	glBindTexture(GL_TEXTURE_2D, TexID);
	
	glPushMatrix();

		glTranslatef(0.0f, -wh, 0.0f);

		glBegin(GL_QUADS);
			glTexCoord2f(ul, vt); glVertex2f(l, t);
			glTexCoord2f(ur, vt); glVertex2f(r, t);
			glTexCoord2f(ur, vb); glVertex2f(r, b);
			glTexCoord2f(ul, vb); glVertex2f(l, b);
		glEnd();

	glPopMatrix();

	//glDisable(GL_SCISSOR_TEST);
}
#endif
//-----------------------------------------------------------------------------
// Render without shaders
//-----------------------------------------------------------------------------
void CGLEngine::Render()
{
	if(!IsInitialized())
		return;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	Set2DMode();
	
	DrawQuad();

	SwapBuffers(hDC);
}
