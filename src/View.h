#pragma once

#include "glm/glm.hpp"
#include "Utl_Global.h"

class CView {

public:
    /////////////////////////////////////////////////////////////////
    //
    // 
    //
    /////////////////////////////////////////////////////////////////
    struct SViewPort {
        us _x, _y, _width, _height;
        float _maxZ, _minZ;

        SViewPort() {
            // defaults
            _x = 0;
            _y = 0;
            _width = 800;
            _height = 600;
            _maxZ = 1;
            _minZ = -1;
        }

        void SetAsActive() {
            // fill it!


        }
    };

    struct SFrustum {

    };

public:
    CView() { initDefaults();  }
    virtual ~CView() {}


protected:

    /////////////////////////////////////////////////////////////////
    //
    // if it is a perspective view or orthographic view 
    //
    /////////////////////////////////////////////////////////////////
    bool _bPerspective;

    /////////////////////////////////////////////////////////////////
    //
    // view port, 
    // this could be a full window view,
    // or just part of the whole screen.
    //
    /////////////////////////////////////////////////////////////////
     SViewPort _viewPort;

    /////////////////////////////////////////////////////////////////
    //
    // transform matrix converting points from world to view space.
    //
    // View matrix, is the V in MVP.
    //
    /////////////////////////////////////////////////////////////////
    glm::mat4 _world2ViewMatrix;

    /////////////////////////////////////////////////////////////////
    //
    // transform matrix converting points from view to world space.
    //
    // this is the inverse of _world2ViewMatrix, 
    //
    /////////////////////////////////////////////////////////////////
    glm::mat4 _view2WorldMatrix;


    /////////////////////////////////////////////////////////////////
    //
    // transform matrix converting points from view space to homogeneous 
    // clip space
    // 
    // Projection matrix, is the P in MVP.
    //
    /////////////////////////////////////////////////////////////////
    glm::mat4 _view2ProjMatrix;

    /////////////////////////////////////////////////////////////////
    //
    // transform matrix converting points from homogeneous 
    // clip space to view space.
    //
    // this is the inverse of _view2ProjMatrix,
    //
    /////////////////////////////////////////////////////////////////
    glm::mat4 _proj2ViewMatrix;

    /////////////////////////////////////////////////////////////////
    //
    // transform matrix converting points from world space 
    // to clip space.
    // 
    // _view2ProjMatrix * _view2WorldMatrix
    //
    /////////////////////////////////////////////////////////////////
    glm::mat4 _world2ProjMatrix;

    /////////////////////////////////////////////////////////////////
    //
    // dirty flags.
    // update components only if they are tainted.
    //
    /////////////////////////////////////////////////////////////////
    us _dirtyFlags;
    static const us _DIRTY_FLAG_WORLD_TO_VIEW_MATRIX    = 1 << 0;
    static const us _DIRTY_FLAG_VIEW_TO_WORLD_MATRIX    = 1 << 1;
    static const us _DIRTY_FLAG_VIEW_TO_PROJ_MATRIX     = 1 << 2;
    static const us _DIRTY_FLAG_PROJ_TO_VIEW_MATRIX     = 1 << 3;
    static const us _DIRTY_FLAG_WORLD_TO_PROJ_MATRIX    = 1 << 4;
    static const us _DIRTY_FLAG_VIEW_PORT               = 1 << 5;

    static const us _DIRTY_FLAG_ALL                     = _DIRTY_FLAG_WORLD_TO_VIEW_MATRIX | 
                                                          _DIRTY_FLAG_VIEW_TO_WORLD_MATRIX |
                                                          _DIRTY_FLAG_VIEW_TO_PROJ_MATRIX |
                                                          _DIRTY_FLAG_PROJ_TO_VIEW_MATRIX |
                                                          _DIRTY_FLAG_WORLD_TO_PROJ_MATRIX |
                                                          _DIRTY_FLAG_VIEW_PORT;

protected:
    void initDefaults();

    /////////////////////////////////////////////////////////////////
    //
    // update attributes
    //
    /////////////////////////////////////////////////////////////////
    void updateWorld2ViewMatrix();
    void updateView2WorldMatrix();
    void updateView2ProjMatrix();
    void updateProj2ViewMatrix();
    void updateWorld2ProjMatrix();
    void updateViewPort();

//     /////////////////////////////////////////////////////////////////
//     //
//     // compute view2world and world2view matrices from scratch
//     //
//     /////////////////////////////////////////////////////////////////
//     void computeWorldViewMatrices();

    /////////////////////////////////////////////////////////////////
    //
    // compute view2world and world2view matrices from scratch
    //
    /////////////////////////////////////////////////////////////////
    void computePerspectiveViewProjMatrices( glm::mat4& t_view2Proj, glm::mat4& t_proj2View );
    void computeOrthographicViewProjMatrices( glm::mat4& t_view2Proj, glm::mat4& t_proj2View );
    void getInverseOfProj2View( const glm::mat4& t_proj2View, glm::mat4& t_view2Proj );
    void getInverseOfView2Proj( const glm::mat4& t_view2Proj, glm::mat4& t_proj2View );
    float getHorizontalPerspective( const glm::mat4& t_view2Proj );
    float getVerticalPerspective( const glm::mat4& t_view2Proj );


public:
    /////////////////////////////////////////////////////////////////
    //
    // return the updated matrix
    //
    /////////////////////////////////////////////////////////////////
    const glm::mat4& GetWorld2ViewMatrix();
    const glm::mat4& GetView2WorldMatrix();
    const glm::mat4& GetView2ProjMatrix();
    const glm::mat4& GetProj2ViewMatrix();
    const glm::mat4& GetWorld2ProjMatrix();
    SViewPort* GetViewPort();

    void GetCameraPosition( glm::vec4& t_pos );
    void GetCameraPositionFaceUpAndRight( glm::vec4& t_pos, glm::vec4& t_face, glm::vec4& t_up, glm::vec4& t_right );

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    //
    // init camera
    //
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////
    //
    // setup camera.
    // position.
    // facing direction.
    // almost up direction.
    // 
    // it doesn't require the facing direction being 
    // perpendicular to the up direction
    //
    /////////////////////////////////////////////////////////////////
    void SetCameraPostionFaceAndUp( glm::vec4 t_pos, glm::vec4 t_facing, glm::vec4 t_up = glm::vec4( 0.f, 1.f, 0.f, 0.f ) );



    /////////////////////////////////////////////////////////////////
    //
    // set horizontal perspective by horizontal fov angle
    //
    /////////////////////////////////////////////////////////////////
    void SetHorizontalFieldOfView( float t_radAngle );

    /////////////////////////////////////////////////////////////////
    //
    // set horizontal perspective directly
    // this value is a non linear scale where 1.0 == 45 degrees angle of view
    // smaller value gives a wider fov.
    //
    /////////////////////////////////////////////////////////////////
    void SetHorizontalPerspective( float t_perspective );

    void SetAspectRatio( float t_aspect );

    void SetViewport( const SViewPort& t_viewport );
};

void View_SetAsActive( CView* t_view );
CView* View_GetActive();