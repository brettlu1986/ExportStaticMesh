

#include "LSceneCamera.h"
#include "FD3D12Helper.h"

#include "LEngine.h"
#include "LDeviceWindows.h"
#include "FRenderThread.h"
#include "LLog.h"
#include "LGameTimer.h"


static const float MAX_PITCH = XMConvertToRadians(89.99f);
static const float MIN_PITCH = XMConvertToRadians(-89.99f);

LSceneCamera::LSceneCamera()
	:LCamera()
	,LastMousePoint({ 0, 0 })
	,bUpdateDirty(false)
{
	CameraType = E_CAMERA_TYPE::CAMERA_SCENE;
	for (UINT i = 0; i < KEY_SIZE; i++)
	{
		Keys[i] = false;
	}
}

LSceneCamera::~LSceneCamera()
{

}

void LSceneCamera::Init(LCameraData Data)
{
	LCamera::Init(Data);
	OnResize();
}

void LSceneCamera::OnResize()
{
	LCamera::OnResize();
}

void LSceneCamera::ProcessInput()
{
	bUpdateDirty = false; 
	
	XMFLOAT3 Zero = XMFLOAT3(0.f, 0.f, 0.f);
	MoveDirection = XMLoadFloat3(&Zero);

	if(IsKeyDown('W'))
	{
		MoveDirection += XMLoadFloat3(&LookDirection);
		bUpdateDirty = true;
	}

	if(IsKeyDown('S'))
	{
		MoveDirection += -1.f * XMLoadFloat3(&LookDirection);
		bUpdateDirty = true;
	}

	RightDirection = XMVector3Cross(XMLoadFloat3(&LookDirection), XMLoadFloat3(&UpDirection));

	if(IsKeyDown('A'))
	{
		MoveDirection += RightDirection;
		bUpdateDirty = true;
	}

	if (IsKeyDown('D'))
	{
		MoveDirection = MoveDirection + (-1 * RightDirection);
		bUpdateDirty = true;
	}

	if (IsKeyDown('E'))
	{
		MoveDirection += XMLoadFloat3(&UpDirection);
		bUpdateDirty = true;
	}

	if (IsKeyDown('Q'))
	{
		MoveDirection = MoveDirection + (-1 * XMLoadFloat3(&UpDirection));
		bUpdateDirty = true;
	}
}

void LSceneCamera::UpdateInput(float dt)
{
	if (bUpdateDirty)
	{
		float MoveSpeed = 2.f;
		XMVECTOR UnitDir = XMVector3Normalize(MoveDirection);
		XMVECTOR FocusVec = XMLoadFloat3(&FocusPosition) +  MoveSpeed * dt * UnitDir;
		FocusPosition.x = XMVectorGetX(FocusVec);
		FocusPosition.y = XMVectorGetY(FocusVec);
		FocusPosition.z = XMVectorGetZ(FocusVec);
		CalculateLocation();
	}
}

void LSceneCamera::SetActive(bool bActivate)
{
	LCamera::SetActive(bActivate);

	if(bActive)
	{
		UpdateViewProjectionRenderThread();
	}
}

void LSceneCamera::Update(float DeltaTime)
{
	if(!bActive)
		return;
	LCamera::Update(DeltaTime);
	UpdateInput(DeltaTime);
}

void LSceneCamera::ProcessCameraMouseInput(FInputResult& MouseInput)
{
	if (MouseInput.TouchType == E_TOUCH_TYPE::MOUSE_LEFT_MOVE)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(MouseInput.X - LastMousePoint.x)); //dx
		float dy = XMConvertToRadians(0.25f * static_cast<float>(MouseInput.Y - LastMousePoint.y)); //dy

		Pitch -= dy;
		Yaw += dx;

		Pitch = MathHelper::Clamp(Pitch, MIN_PITCH, MAX_PITCH);

		CalculateLocation();
		LastMousePoint.x = MouseInput.X;
		LastMousePoint.y = MouseInput.Y;
	}

	LastMousePoint.x = MouseInput.X;
	LastMousePoint.y = MouseInput.Y;
}

void LSceneCamera::ProcessCameraKeyInput(FInputResult& KeyInput)
{
	if (KeyInput.TouchType == E_TOUCH_TYPE::KEY_DOWN)
	{
		Keys[KeyInput.KeyMapType] = true;
	}
	else if (KeyInput.TouchType == E_TOUCH_TYPE::KEY_UP)
	{
		Keys[KeyInput.KeyMapType] = false;
	}
	ProcessInput();
}

bool LSceneCamera::IsKeyDown(char InKey)
{
	UINT8 Key = static_cast<UINT8>(InKey);
	return Keys[Key] == true;
}

bool LSceneCamera::IsKeyUp(char InKey)
{
	UINT8 Key = static_cast<UINT8>(InKey);
	return Keys[Key] == false;
}

void LSceneCamera::CalculateLocation()
{
	float R = cosf(Pitch);
	LookDirection.y = R * sinf(Yaw);
	LookDirection.z = sinf(Pitch);
	LookDirection.x = R * cosf(Yaw);

	RightDirection = XMVector3Cross(XMLoadFloat3(&LookDirection), XMLoadFloat3(&UpDirection));

	XMVECTOR Offset = -ArmLength * XMLoadFloat3(&LookDirection);
	XMVECTOR V = XMVectorAdd(XMLoadFloat3(&FocusPosition), Offset);

	Position.x = XMVectorGetX(V);
	Position.y = XMVectorGetY(V);
	Position.z = XMVectorGetZ(V);

	UpdateViewProjectionRenderThread();
}

XMMATRIX LSceneCamera::GetViewMarix()
{
	return XMMatrixLookAtLH(XMLoadFloat3(&Position), XMLoadFloat3(&FocusPosition), XMLoadFloat3(&UpDirection));
}


