#pragma  once 

#include "Geometry.h"

#include "glm/fwd.hpp"

class CGeo;

class CObject {
public:
    CObject( GEO_TYPE t_geo = GEO_BOX );
    ~CObject() {}

private:
    // don't set the model matrix and inverse model matrix directly
    glm::vec3 _translate;
    glm::mat3 _rot;
    float _scale;   // only support uniform scaling
    glm::mat4 _modelMat;
    glm::mat4 _invModelMat;

    GEO_TYPE _geo;

protected:
    void resetModelMatrix();

public:
    void SetupModelMatrix( const glm::vec3& t_translate, const glm::vec3& t_rot, const float& t_scale );
    void SetupModelMatrix( const glm::vec3& t_translate, const glm::mat3& t_rot, const float& t_scale );
    
    void RevertModelMatrix() {
        _rot = glm::mat3( 1.f );
        _translate = glm::vec3( 0.f );
        _scale = 1.f;

        resetModelMatrix();
    }


    void SetRot( const glm::mat3& t_rot ) {
        _rot = t_rot;
        resetModelMatrix();
    }

    void Draw();

    glm::mat4 GetModelMat() const { return _modelMat;  }

};