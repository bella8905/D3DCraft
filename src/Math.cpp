#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"

#include "Utl_LogMsg.h"
#include "Math.h"


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//
// global variables
//
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
const float g_Pi = atan( 1.0f ) * 4.0f;
const float g_Pi2o = 180.0f / g_Pi;
const float g_o2Pi = g_Pi / 180.0f;


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//
// angle conversion
//
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

// radian to degree
float RadToDeg( const float& t_rad ) {
    return t_rad * g_Pi2o;
}

// degree to radian
float DegToRad( const float& t_deg ) {
    return t_deg * g_o2Pi;
}



/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//
// vectors and points 
//
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
glm::vec3 ToVec3( const glm::vec4& t_v ) {
    return glm::vec3( t_v.x, t_v.y, t_v.z );
}

glm::vec4 ToPositon( const glm::vec3& t_v ) {
    return glm::vec4( t_v, 1.f );
}


glm::vec4 ToDirection( const glm::vec3& t_v ) {
    return glm::vec4( t_v, 0.f );
}

bool SamePoints( const glm::vec3& t_p1, const glm::vec3& t_p2 ) {
    return Equals( t_p1.x, t_p2.x ) && Equals( t_p1.y, t_p2.y ) && Equals( t_p1.z, t_p2.z );
}


bool AreParallel( const glm::vec4& t_v1, const glm::vec4& t_v2 ) {
    // assert( Equals( t_v1.w, 0.f ) );
    if( Equals( t_v1.y * t_v2.z, t_v1.z * t_v2.y ) &&
        Equals( t_v1.z * t_v2.x, t_v1.x * t_v2.z ) &&
        Equals( t_v1.x * t_v2.y, t_v1.y * t_v2.x ) ) {
        return true;
    }

    return false;
}

bool ArePerpendicular( const glm::vec4& t_v1, const glm::vec4& t_v2 ) {
    return Equals( glm::dot( t_v1, t_v2 ), 0.f );
}

bool IsPosition( const glm::vec4& t_v ) {
    return Equals( t_v.w, 1.f );
}


bool IsDirection( const glm::vec4& t_v ) {
    return Equals( t_v.w, 0.f );
}




/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//
// matrix 
//
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
glm::mat4 Matrix_ToMat4( const glm::mat3& t_rot ) {
    glm::mat4 rot(	
                glm::vec4( t_rot[0], 0.f ),
                glm::vec4( t_rot[1], 0.f ),
                glm::vec4( t_rot[2], 0.f ),
                glm::vec4( 0.f, 0.f, 0.f, 1.f )
            );
    return rot;
}

glm::mat3 Matrix_ToMat3( const glm::mat4& t_mat ) {
    glm::mat3 rot = glm::mat3(	
                        glm::vec3( t_mat[0] ),
                        glm::vec3( t_mat[1] ),
                        glm::vec3( t_mat[2] )
                );
    return rot;

}

glm::mat4 Matrix_GetTransformMatrixFromTranslateRotateScale( const glm::vec3& t_Trans, const glm::vec3& t_Rotat, const glm::vec3& t_Scale ) {
    glm::mat4 translationMat = glm::translate( t_Trans );
    glm::mat4 rotationMat = glm::rotate( t_Rotat.x, glm::vec3( 1.f, 0.f, 0.f ) );
    rotationMat = rotationMat*glm::rotate( glm::mat4(), t_Rotat.y, glm::vec3( 0.f, 1.f, 0.f ) );
    rotationMat = rotationMat*glm::rotate( glm::mat4(), t_Rotat.z, glm::vec3( 0.f, 0.f, 1.f ) );
    glm::mat4 scaleMat = glm::scale( t_Scale );
    return translationMat*rotationMat*scaleMat;
}
    

glm::mat4 Matrix_GetInverseTranslationRotation( const glm::mat4& t_transform ) {
    glm::mat3 rot = Matrix_ToMat3( t_transform );
    glm::mat3 inv_rot = glm::transpose( rot );
    glm::vec3 translate;
    Matrix_GetTranslation( t_transform, translate );

    glm::mat4 result = Matrix_ToMat4( inv_rot );
    result[3][0] = -result[0][0] * translate[0] - result[1][0] * translate[1] - result[2][0] * translate[2];
    result[3][1] = -result[0][1] * translate[0] - result[1][1] * translate[1] - result[2][1] * translate[2];
    result[3][2] = -result[0][2] * translate[0] - result[1][2] * translate[1] - result[2][2] * translate[2];

    return result;
}

bool Matrix_IsOrthonormal( const glm::mat4& t_m, float t_lengthThreshold, float t_cosineThreshold ) {
    float len0 = glm::length( t_m[0] );
    float len1 = glm::length( t_m[1] );
    float len2 = glm::length( t_m[2] );
    float dot0 = glm::dot( t_m[0],  t_m[1] );
    float dot1 = glm::dot( t_m[1],  t_m[2] );
    float dot2 = glm::dot( t_m[2],  t_m[0] );
    return
        Equals( t_m[3][3], 1.f ) &&                                                            //m33 must be exactly 1.0
        Equals( t_m[0][3], 0.f ) && Equals( t_m[1][3], 0.f ) && Equals( t_m[2][3], 0.f ) &&    //m03, m13 and m23 must be exactly 0.0
        abs( len0 - 1.0 ) <= t_lengthThreshold &&                                           //the length of each rotation row must be unit.
        abs( len1 - 1.0 ) <= t_lengthThreshold &&                                           //the length of each rotation row must be unit.
        abs( len2 - 1.0 ) <= t_lengthThreshold &&                                           //the length of each rotation row must be unit.
        abs( dot0 ) <= t_cosineThreshold &&                                                 //the angle between each rotation row must be 90 degrees.
        abs( dot1 ) <= t_cosineThreshold &&                                                 //the angle between each rotation row must be 90 degrees.
        abs( dot2 ) <= t_cosineThreshold;
}



