#pragma  once

#include <d3d11.h>
#include "glm/fwd.hpp"


class CObject;

enum SHADER_TYPE {
    VS,
    PS,
    CS,
    GS,

    // counter
    SHADER_COUNTER,
};

class CShader {
public:
    CShader();
    virtual ~CShader() = 0;

protected:
    ID3D11Device* _device;
    ID3D11DeviceContext* _deviceContext;

public:
    void Init( ID3D11Device* t_device, ID3D11DeviceContext* t_deviceContext );
    void Deinit(); 


    virtual void Bind( const CObject* t_obj ) = 0;

protected:
    void _createShader( LPCWCHAR t_filename, LPCSTR t_entryPoint );
    ID3D10Blob* _compileShaderToBuffer( LPCWCHAR t_filename, LPCSTR t_entryPoint, LPCSTR t_target );
    void _outputErrorMessage( ID3D10Blob* t_errorMessage, LPCWCHAR _shaderFilename );

    virtual void _onInit() {}
    virtual void _onDeinit() {}
  
    virtual bool _postCreateShader() { return true;  }
    virtual bool _createFromFile( LPCWCHAR t_filename, LPCSTR t_entryPoint ) = 0;
    
  
};



class CVertexShader : public CShader {
public:
    CVertexShader() : _vertexShader( nullptr ), _layout( nullptr ) {}
    virtual ~CVertexShader() = 0;

protected:
    ID3D11VertexShader* _vertexShader;
    ID3D11InputLayout* _layout;

public:

    virtual void Bind( const CObject* t_obj );

protected:
    virtual bool _createFromFile( LPCWCHAR t_filename, LPCSTR t_entryPoint );
    virtual void _onDeint();


    virtual bool _createIALayout( ID3D10Blob* t_vertexShaderBuffer ) = 0;
    

};


class CPixelShader : public CShader {
public:
    CPixelShader() : _pixelShader( nullptr ) {}
    virtual ~CPixelShader() = 0;

protected:
    ID3D11PixelShader* _pixelShader;

public:
    virtual void Bind( const CObject* t_obj );

protected:
    virtual bool _createFromFile( LPCWCHAR t_filename, LPCSTR t_entryPoint );
    virtual void _onDeint();

};






// the actual shaders start from here

// mvp vertex shader
// a shader that includes a set of mvp matrices,
// to do a perspective transformation to vertices.
class CMVPVS : public CVertexShader {
public:
    CMVPVS() : _MVPBuffer( nullptr ) {}
    ~CMVPVS() {}

protected:
    ID3D11Buffer* _MVPBuffer;
    struct MVPBlock {
        glm::mat4 _modelMat;
        glm::mat4 _viewMat;
        glm::mat4 _projMat;

        MVPBlock() : _modelMat( 1.f ), _viewMat( 1.f ), _projMat( 1.f ) {}
    }_mvpMatrices;

public:
    virtual void Bind( const CObject* t_obj );

protected:
    virtual void _onInit();
    virtual void _onDeinit();
    virtual bool _createIALayout( ID3D10Blob* t_vertexShaderBuffer );
    virtual bool _postCreateShader();
};


// simple pixel shader 
// a simple pixel shader which passes by colors
class CSimpleColorPS : public CPixelShader {
public:
    CSimpleColorPS() {}
    ~CSimpleColorPS() {}

protected:
    virtual void _onInit();
};



void Shaders_SetAsActive( SHADER_TYPE t_type, CShader* t_shader );
void Shaders_BindShader( const CObject* t_obj );
void Shaders_Clear();

extern CMVPVS g_mvpVS;
extern CSimpleColorPS g_simpleColorPS;