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

	Buffer = NULL;

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
    pfd.cDepthBits  = 32;
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
bool CGLEngine::IsEngineInitialized()
{ 
	return IsOpenGLInitialized;
}

//-----------------------------------------------------------------------------
// Initialize OpenGL
//-----------------------------------------------------------------------------
bool CGLEngine::Initialize(HWND hwnd, bool vsync)
{
	if(IsEngineInitialized())
		return false;

	hWnd = hwnd;
	hDC  = GetDC(hWnd);

	if(!SetupPixelFormatDescriptor(hDC)){
		ReleaseDC(hWnd, hDC);
		hWnd = NULL;
		hDC  = NULL;
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
	if(!IsEngineInitialized())
		return;

	DeleteTexture();
	DeleteTextureBuffer();

	if(hRC){wglDeleteContext(hRC);}
	if(hDC){ReleaseDC(hWnd, hDC);}
	
	Reset();
}

//-----------------------------------------------------------------------------
// Allocate the frame buffer
//-----------------------------------------------------------------------------
bool CGLEngine::CreateFrameBuffer(UINT w, UINT h, UINT bpp)
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
// Get texture format based on the number of bytes per pixels
//-----------------------------------------------------------------------------
UINT CGLEngine::GetOpenGLTexFormat(UINT bpp)
{
	switch(bpp)
	{
	case 1: return GL_RGB;
	case 3: return GL_RGBA;
	case 4: return GL_LUMINANCE;
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Create the OpenGL texture
//-----------------------------------------------------------------------------
bool CGLEngine::CreateTexture(UINT w, UINT h, UINT bpp)
{
	if(!IsEngineInitialized())
		return;		

	DeleteTexture();

	// Save the texture size
	TexWidth  = w;
	TexHeight = h;
	
	// Save the texture format
	TexFormat = GetOpenGLTexFormat(bpp);

	// Make sure we got the format
	if(TexFormat == 0){
		Reset();
		return false;
	}

	// Create the texture object
	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_2D, TexID);

	// Set filters mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Allocate the texture buffer
	CreateTextureBuffer(w, h);

	// Fill the texture
	glTexImage2D(GL_TEXTURE_2D, 0, TexFormat, BufferWidth, BufferHeight, 0, TexFormat, GL_UNSIGNED_BYTE, Buffer);

	return true;
}

//-----------------------------------------------------------------------------
// Update the texture
//-----------------------------------------------------------------------------
void CGLEngine::UpdateTexture(BYTE *pY, BYTE *pU, BYTE *pV)
{
	if(!IsEngineInitialized() || !TexID)
		return;

	yuv420p_to_rgb(pY, pU, pV, Buffer, TexWidth, TexHeight, BufferWidth, BufferBPP);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, BufferWidth, BufferHeight, TexFormat, GL_UNSIGNED_BYTE, Buffer);
}

//-----------------------------------------------------------------------------
// Delete the OpenGL texture
//-----------------------------------------------------------------------------
void CGLEngine::DeleteTexture()
{
	if(!IsEngineInitialized())
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
// Set the viewport to the screen size
//-----------------------------------------------------------------------------
void CGLEngine::SetViewport(int l, int t, int w, int h)
{
	if(h == 0){h = 1;}  // <--- Avoid a division by zero...
	glViewport(l, t, w, h);
	glMatrixMode(GL_PROJECTION);
}

//-----------------------------------------------------------------------------
// Setup an orthgonal (2d) view matrix
//-----------------------------------------------------------------------------
void CGLEngine::SetViewMatrix(float l, float t, float w, float h)
{
	glLoadIdentity();
	
	// Use negative height so the y axis is reversed,
	// this make calculation much easiers later on.
	gluOrtho2D(l, w, t, -h);  
	glMatrixMode(GL_MODELVIEW);
}

//-----------------------------------------------------------------------------
// Setup OpenGL to draw in 2D
//-----------------------------------------------------------------------------
void CGLEngine::Set2DMode()
{
	// Update the window size
	CalcWindowSize();

	// Setup OpenGL to draw in 2d
	SetViewport(0, 0, WndWidth, WndHeight);
	SetViewMatrix(0.0f, 0.0f, (float)WndWidth, (float)WndHeight);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CGLEngine::BuildQuad(GLQuad &Quad)
{
	float ww = (float)WndWidth;
	float wh = (float)WndHeight;
	float tw = (float)TexWidth;
	float th = (float)TexHeight;
	float bw = (float)BufferWidth;
	float bh = (float)BufferHeight;
	
	float scale    = 1.0f;
	float x_offset = 0.0f;
	float y_offset = 0.0f;
	
	float wr = ww / wh;
	float tr = tw / th;

	bool FitWidth  = tr > wr;
	bool FitHeight = tr < wr;

	// Calculate scale and offsets
	if(FitWidth){
		scale = ww / tw;
		y_offset = (wh - th) / 2.0f;
	} else if(FitHeight){
		scale = wh / th;
		x_offset = (ww - tw) / 2.0f;
	}

	Quad.v.l = x_offset;
	Quad.v.b = y_offset;
	Quad.v.r = x_offset + (bw * scale);
	Quad.v.t = y_offset + (bh * scale);
	Quad.t.l = 0.0f;
	Quad.t.b = 0.0f;
	Quad.t.r = 1.0f;
	Quad.t.t = 1.0f;
}

//-----------------------------------------------------------------------------
// Draw a textured quad on screen
//-----------------------------------------------------------------------------
void CGLEngine::DrawQuad()
{
	if(!TexID)
		return;

	GLQuad Quad;
	BuildQuad(Quad);

	// Store texture, buffer and window size
	float ww = (float)WndWidth;
	float wh = (float)WndHeight;
	float tw = (float)TexWidth;
	float th = (float)TexHeight;
	float bw = (float)BufferWidth;
	float bh = (float)BufferHeight;
	
	// Calculate texture and window ratios
	float wr = ww / wh;
	float tr = tw / th;

	float scale = 1.0f;

	float x_offset = 0.0f;
	float y_offset = 0.0f;
	
	// Calculate scale and offsets
	if(tr > wr){
		// Fit width
		scale = ww / tw;
		// Recenter on y axis
		y_offset = (wh - th) / 2.0f;
	} else if(tr < wr){
		// Fit height
		scale = wh / th;
		// Recenter on x axis
		x_offset = (ww - tw) / 2.0f;
	}

	static const float ul = 0.0f;
	static const float ur = 1.0f;
	static const float vt = 1.0f;
	static const float vb = 0.0f;

	float l = x_offset;
	float b = y_offset;
	float r = x_offset + (bw * scale);
	float t = y_offset + (bh * scale);
	
	glPushMatrix();
	{
		glTranslatef(0.0f, -wh, 0.0f);

		glBindTexture(GL_TEXTURE_2D, TexID);

		glBegin(GL_QUADS);
			GLVERTEX(ul, vt, l, t);
			GLVERTEX(ur, vt, r, t);
			GLVERTEX(ur, vb, r, b);
			GLVERTEX(ul, vb, l, b);
		glEnd();
	}
	glPopMatrix();
}

//-----------------------------------------------------------------------------
// Render the frame
//-----------------------------------------------------------------------------
void CGLEngine::RenderTexture(BYTE *pY, BYTE *pU, BYTE *pV)
{
	if(!IsEngineInitialized())
		return;

	bool have_texture = pY && pU && pV;
	if(have_texture)
		UpdateTexture(pY, pU, pV);

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	Set2DMode();
	
	if(have_texture)
		DrawQuad();

	SwapBuffers(hDC);
}
