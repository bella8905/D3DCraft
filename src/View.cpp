#include "glm/glm.hpp"

#include "Math.h"
#include "View.h"
#include "Graphics.h"



static struct {
    CView _view;
    
}View;

void View_SetAsActive( CView* t_view ) {
    View._view = *t_view;
    // also marked as being used viewport
    CView::SViewPort* viewport = t_view->GetViewPort();
    viewport->SetAsActive();
}

CView* View_GetActive() {
    return &View._view;
}

void CView::initDefaults() {
    // mark all things as dirty at start
    _dirtyFlags = _DIRTY_FLAG_ALL;

    _bPerspective = true;

    // view 
    // glm::vec4 camPos( 0.15f, -0.63f, 1.89f, 1.f );
    glm::vec4 camPos( 0.f, 2.f, 2.f, 1.f );
    // glm::vec4 camFace( 0.f, 0.f, -1.f, 0.f );
    glm::vec4 camFace = -ToDirection( glm::normalize( glm::vec3( camPos ) ) );
    glm::vec4 camUp( 0.f, 1.f, 0.f, 0.f );
    SetCameraPostionFaceAndUp( camPos, camFace, camUp );

    // horizontal filed of view
    float horizontalFOV = 80;
    SetHorizontalFieldOfView( DegToRad( horizontalFOV ) );

    // set viewport
    SViewPort viewport;

    int winWidth, winHeight;
    CGraphics::GetScreenSize( winWidth, winHeight );
    viewport._width = ( us )winWidth;
    viewport._height = ( us )winHeight;


    SetViewport( viewport );

    
}

void CView::updateWorld2ViewMatrix() {
    if( ( _dirtyFlags & _DIRTY_FLAG_WORLD_TO_VIEW_MATRIX ) != 0  ) {
        if( ( _dirtyFlags & _DIRTY_FLAG_VIEW_TO_WORLD_MATRIX ) == 0 ) {
            // inverse matrix
            _world2ViewMatrix = Matrix_GetInverseTranslationRotation( _view2WorldMatrix );
            _dirtyFlags &= ~_DIRTY_FLAG_WORLD_TO_VIEW_MATRIX;
        } else {
            assert( false && "World to View matrix was never set. Call a SetCamera** function first. " );
        }
    }
}

void CView::updateView2WorldMatrix() {
    if( ( _dirtyFlags & _DIRTY_FLAG_VIEW_TO_WORLD_MATRIX ) != 0  ) {
        if( ( _dirtyFlags & _DIRTY_FLAG_WORLD_TO_VIEW_MATRIX ) == 0 ) {
            // inverse matrix
            _view2WorldMatrix = Matrix_GetInverseTranslationRotation( _world2ViewMatrix );
            _dirtyFlags &= ~_DIRTY_FLAG_VIEW_TO_WORLD_MATRIX;
        } else {
            assert( false && "View to World matrix was never set. Call a SetCamera** function first. " );
        }
    }
}



void CView::updateView2ProjMatrix() {
    if( ( _dirtyFlags & _DIRTY_FLAG_VIEW_TO_PROJ_MATRIX ) != 0  ) {
        if( ( _dirtyFlags & _DIRTY_FLAG_PROJ_TO_VIEW_MATRIX ) == 0 ) {
            // inverse matrix
            getInverseOfView2Proj( _proj2ViewMatrix, _view2ProjMatrix );
            _dirtyFlags &= ~_DIRTY_FLAG_VIEW_TO_PROJ_MATRIX;
        } else {
            // compute view2world and world2view matrix from scratch
            if( _bPerspective ) {
                computePerspectiveViewProjMatrices( _view2ProjMatrix, _proj2ViewMatrix );
            } else {
                computeOrthographicViewProjMatrices( _view2ProjMatrix, _proj2ViewMatrix );
            }

            _dirtyFlags &= ~_DIRTY_FLAG_VIEW_TO_PROJ_MATRIX;
            _dirtyFlags &= ~_DIRTY_FLAG_PROJ_TO_VIEW_MATRIX;

        }
    }
}

void CView::updateProj2ViewMatrix() {
    if( ( _dirtyFlags & _DIRTY_FLAG_PROJ_TO_VIEW_MATRIX ) != 0  ) {
        if( ( _dirtyFlags & _DIRTY_FLAG_VIEW_TO_PROJ_MATRIX ) == 0 ) {
            // inverse matrix
            getInverseOfView2Proj( _view2ProjMatrix, _proj2ViewMatrix );
            _dirtyFlags &= ~_DIRTY_FLAG_PROJ_TO_VIEW_MATRIX;
        } else {
            // compute view2world and world2view matrix from scratch
            if( _bPerspective ) {
                computePerspectiveViewProjMatrices( _view2ProjMatrix, _proj2ViewMatrix );
            } else {
                computeOrthographicViewProjMatrices( _view2ProjMatrix, _proj2ViewMatrix );
            }

            _dirtyFlags &= ~_DIRTY_FLAG_VIEW_TO_PROJ_MATRIX;
            _dirtyFlags &= ~_DIRTY_FLAG_PROJ_TO_VIEW_MATRIX;

        }
    }
}


