#include <d3dcompiler.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Utl_LogMsg.h"
#include "Math.h"
#include "D3DSystem.h"
#include "Object.h"
#include "View.h"

#include "Shader.h"


CMVPVS g_mvpVS;
CSimpleColorPS g_simpleColorPS;


static struct SHADERS {
    CShader* _shaders[SHADER_COUNTER];

    // ...

    SHADERS()  {
        for( int i = 0; i < SHADER_COUNTER; ++i ) {
            _shaders[i] = nullptr;
        }
    }

    void SetShader( SHADER_TYPE t_type, CShader* t_shader ) {
        if( t_shader ) {
            bool bInvalid = false;
            switch( t_type ) {
                case VS:
                {
                    CVertexShader* vs = dynamic_cast< CVertexShader* >( t_shader );
                    if( !vs ) {
                        bInvalid = true;
                    }
                } break;
                case PS:
                {
                    CPixelShader* ps = dynamic_cast< CPixelShader* >( t_shader );
                    if( !ps ) {
                        bInvalid = true;
                    }
                } break;
                default:
                    bInvalid = true;
                    break;
            }

            if( bInvalid ) {
                LogError << "shader can't be set" << LogEndl;
                return;
            }
        }

        _shaders[t_type] = t_shader;
    }

    void BindShader( const CObject* t_object ) {
        for( int i = 0; i < SHADER_COUNTER; ++i ) {
            if( _shaders[i] ) {
                _shaders[i]->Bind( t_object );
            }
        }
    }

} ActiveShaders;

void Shaders_SetAsActive( SHADER_TYPE t_type, CShader* t_shader ) {
    ActiveShaders.SetShader( t_type, t_shader );
}

void Shaders_BindShader( const CObject* t_obj ) {
    ActiveShaders.BindShader( t_obj );
}

void Shaders_Clear() {
    for( int i = 0; i < SHADER_COUNTER; ++i ) {
        ActiveShaders.SetShader( ( SHADER_TYPE )i, nullptr );
    }
}


CShader::CShader() : _device( nullptr ), _deviceContext( nullptr ) {

}

CShader::~CShader() {

}

void CShader::Init( ID3D11Device* t_device, ID3D11DeviceContext* t_deviceContext ) {
    _device = t_device;
    _deviceContext = t_deviceContext;

    _onInit();
}

void CShader::Deinit() {
    _onDeinit();
    _device = nullptr;
    _deviceContext = nullptr;
}

void CShader::_outputErrorMessage( ID3D10Blob* t_errorMessage, LPCWCHAR _shaderFilename ) {
    if( !t_errorMessage ) return;

    char* compileErrors;

    // Get a pointer to the error message text buffer.
    compileErrors = ( char* )( t_errorMessage->GetBufferPointer() );
    LogError << "Shader \' "<< _shaderFilename << "\' Compile Failed" << LogEndl;
    LogError << compileErrors << LogEndl;

    ReleaseCOM( t_errorMessage );


}

