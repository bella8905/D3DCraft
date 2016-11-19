
#include <d3dcompiler.h>


#include "Utl_Global.h"
#include "Utl_LogMsg.h"


#include "Graphics.h"
#include "D3DSystem.h"



/////////////////////////////////////////////////////////////////
// d3d related stuff
/////////////////////////////////////////////////////////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")




void CD3D::VIDEOCARD_INFO::PrintInfo() {
    LogMsg << "Video Card: " << _videoCardDescription << LogEndl;
    LogMsg << "Dedicated Video Memory: " << _videoCardMemory << LogEndl;
    LogMsg << "Dedicated System Memory: " << _dedicatedSystemMemory << LogEndl;
    LogMsg << "Shared System Memory: " << _sharedSystemMemory << LogEndl;

}


CD3D::CD3D() {
    _swapChain          = nullptr;
    _device             = nullptr;
    _deviceContext      = nullptr;
    _renderTargetView   = nullptr;
    _depthStencilBuffer = nullptr;
    _depthStencilState  = nullptr;
    _depthStencilView   = nullptr;
    _rasterState        = nullptr;
}



CD3D::~CD3D() {
}


bool CD3D::Initialize( HWND t_hwnd, int t_clientWidth, int t_clientHeight, bool t_bVsync, bool t_bFullscreen ) {
    HRESULT result;


    _bVsync = t_bVsync;

    /////////////////////////////////////////////////////////////////////////////
    // query refresh rate, and video card information
    /////////////////////////////////////////////////////////////////////////////


    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    unsigned int numModes, i, numerator, denominator;
    DXGI_MODE_DESC* displayModeList;
    DXGI_ADAPTER_DESC adapterDesc;

    // Create a DirectX graphics interface factory.
    result = CreateDXGIFactory( __uuidof( IDXGIFactory ), ( void** )&factory );
    if( FAILED( result ) ) {
        return false;
    }

    // Use the factory to create an adapter for the primary graphics interface (video card).
    result = factory->EnumAdapters( 0, &adapter );
    if( FAILED( result ) ) {
        return false;
    }

    // Enumerate the primary adapter output (monitor).
    result = adapter->EnumOutputs( 0, &adapterOutput );
    if( FAILED( result ) ) {
        return false;
    }

    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
    result = adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL );
    if( FAILED( result ) ) {
        return false;
    }

    // Create a list to hold all the possible display modes for this monitor/video card combination.
    displayModeList = new DXGI_MODE_DESC[numModes];
    if( !displayModeList ) {
        return false;
    }

    // Now fill the display mode list structures.
    result = adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList );
    if( FAILED( result ) ) {
        return false;
    }

    // Now go through all the display modes and find the one that matches the screen width and height.
    // When a match is found store the numerator and denominator of the refresh rate for that monitor.
    for( i = 0; i < numModes; i++ ) {
        if( displayModeList[i].Width == ( unsigned int )t_clientWidth ) {
            if( displayModeList[i].Height == ( unsigned int )t_clientHeight ) {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    // Get the adapter (video card) description.
    result = adapter->GetDesc( &adapterDesc );
    if( FAILED( result ) ) {
        return false;
    }

    //  Store video card info.
    _videoCardInfo._videoCardMemory = ( int )( adapterDesc.DedicatedVideoMemory / 1024 / 1024 );
    _videoCardInfo._dedicatedSystemMemory = ( int )( adapterDesc.DedicatedSystemMemory / 1024 / 1024 );
    _videoCardInfo._sharedSystemMemory = ( int )( adapterDesc.SharedSystemMemory / 1024 / 1024 );
    size_t strLen;
    wcstombs_s( &strLen, _videoCardInfo._videoCardDescription, adapterDesc.Description, sizeof( _videoCardInfo._videoCardDescription ) );
    if( strLen == array_size_of( _videoCardInfo._videoCardDescription ) ) {
        _videoCardInfo._videoCardDescription[strLen - 1] = '\0';
    }
    _videoCardInfo.PrintInfo();

    // Release the display mode list.
    delete[] displayModeList;
    displayModeList = 0;

    // Release the adapter output.
    ReleaseCOM( adapterOutput );

    // Release the adapter.
    ReleaseCOM( adapter );

    // Release the factory.
    ReleaseCOM( factory );


    /////////////////////////////////////////////////////////////////////////////
    // Create Device and Device Context.
    //  We can create the device and context together with the swap chain,
    //  but we'd like to also setup the msaa.
    /////////////////////////////////////////////////////////////////////////////
    UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    result = D3D11CreateDevice(
        0,                 // default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        0,                 // no software device
        createDeviceFlags,
        0, 0,              // default feature level array
        D3D11_SDK_VERSION,
        &_device,
        &featureLevel,
        &_deviceContext );

    if( FAILED( result ) ) {
        // MessageBox( 0, L"D3D11CreateDevice Failed.", 0, 0 );
        return false;
    }


    if( featureLevel != D3D_FEATURE_LEVEL_11_0 ) {
        MessageBox( 0, L"Direct3D Feature Level 11 unsupported.", 0, 0 );
        return false;
    }


    /////////////////////////////////////////////////////////////////////////////
    // Check MSAA
    //  All Direct3D 11 capable devices support 4X MSAA for all render 
    //  target formats, so we only need to check quality support.
    /////////////////////////////////////////////////////////////////////////////
    _device->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, 4, &_4xMsaaQuality );
    if( _4xMsaaQuality == 0 ) {
        LogError << "Device doesn't support 4xMSAA" << LogEndl;
        CGraphics::SetIs4xMSAAEnabled( false );
    }


    /////////////////////////////////////////////////////////////////////////////
    // Swap Chain
    /////////////////////////////////////////////////////////////////////////////
    // Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory( &swapChainDesc, sizeof( swapChainDesc ) );
    // Set the handle for the window to render to.
    swapChainDesc.OutputWindow = t_hwnd;
    // Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = t_clientWidth;
    swapChainDesc.BufferDesc.Height = t_clientHeight;
    // Set to a single back buffer.
    swapChainDesc.BufferCount = 1;
    // Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;


    // Set to full screen or windowed mode.
    if( t_bFullscreen ) {
        swapChainDesc.Windowed = false;
    } else {
        swapChainDesc.Windowed = true;
    }
    // Set the refresh rate of the back buffer.
    if( _bVsync ) {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    } else {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }
    // Use 4X MSAA? 
    if( CGraphics::GetIs4xMSAAEnabled() ) {
        swapChainDesc.SampleDesc.Count = 4;
        swapChainDesc.SampleDesc.Quality = _4xMsaaQuality - 1;
    }
    // No MSAA
    else {
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
    }
    // Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    // Set the scan line ordering and scaling to unspecified.
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    // Discard the back buffer contents after presenting.
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    // Don't set the advanced flags.
    swapChainDesc.Flags = 0;

    // To correctly create the swap chain, we must use the IDXGIFactory that was
    // used to create the device.  If we tried to use a different IDXGIFactory instance
    // (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
    // This function is being called with a device from a different IDXGIFactory."

    IDXGIDevice* dxgiDevice = 0;
    result = _device->QueryInterface( __uuidof( IDXGIDevice ), ( void** )&dxgiDevice );
    if( FAILED( result ) ) {
        return false;
    }

    IDXGIAdapter* dxgiAdapter = 0;
    result = dxgiDevice->GetParent( __uuidof( IDXGIAdapter ), ( void** )&dxgiAdapter );
    if( FAILED( result ) ) {
        return false;
    }

    IDXGIFactory* dxgiFactory = 0;
    result = dxgiAdapter->GetParent( __uuidof( IDXGIFactory ), ( void** )&dxgiFactory );
    if( FAILED( result ) ) {
        return false;
    }

    result = dxgiFactory->CreateSwapChain( _device, &swapChainDesc, &_swapChain );
    if( FAILED( result ) ) {
        return false;
    }

    ReleaseCOM( dxgiDevice );
    ReleaseCOM( dxgiAdapter );
    ReleaseCOM( dxgiFactory );



    /////////////////////////////////////////////////////////////////////////////
    // Back buffer
    /////////////////////////////////////////////////////////////////////////////
    // Get the pointer to the back buffer.
    ID3D11Texture2D* backBufferPtr;
    result = _swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&backBufferPtr );
    if( FAILED( result ) ) {
        return false;
    }

    // Create the render target view with the back buffer pointer.
    result = _device->CreateRenderTargetView( backBufferPtr, NULL, &_renderTargetView );
    if( FAILED( result ) ) {
        return false;
    }    
    // Release pointer to the back buffer as we no longer need it
    ReleaseCOM( backBufferPtr );

    /////////////////////////////////////////////////////////////////////////////
    // Depth Stenciil Buffer
    /////////////////////////////////////////////////////////////////////////////
    // setup the description of the depth buffer.
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory( &depthBufferDesc, sizeof( depthBufferDesc ) );
    depthBufferDesc.Width = t_clientWidth;
    depthBufferDesc.Height = t_clientHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    // Use 4X MSAA? --must match swap chain MSAA values.
    if( CGraphics::GetIs4xMSAAEnabled() ) {
        depthBufferDesc.SampleDesc.Count = 4;
        depthBufferDesc.SampleDesc.Quality = _4xMsaaQuality - 1;
    }
    // No MSAA
    else {
        depthBufferDesc.SampleDesc.Count = 1;
        depthBufferDesc.SampleDesc.Quality = 0;
    }

    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    result = _device->CreateTexture2D( &depthBufferDesc, NULL, &_depthStencilBuffer );
    if( FAILED( result ) ) {
        return false;
    }



    /////////////////////////////////////////////////////////////////////////////
    // Depth Stencil State
    /////////////////////////////////////////////////////////////////////////////
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory( &depthStencilDesc, sizeof( depthStencilDesc ) );
    // Set up the description of the stencil state.
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the depth stencil state.
    result = _device->CreateDepthStencilState( &depthStencilDesc, &_depthStencilState );
    if( FAILED( result ) ) {
        return false;
    }

    // Set the depth stencil state.
    _deviceContext->OMSetDepthStencilState( _depthStencilState, 1 );


    /////////////////////////////////////////////////////////////////////////////
    // Depth Stencil View
    /////////////////////////////////////////////////////////////////////////////
    // Initialize the depth stencil view.
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    // Set up the depth stencil view description.
    ZeroMemory( &depthStencilViewDesc, sizeof( depthStencilViewDesc ) );
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    if( CGraphics::GetIs4xMSAAEnabled() ) {
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    } else {
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    }   
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view.
    result = _device->CreateDepthStencilView( _depthStencilBuffer, &depthStencilViewDesc, &_depthStencilView );
    if( FAILED( result ) ) {
        return false;
    }


    /////////////////////////////////////////////////////////////////////////////
    // Set Render Targets
    /////////////////////////////////////////////////////////////////////////////
    // Bind the render target view and depth stencil buffer to the output render pipeline.
    _deviceContext->OMSetRenderTargets( 1, &_renderTargetView, _depthStencilView );


    /////////////////////////////////////////////////////////////////////////////
    // Rasterizer
    /////////////////////////////////////////////////////////////////////////////
    // Setup the raster description which will determine how and what polygons will be drawn.
    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = true;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    // Create the rasterizer state from the description we just filled out.
    result = _device->CreateRasterizerState( &rasterDesc, &_rasterState );
    if( FAILED( result ) ) {
        return false;
    }

    _deviceContext->RSSetState( _rasterState );


    /////////////////////////////////////////////////////////////////////////////
    // Viewport
    /////////////////////////////////////////////////////////////////////////////
    // Setup the viewport for rendering.
    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<float>( t_clientWidth );
    viewport.Height = static_cast<float>( t_clientHeight );
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    // Create the viewport.
    _deviceContext->RSSetViewports( 1, &viewport );
   

    return true;
}




void CD3D::ShutDown() {

    // Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
    if( _swapChain ) {
        _swapChain->SetFullscreenState( false, NULL );
    }

    ReleaseCOM( _rasterState );
    ReleaseCOM( _depthStencilView );
    ReleaseCOM( _depthStencilState );
    ReleaseCOM( _depthStencilBuffer );
    ReleaseCOM( _renderTargetView );
    ReleaseCOM( _deviceContext );
    ReleaseCOM( _device );
    ReleaseCOM( _swapChain );

    return;
}


void CD3D::BeginScene( float red, float green, float blue, float alpha ) {
    float color[4];


    // Setup the color to clear the buffer to.
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    // Clear the back buffer.
    _deviceContext->ClearRenderTargetView( _renderTargetView, color );

    // Clear the depth buffer.
    _deviceContext->ClearDepthStencilView( _depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

    return;
}


void CD3D::EndScene() {
    // Present the back buffer to the screen since rendering is complete.
    if( _bVsync ) {
        // Lock to screen refresh rate.
        _swapChain->Present( 1, 0 );
    } else {
        // Present as fast as possible.
        _swapChain->Present( 0, 0 );
    }

    return;
}

