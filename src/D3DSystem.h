#pragma once

#include <d3d11.h>
#include <windows.h>


/////////////////////////////////////////////////////////////////
// Convenience macro for releasing COM objects.
/////////////////////////////////////////////////////////////////
#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }



class CD3D {
public:
    CD3D();
    ~CD3D();


public:
    virtual bool Initialize( HWND t_hwnd, int t_clientWidth, int t_clientHeight, bool t_bVsync, bool t_bFullscreen );
    virtual void ShutDown();

    ID3D11Device* GetDevice() { return _device; }
    ID3D11DeviceContext* GetDeviceContext() { return _deviceContext;  }

    void BeginScene( float, float, float, float );
    void EndScene();

private:
    struct VIDEOCARD_INFO {
        int     _videoCardMemory;
        int     _dedicatedSystemMemory;
        int     _sharedSystemMemory;
        char    _videoCardDescription[256];
        
        void PrintInfo();
    } _videoCardInfo;

    bool                     _bVsync;
    UINT                     _4xMsaaQuality;
    IDXGISwapChain*          _swapChain;
    ID3D11Device*            _device;
    ID3D11DeviceContext*     _deviceContext;
    ID3D11RenderTargetView*  _renderTargetView;
    ID3D11Texture2D*         _depthStencilBuffer;
    ID3D11DepthStencilState* _depthStencilState;
    ID3D11DepthStencilView*  _depthStencilView;
    ID3D11RasterizerState*   _rasterState;



};
