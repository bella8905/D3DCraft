#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "Utl_Global.h"
#include "Math.h"
#include "D3DSystem.h"
#include "Shader.h"
#include "Geometry.h"
#include "View.h"
#include "Camera.h"

#include "Graphics.h"



// global video settings
static struct SGraphicSettings {
    bool _bFullScreen;
    bool _bVsyncEnabled;
    bool _4xMSAAEnabled;
    // http://www.directxtutorial.com/Lesson.aspx?lessonid=11-1-4
    int _defaultClientWidth;    // default client width
    int _defaultClientHeight;   // default client height

    int _currentClientWidth;        // current client width
    int _currentClientHeight;       // current client height


    SGraphicSettings() {
        _bFullScreen = false;
        _bVsyncEnabled = true;
        _4xMSAAEnabled = false;
        _defaultClientWidth = 800;
        _defaultClientHeight = 600;
    }

} Graphics_GlobalSettings;


bool CGraphics::GetIsFullScreen() {
    return Graphics_GlobalSettings._bFullScreen;
}

bool CGraphics::GetIsVsyncEnabled() {
    return Graphics_GlobalSettings._bVsyncEnabled;
}

bool CGraphics::GetIs4xMSAAEnabled() {
    return Graphics_GlobalSettings._4xMSAAEnabled;
}

void CGraphics::SetIs4xMSAAEnabled( bool t_bEnabled ) {
    Graphics_GlobalSettings._4xMSAAEnabled = t_bEnabled;
}

void CGraphics::GetDefaultScreenSize( int& t_clientWidth, int& t_clientHeight ) {
    t_clientWidth = Graphics_GlobalSettings._defaultClientWidth;
    t_clientHeight = Graphics_GlobalSettings._defaultClientHeight;
}

void CGraphics::GetScreenSize( int& t_clientWidth, int& t_clientHeight ) {
    t_clientWidth = Graphics_GlobalSettings._currentClientWidth;
    t_clientHeight = Graphics_GlobalSettings._currentClientHeight;
}

void CGraphics::SetScreenSize( int t_clientWidth, int t_clientHeight ) {
    Graphics_GlobalSettings._currentClientWidth = t_clientWidth;
    Graphics_GlobalSettings._currentClientHeight = t_clientHeight;
}

CGraphics::CGraphics() : _d3d( nullptr ), _pFreeflyCam( nullptr ) {

}

CGraphics::~CGraphics() {

}



bool CGraphics::Initialize( const int& t_clientWidth, const int& t_clientHeight, HWND t_hwnd ) {
    // Create the Direct3D object.
    _d3d = new CD3D();

    if( !_d3d ) {
        return false;
    }

    // Initialize the Direct3D object.
    if( !_d3d->Initialize( t_hwnd, t_clientWidth, t_clientHeight, GetIsVsyncEnabled(), GetIsFullScreen() ) ) {
        return false;
    }

    _initModules();
    _initScene();

    return true;
}

void CGraphics::ShutDown() {
    _deinitModules();

    if( _d3d ) {
        _d3d->ShutDown();
        delete _d3d;
        _d3d = nullptr;
    }

    if( _pFreeflyCam ) {
        delete _pFreeflyCam;
        _pFreeflyCam = nullptr;
    }
}

bool CGraphics::Frame() {
    _pFreeflyCam->UpdateControl( 1.f / 60.f );
    _pFreeflyCam->SetToView( View_GetActive() );

    return _render();
}

bool CGraphics::_render() {

    if( !_d3d ) return false;

    _d3d->BeginScene( 0.8f, 0.8f, 0.8f, 1.f );


    for( uint i = 0, numOfObj = _objects.size(); i < numOfObj; ++i ) {
        Shaders_BindShader( &_objects[i] );
        _objects[i].Draw();
    }



    _d3d->EndScene();
    return true;
}

void CGraphics::_initScene() {

    // init objects
    CObject box( GEO_BOX );
    glm::mat4 rot_x30 = glm::rotate( glm::mat4(), 30 * g_o2Pi, glm::vec3( 1, 0, 0 ) );
    box.SetRot( rot_x30 );
    _objects.push_back( box );



    // view matrix
    glm::vec3 camPos( 0.f, 0.f, -2.0f );
    glm::vec3 camTarget( 0.f, 0.f, 0.f );
    float dist = glm::distance( camPos, camTarget );
    _pFreeflyCam = new CFreeFlyCamera( ToPositon( camPos ), ToPositon( camTarget ), glm::vec4( 0.f, 1.f, 0.f, 0.f ) );

    CView view;
    _pFreeflyCam->SetToView( &view );
    // view.SetCameraPostionFaceAndUp( ToPositon( camPos ), ToDirection( camFace ) );
    view.SetHorizontalFieldOfView( DegToRad( 80.f ) );
    View_SetAsActive( &view );
}

void CGraphics::_initModules() {
    // init geo
    CGeoContainer::GetInstance().Init( _d3d->GetDevice(), _d3d->GetDeviceContext() );
    // init shader
    g_mvpVS.Init( _d3d->GetDevice(), _d3d->GetDeviceContext() );
    g_simpleColorPS.Init( _d3d->GetDevice(), _d3d->GetDeviceContext() );
    Shaders_SetAsActive( VS, &g_mvpVS );
    Shaders_SetAsActive( PS, &g_simpleColorPS );
}

void CGraphics::_deinitModules() {
    // shaders
    g_mvpVS.Deinit();
    g_simpleColorPS.Deinit();
    Shaders_Clear();

    // geo container
    CGeoContainer::GetInstance().Deinit();

}
