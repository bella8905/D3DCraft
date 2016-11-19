#include "glm/glm.hpp"
#include "Object.h"


CObject::CObject( GEO_TYPE t_geo ) : _geo( t_geo ), _translate( 0.f ), _rot( 1.f ), _scale( 1.f ) {

}



void CObject::resetModelMatrix() {
    glm::mat4 scaleMat = glm::mat4( glm::vec4( _scale, 0.f, 0.f, 0.f ),
                          glm::vec4( 0.f, _scale, 0.f, 0.f ),
                          glm::vec4( 0.f, 0.f, _scale, 0.f ),
                          glm::vec4( 0.f, 0.f, 0.f, 1.f ) );

    glm::mat4 translateMat = glm::mat4( glm::vec4( 1.f, 0.f, 0.f, 0.f ),
                              glm::vec4( 0.f, 1.f, 0.f, 0.f ),
                              glm::vec4( 0.f, 0.f, 1.f, 0.f ),
                              glm::vec4( _translate, 1.f ) );

    glm::mat4 rotMat = glm::mat4( glm::vec4( _rot[0], 0.f ),
                        glm::vec4( _rot[1], 0.f ),
                        glm::vec4( _rot[2], 0.f ),
                        glm::vec4( 0.f, 0.f, 0.f, 1.f ) );

    _modelMat = translateMat * rotMat * scaleMat;

    // not use glm::inverse to cal inverse matrix, that's probably expensive
    glm::mat4 inv_scaleMat = glm::mat4( glm::vec4( 1.f / _scale, 0.f, 0.f, 0.f ),
                              glm::vec4( 0.f, 1.f / _scale, 0.f, 0.f ),
                              glm::vec4( 0.f, 0.f, 1.f / _scale, 0.f ),
                              glm::vec4( 0.f, 0.f, 0.f, 1.f ) );

    glm::mat4 inv_translateMat = glm::mat4( glm::vec4( 1.f, 0.f, 0.f, 0.f ),
                                  glm::vec4( 0.f, 1.f, 0.f, 0.f ),
                                  glm::vec4( 0.f, 0.f, 1.f, 0.f ),
                                  glm::vec4( -_translate, 1.f ) );

    glm::mat4 inv_rotMat = glm::transpose( rotMat );

    _invModelMat = inv_scaleMat * inv_rotMat * inv_translateMat;

}

void CObject::SetupModelMatrix( const glm::vec3& t_translate, const glm::mat3& t_rot, const float& t_scale ) {
    _translate = t_translate;
    _rot = t_rot;
    _scale = t_scale;

    resetModelMatrix();
}

void CObject::SetupModelMatrix( const glm::vec3& t_translate, const glm::vec3& t_rot, const float& t_scale ) {

    // fill in rot matrix
    glm::mat4 rot = glm::mat4( 1.f );
    SetupModelMatrix( t_translate, rot, t_scale );
}


void CObject::Draw() {
    CGeoContainer::GetInstance().DrawGeo( _geo );
}

