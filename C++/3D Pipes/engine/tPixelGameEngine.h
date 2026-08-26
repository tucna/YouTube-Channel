/*
  +-------------------------------------------------------------+
  |   Tucna's version of OneLoneCoder Pixel Game Engine v1.21   |
  | "Like the command prompt console one, but not..." - javidx9 |
  +-------------------------------------------------------------+

  What is this?
  ~~~~~~~~~~~~~
  A simple engine built on the javidx9's oclPixelGameEngine. The main difference
  is in backend where OpenGL was replaced by DirectX 11.

  License (OLC-3)
  ~~~~~~~~~~~~~~~

  Copyright 2018 - 2019 OneLoneCoder.com

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

  1. Redistributions or derivations of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions or derivative works in binary form must reproduce the above
  copyright notice. This list of conditions and the following	disclaimer must be
  reproduced in the documentation and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its contributors may
  be used to endorse or promote products derived from this software without specific
  prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS	"AS IS" AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
  SHALL THE COPYRIGHT	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL,	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGE.
*/

#ifndef T_PGE_DEF
#define T_PGE_DEF

#ifdef _WIN32
// Link to libraries
#ifdef _MSC_VER
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#else
#error unsupported compiler
#endif

#include <windows.h>
#include <gdiplus.h>
#include <Shlwapi.h>

// DirectX related headers
#include <wrl/client.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#else
#error unsupported os
#endif

// Standard includes
#include <cmath>
#include <cstdint>
#include <string>
#include <iostream>
#include <streambuf>
#include <sstream>
#include <chrono>
#include <vector>
#include <fstream>
#include <map>
#include <functional>
#include <algorithm>

  // C++17 onwards
#include <filesystem>
namespace _gfs = std::filesystem;

#undef min
#undef max
#define UNUSED(x) (void)(x)

namespace tDX // tucna - DirectX
{
  struct Pixel
  {
    union
    {
      uint32_t n = 0xFF000000;
      struct
      {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
      };
    };

    Pixel();
    Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
    Pixel(uint32_t p);
    enum Mode { NORMAL, MASK, ALPHA, CUSTOM };

    bool operator==(const Pixel& p) const;
    bool operator!=(const Pixel& p) const;
  };

  // Some constants for symbolic naming of Pixels
  static const Pixel
    WHITE(255, 255, 255),
    GREY(192, 192, 192), DARK_GREY(128, 128, 128), VERY_DARK_GREY(64, 64, 64),
    RED(255, 0, 0), DARK_RED(128, 0, 0), VERY_DARK_RED(64, 0, 0),
    YELLOW(255, 255, 0), DARK_YELLOW(128, 128, 0), VERY_DARK_YELLOW(64, 64, 0),
    GREEN(0, 255, 0), DARK_GREEN(0, 128, 0), VERY_DARK_GREEN(0, 64, 0),
    CYAN(0, 255, 255), DARK_CYAN(0, 128, 128), VERY_DARK_CYAN(0, 64, 64),
    BLUE(0, 0, 255), DARK_BLUE(0, 0, 128), VERY_DARK_BLUE(0, 0, 64),
    MAGENTA(255, 0, 255), DARK_MAGENTA(128, 0, 128), VERY_DARK_MAGENTA(64, 0, 64),
    BLACK(0, 0, 0),
    BLANK(0, 0, 0, 0);

  enum rcode
  {
    FAIL = 0,
    OK = 1,
    NO_FILE = -1,
  };

  //==================================================================================

  template <class T>
  struct v2d_generic
  {
    T x = 0;
    T y = 0;

    inline v2d_generic() : x(0), y(0) {                                                      }
    inline v2d_generic(T _x, T _y) : x(_x), y(_y) {                                                      }
    inline v2d_generic(const v2d_generic& v) : x(v.x), y(v.y) {                                                      }
    inline T mag() { return sqrt(x * x + y * y); }
    inline T mag2() { return x * x + y * y; }
    inline v2d_generic  norm() { T r = 1 / mag(); return v2d_generic(x*r, y*r); }
    inline v2d_generic  perp() { return v2d_generic(-y, x); }
    inline T dot(const v2d_generic& rhs) { return this->x * rhs.x + this->y * rhs.y; }
    inline T cross(const v2d_generic& rhs) { return this->x * rhs.y - this->y * rhs.x; }
    inline v2d_generic  operator +  (const v2d_generic& rhs) const { return v2d_generic(this->x + rhs.x, this->y + rhs.y); }
    inline v2d_generic  operator -  (const v2d_generic& rhs) const { return v2d_generic(this->x - rhs.x, this->y - rhs.y); }
    inline v2d_generic  operator *  (const T& rhs)           const { return v2d_generic(this->x * rhs, this->y * rhs); }
    inline v2d_generic  operator /  (const T& rhs)           const { return v2d_generic(this->x / rhs, this->y / rhs); }
    inline v2d_generic& operator += (const v2d_generic& rhs) { this->x += rhs.x; this->y += rhs.y; return *this; }
    inline v2d_generic& operator -= (const v2d_generic& rhs) { this->x -= rhs.x; this->y -= rhs.y; return *this; }
    inline v2d_generic& operator *= (const T& rhs) { this->x *= rhs; this->y *= rhs; return *this; }
    inline v2d_generic& operator /= (const T& rhs) { this->x /= rhs; this->y /= rhs; return *this; }
    inline T& operator [] (std::size_t i) { return *((T*)this + i);	   /* <-- D'oh :( */ }
    inline operator v2d_generic<int>() const { return { static_cast<int32_t>(this->x), static_cast<int32_t>(this->y) }; }
    inline operator v2d_generic<float>() const { return { static_cast<float>(this->x), static_cast<float>(this->y) }; }
  };

  template<class T> inline v2d_generic<T> operator * (const float& lhs, const v2d_generic<T>& rhs) { return v2d_generic<T>(lhs * rhs.x, lhs * rhs.y); }
  template<class T> inline v2d_generic<T> operator * (const double& lhs, const v2d_generic<T>& rhs) { return v2d_generic<T>(lhs * rhs.x, lhs * rhs.y); }
  template<class T> inline v2d_generic<T> operator * (const int& lhs, const v2d_generic<T>& rhs) { return v2d_generic<T>(lhs * rhs.x, lhs * rhs.y); }
  template<class T> inline v2d_generic<T> operator / (const float& lhs, const v2d_generic<T>& rhs) { return v2d_generic<T>(lhs / rhs.x, lhs / rhs.y); }
  template<class T> inline v2d_generic<T> operator / (const double& lhs, const v2d_generic<T>& rhs) { return v2d_generic<T>(lhs / rhs.x, lhs / rhs.y); }
  template<class T> inline v2d_generic<T> operator / (const int& lhs, const v2d_generic<T>& rhs) { return v2d_generic<T>(lhs / rhs.x, lhs / rhs.y); }

  typedef v2d_generic<int32_t> vi2d;
  typedef v2d_generic<float> vf2d;
  typedef v2d_generic<double> vd2d;

  //=============================================================

  struct HWButton
  {
    bool bPressed = false;	// Set once during the frame the event occurs
    bool bReleased = false;	// Set once during the frame the event occurs
    bool bHeld = false;		// Set true for all frames between pressed and released events
  };

  //=============================================================

  struct ResourceBuffer : public std::streambuf
  {
    ResourceBuffer(std::ifstream &ifs, uint32_t offset, uint32_t size);
    std::vector<char> vMemory;
  };

  class ResourcePack : public std::streambuf
  {
  public:
    ResourcePack();
    ~ResourcePack();
    bool AddFile(const std::string& sFile);
    bool LoadPack(const std::string& sFile, const std::string& sKey);
    bool SavePack(const std::string& sFile, const std::string& sKey);
    ResourceBuffer GetFileBuffer(const std::string& sFile);
    bool Loaded();
  private:
    struct sResourceFile { uint32_t nSize; uint32_t nOffset; };
    std::map<std::string, sResourceFile> mapFiles;
    std::ifstream baseFile;
    const std::string scramble(const std::string& data, const std::string& key);
    std::string makeposix(const std::string& path);
  };

  //=============================================================

  // A bitmap-like structure that stores a 2D array of Pixels
  class Sprite
  {
  public:
    Sprite();
    Sprite(std::string sImageFile, tDX::ResourcePack *pack = nullptr);
    Sprite(int32_t w, int32_t h);
    ~Sprite();

  public:
    tDX::rcode LoadFromFile(std::string sImageFile, tDX::ResourcePack *pack = nullptr);
    tDX::rcode LoadFromPGESprFile(std::string sImageFile, tDX::ResourcePack *pack = nullptr);
    tDX::rcode SaveToPGESprFile(std::string sImageFile);

  public:
    int32_t width = 0; // int32 here, really?
    int32_t height = 0;
    enum Mode { NORMAL, PERIODIC };

  public:
    void SetSampleMode(tDX::Sprite::Mode mode = tDX::Sprite::Mode::NORMAL);
    Pixel GetPixel(int32_t x, int32_t y);
    bool  SetPixel(int32_t x, int32_t y, Pixel p);

    Pixel Sample(float x, float y);
    Pixel SampleBL(float u, float v);
    Pixel* GetData();

  private:
    Pixel *pColData = nullptr;
    Mode modeSample = Mode::NORMAL;

#ifdef T_DBG_OVERDRAW
  public:
    static int nOverdrawCount;
#endif

  };

  //=============================================================

  enum Key
  {
    NONE,
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    K0, K1, K2, K3, K4, K5, K6, K7, K8, K9,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    UP, DOWN, LEFT, RIGHT,
    SPACE, TAB, SHIFT, CTRL, INS, DEL, HOME, END, PGUP, PGDN,
    BACK, ESCAPE, RETURN, ENTER, PAUSE, SCROLL,
    NP0, NP1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9,
    NP_MUL, NP_DIV, NP_ADD, NP_SUB, NP_DECIMAL,
  };


