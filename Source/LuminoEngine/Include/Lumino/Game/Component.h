﻿
#pragma once
#include "../Common.h"

LN_NAMESPACE_BEGIN
class WorldObject;
class Transform;

/**
	@brief		
*/
class Component
	: public Object
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:
	Transform*	transfotm;		/**< アタッチされた WorldObject の transform へのポインタ */

public:
	Component();
	virtual ~Component();
	WorldObject* GetOwnerObject() const;

	virtual void OnAttached();
	virtual void OnDetaching();
	virtual void OnUpdate();

private:
	void Attach(WorldObject* owner);
	void Detach();

	WorldObject*	m_owner;

	friend class WorldObject;
};

/**
	@brief		
*/
class Transform
	: public Component
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:
	LN_TR_PROPERTY(Vector3,		position);
	LN_TR_PROPERTY(Quaternion,	rotation);
	LN_TR_PROPERTY(Vector3,		scale);
	LN_TR_PROPERTY(Vector3,		center);

public:
	Transform();
	virtual ~Transform();

	/** 指定した方向と距離に移動します。*/
	void Translate(const Vector3& translation);
	void Translate(float x, float y, float z = 0.0f);

	Matrix GetTransformMatrix() const;

private:
};

LN_NAMESPACE_END