float CView::getHorizontalPerspective( const glm::mat4& t_view2Proj ) {
    return t_view2Proj[0][0];
}

float CView::getVerticalPerspective( const glm::mat4& t_view2Proj ) {
    return t_view2Proj[1][1];
}

void CView::computePerspectiveViewProjMatrices( glm::mat4& t_view2Proj, glm::mat4& t_proj2View ) {
    float perspHorizontal = getHorizontalPerspective( _view2ProjMatrix );
    float perspVertical = getVerticalPerspective( _view2ProjMatrix );

    float clipNear = 0.1f;
    float clipFar = 100.f;
    float sx = perspHorizontal;
    float sy = perspVertical;

    float sz = clipFar / ( clipFar - clipNear );
    float pz = -( clipFar * clipNear ) / ( clipFar - clipNear );
    t_view2Proj = glm::mat4( glm::vec4( sx, 0.f, 0.f, 0.f ),
                             glm::vec4( 0.f, sy, 0.f, 0.f ),
                             glm::vec4( 0.f, 0.f, sz, 1.f ),
                             glm::vec4( 0.f, 0.f, pz, 0.f )
    );

    getInverseOfView2Proj( _view2ProjMatrix, t_proj2View );

}

void CView::computeOrthographicViewProjMatrices( glm::mat4& t_view2Proj, glm::mat4& t_proj2View  ) {

}

void CView::getInverseOfProj2View( const glm::mat4& t_proj2View, glm::mat4& t_view2Proj ) {
    // proj to view matrix
    //	a	0	0	0 
    //	0	b	0	0
    //	0	0	0	-1
    //	0	0	c	d

    float a = t_proj2View[0][0];
    float b = t_proj2View[1][1];
    float c = t_proj2View[2][3];
    float d = t_proj2View[3][3];
    t_view2Proj = glm::mat4( glm::vec4( 1.f / a, 0.f, 0.f, 0.f ),
                        glm::vec4( 0.f, 1.f / b, 0.f, 0.f ),
                        glm::vec4( 0.f, 0.f, d / c, -1.f ),
                        glm::vec4( 0.f, 0.f, 1.f / c, 0.f ) 
                        );

}

void CView::getInverseOfView2Proj( const glm::mat4& t_view2Proj, glm::mat4& t_proj2View ) {
    // Opengl
    // view to proj matrix
    //	sx	0	0	0 
    //	0	sy	0	0
    //	0	0	sz	pz
    //	0	0	-1	0

    // D3D
    //	sx	0	0	0 
    //	0	sy	0	0
    //	0	0	sz	pz
    //	0	0	1	0

    float sx = t_view2Proj[0][0];
    float sy = t_view2Proj[1][1];
    float sz = t_view2Proj[2][2];
    float pz = t_view2Proj[3][2];

    t_proj2View = glm::mat4( glm::vec4( 1.f / sx, 0.f, 0.f, 0.f ),
                             glm::vec4( 0.f, 1.f / sy, 0.f, 0.f ),
                             glm::vec4( 0.f, 0.f, 0.f, 1.f / pz ),
                             glm::vec4( 0.f, 0.f, 1.f, -sz / pz )
    );
}


void CView::updateWorld2ProjMatrix() {
    if( ( _dirtyFlags & _DIRTY_FLAG_WORLD_TO_PROJ_MATRIX ) != 0  ) {
        _world2ProjMatrix = GetView2ProjMatrix() * GetWorld2ViewMatrix();
        _dirtyFlags &= ~_DIRTY_FLAG_WORLD_TO_PROJ_MATRIX;
    }
}

const glm::mat4& CView::GetWorld2ViewMatrix()  { 
    if( ( _dirtyFlags & _DIRTY_FLAG_WORLD_TO_VIEW_MATRIX ) != 0 ) {
        updateWorld2ViewMatrix();
    } 

    return _world2ViewMatrix;
}

const glm::mat4& CView::GetView2WorldMatrix() {
    if( ( _dirtyFlags & _DIRTY_FLAG_VIEW_TO_WORLD_MATRIX ) != 0 ) {
        updateView2WorldMatrix();
    } 

    return _view2WorldMatrix;
}


const glm::mat4& CView::GetView2ProjMatrix() {
    if( ( _dirtyFlags & _DIRTY_FLAG_VIEW_TO_PROJ_MATRIX ) != 0 ) {
        updateView2ProjMatrix();
    } 

    return _view2ProjMatrix;
}


const glm::mat4& CView::GetProj2ViewMatrix() {
    if( ( _dirtyFlags & _DIRTY_FLAG_PROJ_TO_VIEW_MATRIX ) != 0 ) {
        updateProj2ViewMatrix();
    } 

    return _proj2ViewMatrix;
}


const glm::mat4& CView::GetWorld2ProjMatrix() {
    if( ( _dirtyFlags & _DIRTY_FLAG_WORLD_TO_PROJ_MATRIX ) != 0 ) {
        updateWorld2ProjMatrix();
    }

    return _world2ProjMatrix;
}