ID3D10Blob* CShader::_compileShaderToBuffer( LPCWCHAR t_filename, LPCSTR t_entryPoint, LPCSTR t_target ) {
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;
    ID3D10Blob* shaderBuffer = nullptr;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    // Compile the vertex shader code.
    result = D3DCompileFromFile( t_filename, NULL, NULL, t_entryPoint, t_target, dwShaderFlags, 0,
                                 &shaderBuffer, &errorMessage );
    if( FAILED( result ) ) {
        // If the shader failed to compile it should have writen something to the error message.
        if( errorMessage ) {
            _outputErrorMessage( errorMessage, t_filename );
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else {
            LogError << "Can't open vertex shader \'" << t_filename << "\'." << LogEndl;
        }
    }
   
    return shaderBuffer;
}

void CShader::_createShader( LPCWCHAR t_filename, LPCSTR t_entryPoint ) {
    if( !_createFromFile( t_filename, t_entryPoint ) ) {
        return;
    }

    _postCreateShader();
}


CVertexShader::~CVertexShader() {}

void CVertexShader::_onDeint() {
    ReleaseCOM( _vertexShader );
}


bool CVertexShader::_createFromFile( LPCWCHAR t_filename, LPCSTR t_entryPoint ) {
    HRESULT result;
    ID3D10Blob* vertexShaderBuffer = _compileShaderToBuffer( t_filename, t_entryPoint, "vs_5_0" );

    if( !vertexShaderBuffer ) return false;

    // Create the vertex shader from the buffer.
    result = _device->CreateVertexShader( vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &_vertexShader );
    if( FAILED( result ) ) {
        return false;
    }

    result = _createIALayout( vertexShaderBuffer );
    
    ReleaseCOM( vertexShaderBuffer );

    if( FAILED( result ) ) {
        return false;
    }
    return true;

}

void CVertexShader::Bind( const CObject* t_obj ) {
    _deviceContext->IASetInputLayout( _layout );

    // Set the vertex and pixel shaders that will be used to render this triangle.
    _deviceContext->VSSetShader( _vertexShader, NULL, 0 );
}



CPixelShader::~CPixelShader() {}

void CPixelShader::_onDeint() {
    ReleaseCOM( _pixelShader );
}

bool CPixelShader::_createFromFile( LPCWCHAR t_filename, LPCSTR t_entryPoint ) {
    HRESULT result;
    ID3D10Blob* pixelShaderBuffer = _compileShaderToBuffer( t_filename, t_entryPoint, "ps_5_0" );

    if( !pixelShaderBuffer ) return false;

    // Create the vertex shader from the buffer.
    result = _device->CreatePixelShader( pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &_pixelShader );
    if( FAILED( result ) ) {
        return false;
    }


    ReleaseCOM( pixelShaderBuffer );

    return true;
}

void CPixelShader::Bind( const CObject* t_obj ) {
    _deviceContext->PSSetShader( _pixelShader, NULL, 0 );
}






void CMVPVS::_onInit() {
    _createShader( L"../common/shaders/mvp.vs", "ColorVertexShader" );
}

void CMVPVS::_onDeinit() {
    CVertexShader::_onDeint();
    ReleaseCOM( _MVPBuffer );
}

bool CMVPVS::_createIALayout( ID3D10Blob* t_vertexShaderBuffer ) {
    if( !t_vertexShaderBuffer ) return false;

    D3D11_INPUT_ELEMENT_DESC polygonLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA , 0 },
        { "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA , 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT , 0, 24, D3D11_INPUT_PER_VERTEX_DATA , 0 },
    };


    // Get a count of the elements in the layout.
    uint numElements = sizeof( polygonLayout ) / sizeof( polygonLayout[0] );

    // Create the vertex input layout.
    HRESULT result = _device->CreateInputLayout( polygonLayout, numElements, t_vertexShaderBuffer->GetBufferPointer(),
                                         t_vertexShaderBuffer->GetBufferSize(), &_layout );
    if( FAILED( result ) ) {
        return false;
    }

    return true;

}


bool CMVPVS::_postCreateShader() {
    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof( MVPBlock );
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    HRESULT result = _device->CreateBuffer( &matrixBufferDesc, NULL, &_MVPBuffer );
    if( FAILED( result ) ) {
        return false;
    }

    return true;

}

void CMVPVS::Bind( const CObject* t_obj ) {
    // set shader parameter
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    // Lock the constant buffer so it can be written to.
    result = _deviceContext->Map( _MVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
    if( FAILED( result ) ) {
        return;
    }

    // Get a pointer to the data in the constant buffer.
    MVPBlock* dataPtr = ( MVPBlock* )mappedResource.pData;

    // Copy the matrices into the constant buffer.
    dataPtr->_modelMat = t_obj ? t_obj->GetModelMat() : glm::mat4( 1.f );
    dataPtr->_viewMat = View_GetActive()->GetWorld2ViewMatrix();
    dataPtr->_projMat = View_GetActive()->GetView2ProjMatrix();

    // Unlock the constant buffer.
    _deviceContext->Unmap( _MVPBuffer, 0 );

    // Set the position of the constant buffer in the vertex shader.
    unsigned int bufferNumber = 0;

    // Finanly set the constant buffer in the vertex shader with the updated values.
    _deviceContext->VSSetConstantBuffers( bufferNumber, 1, &_MVPBuffer );


    CVertexShader::Bind( t_obj );

}



void CSimpleColorPS::_onInit() {
    _createShader( L"../common/shaders/single_color.ps", "ColorPixelShader" );

}

