
LN_NAMESPACE_BEGIN

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion::Quaternion()
{
	x = y = z = 0.0f;
	w = 1.0f;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion::Quaternion(float x_, float y_, float z_, float w_)
{
	x = x_;
	y = y_;
	z = z_;
    w = w_;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline void Quaternion::set(float x_, float y_, float z_, float w_)
{
	x = x_;
	y = y_;
	z = z_;
    w = w_;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline bool Quaternion::isIdentity() const
{
	return (x == 0.0f && y == 0.0f && z == 0.0f && w == 1.0f);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion& Quaternion::operator += (const Quaternion& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return (*this);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion& Quaternion::operator += (float v)
{
	x += v;
	y += v;
	z += v;
	w += v;
	return (*this);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion& Quaternion::operator -= (const Quaternion& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return (*this);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion& Quaternion::operator -= (float v)
{
	x -= v;
	y -= v;
	z -= v;
	w -= v;
	return (*this);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion& Quaternion::operator *= (const Quaternion& v)
{
	this->multiply(v);
	return (*this);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion& Quaternion::operator *= (float v)
{
	x *= v;
	y *= v;
	z *= v;
	w *= v;
	return (*this);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion& Quaternion::operator /= (float v)
{
	v = 1.0f / v;
	x *= v;
	y *= v;
	z *= v;
	w *= v;
	return (*this);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion operator + (const Quaternion& v1, const Quaternion& v2)
{
	return Quaternion(
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z,
		v1.w + v2.w);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion operator + (const Quaternion& v1, float v2)
{
	return Quaternion(
		v1.x + v2,
		v1.y + v2,
		v1.z + v2,
		v1.w + v2);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion operator + (float v1, const Quaternion& v2)
{
	return Quaternion(
		v1 + v2.x,
		v1 + v2.y,
		v1 + v2.z,
		v1 + v2.w);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion operator - (const Quaternion& v1, const Quaternion& v2)
{
	return Quaternion(
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z,
		v1.w - v2.w);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion operator - (const Quaternion& v1, float v2)
{
	return Quaternion(
		v1.x - v2,
		v1.y - v2,
		v1.z - v2,
		v1.w - v2);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion operator - (float v1, const Quaternion& v2)
{
	return Quaternion(
		v1 - v2.x,
		v1 - v2.y,
		v1 - v2.z,
		v1 - v2.w);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion operator * (const Quaternion& v1, const Quaternion& v2)
{
	return Quaternion::multiply(v1, v2);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion operator * (const Quaternion& v1, float v2)
{
	return Quaternion(
		v1.x * v2,
		v1.y * v2,
		v1.z * v2,
		v1.w * v2);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion operator * (float v1, const Quaternion& v2)
{
	return Quaternion(
		v1 * v2.x,
		v1 * v2.y,
		v1 * v2.z,
		v1 * v2.w);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion operator / (const Quaternion& v1, float v2)
{
	v2 = 1.0f / v2;
	return Quaternion(
		v1.x * v2,
		v1.y * v2,
		v1.z * v2,
		v1.w * v2);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline Quaternion operator - (const Quaternion& v1)
{
	return Quaternion(
		-v1.x,
		-v1.y,
		-v1.z,
		-v1.w);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline bool Quaternion::operator == (const Quaternion& v) const
{
	return (x == v.x && y == v.y && z == v.z && w == v.w);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline bool Quaternion::operator != (const Quaternion& v) const
{
	return (x != v.x || y != v.y || z != v.z || w != v.w);
}

LN_NAMESPACE_END