  //=============================================================

  class PixelGameEngine
  {
  public:
    PixelGameEngine();

  public:
    tDX::rcode	Construct(uint32_t screen_w, uint32_t screen_h, uint32_t pixel_w, uint32_t pixel_h, bool full_screen = false, bool vsync = false);
    tDX::rcode	Start();

  public: // Override Interfaces
    // Called once on application startup, use to load your resources
    virtual bool OnUserCreate();
    // Called every frame, and provides you with a time per frame value
    virtual bool OnUserUpdate(float fElapsedTime);
    // Called once on application termination, so you can be a clean coder
    virtual bool OnUserDestroy();

  public: // Hardware Interfaces
    // Returns true if window is currently in focus
    bool IsFocused();
    // Get the state of a specific keyboard button
    HWButton GetKey(Key k);
    // Get the state of a specific mouse button
    HWButton GetMouse(uint32_t b);
    // Get Mouse X coordinate in "pixel" space
    int32_t GetMouseX();
    // Get Mouse Y coordinate in "pixel" space
    int32_t GetMouseY();
    // Get Mouse Wheel Delta
    int32_t GetMouseWheel();

  public: // Utility
    // Returns the width of the screen in "pixels"
    int32_t ScreenWidth();
    // Returns the height of the screen in "pixels"
    int32_t ScreenHeight();
    // Returns the width of the currently selected drawing target in "pixels"
    int32_t GetDrawTargetWidth();
    // Returns the height of the currently selected drawing target in "pixels"
    int32_t GetDrawTargetHeight();
    // Returns the currently active draw target
    Sprite* GetDrawTarget();

  public: // Draw Routines
    // Specify which Sprite should be the target of drawing functions, use nullptr
    // to specify the primary screen
    void SetDrawTarget(Sprite *target);
    // Change the pixel mode for different optimisations
    // tDX::Pixel::NORMAL = No transparency
    // tDX::Pixel::MASK   = Transparent if alpha is < 255
    // tDX::Pixel::ALPHA  = Full transparency
    void SetPixelMode(Pixel::Mode m);
    Pixel::Mode GetPixelMode();
    // Use a custom blend function
    void SetPixelMode(std::function<tDX::Pixel(const int x, const int y, const tDX::Pixel& pSource, const tDX::Pixel& pDest)> pixelMode);
    // Change the blend factor form between 0.0f to 1.0f;
    void SetPixelBlend(float fBlend);
    // Offset texels by sub-pixel amount (advanced, do not use)
    void SetSubPixelOffset(float ox, float oy);

    // Draws a single Pixel
    virtual bool Draw(int32_t x, int32_t y, Pixel p = tDX::WHITE);
    bool Draw(const tDX::vi2d& pos, Pixel p = tDX::WHITE);
    // Draws a line from (x1,y1) to (x2,y2)
    void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pixel p = tDX::WHITE, uint32_t pattern = 0xFFFFFFFF);
    void DrawLine(const tDX::vi2d& pos1, const tDX::vi2d& pos2, Pixel p = tDX::WHITE, uint32_t pattern = 0xFFFFFFFF);
    // Draws a circle located at (x,y) with radius
    void DrawCircle(int32_t x, int32_t y, int32_t radius, Pixel p = tDX::WHITE, uint8_t mask = 0xFF);
    void DrawCircle(const tDX::vi2d& pos, int32_t radius, Pixel p = tDX::WHITE, uint8_t mask = 0xFF);
    // Fills a circle located at (x,y) with radius
    void FillCircle(int32_t x, int32_t y, int32_t radius, Pixel p = tDX::WHITE);
    void FillCircle(const tDX::vi2d& pos, int32_t radius, Pixel p = tDX::WHITE);
    // Draws a rectangle at (x,y) to (x+w,y+h)
    void DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p = tDX::WHITE);
    void DrawRect(const tDX::vi2d& pos, const tDX::vi2d& size, Pixel p = tDX::WHITE);
    // Fills a rectangle at (x,y) to (x+w,y+h)
    void FillRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p = tDX::WHITE);
    void FillRect(const tDX::vi2d& pos, const tDX::vi2d& size, Pixel p = tDX::WHITE);
    // Draws a triangle between points (x1,y1), (x2,y2) and (x3,y3)
    void DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p = tDX::WHITE);
    void DrawTriangle(const tDX::vi2d& pos1, const tDX::vi2d& pos2, const tDX::vi2d& pos3, Pixel p = tDX::WHITE);
    // Flat fills a triangle between points (x1,y1), (x2,y2) and (x3,y3)
    void FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p = tDX::WHITE);
    void FillTriangle(const tDX::vi2d& pos1, const tDX::vi2d& pos2, const tDX::vi2d& pos3, Pixel p = tDX::WHITE);
    // Draws an entire sprite at location (x,y)
    void DrawSprite(int32_t x, int32_t y, Sprite *sprite, uint32_t scale = 1);
    void DrawSprite(const tDX::vi2d& pos, Sprite *sprite, uint32_t scale = 1);
    // Draws an area of a sprite at location (x,y), where the
    // selected area is (ox,oy) to (ox+w,oy+h)
    void DrawPartialSprite(int32_t x, int32_t y, Sprite *sprite, int32_t ox, int32_t oy, int32_t w, int32_t h, uint32_t scale = 1);
    void DrawPartialSprite(const tDX::vi2d& pos, Sprite *sprite, const tDX::vi2d& sourcepos, const tDX::vi2d& size, uint32_t scale = 1);
    // Draws a single line of text
    void DrawString(int32_t x, int32_t y, const std::string& sText, Pixel col = tDX::WHITE, uint32_t scale = 1);
    void DrawString(const tDX::vi2d& pos, const std::string& sText, Pixel col = tDX::WHITE, uint32_t scale = 1);
    // Clears entire draw target to Pixel
    void Clear(Pixel p);
    // Resize the primary screen sprite
    void SetScreenSize(int w, int h);

  public: // Branding
    std::string sAppName;

  private: // Inner mysterious workings
    struct Vertex
    {
      DirectX::XMFLOAT3 position;
      DirectX::XMFLOAT2 texCoord;
    };

    Sprite		*pDefaultDrawTarget = nullptr;
    Sprite		*pDrawTarget = nullptr;
    Pixel::Mode	nPixelMode = Pixel::Mode::NORMAL;
    float		fBlendFactor = 1.0f;
    uint32_t	nScreenWidth = 256;
    uint32_t	nScreenHeight = 240;
    uint32_t	nPixelWidth = 4;
    uint32_t	nPixelHeight = 4;
    int32_t		nMousePosX = 0;
    int32_t		nMousePosY = 0;
    int32_t		nMouseWheelDelta = 0;
    int32_t		nMousePosXcache = 0;
    int32_t		nMousePosYcache = 0;
    int32_t		nMouseWheelDeltaCache = 0;
    int32_t		nWindowWidth = 0;
    int32_t		nWindowHeight = 0;
    int32_t		nViewX = 0;
    int32_t		nViewY = 0;
    int32_t		nViewW = 0;
    int32_t		nViewH = 0;
    bool		bFullScreen = false;
    float		fPixelX = 1.0f;
    float		fPixelY = 1.0f;
    float		fSubPixelOffsetX = 0.0f;
    float		fSubPixelOffsetY = 0.0f;
    bool		bHasInputFocus = false;
    bool		bHasMouseFocus = false;
    bool		bEnableVSYNC = false;
    float		fFrameTimer = 1.0f;
    int			nFrameCount = 0;
    Sprite		*fontSprite = nullptr;
    std::function<tDX::Pixel(const int x, const int y, const tDX::Pixel&, const tDX::Pixel&)> funcPixelMode;

    static std::map<size_t, uint8_t> mapKeys;
    bool		pKeyNewState[256]{ 0 };
    bool		pKeyOldState[256]{ 0 };
    HWButton	pKeyboardState[256];

    bool		pMouseNewState[5]{ 0 };
    bool		pMouseOldState[5]{ 0 };
    HWButton	pMouseState[5];

    Microsoft::WRL::ComPtr<ID3D11Device>              m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>       m_d3dContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain1>           m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>    m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>         m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>        m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>         m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>              m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>              m_indexBuffer;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>        m_samplerState;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>           m_texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_textureView;

    // If anything sets this flag to false, the engine "should" shut down gracefully
    static bool bActive;
    // If anything sets this flag to true, the window resizing shoudl be handled
    static bool bResize;

    // Common initialisation functions
    void tDX_UpdateMouse(int32_t x, int32_t y);
    void tDX_UpdateMouseWheel(int32_t delta);
    void tDX_UpdateWindowSize(int32_t x, int32_t y);
    void tDX_UpdateViewport();
    void tDX_DirectXCreateResources();
    bool tDX_DirectXCreateDevice();
    void tDX_ConstructFontSheet();

    // Windows specific window handling
    HWND tDX_hWnd = nullptr;
    HWND tDX_WindowCreate();
    std::wstring wsAppName;
    static LRESULT CALLBACK tDX_WindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  };


  class PGEX
  {
    friend class tDX::PixelGameEngine;
  protected:
    static PixelGameEngine* pge;
  };

  //=============================================================
}

#endif // T_PGE_DEF




