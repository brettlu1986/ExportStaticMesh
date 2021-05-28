

#include "stdafx.h"
#include "MathHelper.h"



const float MathHelper::Infinity = FLT_MAX;
const float MathHelper::Pi = 3.1415926535f;

float MathHelper::AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	// Quadrant I or IV
	if (x >= 0.0f)
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f * Pi; // in [0, 2*pi).
	}

	// Quadrant II or III
	else
		theta = atanf(y / x) + Pi; // in [0, 2*pi).

	return theta;
}

XMVECTOR MathHelper::RandUnitVec3()
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		return XMVector3Normalize(v);
	}
}

XMVECTOR MathHelper::RandHemisphereUnitVec3(XMVECTOR n)
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), MathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		// Ignore points in the bottom hemisphere.
		if (XMVector3Less(XMVector3Dot(n, v), Zero))
			continue;

		return XMVector3Normalize(v);
	}
}

//XMFLOAT4 Quat:x, y, z, w  XMFLOAT3 Rotate:Pitch, Yaw, Roll
DirectX::XMFLOAT4 MathHelper::EulerToQuaternion(DirectX::XMFLOAT3 CurrentRotation)
{
	DirectX::XMFLOAT4 q;
	float yaw = CurrentRotation.y * Pi / 180;
	float roll = CurrentRotation.z * Pi / 180;
	float pitch = CurrentRotation.x * Pi / 180;

	float cy = cos(yaw * 0.5f);
	float sy = sin(yaw * 0.5f);
	float cr = cos(roll * 0.5f);
	float sr = sin(roll * 0.5f);
	float cp = cos(pitch * 0.5f);
	float sp = sin(pitch * 0.5f);

	q.w = cy * cr * cp + sy * sr * sp;
	q.x = cy * sr * cp - sy * cr * sp;
	q.y = cy * cr * sp + sy * sr * cp;
	q.z = sy * cr * cp - cy * sr * sp;

	return q;
}

DirectX::XMFLOAT3 MathHelper::QuaternionToEuler(DirectX::XMFLOAT4 Quat)
{
	float ysqr = Quat.y * Quat.y;

	// roll (x-axis rotation)
	float t0 = 2.0f * (Quat.w * Quat.x + Quat.y * Quat.z);
	float t1 = 1.0f - 2.0f * (Quat.x * Quat.x + ysqr);
	float roll = atanf(t0 / t1);

	// pitch (y-axis rotation)
	float t2 = 2.0f * (Quat.w * Quat.y - Quat.z * Quat.x);
	t2 = ((t2 > 1.0f) ? 1.0f : t2);
	t2 = ((t2 < -1.0f) ? -1.0f : t2);
	float pitch = asinf(t2);

	// yaw (z-axis rotation)
	float t3 = 2.0f * (Quat.w * Quat.z + Quat.x * Quat.y);
	float t4 = 1.0f - 2.0f * (ysqr + Quat.z * Quat.z);
	float yaw = atanf(t3 / t4);

	return XMFLOAT3(pitch / Pi * 180.f, yaw / Pi * 180.f, roll / Pi * 180.f);
}