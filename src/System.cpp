
#include <windowsx.h>

#include "Utl_LogMsg.h"
#include "Input.h"
#include "Graphics.h"
#include "System.h"
#include "Geometry.h"

CSystem* CSystem::_pActiveInstance = nullptr;


CSystem::CSystem( HINSTANCE t_hinstance ) : _hinstance( t_hinstance ), _inputs( nullptr ), _graphics( nullptr ) {

}


CSystem::~CSystem() {

}


bool CSystem::Initialize() {
    
    int clientWidth, clientHeight;

    clientWidth = 0;
    clientHeight = 0;

    // Initialize the windows api.
    _initWindow( clientWidth, clientHeight );


    // Inputs module
    _inputs = &gInputs;
    _inputs->Initialize();

    // Graphics module
    _graphics = new CGraphics();
    if( !_graphics ) {
        return false;
    }

    if( !_graphics->Initialize( clientWidth, clientHeight, _hwnd ) ) {
        return false;
    }

    return true;
}


void CSystem::ShutDown() {
    if( _graphics ) {
        _graphics->ShutDown();
        delete _graphics;
        _graphics = nullptr;
    }


    _inputs = nullptr;


    _shutDownWindow();

}

void CSystem::Run() {
    MSG msg;
    bool bDone, result;


    // Initialize the message structure.
    ZeroMemory( &msg, sizeof( MSG ) );

    // Loop until there is a quit message from the window or the user.
    bDone = false;
    while( !bDone ) {
        // Handle the windows messages.
        if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) ) {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }

        // If windows signals to end the application then exit out.
        if( msg.message == WM_QUIT ) {
            bDone = true;
        } else {
            // Otherwise do the frame processing.
            result = _update();
            if( !result ) {
                bDone = true;
            }
        }

    }
}

LRESULT CALLBACK CSystem::MessageHandler( HWND t_hwnd, UINT t_umsg, WPARAM t_wparam, LPARAM t_lparam ) {
    switch( t_umsg ) {
        /*
        // WM_ACTIVATE is sent when the window is activated or deactivated.  
        // We pause the game when the window is deactivated and unpause it 
        // when it becomes active.  
        case WM_ACTIVATE:
            if( LOWORD( wParam ) == WA_INACTIVE ) {
                mAppPaused = true;
                mTimer.Stop();
            } else {
                mAppPaused = false;
                mTimer.Start();
            }
            return 0;

            // WM_SIZE is sent when the user resizes the window.  
        case WM_SIZE:
            // Save the new client area dimensions.
            mClientWidth = LOWORD( lParam );
            mClientHeight = HIWORD( lParam );
            if( md3dDevice ) {
                if( wParam == SIZE_MINIMIZED ) {
                    mAppPaused = true;
                    mMinimized = true;
                    mMaximized = false;
                } else if( wParam == SIZE_MAXIMIZED ) {
                    mAppPaused = false;
                    mMinimized = false;
                    mMaximized = true;
                    OnResize();
                } else if( wParam == SIZE_RESTORED ) {

                    // Restoring from minimized state?
                    if( mMinimized ) {
                        mAppPaused = false;
                        mMinimized = false;
                        OnResize();
                    }

                    // Restoring from maximized state?
                    else if( mMaximized ) {
                        mAppPaused = false;
                        mMaximized = false;
                        OnResize();
                    } else if( mResizing ) {
                        // If user is dragging the resize bars, we do not resize 
                        // the buffers here because as the user continuously 
                        // drags the resize bars, a stream of WM_SIZE messages are
                        // sent to the window, and it would be pointless (and slow)
                        // to resize for each WM_SIZE message received from dragging
                        // the resize bars.  So instead, we reset after the user is 
                        // done resizing the window and releases the resize bars, which 
                        // sends a WM_EXITSIZEMOVE message.
                    } else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
                    {
                        OnResize();
                    }
                }
            }
            return 0;

            // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
            case WM_ENTERSIZEMOVE:
            mAppPaused = true;
            mResizing  = true;
            mTimer.Stop();
            return 0;

            // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
            // Here we reset everything based on the new window dimensions.
            case WM_EXITSIZEMOVE:
            mAppPaused = false;
            mResizing  = false;
            mTimer.Start();
            OnResize();
            return 0;
            */

            // WM_DESTROY is sent when the window is being destroyed.
        case WM_DESTROY:
        {
            PostQuitMessage( 0 );
            return 0;
        }

        // Check if the window is being closed.
        case WM_CLOSE:
        {
            PostQuitMessage( 0 );
            return 0;
        }

        // The WM_MENUCHAR message is sent when a menu is active and the user presses 
        // a key that does not correspond to any mnemonic or accelerator key. 
        case WM_MENUCHAR:
            // Don't beep when we alt-enter.
            return MAKELRESULT( 0, MNC_CLOSE );

            // Catch this message so to prevent the window from becoming too small.
        case WM_GETMINMAXINFO:
            ( ( MINMAXINFO* )t_lparam )->ptMinTrackSize.x = 200;
            ( ( MINMAXINFO* )t_lparam )->ptMinTrackSize.y = 200;
            return 0;


        case WM_LBUTTONDOWN:
            _inputs->OnMouseDown( MK_LBUTTON, t_wparam, GET_X_LPARAM( t_lparam ), GET_Y_LPARAM( t_lparam ) );
            return 0;
        case WM_MBUTTONDOWN:
            _inputs->OnMouseDown( MK_MBUTTON, t_wparam, GET_X_LPARAM( t_lparam ), GET_Y_LPARAM( t_lparam ) );
            return 0;
        case WM_RBUTTONDOWN:
            _inputs->OnMouseDown( MK_RBUTTON, t_wparam, GET_X_LPARAM( t_lparam ), GET_Y_LPARAM( t_lparam ) );
            return 0;
        case WM_LBUTTONUP:
            _inputs->OnMouseUp( MK_LBUTTON, t_wparam, GET_X_LPARAM( t_lparam ), GET_Y_LPARAM( t_lparam ) );
            return 0;
        case WM_MBUTTONUP:
            _inputs->OnMouseUp( MK_MBUTTON, t_wparam, GET_X_LPARAM( t_lparam ), GET_Y_LPARAM( t_lparam ) );
            return 0;
        case WM_RBUTTONUP:
            _inputs->OnMouseUp( MK_RBUTTON, t_wparam, GET_X_LPARAM( t_lparam ), GET_Y_LPARAM( t_lparam ) );
            return 0;
        case WM_MOUSEMOVE:
            _inputs->OnMouseMove( t_wparam, GET_X_LPARAM( t_lparam ), GET_Y_LPARAM( t_lparam ) );
            return 0;


        // Check if a key has been pressed on the keyboard.
        case WM_KEYDOWN:
        {
            // If a key is pressed send it to the input object so it can record that state.
            _inputs->OnKeyDown( ( unsigned int )t_wparam );
            return 0;
        }

        // Check if a key has been released on the keyboard.
        case WM_KEYUP:
        {
            // If a key is released then send it to the input object so it can unset the state for that key.
            _inputs->OnKeyUp( ( unsigned int )t_wparam );
            return 0;
        }



        // Any other messages send to the default message handler as our application won't make use of them.
        default:
        {
            return DefWindowProc( t_hwnd, t_umsg, t_wparam, t_lparam );
        }
    }
}


