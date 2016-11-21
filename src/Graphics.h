#pragma once

#include <windows.h>
#include <vector>

#include "Object.h"

class CD3D;
class CCamera;

class CGraphics {
public:
    CGraphics();
    ~CGraphics();

    bool Initialize( const int& t_clientWidth, const int& t_clientHeight, HWND t_hwnd );
    void ShutDown();
    bool Frame();


    // global settings
    static bool GetIsFullScreen();
    static bool GetIsVsyncEnabled();
    static bool GetIs4xMSAAEnabled();
    static void SetIs4xMSAAEnabled( bool t_bEnabled );
    static void GetDefaultScreenSize( int& t_clientWidth, int& t_clientHeight );
    static void GetScreenSize( int& t_clientWidth, int& t_clientHeight );
    static void SetScreenSize( int t_clientWidth, int t_clientHeight );

private:
    CD3D* _d3d;
    struct SCENE {
        std::vector<CObject> _objects;
        CCamera* _pFreeflyCam;

        SCENE() : _pFreeflyCam( nullptr ) {

        }

        void Init();
        void Deinit();
        void Update();
        void Draw();

    } _scene;

    

private:
    bool _render();


private:
    void _initModules();
    void _deinitModules();

    void _initScene();

};