void CView::updateViewPort() {
    if( ( _dirtyFlags & _DIRTY_FLAG_VIEW_PORT ) != 0  ) {

        _dirtyFlags &= ~_DIRTY_FLAG_VIEW_PORT;
    }
}

CView::SViewPort* CView::GetViewPort() {
    if( ( _dirtyFlags & _DIRTY_FLAG_VIEW_PORT ) != 0 ) {
        updateViewPort();
    }

    return &_viewPort;
}



void CView::SetCameraPostionFaceAndUp( glm::vec4 t_pos, glm::vec4 t_facing, glm::vec4 t_up ) {
    assert( IsPosition( t_pos ) );
    assert( IsDirection( t_facing ) );
    assert( IsDirection( t_up ) );
    
    glm::vec3 pos = ToVec3( t_pos );
    glm::vec3 face = ToVec3( t_facing );
    glm::vec3 up = ToVec3( t_up );

    glm::vec3 z = face;
    glm::vec3 x = glm::normalize( glm ::cross( up, z ) );
    glm::vec3 y = glm::normalize( glm::cross( z, x ) );


// 	// inverse of a translation matrix
// 	// T(v)^-1 = T(-v)
// 	glm::mat4 t( glm::vec4( 1.f, 0.f, 0.f, 0.f ),
// 			glm::vec4( 0.f, 1.f, 0.f, 0.f ),
// 			glm::vec4( 0.f, 0.f, 1.f, 0.f ),
// 			glm::vec4( -pos, 1.f ) );
// 
// 	// inverse of a rotation matrix
// 	// for orthonormalized matrix,
// 	// R(v)^-1 = transpose( R(v) )
// 	glm::mat4 r( glm::vec4( x.x, y.x, z.x, 0.f ),
// 			glm::vec4( x.y, y.y, z.y, 0.f ),
// 			glm::vec4( x.z, y.z, z.z, 0.f ),
// 			glm::vec4( 0.f, 0.f, 0.f, 1.f ) );
// 
// 	
// 	_world2ViewMatrix = r * t;

    _view2WorldMatrix = glm::mat4( 
                                ToDirection( x ), 
                                ToDirection( y ), 
                                ToDirection( z ), 
                                t_pos 
                                );


    // dirty flags
    _dirtyFlags |= _DIRTY_FLAG_WORLD_TO_VIEW_MATRIX | _DIRTY_FLAG_WORLD_TO_PROJ_MATRIX;
    _dirtyFlags &= ~_DIRTY_FLAG_VIEW_TO_WORLD_MATRIX;
}


void CView::SetHorizontalFieldOfView( float t_radAngle ) {
    assert( _bPerspective );

    float perspecive = 1.f / tan( t_radAngle * 0.5f );
    SetHorizontalPerspective( perspecive );
}


void CView::SetHorizontalPerspective( float t_perspective ) {
    assert( !Equals( t_perspective, 0.f ) );
    assert( _bPerspective );

    if( Equals( _view2ProjMatrix[0][0], t_perspective ) ) {
        return;
    }

    _view2ProjMatrix[0][0] = t_perspective;

    SViewPort* viewport = GetViewPort();
    float aspect = (float)viewport->_width / (float)viewport->_height;

    float verticalPerspective = t_perspective * aspect;
    _view2ProjMatrix[1][1] = verticalPerspective;

    // dirty flags
    _dirtyFlags |= _DIRTY_FLAG_VIEW_TO_PROJ_MATRIX | _DIRTY_FLAG_PROJ_TO_VIEW_MATRIX | _DIRTY_FLAG_WORLD_TO_PROJ_MATRIX;
}



void CView::SetAspectRatio( float t_aspect ) {
    float oldAspect = _view2ProjMatrix[1][1] / _view2ProjMatrix[0][0];
    if( Equals( oldAspect, t_aspect ) ) {
        return;
    }

    float verticalPerspective = _view2ProjMatrix[0][0] * t_aspect;
    _view2ProjMatrix[1][1] = verticalPerspective;

    // dirty flags
    _dirtyFlags |= _DIRTY_FLAG_VIEW_TO_PROJ_MATRIX | _DIRTY_FLAG_PROJ_TO_VIEW_MATRIX | _DIRTY_FLAG_WORLD_TO_PROJ_MATRIX;

}

void CView::SetViewport( const SViewPort& t_viewport ) {
    SViewPort* viewport = GetViewPort();
    *viewport = t_viewport;

    SetAspectRatio( (float)viewport->_width / (float)viewport->_height );
}


void CView::GetCameraPosition( glm::vec4& t_pos ) {
    glm::mat4 view2world = GetView2WorldMatrix();
    t_pos = view2world[3];
}

void CView::GetCameraPositionFaceUpAndRight( glm::vec4& t_pos, glm::vec4& t_face, glm::vec4& t_up, glm::vec4& t_right ) {
    glm::mat4 view2world = GetView2WorldMatrix();
    // tx, ty, tz
    t_pos   = view2world[3];
    // r00, r01, r02
    t_right = view2world[0];
    // r10, r11, r12
    t_up    = view2world[1];
    // r20, r21, r22
    t_face  = -view2world[2];
}