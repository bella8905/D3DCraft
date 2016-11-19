#pragma once

#include <windows.h>

class CInput;
class CGraphics;


class CSystem {
public:
    CSystem( HINSTANCE t_instance );
    ~CSystem();


private:
    static CSystem* _pActiveInstance;

public:
    static CSystem* GetActiveInstance() { return _pActiveInstance;  }

    bool Initialize();
    void ShutDown();
    void Run();

    LRESULT CALLBACK MessageHandler( HWND t_hwnd, UINT t_umsg, WPARAM t_wparam, LPARAM t_lparam );


private:
    bool _update();
    void _initWindow( int& t_width, int& t_height );
    void _shutDownWindow();

private:
    LPCWSTR _applicationName;
    HINSTANCE _hinstance;
    HWND _hwnd;
 
    CInput* _inputs;
    CGraphics* _graphics;

};


static LRESULT CALLBACK WndProc( HWND t_hwnd, UINT t_umsg, WPARAM t_wparam, LPARAM t_lparam );