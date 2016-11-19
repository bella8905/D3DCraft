#pragma once

#include <d3d11.h>
#include <vector>
#include "glm/glm.hpp"

#include "Utl_Global.h"

struct SVertex {
    glm::vec3 _position;
    glm::vec3 _normal;
    glm::vec4 _color;
//     glm::vec2 _texCoord0;
//     glm::vec2 _texCoord1;

    SVertex() : _position( 0.f ), _normal( 0.f, 0.f, -1.f ), _color( 1.f, 0.f, 0.f, 1.f ) {

    }

    SVertex( glm::vec3 t_pos, glm::vec3 t_normal, glm::vec4 t_color );
};


enum GEO_TYPE {
    GEO_TRIANGLE,
    GEO_BOX,



    GEO_COUNTER,
};

class CGeo {
public:
    CGeo() : _bBufferGenerated( false ), _device( nullptr ), _deviceContext( nullptr ) {}
    virtual ~CGeo() {}


public:
    void Init( ID3D11Device* t_device, ID3D11DeviceContext* t_deviceContext );
    void Deinit();
    void Draw();


protected:
    void _clearBuffer();
    void _generateBufferFromData( std::vector<SVertex>& t_vertices, std::vector<uint>& _indices );
    virtual void _generateBuffer() = 0;

protected:
    bool _bBufferGenerated;

    ID3D11Device* _device;
    ID3D11DeviceContext* _deviceContext;

    struct VSINPUT {
        ID3D11Buffer* _vertexBuffer;
        ID3D11Buffer* _indexBuffer;
        uint _numOfIndices;

        VSINPUT() : _vertexBuffer( nullptr ), _indexBuffer( nullptr ), _numOfIndices( 0 ) {

        }
    }_vsInput;
};


class CTriangelGeo : public CGeo {
public:
    CTriangelGeo() {}
    ~CTriangelGeo() {}
protected:
    virtual void _generateBuffer();
};


class CBoxGeo : public CGeo {
public:
    CBoxGeo() {}
    ~CBoxGeo() {}
protected:
    virtual void _generateBuffer();
};



// a interface to get all possible geos we have 
class CGeoContainer {
private:
    CGeoContainer();
    CGeoContainer( const CGeoContainer& t_cont );

private:
    CGeo* _geos[GEO_COUNTER];
    bool _inited;

public:
    static CGeoContainer& GetInstance() {
        static CGeoContainer instance;
        return instance;
    }

    void Init( ID3D11Device* t_device, ID3D11DeviceContext* t_deviceContext );
    void Deinit();
    void DrawGeo( GEO_TYPE t_type ) {
        if( t_type >= 0 && t_type < GEO_COUNTER ) {
            _geos[t_type]->Draw();
        }
    }

};
