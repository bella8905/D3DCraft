
#include "glm\glm.hpp"
#include "glm\gtx\quaternion.hpp"

#include "View.h"
#include "Input.h"
#include "Graphics.h"

#include "Camera.h"



CCamera::CCamera( glm::vec4 t_pos, glm::vec4 t_facing, glm::vec4 t_up ) : _pos( t_pos ), _facing( t_facing ), _up( t_up ) 
{
}


CCamera::~CCamera( void ) {
}


void CCamera::Setup( const glm::vec4& t_pos, const glm::vec4& t_facing, const glm::vec4& t_up ) {
    _pos = t_pos;
    _facing = t_facing;
    _up = t_up;
}


void CCamera::SetToView( CView* t_view ) {
    if( !t_view ) return;

    if( Equals( glm::length( _facing ), 0.f ) ) {
        _facing = glm::vec4( 0.f, 0.f, 1.f, 0.f );
    }

    t_view->SetCameraPostionFaceAndUp( _pos, _facing, _up );
}



CFreeFlyCamera::CFreeFlyCamera( glm::vec4 t_pos, glm::vec4 t_aim, glm::vec4 t_up ) : CCamera( t_pos, glm::normalize( t_aim - t_pos ), t_up ), _aim( t_aim ) {
    _cameraHehaviors[CAMERA_PAN]._key = CInput::KEY::KEY_MOUSE_MIDDLE;
    _cameraHehaviors[CAMERA_PAN]._speed = 0.3f;
    _cameraHehaviors[CAMERA_ORBIT]._key = CInput::KEY::KEY_MOUSE_LEFT;
    _cameraHehaviors[CAMERA_ORBIT]._speed = 100.f;
    _cameraHehaviors[CAMERA_ZOOM]._key = CInput::KEY::KEY_MOUSE_RIGHT;
    _cameraHehaviors[CAMERA_ZOOM]._speed = 1.f;

}


void CFreeFlyCamera::UpdateControl( double t_delta ) {
    /////////////////////////////////////////////////////////////////
    //
    // control buttons
    // 
    // middle button dragging, camera pan,
    //  keep the aiming direction and distance to camera, 
    //  but move the camera left/right along camera up and right axis.
    //
    // left button dragging, camera orbit, 
    //  keep the aiming point and distance to camera, 
    //  but orbit the camera along camera aim/position sphere
    //
    // right button dragging, camera zoom,
    //  keep the aiming point and the aiming direction, 
    //  move camera along camera facing axis.
    //
    /////////////////////////////////////////////////////////////////
    for( int b = 0; b < CAMERA_BEHAVIOR_COUNTER; ++b ) {
        if( gInputs.GetKeyState( _cameraHehaviors[b]._key, CInput::STATE_PRESSED ) ) {
            gInputs.GetMousePosition( _cameraHehaviors[b]._cursorX, _cameraHehaviors[b]._cursorY );
            return;
        }

        if( gInputs.GetKeyState( _cameraHehaviors[b]._key, CInput::STATE_HELD ) ) {
            int x, y;
            gInputs.GetMousePosition( x, y );
            int deltaX = x - _cameraHehaviors[b]._cursorX;
            int deltaY = y - _cameraHehaviors[b]._cursorY;

            if( deltaX == 0  && deltaY == 0 ) {
                return;
            }

            switch( b ) {
            case CAMERA_PAN:
                {
                    glm::vec3 z = glm::normalize( ToVec3( _facing ) );
                    glm::vec3 right = glm::normalize( glm::cross( z, glm::vec3( _up ) ) );
                    _pos += (float)deltaX * (float)t_delta * _cameraHehaviors[b]._speed * ToDirection( right ) + (float)deltaY * (float)t_delta * _cameraHehaviors[b]._speed * _up;
                } break;
            case CAMERA_ORBIT:
                { 
                    SRay oldRay = _getCameraCursorRayFromCursorPos( _cameraHehaviors[b]._cursorX, _cameraHehaviors[b]._cursorY );
                    SRay newRay = _getCameraCursorRayFromCursorPos( x, y );
                    glm::vec3 startSide = glm::vec3( oldRay._Dir );
                    glm::vec3 endSide = glm::vec3( newRay._Dir );
                    glm::vec3 rotAxis = glm::normalize( glm::cross( startSide, endSide ) );
                    float rotAngle = acos( glm::dot( startSide, endSide ) ) * 2 * (float)t_delta * _cameraHehaviors[b]._speed; 
                    glm::quat rotQuat = glm::angleAxis( rotAngle, rotAxis );
                    glm::mat3 rot_quat = glm::toMat3( rotQuat );
                    _facing = ToDirection( glm::normalize( rot_quat * ToVec3( _facing ) ) );
                    _up = ToDirection( glm::normalize( rot_quat * glm::vec3( _up ) ) );
                    float dist = glm::distance( _pos, _aim );
                    _pos = _aim - ToDirection( dist * _facing );

                } break;
            case CAMERA_ZOOM:
                {
                    float delta = abs( deltaX ) > abs( deltaY ) ? ( float )deltaX : ( float )deltaY;
                    float dist = glm::distance( _pos, _aim );
                    dist = max( dist - delta * _cameraHehaviors[b]._speed * ( float )t_delta, 0.5f );
                    _pos = _aim - _facing * dist;
                } break;
            default: break;
            }

            _cameraHehaviors[b]._cursorX = x;
            _cameraHehaviors[b]._cursorY = y;
        }


    }

}


void CFreeFlyCamera::Update( double t_delta ) {
}


SRay CFreeFlyCamera::_getCameraCursorRayFromCursorPos( const double& t_x, const double& t_y ) {
    // nds ( -1 , 1 )
    int winWidth, winHeight;
    CGraphics::GetScreenSize( winWidth, winHeight );
    float x = ( 2.f * (float)t_x ) / ( float )winWidth - 1.f;
    float y = 1.f - ( 2.f * (float)t_y ) / ( float )winHeight;
    glm::vec3 dir_nds( x, y, 1.f );
    glm::vec4 dir_clip( dir_nds.x, dir_nds.y, 0.f, 0.f );
    // get the current used camera's proj matrix
    glm::vec4 dir_eye = View_GetActive()->GetProj2ViewMatrix() * dir_clip;
    dir_eye = glm::vec4( dir_eye.x, dir_eye.y, 1.f, 0.f );
    SRay ray_eye( glm::vec4( 0.f, 0.f, 0.f, 1.f ), dir_eye );
    SRay ray_wor = ray_eye.Transform( View_GetActive()->GetView2WorldMatrix() );

    return ray_wor;
}



