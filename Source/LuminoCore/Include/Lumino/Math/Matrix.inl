
LN_NAMESPACE_BEGIN

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline bool Matrix::isIdentity() const
{
	return (memcmp(this, &Matrix::Identity, sizeof(Matrix)) == 0);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline const Vector3& Matrix::getRight() const
{ 
	return (Vector3&)m[0][0];
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline const Vector3& Matrix::getUp() const
{
	return (Vector3&)m[1][0];
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline const Vector3& Matrix::getFront() const
{
	return (Vector3&)m[2][0];
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline const Vector3& Matrix::getPosition() const
{
	return (Vector3&)m[3][0];
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline void Matrix::setRow(int index, const Vector4& row)
{
	((Vector4&)m[index][0]) = row;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline const Vector4& Matrix::getRow(int index) const
{
	return (Vector4&)m[index][0];
}

LN_NAMESPACE_END