// only for affine transformation
void Matrix_GetTranslation( const glm::mat4& t_transform, glm::vec3& t_out ) {
    t_out = glm::vec3( t_transform[3][0], t_transform[3][1], t_transform[3][2] );
}

void Matrix_GetTranslation( const glm::mat4& t_transform, glm::mat4& t_out ) {
    t_out = glm::mat4();
    t_out[3] = t_transform[3];
}


void Matrix_GetRotation( const glm::mat4& t_transform, glm::mat3& t_out ) {
    glm::vec3 scale;
    Matrix_GetScale( t_transform, scale );
    glm::mat3 rotScale = Matrix_ToMat3( t_transform );
    t_out = glm::mat3( rotScale[0] / scale[0],
                    rotScale[1] / scale[1],
                    rotScale[2] / scale[2]
                    );
}

void Matrix_GetRotation( const glm::mat4& t_transform, glm::mat4& t_out ) {
    glm::mat3 rot;
    Matrix_GetRotation( t_transform, rot );
    t_out = Matrix_ToMat4( rot );
}


void Matrix_GetScale( const glm::mat4& t_transform, glm::vec3& t_out ) {
    glm::mat3 rotScale = Matrix_ToMat3( t_transform );
    t_out[0] = glm::length( rotScale[0] );
    t_out[1] = glm::length( rotScale[1] );
    t_out[2] = glm::length( rotScale[2] );
}

void Matrix_GetScale( const glm::mat4& t_transform, glm::mat4& t_out ) {
    glm::vec3 scale;
    Matrix_GetScale( t_transform, scale );
    t_out = glm::mat4( glm::vec4( scale[0], 0.f, 0.f, 0.f ),
                    glm::vec4( 0.f, scale[1], 0.f, 0.f ),
                    glm::vec4( 0.f, 0.f, scale[2], 0.f ),
                    glm::vec4( 0.f, 0.f, 0.f, 1.f )
                    );
}


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//
// Ray
//
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

SRay SRay::GetRefletedRay( const glm::vec4& t_InstPoint, const glm::vec4& t_SurfaceNormal ) const {
    glm::vec4 RflRayDir;
    glm::vec4 NormalizedNormal = glm::normalize( t_SurfaceNormal );
    if( AreParallel( _Dir, NormalizedNormal ) ) RflRayDir = NormalizedNormal;
    else if( ArePerpendicular( _Dir, NormalizedNormal ) ) RflRayDir = _Dir;
    else RflRayDir = _Dir - 2.f* NormalizedNormal* ( glm::dot( _Dir, NormalizedNormal ) );

    return SRay( t_InstPoint, RflRayDir );
}


float SRay::RayIntersectTestWithSphere( const glm::vec4& t_center, const float& t_radius ) {
    glm::vec4 posObj = _Origin;
    glm::vec4 dirObj = _Dir;

    // solve function: At2 + Bt + C = 0
    float A = dirObj.x * dirObj.x + dirObj.y * dirObj.y + dirObj.z * dirObj.z;
    float B = 2 * ( ( posObj.x - t_center.x ) * dirObj.x + ( posObj.y - t_center.y ) * dirObj.y + ( posObj.z - t_center.z ) * dirObj.z );
    float C = ( posObj.x - t_center.x ) * ( posObj.x - t_center.x ) + ( posObj.y - t_center.y ) * ( posObj.y - t_center.y ) + ( posObj.z - t_center.z ) * ( posObj.z - t_center.z ) - t_radius * t_radius;

    if( Equals( A, 0.f ) ) {
        LogError << "anything wrong with ray?" << LogEndl;
        return -1.f;
    }

    float Radicand = B * B - 4 * A * C;
    if( Radicand < 0 )  return -1.f;

    float t1 = ( -B + sqrt( Radicand ) ) * 0.5f / A;
    float t2 = ( -B - sqrt( Radicand ) ) * 0.5f / A;

    float tmax = max( t1, t2 );
    float tmin = min( t1, t2 );

    if( tmax < 0 ) return -1.f;

    float t;
    if( tmin >= 0 ) t = tmin;
    else t = tmax;

    return t;
}


float SRay::RayIntersectTestWithAxisAlignedBox( const glm::vec3& t_mins, const glm::vec3& t_maxs ) {
    glm::vec4 posObj = _Origin;
    glm::vec4 dirObj = _Dir;

    // if xd, yd, zd equals to zero, skip this direction
    float tNear = -std::numeric_limits<float>::infinity();
    float tFar = std::numeric_limits<float>::infinity();
    float t1, t2;

    if( Equals( dirObj[0], 0.f ) && Equals( dirObj[1], 0.f ) && Equals( dirObj[2], 0.f ) )   return -1.f;

    for( int i = 0; i < 3; ++i ) {
        if( Equals( dirObj[i], 0.f ) ) {
            if( posObj[i] > t_maxs[i] || posObj[i] < t_mins[i] )  return -1.f;
        } else {
            t1 = ( t_mins[i] - posObj[i] ) / dirObj[i];
            t2 = ( t_maxs[i] - posObj[i] ) / dirObj[i];
            tNear = max( tNear, min( t1, t2 ) );
            tFar = min( tFar, max( t1, t2 ) );

            if( tNear > tFar || tFar < 0 )  return -1.f;
        }
    }

    return tNear;
}


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//
// misc
//
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

bool IsPOT( int t_val ) {
    return ( t_val  & ( t_val -1 ) ) == 0 ;
}

