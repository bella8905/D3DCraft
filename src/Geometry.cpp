#include <glm/glm.hpp>

#include "Utl_Global.h"
#include "Utl_LogMsg.h"
#include "D3DSystem.h"
#include "Geometry.h"


SVertex::SVertex( glm::vec3 t_pos, glm::vec3 t_normal, glm::vec4 t_color ) : _position( t_pos ), _normal( t_normal ), _color( t_color ) {
    
}


void CGeo::Init( ID3D11Device* t_device, ID3D11DeviceContext* t_deviceContext ) {
    _device = t_device;
    _deviceContext = t_deviceContext;

    _generateBuffer();
}


void CGeo::Deinit() {
    _clearBuffer();
}

void CGeo::Draw() {
    uint stride;
    uint offset;


    // Set vertex buffer stride and offset.
    stride = sizeof( SVertex );
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    _deviceContext->IASetVertexBuffers( 0, 1, &_vsInput._vertexBuffer, &stride, &offset );

    // Set the index buffer to active in the input assembler so it can be rendered.
    _deviceContext->IASetIndexBuffer( _vsInput._indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    _deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // issue draw call 
    _deviceContext->DrawIndexed( _vsInput._numOfIndices, 0, 0 );
}

void CGeo::_generateBufferFromData( std::vector<SVertex>& t_vertices, std::vector<uint>& t_indices ) {
    // Set up the description of the static vertex buffer.
    ID3D11Buffer* vertexBuffer;
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof( SVertex ) * t_vertices.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = &t_vertices[0];
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now create the vertex buffer.
    HRESULT result = _device->CreateBuffer( &vertexBufferDesc, &vertexData, &vertexBuffer );
    if( FAILED( result ) ) {
        return;
    }

    _vsInput._vertexBuffer = vertexBuffer;


    ID3D11Buffer* indexBuffer;
    // Set up the description of the static index buffer.
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof( uint ) * t_indices.size();
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = &t_indices[0];
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = _device->CreateBuffer( &indexBufferDesc, &indexData, &indexBuffer );
    if( FAILED( result ) ) {
        return;
    }

    _vsInput._indexBuffer = indexBuffer;
    _vsInput._numOfIndices = t_indices.size();

}


void CGeo::_clearBuffer() {
    ReleaseCOM( _vsInput._indexBuffer );
    ReleaseCOM( _vsInput._indexBuffer );
}



void CTriangelGeo::_generateBuffer() {
    // vertices
    std::vector<SVertex> vertices;
    std::vector<uint> indices;

    glm::vec3 positions[] = {
        { 0.0f, 0.5f, 0.5f },
        { -0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, 0.5f },
    };

    glm::vec3 normal = { 0.f, 0.f, 1.f };
    glm::vec4 color = { 1.f, 0.f, 0.f, 1.f };

    uint numberOfVertices = array_size_of( positions );
    for( uint i = 0; i < numberOfVertices; ++i ) {
        SVertex vertex( positions[i], normal, color );
        vertices.push_back( vertex );
        indices.push_back( i );
    }

    _generateBufferFromData( vertices, indices );
}


void CBoxGeo::_generateBuffer() {
    // cube ///////////////////////////////////////////////////////////////////////
    //    v6----- v5
    //   /|      /|
    //  v1------v0|
    //  | |     | |
    //  | |v7---|-|v4
    //  |/      |/
    //  v2------v3

    std::vector<SVertex> vertices;
    std::vector<uint> indices;

    float cubeVertices[] = {
        0.5 , 0.5, -0.5, -0.5, 0.5 , -0.5, -0.5, -0.5, -0.5, 0.5 , -0.5, -0.5,         // v0-v1-v2-v3
        0.5 , 0.5, -0.5, 0.5 , -0.5, -0.5, 0.5 , -0.5, 0.5 , 0.5 , 0.5 , 0.5 ,         // v0-v3-v4-v5
        0.5 , 0.5, -0.5, 0.5 , 0.5 , 0.5 , -0.5, 0.5 , 0.5 , -0.5, 0.5 , -0.5,         // v0-v5-v6-v1
        -0.5, 0.5, -0.5, -0.5, 0.5 , 0.5 , -0.5, -0.5, 0.5 , -0.5, -0.5, -0.5,		   // v1-v6-v7-v2
        -0.5, -0.5, 0.5, 0.5 , -0.5, 0.5 , 0.5 , -0.5, -0.5, -0.5, -0.5, -0.5,		   // v7-v4-v3-v2
        0.5 , -0.5, 0.5, -0.5, -0.5, 0.5 , -0.5, 0.5 , 0.5 , 0.5 , 0.5 , 0.5           // v4-v7-v6-v5
    };

    float cubeNormals[] = {
        0 , 0 , -1, 0 , 0 , -1, 0 , 0 , -1, 0 , 0 , -1,             // v0-v1-v2-v3
        1 , 0 , 0 , 1 , 0 , 0 , 1 , 0 , 0 , 1 , 0 , 0 ,             // v0-v3-v4-v5
        0 , 1 , 0 , 0 , 1 , 0 , 0 , 1 , 0 , 0 , 1 , 0 ,             // v0-v5-v6-v1
        -1, 0 , 0 , -1, 0 , 0 , -1, 0 , 0 , -1, 0 , 0 ,             // v1-v6-v7-v2
        0 , -1, 0 , 0 , -1, 0 , 0 , -1, 0 , 0 , -1, 0 ,             // v7-v4-v3-v2
        0 , 0 , 1 , 0 , 0 , 1 , 0 , 0 , 1 , 0 , 0 , 1               // v4-v7-v6-v5
    };



    glm::vec4 colors[] = {
        { 1.f, 0.f, 0.f, 1.f },
        { 0.f, 1.f, 0.f, 1.f },
        { 0.f, 0.f, 1.f, 1.f },
        { 1.f, 1.f, 0.f, 1.f },
        { 1.f, 0.f, 1.f, 1.f },
        { 0.f, 1.f, 1.f, 1.f },

    };


    uint numOfVertices = sizeof( cubeVertices ) / sizeof( float ) / 3;
    for( uint i = 0; i < numOfVertices; ++i ) {
        int startIndex = 3 * i;
        int colorIdx = startIndex / 12;
        vertices.push_back( SVertex(   glm::vec3( cubeVertices[startIndex], cubeVertices[startIndex + 1], cubeVertices[startIndex + 2] ),
                                        glm::vec3( cubeNormals[startIndex], cubeNormals[startIndex + 1], cubeNormals[startIndex + 2] ),
                                        colors[colorIdx] ) );
    }

    uint cubeIndices[] = {
        0 , 1 , 2 , 0 , 2 , 3 ,         // front
        4 , 5 , 6 , 4 , 6 , 7 ,         // right
        8 , 9 , 10, 8 , 10, 11,         // top
        12, 13, 14, 12, 14, 15,         // left
        16, 17, 18, 16, 18, 19,         // bottom
        20, 21, 22, 20, 22, 23,         // back
    };

    for( uint i = 0, numOfIndices = array_size_of( cubeIndices ); i < numOfIndices; ++i ) {
        indices.push_back( cubeIndices[i] );
    }

    _generateBufferFromData( vertices, indices );
}




/////////////////////////////////////////////////////////////////
//
// Geo Container
//  
/////////////////////////////////////////////////////////////////
CGeoContainer::CGeoContainer() : _inited( false ) {
    for( us i = 0; i < GEO_COUNTER; ++i ) {
        _geos[i] = nullptr;
    }
}

void CGeoContainer::Init( ID3D11Device* t_device, ID3D11DeviceContext* t_deviceContext ) {
    if( _inited ) {
        LogError << "geo container already inited" << LogEndl;
        return;
    }


    _geos[GEO_TRIANGLE] = new CTriangelGeo();
    _geos[GEO_BOX] = new CBoxGeo();

    for( uint i = 0; i < GEO_COUNTER; ++i ) {
        _geos[i]->Init( t_device, t_deviceContext );
    }

    _inited = true;

}

void CGeoContainer::Deinit() {
    if( !_inited ) {
        LogError << "geo container not inited" << LogEndl;
        return;
    }

    for( us i = 0; i < GEO_COUNTER; ++i ) {
        if( _geos[i] ) {
            _geos[i]->Deinit();
            delete _geos[i];
            _geos[i] = 0;
        }
    }

    _inited = false;
}
