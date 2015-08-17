﻿
#pragma once

#include <LinearMath/btVector3.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btTransform.h>
#include <LuminoMath.h>

namespace Lumino
{
namespace Physics
{

/// Bullet とのデータ変換などのユーティリティ
class BulletUtil
{
public:

	static const btVector3 Zero;

	static Vector3 btVector3ToLNVector3(const btVector3& v)
	{
		return Vector3(v.getX(), v.getY(), v.getZ());
	}

	static btVector3 LNVector3ToBtVector3(const Vector3& v)
	{
		return btVector3(v.X, v.Y, v.Z);
	}

	static Quaternion btQuaternionToLNQuaternion(const btQuaternion& q)
	{
		return Quaternion(q.getX(), q.getY(), q.getZ(), q.getW());
	}

	static btQuaternion LNQuaternionToBtQuaternion(const Quaternion& q)
	{
		return btQuaternion(q.X, q.Y, q.Z, q.W);
	}


	static void DumpBtVector3(const btVector3& v)
	{
		printf("Vector3\n%f, %f, %f\n", v.x(), v.y(), v.z());
	}

	static void DumpBtTransform(const btTransform& t)
	{
		printf("btTransform\n%f, %f, %f\n%f, %f, %f\n%f, %f, %f\n%f, %f, %f\n",
			t.getBasis().getRow(0).x(), t.getBasis().getRow(0).y(), t.getBasis().getRow(0).z(),
			t.getBasis().getRow(1).x(), t.getBasis().getRow(1).y(), t.getBasis().getRow(1).z(),
			t.getBasis().getRow(2).x(), t.getBasis().getRow(2).y(), t.getBasis().getRow(2).z(),
			t.getOrigin().x(), t.getOrigin().y(), t.getOrigin().z());
	}
};

} // namespace Physics
} // namespace Lumino
