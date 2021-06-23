// AMD Cauldron code

#pragma once
#include "stdafx.h"
using namespace DirectX;

class Camera
{
public:
    Camera();
    void SetMatrix(XMMATRIX cameraMatrix);
    void LookAt(XMVECTOR eyePos, XMVECTOR lookAt);
    void LookAt(float yaw, float pitch, float distance, XMVECTOR at);
    void SetFov(float fov, uint32_t width, uint32_t height, float nearPlane, float farPlane);
    void UpdateCameraPolar(float yaw, float pitch, float x, float y, float distance);
    void UpdateCameraWASD(float yaw, float pitch, const bool keyDown[256], double deltaTime);

    XMMATRIX GetView() const { return m_View; }
    XMMATRIX GetPrevView() const { return m_PrevView; }
    XMMATRIX GetViewport() const { return m_Viewport; }
    XMVECTOR GetPosition() const { return m_eyePos;   }

    XMVECTOR GetDirection() const { return XMVectorSetW(XMVector4Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMMatrixTranspose(m_View)), 0); }
    XMVECTOR GetUp() const       { return XMVectorSetW(XMVector4Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMMatrixTranspose(m_View)), 0); }
    XMVECTOR GetSide() const     { return XMVectorSetW(XMVector4Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMMatrixTranspose(m_View)), 0); }
    XMMATRIX GetProjection() const { return m_Proj; }

    float GetFovH() const { return m_fovH; }
    float GetFovV() const { return m_fovV; }

    float GetNearPlane() const { return m_near; }
    float GetFarPlane() const { return m_far; }

    float GetYaw() const { return m_yaw; }
    float GetPitch() const { return m_pitch; }
    float GetDistance() const { return m_distance; }

    void SetSpeed( float speed ) { m_speed = speed; }
    void SetProjectionJitter(float jitterX, float jitterY);
    void SetProjectionJitter(uint32_t width, uint32_t height, uint32_t &seed);
    void UpdatePreviousMatrices() { m_PrevView = m_View; }

private:
    XMMATRIX            m_View;
    XMMATRIX            m_Proj;
    XMMATRIX            m_PrevView;
    XMMATRIX            m_Viewport;
    XMVECTOR            m_eyePos;
    float               m_distance;
    float               m_fovV, m_fovH;
    float               m_near, m_far;
    float               m_aspectRatio;

    float               m_speed = 1.0f;
    float               m_yaw = 0.0f;
    float               m_pitch = 0.0f;
};

XMVECTOR PolarToVector(float roll, float pitch);
XMMATRIX LookAtRH(XMVECTOR eyePos, XMVECTOR lookAt);
XMVECTOR MoveWASD(const bool keyDown[256]);