bool CSystem::_update() {
    // press esc to exit.
    if( _inputs->GetKeyState( CInput::KEY_ESC, CInput::STATE_PRESSED ) ) {
        return false;
    }

    if( _graphics != nullptr ) {
        if( !_graphics->Frame() ) {
            return false;
        }
    }

    _inputs->Update();

    return true;
}

void CSystem::_initWindow( int& t_width, int& t_height ) {
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;
    
    // set as current instance
    _pActiveInstance = this;

    // Give the application a name.
    _applicationName = L"D3DCraft";

    // Setup the windows class with default settings.
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = _hinstance;
    wc.hIcon = LoadIcon( nullptr, IDI_WINLOGO );
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor( nullptr, IDC_ARROW );
    wc.hbrBackground = ( HBRUSH )GetStockObject( BLACK_BRUSH );
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = _applicationName;
    wc.cbSize = sizeof( WNDCLASSEX );

    // Register the window class.
    RegisterClassEx( &wc );

    // Determine the resolution of the clients desktop screen.
    t_width = GetSystemMetrics( SM_CXSCREEN );
    t_height = GetSystemMetrics( SM_CYSCREEN );

    int width = t_width;
    int height = t_height;

    // Setup the screen settings depending on whether it is running in full screen or in windowed mode.
    if( CGraphics::GetIsFullScreen() ) {
        // If full screen set the screen to maximum size of the users desktop and 32bit.
        memset( &dmScreenSettings, 0, sizeof( dmScreenSettings ) );
        dmScreenSettings.dmSize = sizeof( dmScreenSettings );
        dmScreenSettings.dmPelsWidth = ( unsigned long )t_width;
        dmScreenSettings.dmPelsHeight = ( unsigned long )t_height;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Change the display settings to full screen.
        ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN );

        // Set the position of the window to the top left corner.
        posX = posY = 0;
    } else {
        // If windowed then set it to default resolution.
        CGraphics::GetDefaultScreenSize( t_width, t_height );
        // calculate window dimensions using client dimensions
        RECT wr = { 0, 0, t_width, t_height };    
        AdjustWindowRect( &wr, WS_OVERLAPPEDWINDOW, FALSE );  

        // Place the window in the middle of the screen.
        posX = ( GetSystemMetrics( SM_CXSCREEN ) - t_width ) / 2;
        posY = ( GetSystemMetrics( SM_CYSCREEN ) - t_height ) / 2;

        width = wr.right - wr.left;
        height = wr.bottom - wr.top;
    }

    CGraphics::SetScreenSize( width, height );
    // Create the window with the screen settings and get the handle to it.
    _hwnd = CreateWindowEx( WS_EX_APPWINDOW, _applicationName, _applicationName,
                            WS_OVERLAPPEDWINDOW,
                             posX, posY, width, height, nullptr, nullptr, _hinstance, nullptr );

    // Bring the window up on the screen and set it as main focus.
    ShowWindow( _hwnd, SW_SHOW );
    SetForegroundWindow( _hwnd );
    SetFocus( _hwnd );

    // Hide the mouse cursor.
    // ShowCursor( false );

    return;
}

void CSystem::_shutDownWindow() {
    // Show the mouse cursor.
    // ShowCursor( true );

    // Fix the display settings if leaving full screen mode.
    if( CGraphics::GetIsFullScreen() ) {
        ChangeDisplaySettings( nullptr, 0 );
    }

    // Remove the window.
    DestroyWindow( _hwnd );
    _hwnd = nullptr;

    // Remove the application instance.
    UnregisterClass( _applicationName, _hinstance );
    _hinstance = nullptr;

    // Release the pointer to this class.
    _pActiveInstance = nullptr;

    return;
}



LRESULT CALLBACK WndProc( HWND t_hwnd, UINT t_umsg, WPARAM t_wparam, LPARAM t_lparam ) {

    if( CSystem::GetActiveInstance() != nullptr ) {
        return CSystem::GetActiveInstance()->MessageHandler( t_hwnd, t_umsg, t_wparam, t_lparam );
    }
    return 0;
}

