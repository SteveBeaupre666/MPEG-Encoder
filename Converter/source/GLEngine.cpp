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
// Swap two floats values
//-----------------------------------------------------------------------------
inline void CGLEngine::Swap(float &a, float &b)
{
	float t;
	t = a;
	a = b;
	b = t;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
int CGLEngine::CharToInt(char c)
{
	if(c >= '0' && c <= '9')
		c -= 0x30;

	return (int)c;
}

//-----------------------------------------------------------------------------
// Return true if the given version is greater than current OpenGL version
//-----------------------------------------------------------------------------
bool CGLEngine::CheckVersion(OpenGLVersion version_number)
{
	char *szVersion = (char*)glGetString(GL_VERSION);

	int MajorVersion = CharToInt(szVersion[0]);
	int MinorVersion = CharToInt(szVersion[2]);

	if(MajorVersion != version_number.Major)
		return MajorVersion < version_number.Major;

	if(MajorVersion > version_number.Major){
		return false;
	} else if(MajorVersion < version_number.Major){
		return true;
	}

	return MinorVersion >= version_number.Minor;
}

//-----------------------------------------------------------------------------
// Return true if the requested extention is available
//-----------------------------------------------------------------------------
bool CGLEngine::CheckExtension(char *extName)
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
void CGLEngine::CreateTexture(UINT w, UINT h, TextureFormat fmt)
{
	if(!IsEngineInitialized())
		return;

	DeleteTexture();

	TexWidth  = w;
	TexHeight = h;

	switch(fmt)
	{
	case tfRGB:       TexFormat = GL_RGB;       break;
	case tfRGBA:      TexFormat = GL_RGBA;      break;
	case tfGrayscale: TexFormat = GL_LUMINANCE; break;
	}

	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_2D, TexID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	CreateTextureBuffer(w, h);
	glTexImage2D(GL_TEXTURE_2D, 0, TexFormat, BufferWidth, BufferHeight, 0, TexFormat, GL_UNSIGNED_BYTE, Buffer);
}

//-----------------------------------------------------------------------------
// Update the main texture
//-----------------------------------------------------------------------------
void CGLEngine::UpdateTexture(BYTE *pY, BYTE *pU, BYTE *pV)
{
	if(!IsEngineInitialized() || !TexID)
		return;

	yuv420p_to_rgb(pY, pU, pV, Buffer, TexWidth, TexHeight, BufferWidth, BufferBPP);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, BufferWidth, BufferHeight, TexFormat, GL_UNSIGNED_BYTE, Buffer);
}

//-----------------------------------------------------------------------------
// Delete the main texture
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
// Setup OpenGL to draw in 2D
//-----------------------------------------------------------------------------
void CGLEngine::Set2DMode(bool ReverseYAxis)
{
	CalcWindowSize();
	glViewport(0, 0, WndWidth, WndHeight);
	glMatrixMode(GL_PROJECTION);

	float l = 0.0f;
	float t = 0.0f;
	float w = (float)WndWidth;
	float h = (float)WndHeight;

	if(ReverseYAxis)
		Swap(t, h);

	glLoadIdentity();
	gluOrtho2D(l, w, t, h);
	glMatrixMode(GL_MODELVIEW);
}

#ifndef NEW_RENDER_CODE
//-----------------------------------------------------------------------------
// Draw a textured quad on screen
//-----------------------------------------------------------------------------
void CGLEngine::DrawQuad(UINT TextureID)
{
	if(!TextureID)
		return;

	CalcWindowSize();

	float tw = (float)TexWidth;
	float th = (float)TexHeight;
	float bw = (float)BufferWidth;
	float bh = (float)BufferHeight;
	
	float l = 0.0f;
	float r = bw;
	float t = th - bh;
	float b = th;

	//float ww = (float)WndWidth;
	//float wh = (float)WndHeight;

	//float lo = ((float)ww - tw) / 2.0f;
	//float to = ((float)wh - th) / 2.0f;

	//l -= lo;
	//r -= lo;
	//t += to;
	//b += to;

	glBindTexture(GL_TEXTURE_2D, TextureID);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(l, t);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(r, t);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(r, b);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(l, b);
	glEnd();
}
#else
//-----------------------------------------------------------------------------
// Draw a textured quad on screen
//-----------------------------------------------------------------------------
void CGLEngine::DrawQuad(UINT TextureID)
{
	if(!TextureID)
		return;

	// jouer avec les uv au lieu des vertex

	float ww = (float)WndWidth;
	float wh = (float)WndHeight;

	float tw = (float)TexWidth;
	float th = (float)TexHeight;

	float bw = (float)BufferWidth;
	float bh = (float)BufferHeight;

	float rw = 1.0f / (tw / bw);
	float rh = 1.0f / (th / bh);

	float tl = 0.0f;
	float tb = 0.0f;
	float tr = 1.0f;
	float tt = 1.0f;

	float l = 0.0f;
	float t = 0.0f;
	//float r = ww;
	//float b = wh;
	float r = ww * rw;
	float b = wh * rh;
	
	float x_offset = 0.0f;
	float y_offset = -100.0f;

	l += x_offset;
	r += x_offset;

	t += y_offset;
	b += y_offset;

	//float d = th;
	//t -= d;
	//b -= d;

	glBindTexture(GL_TEXTURE_2D, TextureID);

	glPushMatrix();

	glBegin(GL_QUADS);
		glTexCoord2f(tl, tt); glVertex2f(l, t);
		glTexCoord2f(tr, tt); glVertex2f(r, t);
		glTexCoord2f(tr, tb); glVertex2f(r, b);
		glTexCoord2f(tl, tb); glVertex2f(l, b);
	glEnd();

	//glTranslatef(0.0f, -b, 0.0f);
	glPopMatrix();
}
#endif
//-----------------------------------------------------------------------------
// Render without shaders
//-----------------------------------------------------------------------------
void CGLEngine::Render(BYTE *pY, BYTE *pU, BYTE *pV)
{
	if(!IsEngineInitialized())
		return;

	bool have_texture = pY && pU && pV;
	if(have_texture)
		UpdateTexture(pY, pU, pV);

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);

	Set2DMode();
	
	UINT TextureID = have_texture ? TexID : 0;
	DrawQuad(TextureID);

	SwapBuffers(hDC);
}