/*
  Object Oriented Mode
  ~~~~~~~~~~~~~~~~~~~~

  If the tPixelGameEngine.h is called from several sources it can cause
  multiple definitions of objects. To prevent this, ONLY ONE of the pathways
  to including this file must have T_PGE_APPLICATION defined before it. This prevents
  the definitions being duplicated.

  If all else fails, create a file called "tPixelGameEngine.cpp" with the following
  two lines. Then you can just #include "tPixelGameEngine.h" as normal without worrying
  about defining things. Dont forget to include that cpp file as part of your build!

  #define T_PGE_APPLICATION
  #include "tPixelGameEngine.h"

*/

#ifdef T_PGE_APPLICATION
#undef T_PGE_APPLICATION

namespace tDX
{
  Pixel::Pixel()
  {
    r = 0; g = 0; b = 0; a = 255;
  }

  Pixel::Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
  {
    r = red; g = green; b = blue; a = alpha;
  }

  Pixel::Pixel(uint32_t p)
  {
    n = p;
  }

  bool Pixel::operator==(const Pixel& p) const
  {
    return n == p.n;
  }

  bool Pixel::operator!=(const Pixel& p) const
  {
    return n != p.n;
  }

  //==========================================================

  std::wstring ConvertS2W(std::string s)
  {
    int count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
    wchar_t* buffer = new wchar_t[count];
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buffer, count);

    std::wstring w(buffer);
    delete[] buffer;
    return w;
  }

  Sprite::Sprite()
  {
    pColData = nullptr;
    width = 0;
    height = 0;
  }

  Sprite::Sprite(std::string sImageFile, tDX::ResourcePack *pack)
  {
    LoadFromFile(sImageFile, pack);
  }

  Sprite::Sprite(int32_t w, int32_t h)
  {
    if (pColData) delete[] pColData;
    width = w;		height = h;
    pColData = new Pixel[width * height];
    for (int32_t i = 0; i < width*height; i++)
      pColData[i] = Pixel();
  }

  Sprite::~Sprite()
  {
    if (pColData) delete pColData;
  }

  tDX::rcode Sprite::LoadFromPGESprFile(std::string sImageFile, tDX::ResourcePack *pack)
  {
    if (pColData) delete[] pColData;

    auto ReadData = [&](std::istream &is)
    {
      is.read((char*)&width, sizeof(int32_t));
      is.read((char*)&height, sizeof(int32_t));
      pColData = new Pixel[width * height];
      is.read((char*)pColData, width * height * sizeof(uint32_t));
    };

    // These are essentially Memory Surfaces represented by tDX::Sprite
    // which load very fast, but are completely uncompressed
    if (pack == nullptr)
    {
      std::ifstream ifs;
      ifs.open(sImageFile, std::ifstream::binary);
      if (ifs.is_open())
      {
        ReadData(ifs);
        return tDX::OK;
      }
      else
        return tDX::FAIL;
    }
    else
    {
      ResourceBuffer rb = pack->GetFileBuffer(sImageFile);
      std::istream is(&rb);
      ReadData(is);
    }


    return tDX::FAIL;
  }

  tDX::rcode Sprite::SaveToPGESprFile(std::string sImageFile)
  {
    if (pColData == nullptr) return tDX::FAIL;

    std::ofstream ofs;
    ofs.open(sImageFile, std::ifstream::binary);
    if (ofs.is_open())
    {
      ofs.write((char*)&width, sizeof(int32_t));
      ofs.write((char*)&height, sizeof(int32_t));
      ofs.write((char*)pColData, width*height * sizeof(uint32_t));
      ofs.close();
      return tDX::OK;
    }

    return tDX::FAIL;
  }

#if defined(__linux__)
  void pngReadStream(png_structp pngPtr, png_bytep data, png_size_t length)
  {
    png_voidp a = png_get_io_ptr(pngPtr);
    ((std::istream*)a)->read((char*)data, length);
  }
