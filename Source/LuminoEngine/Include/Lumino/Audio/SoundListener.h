﻿
#pragma once
#include "Common.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_AUDIO_BEGIN

/**
	@brief		3D サウンドのリスナーのクラスです。
*/
class SoundListener
{
public:
	
	/**
		@brief		3D音声のリスナーの位置を設定します。
		@param[in]	position	: 3D 空間上の座標
	*/
	static void setPosition(const Vector3& position);

	/**
		@brief		3D音声のリスナーの位置を設定します。
		@param[in]	x			: 3D 空間上の X 座標
		@param[in]	y			: 3D 空間上の Y 座標
		@param[in]	z			: 3D 空間上の Z 座標
	*/
	static void setPosition(float x, float y, float z);
	
	/**
		@brief		3D音声のリスナーの位置を取得します。
	*/
	static const Vector3& getPosition();

	/**
		@brief		3D音声のリスナーの正面方向を設定します。
		@param[in]	direction		: 向き
	*/
	static void setDirection(const Vector3& direction);

	/**
		@brief		3D音声のリスナーの正面方向を設定します。
		@param[in]	x			: 向きの X 成分
		@param[in]	y			: 向きの Y 成分
		@param[in]	z			: 向きの Z 成分
	*/
	static void setDirection(float x, float y, float z);

	/**
		@brief		3D音声のリスナーの正面方向を取得します。
	*/
	static const Vector3& getDirection();

	/**
		@brief		3D音声のリスナーの上方向を設定します。(正面方向とは直交であること)
		@param[in]	direction	: 上方向
	*/
	static void setUpDirection(const Vector3& direction);

	/**
		@brief		3D音声のリスナーの上方向を設定します。(正面方向とは直交であること)
		@param[in]	x			: 向きの X 成分
		@param[in]	y			: 向きの Y 成分
		@param[in]	z			: 向きの Z 成分
	*/
	static void setUpDirection(float x, float y, float z);
	
	/**
		@brief		3D音声のリスナーの上方向を取得します。
	*/
	static const Vector3& getUpDirection(const Vector3& direction);

	/**
		@brief		3D音声のリスナーの速度を設定します。
		@param[in]	velocity	: 速度
	*/
	static void setVelocity(const Vector3& velocity);

	/**
		@brief		3D音声のリスナーの速度を設定します。
		@param[in]	x			: 速度の X 成分
		@param[in]	y			: 速度の Y 成分
		@param[in]	z			: 速度の Z 成分
	*/
	static void setVelocity(float x, float y, float z);

	/**
		@brief		3D音声のリスナーの速度を設定します。
		@param[in]	velocity	: 速度
	*/
	static const Vector3& getVelocity();

};

LN_NAMESPACE_AUDIO_END
LN_NAMESPACE_END
