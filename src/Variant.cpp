
#include "Internal.h"
#include <Lumino/Variant.h>

namespace Lumino
{
namespace GUI
{
	
//=============================================================================
// CoreObject
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
CoreObject::CoreObject()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
CoreObject::~CoreObject()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
String CoreObject::ToString()
{
	return String::GetEmpty();
}
	
//=============================================================================
// Variant
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Variant::Variant()
	: m_type(VariantType_Unknown)
	, m_uint(0)	// union 全てを初期化
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Variant::~Variant()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Variant::Variant(const Variant& obj)
	: m_type(VariantType_Unknown)
	, m_uint(0)	// union 全てを初期化
{
	Copy(obj);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Variant::Variant(CoreObject* obj)
	: m_type(VariantType_Unknown)
	, m_uint(0)	// union 全てを初期化
{
	Set(obj);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Variant::Set(CoreObject* obj)
{
	Release();
	m_type = VariantType_Object;
	LN_REFOBJ_SET(m_object, obj);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
CoreObject* Variant::GetObject()
{
	LN_VERIFY(m_type == VariantType_Object) { return NULL; }
	return m_object;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Variant::Copy(const Variant& obj)
{
	Release();
	m_type = obj.m_type;
	switch (m_type)
	{
	case Lumino::GUI::VariantType_Unknown:
		break;
	case Lumino::GUI::VariantType_Bool:
		m_bool = obj.m_bool;
		break;
	case Lumino::GUI::VariantType_Int:
		m_int = obj.m_int;
		break;
	case Lumino::GUI::VariantType_Float:
		m_float = obj.m_float;
		break;
	case Lumino::GUI::VariantType_String:
		m_string = obj.m_string;
		break;
	case Lumino::GUI::VariantType_List:
		LN_REFOBJ_SET(m_valueList, obj.m_valueList);
		break;
	case Lumino::GUI::VariantType_Object:
		LN_REFOBJ_SET(m_object, obj.m_object);
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Variant::Release()
{
	if (m_type == VariantType_List) {
		LN_SAFE_RELEASE(m_valueList);
	}
	else if (m_type == VariantType_Object) {
		LN_SAFE_RELEASE(m_object);
	}
	m_uint = 0;	// union 全てを初期化
}

} // namespace GUI
} // namespace Lumino