#endif

  tDX::rcode Sprite::LoadFromFile(std::string sImageFile, tDX::ResourcePack *pack)
  {
    UNUSED(pack);

    Gdiplus::Bitmap *bmp = nullptr;
    if (pack != nullptr)
    {
      // Load sprite from input stream
      ResourceBuffer rb = pack->GetFileBuffer(sImageFile);
      bmp = Gdiplus::Bitmap::FromStream(SHCreateMemStream((BYTE*)rb.vMemory.data(), (UINT)rb.vMemory.size()));
    }
    else
    {
      // Load sprite from file
      bmp = Gdiplus::Bitmap::FromFile(ConvertS2W(sImageFile).c_str());
    }

    if (bmp == nullptr) return tDX::NO_FILE;
    width = bmp->GetWidth();
    height = bmp->GetHeight();
    pColData = new Pixel[width * height];

    for (int x = 0; x < width; x++)
      for (int y = 0; y < height; y++)
      {
        Gdiplus::Color c;
        bmp->GetPixel(x, y, &c);
        SetPixel(x, y, Pixel(c.GetRed(), c.GetGreen(), c.GetBlue(), c.GetAlpha()));
      }
    delete bmp;
    return tDX::OK;
  }

  void Sprite::SetSampleMode(tDX::Sprite::Mode mode)
  {
    modeSample = mode;
  }


  Pixel Sprite::GetPixel(int32_t x, int32_t y)
  {
    if (modeSample == tDX::Sprite::Mode::NORMAL)
    {
      if (x >= 0 && x < width && y >= 0 && y < height)
        return pColData[y*width + x];
      else
        return Pixel(0, 0, 0, 0);
    }
    else
    {
      return pColData[abs(y%height)*width + abs(x%width)];
    }
  }

  bool Sprite::SetPixel(int32_t x, int32_t y, Pixel p)
  {

#ifdef T_DBG_OVERDRAW
    nOverdrawCount++;
#endif

    // This check is too expensive
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
      pColData[y*width + x] = p;
      return true;
    }
    else
      return false;
  }

  Pixel Sprite::Sample(float x, float y)
  {
    int32_t sx = std::min((int32_t)((x * (float)width)), width - 1);
    int32_t sy = std::min((int32_t)((y * (float)height)), height - 1);
    return GetPixel(sx, sy);
  }

  Pixel Sprite::SampleBL(float u, float v)
  {
    u = u * width - 0.5f;
    v = v * height - 0.5f;
    int x = (int)floor(u); // cast to int rounds toward zero, not downward
    int y = (int)floor(v); // Thanks @joshinils
    float u_ratio = u - x;
    float v_ratio = v - y;
    float u_opposite = 1 - u_ratio;
    float v_opposite = 1 - v_ratio;

    tDX::Pixel p1 = GetPixel(std::max(x, 0), std::max(y, 0));
    tDX::Pixel p2 = GetPixel(std::min(x + 1, (int)width - 1), std::max(y, 0));
    tDX::Pixel p3 = GetPixel(std::max(x, 0), std::min(y + 1, (int)height - 1));
    tDX::Pixel p4 = GetPixel(std::min(x + 1, (int)width - 1), std::min(y + 1, (int)height - 1));

    return tDX::Pixel(
      (uint8_t)((p1.r * u_opposite + p2.r * u_ratio) * v_opposite + (p3.r * u_opposite + p4.r * u_ratio) * v_ratio),
      (uint8_t)((p1.g * u_opposite + p2.g * u_ratio) * v_opposite + (p3.g * u_opposite + p4.g * u_ratio) * v_ratio),
      (uint8_t)((p1.b * u_opposite + p2.b * u_ratio) * v_opposite + (p3.b * u_opposite + p4.b * u_ratio) * v_ratio));
  }

  Pixel* Sprite::GetData() { return pColData; }

  //==========================================================
  // Resource Packs - Allows you to store files in one large
  // scrambled file


  ResourceBuffer::ResourceBuffer(std::ifstream &ifs, uint32_t offset, uint32_t size)
  {
    vMemory.resize(size);
    ifs.seekg(offset); ifs.read(vMemory.data(), vMemory.size());
    setg(vMemory.data(), vMemory.data(), vMemory.data() + size);
  }

  ResourcePack::ResourcePack() { }
  ResourcePack::~ResourcePack() { baseFile.close(); }

  bool ResourcePack::AddFile(const std::string& sFile)
  {

    const std::string file = makeposix(sFile);

    if (_gfs::exists(file))
    {
      sResourceFile e;
      e.nSize = (uint32_t)_gfs::file_size(file);
      e.nOffset = 0; // Unknown at this stage
      mapFiles[file] = e;
      return true;
    }
    return false;
  }

  bool ResourcePack::LoadPack(const std::string& sFile, const std::string& sKey)
  {
    // Open the resource file
    baseFile.open(sFile, std::ifstream::binary);
    if (!baseFile.is_open()) return false;

    // 1) Read Scrambled index
    uint32_t nIndexSize = 0;
    baseFile.read((char*)&nIndexSize, sizeof(uint32_t));

    std::string buffer(nIndexSize, ' ');
    for (uint32_t j = 0; j < nIndexSize; j++)
      buffer[j] = baseFile.get();

    std::string decoded = scramble(buffer, sKey);
    std::stringstream iss(decoded);

    // 2) Read Map
    uint32_t nMapEntries;
    iss.read((char*)&nMapEntries, sizeof(uint32_t));
    for (uint32_t i = 0; i < nMapEntries; i++)
    {
      uint32_t nFilePathSize = 0;
      iss.read((char*)&nFilePathSize, sizeof(uint32_t));

      std::string sFileName(nFilePathSize, ' ');
      for (uint32_t j = 0; j < nFilePathSize; j++)
        sFileName[j] = iss.get();

      sResourceFile e;
      iss.read((char*)&e.nSize, sizeof(uint32_t));
      iss.read((char*)&e.nOffset, sizeof(uint32_t));
      mapFiles[sFileName] = e;
    }

    // Don't close base file! we will provide a stream
    // pointer when the file is requested
    return true;
  }

  bool ResourcePack::SavePack(const std::string& sFile, const std::string& sKey)
  {
    // Create/Overwrite the resource file
    std::ofstream ofs(sFile, std::ofstream::binary);
    if (!ofs.is_open()) return false;

    // Iterate through map
    uint32_t nIndexSize = 0; // Unknown for now
    ofs.write((char*)&nIndexSize, sizeof(uint32_t));
    size_t nMapSize = mapFiles.size();
    ofs.write((char*)&nMapSize, sizeof(uint32_t));
    for (auto &e : mapFiles)
    {
      // Write the path of the file
      size_t nPathSize = e.first.size();
      ofs.write((char*)&nPathSize, sizeof(uint32_t));
      ofs.write(e.first.c_str(), nPathSize);

      // Write the file entry properties
      ofs.write((char*)&e.second.nSize, sizeof(uint32_t));
      ofs.write((char*)&e.second.nOffset, sizeof(uint32_t));
    }

    // 2) Write the individual Data
    std::streampos offset = ofs.tellp();
    nIndexSize = (uint32_t)offset;
    for (auto &e : mapFiles)
    {
      // Store beginning of file offset within resource pack file
      e.second.nOffset = (uint32_t)offset;

      // Load the file to be added
      std::vector<char> vBuffer(e.second.nSize);
      std::ifstream i(e.first, std::ifstream::binary);
      i.read(vBuffer.data(), e.second.nSize);
      i.close();

      // Write the loaded file into resource pack file
      ofs.write(vBuffer.data(), e.second.nSize);
      offset += e.second.nSize;
    }

    // 3) Scramble Index
    std::stringstream oss;
    oss.write((char*)&nMapSize, sizeof(uint32_t));
    for (auto &e : mapFiles)
    {
      // Write the path of the file
      size_t nPathSize = e.first.size();
      oss.write((char*)&nPathSize, sizeof(uint32_t));
      oss.write(e.first.c_str(), nPathSize);

      // Write the file entry properties
      oss.write((char*)&e.second.nSize, sizeof(uint32_t));
      oss.write((char*)&e.second.nOffset, sizeof(uint32_t));
    }
    std::string sIndexString = scramble(oss.str(), sKey);

    // 4) Rewrite Map (it has been updated with offsets now)
    // at start of file
    ofs.seekp(std::ios::beg);
    ofs.write((char*)&nIndexSize, sizeof(uint32_t));
    ofs.write(sIndexString.c_str(), nIndexSize);
    ofs.close();
    return true;
  }

  ResourceBuffer ResourcePack::GetFileBuffer(const std::string& sFile)
  {
    return ResourceBuffer(baseFile, mapFiles[sFile].nOffset, mapFiles[sFile].nSize);
  }

  bool ResourcePack::Loaded()
  {
    return baseFile.is_open();
  }

  const std::string ResourcePack::scramble(const std::string& data, const std::string& key)
  {
    size_t c = 0; std::string o;
    for (auto s : data)	o += std::string(1, s ^ key[(c++) % key.size()]);
    return o;
  };

  std::string ResourcePack::makeposix(const std::string& path)
  {
    std::string o;
    for (auto s : path) o += std::string(1, s == '\\' ? '/' : s);
    return o;
  };

  //==========================================================

  PixelGameEngine::PixelGameEngine()
  {
    sAppName = "Undefined";
    tDX::PGEX::pge = this;
  }

  tDX::rcode PixelGameEngine::Construct(uint32_t screen_w, uint32_t screen_h, uint32_t pixel_w, uint32_t pixel_h, bool full_screen, bool vsync)
  {
    nScreenWidth = screen_w;
    nScreenHeight = screen_h;
    nPixelWidth = pixel_w;
    nPixelHeight = pixel_h;
    bFullScreen = full_screen;
    bEnableVSYNC = vsync;

    fPixelX = 2.0f / (float)(nScreenWidth);
    fPixelY = 2.0f / (float)(nScreenHeight);

    if (nPixelWidth == 0 || nPixelHeight == 0 || nScreenWidth == 0 || nScreenHeight == 0)
      return tDX::FAIL;

#if defined(UNICODE) && !defined(__MINGW32__)
    wsAppName = ConvertS2W(sAppName);
#endif
    // Load the default font sheet
    tDX_ConstructFontSheet();

    // Create a sprite that represents the primary drawing target
    pDefaultDrawTarget = new Sprite(nScreenWidth, nScreenHeight);
    SetDrawTarget(nullptr);
    return tDX::OK;
  }


  void PixelGameEngine::SetScreenSize(int w, int h)
  {
    delete pDefaultDrawTarget;
    nScreenWidth = w;
    nScreenHeight = h;
    pDefaultDrawTarget = new Sprite(nScreenWidth, nScreenHeight);
    SetDrawTarget(nullptr);

    tDX_UpdateViewport();
  }

  tDX::rcode PixelGameEngine::Start()
  {
    // Create DirectX device
    tDX_DirectXCreateDevice();

    // Construct the window
    if (!tDX_WindowCreate())
      return tDX::FAIL;

    // Create resources changed when whindow size is changed
    tDX_DirectXCreateResources();

    // VS setup
    {
      Microsoft::WRL::ComPtr<ID3DBlob> blob;
      D3DReadFileToBlob(L"engine/vs.cso", &blob);

      m_d3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());

      const D3D11_INPUT_ELEMENT_DESC ied[] =
      {
        { "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
        { "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT   ,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }
      };

      m_d3dDevice->CreateInputLayout(ied, (UINT)std::size(ied), blob->GetBufferPointer(), blob->GetBufferSize(), m_inputLayout.GetAddressOf());
      m_d3dContext->VSSetShader(m_vertexShader.Get(), NULL, 0);
    }

    // PS setup
    {
      Microsoft::WRL::ComPtr<ID3DBlob> blob;
      D3DReadFileToBlob(L"engine/ps.cso", &blob);

      m_d3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());
      m_d3dContext->PSSetShader(m_pixelShader.Get(), NULL, 0);
    }

    // Vertices
    std::vector<Vertex> m_vertices;

    m_vertices.push_back({ {-1, -1, 0}, {0, 1} });
    m_vertices.push_back({ {-1,  1, 0}, {0, 0} });
    m_vertices.push_back({ { 1,  1, 0}, {1, 0} });
    m_vertices.push_back({ { 1, -1, 0}, {1, 1} });

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    bd.ByteWidth = UINT(sizeof(Vertex) * m_vertices.size());
    bd.StructureByteStride = sizeof(Vertex);

    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = m_vertices.data();
    m_d3dDevice->CreateBuffer(&bd, &sd, m_vertexBuffer.GetAddressOf());

    // Index buffer
    std::vector<uint16_t> m_indices = { 0,1,3,1,2,3 };

    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.ByteWidth = UINT(m_indices.size() * sizeof(uint16_t));
    ibd.StructureByteStride = sizeof(uint16_t);

    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = m_indices.data();
    m_d3dDevice->CreateBuffer(&ibd, &isd, m_indexBuffer.GetAddressOf());

    // Sampler
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = 0;

    //Create the sample state
    m_d3dDevice->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());

    // Rest of the stuff to set
    unsigned int stride = sizeof(Vertex);
    unsigned int offset = 0;

    m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_d3dContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    m_d3dContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
    m_d3dContext->IASetInputLayout(m_inputLayout.Get());

    m_d3dContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    // Create user resources as part of this thread
    if (!OnUserCreate())
      bActive = false;

    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();


    // Start the thread
    bActive = true;
    //std::thread t = std::thread(&PixelGameEngine::EngineThread, this);

    // Main message loop
    MSG msg = {};
    while (WM_QUIT != msg.message)
    {
      if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
      else
      {
        // Handle Timing
        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;

        // Our time per frame coefficient
        float fElapsedTime = elapsedTime.count();

        // Handle resize if needed
        if (bResize)
        {
          tDX_DirectXCreateResources();
          bResize = false;
        }

        // Handle User Input - Keyboard
        for (int i = 0; i < 256; i++)
        {
          pKeyboardState[i].bPressed = false;
          pKeyboardState[i].bReleased = false;

          if (pKeyNewState[i] != pKeyOldState[i])
          {
            if (pKeyNewState[i])
            {
              pKeyboardState[i].bPressed = !pKeyboardState[i].bHeld;
              pKeyboardState[i].bHeld = true;
            }
            else
            {
              pKeyboardState[i].bReleased = true;
              pKeyboardState[i].bHeld = false;
            }
          }

          pKeyOldState[i] = pKeyNewState[i];
        }

        // Handle User Input - Mouse
        for (int i = 0; i < 5; i++)
        {
          pMouseState[i].bPressed = false;
          pMouseState[i].bReleased = false;

          if (pMouseNewState[i] != pMouseOldState[i])
          {
            if (pMouseNewState[i])
            {
              pMouseState[i].bPressed = !pMouseState[i].bHeld;
              pMouseState[i].bHeld = true;
            }
            else
            {
              pMouseState[i].bReleased = true;
              pMouseState[i].bHeld = false;
            }
          }

          pMouseOldState[i] = pMouseNewState[i];
        }

        // Cache mouse coordinates so they remain
        // consistent during frame
        nMousePosX = nMousePosXcache;
        nMousePosY = nMousePosYcache;

        nMouseWheelDelta = nMouseWheelDeltaCache;
        nMouseWheelDeltaCache = 0;

#ifdef T_DBG_OVERDRAW
        tDX::Sprite::nOverdrawCount = 0;
#endif

        // Handle Frame Update
        if (!OnUserUpdate(fElapsedTime))
          bActive = false;

        // Update texture to be rendered
        D3D11_MAPPED_SUBRESOURCE mappedTexture = {};
        m_d3dContext->Map(m_texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedTexture);

        BYTE* mappedData = reinterpret_cast<BYTE*>(mappedTexture.pData);
        tDX::Pixel* sourceData = pDrawTarget->GetData();

        for (int row = 0; row < pDrawTarget->height; row++)
        {
          memcpy(mappedData, sourceData, pDrawTarget->width * 4);
          mappedData += mappedTexture.RowPitch;
          sourceData += pDrawTarget->width;
        }

        m_d3dContext->Unmap(m_texture.Get(), 0);

        // Bind RT
        m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), NULL);

        m_d3dContext->DrawIndexed(6, 0, 0);
        m_swapChain->Present(0, 0);

        // Update Title Bar
        fFrameTimer += fElapsedTime;
        nFrameCount++;
        if (fFrameTimer >= 1.0f)
        {
          fFrameTimer -= 1.0f;

          std::string sTitle = "tucna.net - Pixel Game Engine - " + sAppName + " - FPS: " + std::to_string(nFrameCount);

#ifdef UNICODE
          SetWindowText(tDX_hWnd, ConvertS2W(sTitle).c_str());
#else
          SetWindowText(tDX_hWnd, sTitle.c_str());
#endif

          nFrameCount = 0;
        }

        if (!bActive)
          break;
      }
    }

    OnUserDestroy();

    // Finish rendering
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);

    m_renderTargetView.Reset();
    m_texture.Reset();
    m_textureView.Reset();

    m_d3dContext->Flush();

    CoUninitialize();

    return tDX::OK;
  }

  void PixelGameEngine::SetDrawTarget(Sprite *target)
  {
    if (target)
      pDrawTarget = target;
    else
      pDrawTarget = pDefaultDrawTarget;
  }

  Sprite* PixelGameEngine::GetDrawTarget()
  {
    return pDrawTarget;
  }

  int32_t PixelGameEngine::GetDrawTargetWidth()
  {
    if (pDrawTarget)
      return pDrawTarget->width;
    else
      return 0;
  }

  int32_t PixelGameEngine::GetDrawTargetHeight()
  {
    if (pDrawTarget)
      return pDrawTarget->height;
    else
      return 0;
  }

  bool PixelGameEngine::IsFocused()
  {
    return bHasInputFocus;
  }

  HWButton PixelGameEngine::GetKey(Key k)
  {
    return pKeyboardState[k];
  }

  HWButton PixelGameEngine::GetMouse(uint32_t b)
  {
    return pMouseState[b];
  }

  int32_t PixelGameEngine::GetMouseX()
  {
    return nMousePosX;
  }

  int32_t PixelGameEngine::GetMouseY()
  {
    return nMousePosY;
  }

  int32_t PixelGameEngine::GetMouseWheel()
  {
    return nMouseWheelDelta;
  }

  int32_t PixelGameEngine::ScreenWidth()
  {
    return nScreenWidth;
  }

  int32_t PixelGameEngine::ScreenHeight()
  {
    return nScreenHeight;
  }

  bool PixelGameEngine::Draw(const tDX::vi2d& pos, Pixel p)
  {
    return Draw(pos.x, pos.y, p);
  }

  bool PixelGameEngine::Draw(int32_t x, int32_t y, Pixel p)
  {
    if (!pDrawTarget) return false;

    if (nPixelMode == Pixel::Mode::NORMAL)
    {
      return pDrawTarget->SetPixel(x, y, p);
    }

    if (nPixelMode == Pixel::Mode::MASK)
    {
      if (p.a == 255)
        return pDrawTarget->SetPixel(x, y, p);
    }

    if (nPixelMode == Pixel::Mode::ALPHA)
    {
      Pixel d = pDrawTarget->GetPixel(x, y);
      float a = (float)(p.a / 255.0f) * fBlendFactor;
      float c = 1.0f - a;
      float r = a * (float)p.r + c * (float)d.r;
      float g = a * (float)p.g + c * (float)d.g;
      float b = a * (float)p.b + c * (float)d.b;
      return pDrawTarget->SetPixel(x, y, Pixel((uint8_t)r, (uint8_t)g, (uint8_t)b));
    }

    if (nPixelMode == Pixel::Mode::CUSTOM)
    {
      return pDrawTarget->SetPixel(x, y, funcPixelMode(x, y, p, pDrawTarget->GetPixel(x, y)));
    }

    return false;
  }

  void PixelGameEngine::SetSubPixelOffset(float ox, float oy)
  {
    fSubPixelOffsetX = ox * fPixelX;
    fSubPixelOffsetY = oy * fPixelY;
  }

  void PixelGameEngine::DrawLine(const tDX::vi2d& pos1, const tDX::vi2d& pos2, Pixel p, uint32_t pattern)
  {
    DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, p, pattern);
  }

  void PixelGameEngine::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pixel p, uint32_t pattern)
  {
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx = x2 - x1; dy = y2 - y1;

    auto rol = [&](void)
    {
      pattern = (pattern << 1) | (pattern >> 31);
      return pattern & 1;
    };

    // straight lines idea by gurkanctn
    if (dx == 0) // Line is vertical
    {
      if (y2 < y1) std::swap(y1, y2);
      for (y = y1; y <= y2; y++)
        if (rol()) Draw(x1, y, p);
      return;
    }

    if (dy == 0) // Line is horizontal
    {
      if (x2 < x1) std::swap(x1, x2);
      for (x = x1; x <= x2; x++)
        if (rol()) Draw(x, y1, p);
      return;
    }

    // Line is Funk-aye
    dx1 = abs(dx); dy1 = abs(dy);
    px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
    if (dy1 <= dx1)
    {
      if (dx >= 0)
      {
        x = x1; y = y1; xe = x2;
      }
      else
      {
        x = x2; y = y2; xe = x1;
      }

      if (rol()) Draw(x, y, p);

      for (i = 0; x < xe; i++)
      {
        x = x + 1;
        if (px < 0)
          px = px + 2 * dy1;
        else
        {
          if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
          px = px + 2 * (dy1 - dx1);
        }
        if (rol()) Draw(x, y, p);
      }
    }
    else
    {
      if (dy >= 0)
      {
        x = x1; y = y1; ye = y2;
      }
      else
      {
        x = x2; y = y2; ye = y1;
      }

      if (rol()) Draw(x, y, p);

      for (i = 0; y < ye; i++)
      {
        y = y + 1;
        if (py <= 0)
          py = py + 2 * dx1;
        else
        {
          if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
          py = py + 2 * (dx1 - dy1);
        }
        if (rol()) Draw(x, y, p);
      }
    }
  }

  void PixelGameEngine::DrawCircle(const tDX::vi2d& pos, int32_t radius, Pixel p, uint8_t mask)
  {
    DrawCircle(pos.x, pos.y, radius, p, mask);
  }

  void PixelGameEngine::DrawCircle(int32_t x, int32_t y, int32_t radius, Pixel p, uint8_t mask)
  {
    int x0 = 0;
    int y0 = radius;
    int d = 3 - 2 * radius;
    if (!radius) return;

    while (y0 >= x0) // only formulate 1/8 of circle
    {
      if (mask & 0x01) Draw(x + x0, y - y0, p);
      if (mask & 0x02) Draw(x + y0, y - x0, p);
      if (mask & 0x04) Draw(x + y0, y + x0, p);
      if (mask & 0x08) Draw(x + x0, y + y0, p);
      if (mask & 0x10) Draw(x - x0, y + y0, p);
      if (mask & 0x20) Draw(x - y0, y + x0, p);
      if (mask & 0x40) Draw(x - y0, y - x0, p);
      if (mask & 0x80) Draw(x - x0, y - y0, p);
      if (d < 0) d += 4 * x0++ + 6;
      else d += 4 * (x0++ - y0--) + 10;
    }
  }

  void PixelGameEngine::FillCircle(const tDX::vi2d& pos, int32_t radius, Pixel p)
  {
    FillCircle(pos.x, pos.y, radius, p);
  }

  void PixelGameEngine::FillCircle(int32_t x, int32_t y, int32_t radius, Pixel p)
  {
    // Taken from wikipedia
    int x0 = 0;
    int y0 = radius;
    int d = 3 - 2 * radius;
    if (!radius) return;

    auto drawline = [&](int sx, int ex, int ny)
    {
      for (int i = sx; i <= ex; i++)
        Draw(i, ny, p);
    };

    while (y0 >= x0)
    {
      // Modified to draw scan-lines instead of edges
      drawline(x - x0, x + x0, y - y0);
      drawline(x - y0, x + y0, y - x0);
      drawline(x - x0, x + x0, y + y0);
      drawline(x - y0, x + y0, y + x0);
      if (d < 0) d += 4 * x0++ + 6;
      else d += 4 * (x0++ - y0--) + 10;
    }
  }

  void PixelGameEngine::DrawRect(const tDX::vi2d& pos, const tDX::vi2d& size, Pixel p)
  {
    DrawRect(pos.x, pos.y, size.x, size.y, p);
  }

  void PixelGameEngine::DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p)
  {
    DrawLine(x, y, x + w, y, p);
    DrawLine(x + w, y, x + w, y + h, p);
    DrawLine(x + w, y + h, x, y + h, p);
    DrawLine(x, y + h, x, y, p);
  }

  void PixelGameEngine::Clear(Pixel p)
  {
    int pixels = GetDrawTargetWidth() * GetDrawTargetHeight();
    Pixel* m = GetDrawTarget()->GetData();
    for (int i = 0; i < pixels; i++)
      m[i] = p;
#ifdef T_DBG_OVERDRAW
    tDX::Sprite::nOverdrawCount += pixels;
#endif
  }

  void PixelGameEngine::FillRect(const tDX::vi2d& pos, const tDX::vi2d& size, Pixel p)
  {
    FillRect(pos.x, pos.y, size.x, size.y, p);
  }

  void PixelGameEngine::FillRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p)
  {
    int32_t x2 = x + w;
    int32_t y2 = y + h;

    if (x < 0) x = 0;
    if (x >= (int32_t)GetDrawTargetWidth()) x = (int32_t)GetDrawTargetWidth();
    if (y < 0) y = 0;
    if (y >= (int32_t)GetDrawTargetHeight()) y = (int32_t)GetDrawTargetHeight();

    if (x2 < 0) x2 = 0;
    if (x2 >= (int32_t)GetDrawTargetWidth()) x2 = (int32_t)GetDrawTargetWidth();
    if (y2 < 0) y2 = 0;
    if (y2 >= (int32_t)GetDrawTargetHeight()) y2 = (int32_t)GetDrawTargetHeight();

    for (int i = x; i < x2; i++)
      for (int j = y; j < y2; j++)
        Draw(i, j, p);
  }

  void PixelGameEngine::DrawTriangle(const tDX::vi2d& pos1, const tDX::vi2d& pos2, const tDX::vi2d& pos3, Pixel p)
  {
    DrawTriangle(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y, p);
  }

  void PixelGameEngine::DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p)
  {
    DrawLine(x1, y1, x2, y2, p);
    DrawLine(x2, y2, x3, y3, p);
    DrawLine(x3, y3, x1, y1, p);
  }

  void PixelGameEngine::FillTriangle(const tDX::vi2d& pos1, const tDX::vi2d& pos2, const tDX::vi2d& pos3, Pixel p)
  {
    FillTriangle(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y, p);
  }

  // https://www.avrfreaks.net/sites/default/files/triangles.c
  void PixelGameEngine::FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p)
  {
    auto SWAP = [](int &x, int &y) { int t = x; x = y; y = t; };
    auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw(i, ny, p); };

    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;
    int signx1, signx2, dx1, dy1, dx2, dy2;
    int e1, e2;
    // Sort vertices
    if (y1 > y2) { SWAP(y1, y2); SWAP(x1, x2); }
    if (y1 > y3) { SWAP(y1, y3); SWAP(x1, x3); }
    if (y2 > y3) { SWAP(y2, y3); SWAP(x2, x3); }

    t1x = t2x = x1; y = y1;   // Starting points
    dx1 = (int)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y2 - y1);

    dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
    else signx2 = 1;
    dy2 = (int)(y3 - y1);

    if (dy1 > dx1) {   // swap values
      SWAP(dx1, dy1);
      changed1 = true;
    }
    if (dy2 > dx2) {   // swap values
      SWAP(dy2, dx2);
      changed2 = true;
    }

    e2 = (int)(dx2 >> 1);
    // Flat top, just process the second half
    if (y1 == y2) goto next;
    e1 = (int)(dx1 >> 1);

    for (int i = 0; i < dx1;) {
      t1xp = 0; t2xp = 0;
      if (t1x < t2x) { minx = t1x; maxx = t2x; }
      else { minx = t2x; maxx = t1x; }
      // process first line until y value is about to change
      while (i < dx1) {
        i++;
        e1 += dy1;
        while (e1 >= dx1) {
          e1 -= dx1;
          if (changed1) t1xp = signx1;//t1x += signx1;
          else          goto next1;
        }
        if (changed1) break;
        else t1x += signx1;
      }
      // Move line
    next1:
      // process second line until y value is about to change
      while (1) {
        e2 += dy2;
        while (e2 >= dx2) {
          e2 -= dx2;
          if (changed2) t2xp = signx2;//t2x += signx2;
          else          goto next2;
        }
        if (changed2)     break;
        else              t2x += signx2;
      }
    next2:
      if (minx > t1x) minx = t1x;
      if (minx > t2x) minx = t2x;
      if (maxx < t1x) maxx = t1x;
      if (maxx < t2x) maxx = t2x;
      drawline(minx, maxx, y);    // Draw line from min to max points found on the y
                    // Now increase y
      if (!changed1) t1x += signx1;
      t1x += t1xp;
      if (!changed2) t2x += signx2;
      t2x += t2xp;
      y += 1;
      if (y == y2) break;

    }
  next:
    // Second half
    dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y3 - y2);
    t1x = x2;

    if (dy1 > dx1) {   // swap values
      SWAP(dy1, dx1);
      changed1 = true;
    }
    else changed1 = false;

    e1 = (int)(dx1 >> 1);

    for (int i = 0; i <= dx1; i++) {
      t1xp = 0; t2xp = 0;
      if (t1x < t2x) { minx = t1x; maxx = t2x; }
      else { minx = t2x; maxx = t1x; }
      // process first line until y value is about to change
      while (i < dx1) {
        e1 += dy1;
        while (e1 >= dx1) {
          e1 -= dx1;
          if (changed1) { t1xp = signx1; break; }//t1x += signx1;
          else          goto next3;
        }
        if (changed1) break;
        else   	   	  t1x += signx1;
        if (i < dx1) i++;
      }
    next3:
      // process second line until y value is about to change
      while (t2x != x3) {
        e2 += dy2;
        while (e2 >= dx2) {
          e2 -= dx2;
          if (changed2) t2xp = signx2;
          else          goto next4;
        }
        if (changed2)     break;
        else              t2x += signx2;
      }
    next4:

      if (minx > t1x) minx = t1x;
      if (minx > t2x) minx = t2x;
      if (maxx < t1x) maxx = t1x;
      if (maxx < t2x) maxx = t2x;
      drawline(minx, maxx, y);
      if (!changed1) t1x += signx1;
      t1x += t1xp;
      if (!changed2) t2x += signx2;
      t2x += t2xp;
      y += 1;
      if (y > y3) return;
    }
  }

  void PixelGameEngine::DrawSprite(const tDX::vi2d& pos, Sprite *sprite, uint32_t scale)
  {
    DrawSprite(pos.x, pos.y, sprite, scale);
  }

  void PixelGameEngine::DrawSprite(int32_t x, int32_t y, Sprite *sprite, uint32_t scale)
  {
    if (sprite == nullptr)
      return;

    if (scale > 1)
    {
      for (int32_t i = 0; i < sprite->width; i++)
        for (int32_t j = 0; j < sprite->height; j++)
          for (uint32_t is = 0; is < scale; is++)
            for (uint32_t js = 0; js < scale; js++)
              Draw(x + (i*scale) + is, y + (j*scale) + js, sprite->GetPixel(i, j));
    }
    else
    {
      for (int32_t i = 0; i < sprite->width; i++)
        for (int32_t j = 0; j < sprite->height; j++)
          Draw(x + i, y + j, sprite->GetPixel(i, j));
    }
  }

  void PixelGameEngine::DrawPartialSprite(const tDX::vi2d& pos, Sprite *sprite, const tDX::vi2d& sourcepos, const tDX::vi2d& size, uint32_t scale)
  {
    DrawPartialSprite(pos.x, pos.y, sprite, sourcepos.x, sourcepos.y, size.x, size.y, scale);
  }

  void PixelGameEngine::DrawPartialSprite(int32_t x, int32_t y, Sprite *sprite, int32_t ox, int32_t oy, int32_t w, int32_t h, uint32_t scale)
  {
    if (sprite == nullptr)
      return;

    if (scale > 1)
    {
      for (int32_t i = 0; i < w; i++)
        for (int32_t j = 0; j < h; j++)
          for (uint32_t is = 0; is < scale; is++)
            for (uint32_t js = 0; js < scale; js++)
              Draw(x + (i*scale) + is, y + (j*scale) + js, sprite->GetPixel(i + ox, j + oy));
    }
    else
    {
      for (int32_t i = 0; i < w; i++)
        for (int32_t j = 0; j < h; j++)
          Draw(x + i, y + j, sprite->GetPixel(i + ox, j + oy));
    }
  }

  void PixelGameEngine::DrawString(const tDX::vi2d& pos, const std::string& sText, Pixel col, uint32_t scale)
  {
    DrawString(pos.x, pos.y, sText, col, scale);
  }

  void PixelGameEngine::DrawString(int32_t x, int32_t y, const std::string& sText, Pixel col, uint32_t scale)
  {
    int32_t sx = 0;
    int32_t sy = 0;
    Pixel::Mode m = nPixelMode;
    if (col.a != 255)
      SetPixelMode(Pixel::Mode::ALPHA);
    else
      SetPixelMode(Pixel::Mode::MASK);

    for (auto c : sText)
    {
      if (c == '\n')
      {
        sx = 0; sy += 8 * scale;
      }
      else
      {
        int32_t ox = (c - 32) % 16;
        int32_t oy = (c - 32) / 16;

        if (scale > 1)
        {
          for (uint32_t i = 0; i < 8; i++)
            for (uint32_t j = 0; j < 8; j++)
              if (fontSprite->GetPixel(i + ox * 8, j + oy * 8).r > 0)
                for (uint32_t is = 0; is < scale; is++)
                  for (uint32_t js = 0; js < scale; js++)
                    Draw(x + sx + (i*scale) + is, y + sy + (j*scale) + js, col);
        }
        else
        {
          for (uint32_t i = 0; i < 8; i++)
            for (uint32_t j = 0; j < 8; j++)
              if (fontSprite->GetPixel(i + ox * 8, j + oy * 8).r > 0)
                Draw(x + sx + i, y + sy + j, col);
        }
        sx += 8 * scale;
      }
    }
    SetPixelMode(m);
  }

  void PixelGameEngine::SetPixelMode(Pixel::Mode m)
  {
    nPixelMode = m;
  }

  Pixel::Mode PixelGameEngine::GetPixelMode()
  {
    return nPixelMode;
  }

  void PixelGameEngine::SetPixelMode(std::function<tDX::Pixel(const int x, const int y, const tDX::Pixel&, const tDX::Pixel&)> pixelMode)
  {
    funcPixelMode = pixelMode;
    nPixelMode = Pixel::Mode::CUSTOM;
  }

  void PixelGameEngine::SetPixelBlend(float fBlend)
  {
    fBlendFactor = fBlend;
    if (fBlendFactor < 0.0f) fBlendFactor = 0.0f;
    if (fBlendFactor > 1.0f) fBlendFactor = 1.0f;
  }

  // User must override these functions as required. I have not made
  // them abstract because I do need a default behaviour to occur if
  // they are not overwritten
  bool PixelGameEngine::OnUserCreate()
  {
    return false;
  }
  bool PixelGameEngine::OnUserUpdate(float fElapsedTime)
  {
    UNUSED(fElapsedTime);  return false;
  }
  bool PixelGameEngine::OnUserDestroy()
  {
    return true;
  }
  //////////////////////////////////////////////////////////////////

  void PixelGameEngine::tDX_UpdateViewport()
  {
    int32_t ww = nScreenWidth * nPixelWidth;
    int32_t wh = nScreenHeight * nPixelHeight;
    float wasp = (float)ww / (float)wh;

    nViewW = (int32_t)nWindowWidth;
    nViewH = (int32_t)((float)nViewW / wasp);

    if (nViewH > nWindowHeight)
    {
      nViewH = nWindowHeight;
      nViewW = (int32_t)((float)nViewH * wasp);
    }

    nViewX = (nWindowWidth - nViewW) / 2;
    nViewY = (nWindowHeight - nViewH) / 2;
  }

  void PixelGameEngine::tDX_UpdateWindowSize(int32_t x, int32_t y)
  {
    nWindowWidth = x;
    nWindowHeight = y;
    tDX_UpdateViewport();

    // If device already exists recreate resources
    if (m_d3dDevice)
      bResize = true;
  }

  void PixelGameEngine::tDX_UpdateMouseWheel(int32_t delta)
  {
    nMouseWheelDeltaCache += delta;
  }

  void PixelGameEngine::tDX_UpdateMouse(int32_t x, int32_t y)
  {
    // Mouse coords come in screen space
    // But leave in pixel space

    // Full Screen mode may have a weird viewport we must clamp to
    x -= nViewX;
    y -= nViewY;

    nMousePosXcache = (int32_t)(((float)x / (float)(nWindowWidth - (nViewX * 2)) * (float)nScreenWidth));
    nMousePosYcache = (int32_t)(((float)y / (float)(nWindowHeight - (nViewY * 2)) * (float)nScreenHeight));

    if (nMousePosXcache >= (int32_t)nScreenWidth)
      nMousePosXcache = nScreenWidth - 1;
    if (nMousePosYcache >= (int32_t)nScreenHeight)
      nMousePosYcache = nScreenHeight - 1;

    if (nMousePosXcache < 0)
      nMousePosXcache = 0;
    if (nMousePosYcache < 0)
      nMousePosYcache = 0;
  }

  // Thanks @MaGetzUb for this, which allows sprites to be defined
  // at construction, by initialising the GDI subsystem
  static class GDIPlusStartup
  {
  public:
    GDIPlusStartup()
    {
      Gdiplus::GdiplusStartupInput startupInput;
      ULONG_PTR	token;
      Gdiplus::GdiplusStartup(&token, &startupInput, NULL);
    };
  } gdistartup;

  void PixelGameEngine::tDX_ConstructFontSheet()
  {
    std::string data;
    data += "?Q`0001oOch0o01o@F40o0<AGD4090LAGD<090@A7ch0?00O7Q`0600>00000000";
    data += "O000000nOT0063Qo4d8>?7a14Gno94AA4gno94AaOT0>o3`oO400o7QN00000400";
    data += "Of80001oOg<7O7moBGT7O7lABET024@aBEd714AiOdl717a_=TH013Q>00000000";
    data += "720D000V?V5oB3Q_HdUoE7a9@DdDE4A9@DmoE4A;Hg]oM4Aj8S4D84@`00000000";
    data += "OaPT1000Oa`^13P1@AI[?g`1@A=[OdAoHgljA4Ao?WlBA7l1710007l100000000";
    data += "ObM6000oOfMV?3QoBDD`O7a0BDDH@5A0BDD<@5A0BGeVO5ao@CQR?5Po00000000";
    data += "Oc``000?Ogij70PO2D]??0Ph2DUM@7i`2DTg@7lh2GUj?0TO0C1870T?00000000";
    data += "70<4001o?P<7?1QoHg43O;`h@GT0@:@LB@d0>:@hN@L0@?aoN@<0O7ao0000?000";
    data += "OcH0001SOglLA7mg24TnK7ln24US>0PL24U140PnOgl0>7QgOcH0K71S0000A000";
    data += "00H00000@Dm1S007@DUSg00?OdTnH7YhOfTL<7Yh@Cl0700?@Ah0300700000000";
    data += "<008001QL00ZA41a@6HnI<1i@FHLM81M@@0LG81?O`0nC?Y7?`0ZA7Y300080000";
    data += "O`082000Oh0827mo6>Hn?Wmo?6HnMb11MP08@C11H`08@FP0@@0004@000000000";
    data += "00P00001Oab00003OcKP0006@6=PMgl<@440MglH@000000`@000001P00000000";
    data += "Ob@8@@00Ob@8@Ga13R@8Mga172@8?PAo3R@827QoOb@820@0O`0007`0000007P0";
    data += "O`000P08Od400g`<3V=P0G`673IP0`@3>1`00P@6O`P00g`<O`000GP800000000";
    data += "?P9PL020O`<`N3R0@E4HC7b0@ET<ATB0@@l6C4B0O`H3N7b0?P01L3R000000020";

    fontSprite = new tDX::Sprite(128, 48);
    int px = 0, py = 0;
    for (int b = 0; b < 1024; b += 4)
    {
      uint32_t sym1 = (uint32_t)data[b + 0] - 48;
      uint32_t sym2 = (uint32_t)data[b + 1] - 48;
      uint32_t sym3 = (uint32_t)data[b + 2] - 48;
      uint32_t sym4 = (uint32_t)data[b + 3] - 48;
      uint32_t r = sym1 << 18 | sym2 << 12 | sym3 << 6 | sym4;

      for (int i = 0; i < 24; i++)
      {
        int k = r & (1 << i) ? 255 : 0;
        fontSprite->SetPixel(px, py, tDX::Pixel(k, k, k, k));
        if (++py == 48) { px++; py = 0; }
      }
    }
  }

  HWND PixelGameEngine::tDX_WindowCreate()
  {
    WNDCLASS wc = {};
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpfnWndProc = tDX_WindowEvent;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.lpszMenuName = nullptr;
    wc.hbrBackground = nullptr;
#ifdef UNICODE
    wc.lpszClassName = L"tDX_PIXEL_GAME_ENGINE";
#else
    wc.lpszClassName = "tDX_PIXEL_GAME_ENGINE";
#endif

    RegisterClass(&wc);

    nWindowWidth = (LONG)nScreenWidth * (LONG)nPixelWidth;
    nWindowHeight = (LONG)nScreenHeight * (LONG)nPixelHeight;

    // Define window furniture
    DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_NOREDIRECTIONBITMAP;
    DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME;

    int nCosmeticOffset = 30;
    nViewW = nWindowWidth;
    nViewH = nWindowHeight;

    // Handle Fullscreen
    if (bFullScreen)
    {
      dwExStyle = 0;
      dwStyle = WS_VISIBLE | WS_POPUP;
      nCosmeticOffset = 0;
      HMONITOR hmon = MonitorFromWindow(tDX_hWnd, MONITOR_DEFAULTTONEAREST);
      MONITORINFO mi = { sizeof(mi) };
      if (!GetMonitorInfo(hmon, &mi)) return NULL;
      nWindowWidth = mi.rcMonitor.right;
      nWindowHeight = mi.rcMonitor.bottom;
    }

    // Keep client size as requested
    RECT rWndRect = { 0, 0, nWindowWidth, nWindowHeight };
    AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);
    int width = rWndRect.right - rWndRect.left;
    int height = rWndRect.bottom - rWndRect.top;

