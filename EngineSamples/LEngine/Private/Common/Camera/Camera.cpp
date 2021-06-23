
#include "Camera.h"
#include <algorithm>

Camera::Camera()
{
    m_View = XMMatrixIdentity();
    m_eyePos = XMVectorSet(0, 0, 0, 0);
    m_distance = -1;
}

//--------------------------------------------------------------------------------------
//
// OnCreate 
//
//--------------------------------------------------------------------------------------
void Camera::SetFov(float fovV, uint32_t width, uint32_t height, float nearPlane, float farPlane)
{
    m_aspectRatio = width * 1.f / height;

    m_near = nearPlane;
    m_far = farPlane;

    m_fovV = fovV;
    m_fovH = std::min<float>((m_fovV * width) / height, XM_PI / 2.0f);
    m_fovV = m_fovH * height / width;

    float halfWidth = (float)width / 2.0f;
    float halfHeight = (float)height / 2.0f;
    m_Viewport = XMMATRIX(
        halfWidth, 0.0f, 0.0f, 0.0f,
        0.0f, -halfHeight, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        halfWidth, halfHeight, 0.0f, 1.0f);
    
    if (fovV==0)
        m_Proj = XMMatrixOrthographicRH(height/40.0f, height/40.0f, nearPlane, farPlane);
    else
        m_Proj = XMMatrixPerspectiveFovRH(fovV, m_aspectRatio, nearPlane, farPlane);
}

void Camera::SetMatrix(const XMMATRIX cameraMatrix)
{
    m_eyePos = cameraMatrix.r[3];
    m_View = XMMatrixInverse(nullptr, cameraMatrix);

    XMFLOAT3 zBasis;
    XMStoreFloat3(&zBasis, cameraMatrix.r[2]);

    m_yaw = atan2f(zBasis.x, zBasis.z);
    float fLen = sqrtf(zBasis.z * zBasis.z + zBasis.x * zBasis.x);
    m_pitch = atan2f(zBasis.y, fLen);
}

//--------------------------------------------------------------------------------------
//
// LookAt, use this functions before calling update functions
//
//--------------------------------------------------------------------------------------
void Camera::LookAt(XMVECTOR eyePos, XMVECTOR lookAt)
{
    m_eyePos = eyePos;
    m_View = LookAtRH(eyePos, lookAt);
    m_distance = XMVectorGetX(XMVector3Length(lookAt - eyePos));

    XMMATRIX mInvView = XMMatrixInverse( nullptr, m_View );

    XMFLOAT3 zBasis;
    XMStoreFloat3( &zBasis, mInvView.r[2] );

    m_yaw = atan2f( zBasis.x, zBasis.z );
    float fLen = sqrtf( zBasis.z * zBasis.z + zBasis.x * zBasis.x );
    m_pitch = atan2f( zBasis.y, fLen );
}

void Camera::LookAt(float yaw, float pitch, float distance, XMVECTOR at )
{   
    LookAt(at + PolarToVector(yaw, pitch)*distance, at);
}

//--------------------------------------------------------------------------------------
//
// UpdateCamera
//
//--------------------------------------------------------------------------------------
void Camera::UpdateCameraWASD(float yaw, float pitch, const bool keyDown[256], double deltaTime)
{   
    m_eyePos += XMVector4Transform(MoveWASD(keyDown) * m_speed * (float)deltaTime, XMMatrixTranspose(m_View));
    XMVECTOR dir = PolarToVector(yaw, pitch) * m_distance;   
    LookAt(GetPosition(), GetPosition() - dir);
}

void Camera::UpdateCameraPolar(float yaw, float pitch, float x, float y, float distance)
{
    float minP = min(pitch, XM_PIDIV2 - 1e-6f);
    pitch = max(-XM_PIDIV2 + 1e-6f, minP);

    // Trucks camera, moves the camera parallel to the view plane.
    m_eyePos += GetSide() * x * distance / 10.0f;
    m_eyePos += GetUp() * y * distance / 10.0f;

    // Orbits camera, rotates a camera about the target
    XMVECTOR dir = GetDirection();
    XMVECTOR pol = PolarToVector(yaw, pitch);

    XMVECTOR at = m_eyePos - dir * m_distance;   

    LookAt(at + pol * distance, at);
}

//--------------------------------------------------------------------------------------
//
// SetProjectionJitter
//
//--------------------------------------------------------------------------------------
void Camera::SetProjectionJitter(float jitterX, float jitterY)
{
    XMFLOAT4X4 Proj;
    XMStoreFloat4x4(&Proj, m_Proj);
    Proj.m[2][0] = jitterX;
    Proj.m[2][1] = jitterY;
    m_Proj = XMLoadFloat4x4(&Proj);
}

void Camera::SetProjectionJitter(uint32_t width, uint32_t height, uint32_t &sampleIndex)
{
    static const auto CalculateHaltonNumber = [](uint32_t index, uint32_t base)
    {
        float f = 1.0f, result = 0.0f;

        for (uint32_t i = index; i > 0;)
        {
            f /= static_cast<float>(base);
            result = result + f * static_cast<float>(i % base);
            i = static_cast<uint32_t>(floorf(static_cast<float>(i) / static_cast<float>(base)));
        }

        return result;
    };

    sampleIndex = (sampleIndex + 1) % 16;   // 16x TAA

    float jitterX = 2.0f * CalculateHaltonNumber(sampleIndex + 1, 2) - 1.0f;
    float jitterY = 2.0f * CalculateHaltonNumber(sampleIndex + 1, 3) - 1.0f;

    jitterX /= static_cast<float>(width);
    jitterY /= static_cast<float>(height);

    SetProjectionJitter(jitterX, jitterY);
}

//--------------------------------------------------------------------------------------
//
// Get a vector pointing in the direction of yaw and pitch
//
//--------------------------------------------------------------------------------------
XMVECTOR PolarToVector(float yaw, float pitch)
{
    return XMVectorSet(sinf(yaw) * cosf(pitch), sinf(pitch), cosf(yaw) * cosf(pitch), 0);    
}

XMMATRIX LookAtRH(XMVECTOR eyePos, XMVECTOR lookAt)
{
    return XMMatrixLookAtRH(eyePos, lookAt, XMVectorSet(0, 1, 0, 0));
}

XMVECTOR MoveWASD(const bool keyDown[256])
{ 
    float scale = keyDown[VK_SHIFT] ? 5.0f : 1.0f;
    float x = 0, y = 0, z = 0;

    if (keyDown['W'])
    {
        z = -scale;
    }
    if (keyDown['S'])
    {
        z = scale;
    }
    if (keyDown['A'])
    {
        x = -scale;
    }
    if (keyDown['D'])
    {
        x = scale;
    }
    if (keyDown['E'])
    {
        y = scale;
    }
    if (keyDown['Q'])
    {
        y = -scale;
    }

    return XMVectorSet(x, y, z, 0.0f);
}