#ifdef UNICODE
    tDX_hWnd = CreateWindowEx(dwExStyle, L"tDX_PIXEL_GAME_ENGINE", L"", dwStyle,
      nCosmeticOffset, nCosmeticOffset, width, height, NULL, NULL, GetModuleHandle(nullptr), this);
#else
    tDX_hWnd = CreateWindowEx(dwExStyle, "tDX_PIXEL_GAME_ENGINE", "", dwStyle,
      nCosmeticOffset, nCosmeticOffset, width, height, NULL, NULL, GetModuleHandle(nullptr), this);
#endif

    // Create Keyboard Mapping
    mapKeys[0x00] = Key::NONE;
    mapKeys[0x41] = Key::A; mapKeys[0x42] = Key::B; mapKeys[0x43] = Key::C; mapKeys[0x44] = Key::D; mapKeys[0x45] = Key::E;
    mapKeys[0x46] = Key::F; mapKeys[0x47] = Key::G; mapKeys[0x48] = Key::H; mapKeys[0x49] = Key::I; mapKeys[0x4A] = Key::J;
    mapKeys[0x4B] = Key::K; mapKeys[0x4C] = Key::L; mapKeys[0x4D] = Key::M; mapKeys[0x4E] = Key::N; mapKeys[0x4F] = Key::O;
    mapKeys[0x50] = Key::P; mapKeys[0x51] = Key::Q; mapKeys[0x52] = Key::R; mapKeys[0x53] = Key::S; mapKeys[0x54] = Key::T;
    mapKeys[0x55] = Key::U; mapKeys[0x56] = Key::V; mapKeys[0x57] = Key::W; mapKeys[0x58] = Key::X; mapKeys[0x59] = Key::Y;
    mapKeys[0x5A] = Key::Z;

    mapKeys[VK_F1] = Key::F1; mapKeys[VK_F2] = Key::F2; mapKeys[VK_F3] = Key::F3; mapKeys[VK_F4] = Key::F4;
    mapKeys[VK_F5] = Key::F5; mapKeys[VK_F6] = Key::F6; mapKeys[VK_F7] = Key::F7; mapKeys[VK_F8] = Key::F8;
    mapKeys[VK_F9] = Key::F9; mapKeys[VK_F10] = Key::F10; mapKeys[VK_F11] = Key::F11; mapKeys[VK_F12] = Key::F12;

    mapKeys[VK_DOWN] = Key::DOWN; mapKeys[VK_LEFT] = Key::LEFT; mapKeys[VK_RIGHT] = Key::RIGHT; mapKeys[VK_UP] = Key::UP;
    mapKeys[VK_RETURN] = Key::ENTER; //mapKeys[VK_RETURN] = Key::RETURN;

    mapKeys[VK_BACK] = Key::BACK; mapKeys[VK_ESCAPE] = Key::ESCAPE; mapKeys[VK_RETURN] = Key::ENTER; mapKeys[VK_PAUSE] = Key::PAUSE;
    mapKeys[VK_SCROLL] = Key::SCROLL; mapKeys[VK_TAB] = Key::TAB; mapKeys[VK_DELETE] = Key::DEL; mapKeys[VK_HOME] = Key::HOME;
    mapKeys[VK_END] = Key::END; mapKeys[VK_PRIOR] = Key::PGUP; mapKeys[VK_NEXT] = Key::PGDN; mapKeys[VK_INSERT] = Key::INS;
    mapKeys[VK_SHIFT] = Key::SHIFT; mapKeys[VK_CONTROL] = Key::CTRL;
    mapKeys[VK_SPACE] = Key::SPACE;

    mapKeys[0x30] = Key::K0; mapKeys[0x31] = Key::K1; mapKeys[0x32] = Key::K2; mapKeys[0x33] = Key::K3; mapKeys[0x34] = Key::K4;
    mapKeys[0x35] = Key::K5; mapKeys[0x36] = Key::K6; mapKeys[0x37] = Key::K7; mapKeys[0x38] = Key::K8; mapKeys[0x39] = Key::K9;

    mapKeys[VK_NUMPAD0] = Key::NP0; mapKeys[VK_NUMPAD1] = Key::NP1; mapKeys[VK_NUMPAD2] = Key::NP2; mapKeys[VK_NUMPAD3] = Key::NP3; mapKeys[VK_NUMPAD4] = Key::NP4;
    mapKeys[VK_NUMPAD5] = Key::NP5; mapKeys[VK_NUMPAD6] = Key::NP6; mapKeys[VK_NUMPAD7] = Key::NP7; mapKeys[VK_NUMPAD8] = Key::NP8; mapKeys[VK_NUMPAD9] = Key::NP9;
    mapKeys[VK_MULTIPLY] = Key::NP_MUL; mapKeys[VK_ADD] = Key::NP_ADD; mapKeys[VK_DIVIDE] = Key::NP_DIV; mapKeys[VK_SUBTRACT] = Key::NP_SUB; mapKeys[VK_DECIMAL] = Key::NP_DECIMAL;

    return tDX_hWnd;
  }

  void PixelGameEngine::tDX_DirectXCreateResources()
  {
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_d3dContext->Flush();

    UINT backBufferWidth = static_cast<UINT>(nWindowWidth);
    UINT backBufferHeight = static_cast<UINT>(nWindowHeight);
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    UINT backBufferCount = 2;

    // Swapchain handling
    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
      m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);
    }
    else
    {
      // First, retrieve the underlying DXGI Device from the D3D Device.
      Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
      m_d3dDevice.As(&dxgiDevice);

      // Identify the physical adapter (GPU or card) this device is running on.
      Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
      dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());

      // And obtain the factory object that created it.
      Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
      dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));

      // Create a descriptor for the swap chain.
      DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
      swapChainDesc.Width = backBufferWidth;
      swapChainDesc.Height = backBufferHeight;
      swapChainDesc.Format = backBufferFormat;
      swapChainDesc.SampleDesc.Count = 1;
      swapChainDesc.SampleDesc.Quality = 0;
      swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      swapChainDesc.BufferCount = backBufferCount;
      swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

      DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
      fsSwapChainDesc.Windowed = TRUE;

      // Create a SwapChain from a Win32 window.
      dxgiFactory->CreateSwapChainForHwnd(
        m_d3dDevice.Get(),
        tDX_hWnd,
        &swapChainDesc,
        &fsSwapChainDesc,
        nullptr,
        m_swapChain.ReleaseAndGetAddressOf()
      );

      dxgiFactory->MakeWindowAssociation(tDX_hWnd, DXGI_MWA_NO_ALT_ENTER);
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));

    // Create a view interface on the rendertarget to use on bind.
    m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf());

    // Texture setup
    int32_t fWidth = pDefaultDrawTarget->width;
    int32_t fHeight = pDefaultDrawTarget->height;
    auto components = 4; // RGBA

    D3D11_TEXTURE2D_DESC textureDescription = {};
    textureDescription.Width = fWidth;
    textureDescription.Height = fHeight;

    textureDescription.MipLevels = textureDescription.ArraySize = 1;
    textureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDescription.SampleDesc.Count = 1;
    textureDescription.SampleDesc.Quality = 0;
    textureDescription.Usage = D3D11_USAGE_DYNAMIC;
    textureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    textureDescription.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initialTextureData;
    initialTextureData.pSysMem = pDefaultDrawTarget->GetData();
    initialTextureData.SysMemPitch = fWidth * components;
    initialTextureData.SysMemSlicePitch = 0;

    m_d3dDevice->CreateTexture2D(&textureDescription, &initialTextureData, m_texture.GetAddressOf());
    m_d3dDevice->CreateShaderResourceView(m_texture.Get(), NULL, &m_textureView);

    m_d3dContext->PSSetShaderResources(0, 1, m_textureView.GetAddressOf());

    // Set the viewport
    CD3D11_VIEWPORT viewport(static_cast<float>(nViewX), static_cast<float>(nViewY), static_cast<float>(nViewW), static_cast<float>(nViewH));
    m_d3dContext->RSSetViewports(1, &viewport);
  }

  bool PixelGameEngine::tDX_DirectXCreateDevice()
  {
    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D11CreateDevice(
      nullptr,
      D3D_DRIVER_TYPE_HARDWARE,
      nullptr,
      creationFlags,
      NULL,
      0,
      D3D11_SDK_VERSION,
      m_d3dDevice.ReleaseAndGetAddressOf(),
      NULL,
      m_d3dContext.ReleaseAndGetAddressOf()
    );

    return true; // Return TRUE is good?
  }

  // Windows Event Handler
  LRESULT CALLBACK PixelGameEngine::tDX_WindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    static PixelGameEngine *sge;
    switch (uMsg)
    {
    case WM_CREATE:		sge = (PixelGameEngine*)((LPCREATESTRUCT)lParam)->lpCreateParams;	return 0;
    case WM_MOUSEMOVE:
    {
      uint16_t x = lParam & 0xFFFF;				// Thanks @ForAbby (Discord)
      uint16_t y = (lParam >> 16) & 0xFFFF;
      int16_t ix = *(int16_t*)&x;
      int16_t iy = *(int16_t*)&y;
      sge->tDX_UpdateMouse(ix, iy);
      return 0;
    }
    case WM_SIZE:
    {
      sge->tDX_UpdateWindowSize(lParam & 0xFFFF, (lParam >> 16) & 0xFFFF);
      return 0;
    }
    case WM_MOUSEWHEEL:
    {
      sge->tDX_UpdateMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
      return 0;
    }
    case WM_MOUSELEAVE: sge->bHasMouseFocus = false; return 0;
    case WM_SETFOCUS:	  sge->bHasInputFocus = true;	return 0;
    case WM_KILLFOCUS:	sge->bHasInputFocus = false; return 0;
    case WM_KEYDOWN:	  sge->pKeyNewState[mapKeys[wParam]] = true; return 0;
    case WM_KEYUP:		  sge->pKeyNewState[mapKeys[wParam]] = false; return 0;
    case WM_LBUTTONDOWN:sge->pMouseNewState[0] = true; return 0;
    case WM_LBUTTONUP:	sge->pMouseNewState[0] = false; return 0;
    case WM_RBUTTONDOWN:sge->pMouseNewState[1] = true; return 0;
    case WM_RBUTTONUP:	sge->pMouseNewState[1] = false; return 0;
    case WM_MBUTTONDOWN:sge->pMouseNewState[2] = true; return 0;
    case WM_MBUTTONUP:	sge->pMouseNewState[2] = false;	return 0;
    case WM_DESTROY:	  PostQuitMessage(0); return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }


  // Need a couple of statics as these are singleton instances
  // read from multiple locations
  bool PixelGameEngine::bActive{ false };
  bool PixelGameEngine::bResize{ false };
  std::map<size_t, uint8_t> PixelGameEngine::mapKeys;
  tDX::PixelGameEngine* tDX::PGEX::pge = nullptr;
#ifdef T_DBG_OVERDRAW
  int tDX::Sprite::nOverdrawCount = 0;
#endif
  //=============================================================
}

#endif
