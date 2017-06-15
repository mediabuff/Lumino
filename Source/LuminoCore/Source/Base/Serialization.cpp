﻿/*


MyObject*			m_obj1;
RefPtr<MyObject>*	m_obj2;
MyObject			m_obj3;

ar & NewObjectNVP("obj1", m_obj1);	// 良くない
ar & MakeNVP("obj1", *m_obj1);		// OK (あらかじめ new)
ar & NewObjectNVP("obj2", m_obj2);	// OK	→ MakeMVP で RefPtr<> のみ特殊扱い、で。そうすると List のシリアライズが楽になる
ar & MakeNVP("obj", m_obj3);		// OK


List<MyObject*>			m_list1;	// だめ
List<RefPtr<MyObject>>	m_list2;

ar & MakeNVP("list2", m_list2);

リストのシリアライズについて・・・
non‐intrusive Object のシリアライズ扱いにする。
コレクションは push_back を実装していること。とか。




*/
#include "../Internal.h"
#include <Lumino/Reflection/TypeInfo.h>
#include <Lumino/Base/Serialization.h>
//#include <Lumino/IO/FileStream.h>

LN_NAMESPACE_BEGIN
namespace tr {

//==============================================================================
// ScVariantCore
//==============================================================================

//------------------------------------------------------------------------------
ScVariantCore::~ScVariantCore()
{
	releaseValue();
}

//------------------------------------------------------------------------------
void ScVariantCore::setInt(int value)
{
	resetType(ScVariantType::Int);
	m_int = value;
}

//------------------------------------------------------------------------------
void ScVariantCore::setString(const StringRef& value)
{
	if (m_type != ScVariantType::String)
	{
		resetType(ScVariantType::String);
		m_string = LN_NEW String(value);
	}
	else
	{
		*m_string = value;
	}
}

//------------------------------------------------------------------------------
void ScVariantCore::setList()
{
	resetType(ScVariantType::List);
	m_list = LN_NEW ln::List<ScVariant>();
}

//------------------------------------------------------------------------------
void ScVariantCore::setMap()
{
	resetType(ScVariantType::Map);
	m_map = LN_NEW std::unordered_map<ln::String, ScVariant>();
}

////------------------------------------------------------------------------------
//void ScVariantCore::serialize(Archive& ar, int version)
//{
//	if (ar.IsSaving())
//	{
//
//	}
//	else
//	{
//
//	}
//}

//------------------------------------------------------------------------------
void ScVariantCore::resetType(ScVariantType type)
{
	releaseValue();
	m_type = type;
}

//------------------------------------------------------------------------------
void ScVariantCore::releaseValue()
{
	switch (m_type)
	{
	case ScVariantType::Unknown:
		break;
	case ScVariantType::Bool:
		break;
	case ScVariantType::Int:
		break;
	case ScVariantType::Float:
		break;
	case ScVariantType::String:
		LN_SAFE_DELETE(m_string);
		break;
	case ScVariantType::List:
		LN_SAFE_DELETE(m_list);
		break;
	case ScVariantType::Map:
		LN_SAFE_DELETE(m_map);
		break;
	default:
		LN_UNREACHABLE();
		break;
	}
	m_type = ScVariantType::Unknown;
}

//==============================================================================
// ScVariant
//==============================================================================

//------------------------------------------------------------------------------
ScVariantType ScVariant::getType() const
{
	if (m_core == nullptr) return ScVariantType::Unknown;
	return m_core->m_type;
}

//------------------------------------------------------------------------------
void ScVariant::setInt(int value)
{
	if (m_core == nullptr) m_core = newObject<ScVariantCore>();
	m_core->setInt(value);
}

//------------------------------------------------------------------------------
int ScVariant::getInt() const
{
	if (LN_CHECK_STATE(m_core->m_type == ScVariantType::Int)) return 0;
	return m_core->m_int;
}

//------------------------------------------------------------------------------
void ScVariant::setString(const StringRef& value)
{
	if (m_core == nullptr) m_core = newObject<ScVariantCore>();
	m_core->setString(value);
}

//------------------------------------------------------------------------------
const String& ScVariant::getString() const
{
	if (LN_CHECK_STATE(m_core->m_type == ScVariantType::String)) return String::getEmpty();
	return *m_core->m_string;
}

//------------------------------------------------------------------------------
void ScVariant::saveInternal(ISerializeElement* value)
{

}

//------------------------------------------------------------------------------
void ScVariant::loadInternal(ISerializeElement* value)
{
	switch (value->getSerializationElementType())
	{
	case SerializationElementType::Value:
	{
		SerializationValueType type = value->getSerializationValueType();
		if (type == SerializationValueType::Null)
		{
			m_core = nullptr;
		}
		else
		{
			if (m_core == nullptr)
			{
				m_core = newObject<ScVariantCore>();
			}
			switch (type)
			{
			case SerializationValueType::Bool:
				m_core->resetType(ScVariantType::Bool);
				m_core->m_bool = value->getSerializeValueBool();
				break;
			case SerializationValueType::Int8:
				m_core->resetType(ScVariantType::Int);
				m_core->m_int = value->getSerializeValueInt8();
				break;
			case SerializationValueType::Int16:
				m_core->resetType(ScVariantType::Int);
				m_core->m_int = value->getSerializeValueInt16();
				break;
			case SerializationValueType::Int32:
				m_core->resetType(ScVariantType::Int);
				m_core->m_int = value->getSerializeValueInt32();
				break;
			case SerializationValueType::Int64:
				m_core->resetType(ScVariantType::Int);
				m_core->m_int = value->getSerializeValueInt64();
				break;
			case SerializationValueType::UInt8:
				m_core->resetType(ScVariantType::Int);
				m_core->m_int = value->getSerializeValueUInt8();
				break;
			case SerializationValueType::UInt16:
				m_core->resetType(ScVariantType::Int);
				m_core->m_int = value->getSerializeValueUInt16();
				break;
			case SerializationValueType::UInt32:
				m_core->resetType(ScVariantType::Int);
				m_core->m_int = value->getSerializeValueUInt32();
				break;
			case SerializationValueType::UInt64:
				m_core->resetType(ScVariantType::Int);
				m_core->m_int = value->getSerializeValueUInt64();
				break;
			case SerializationValueType::Float:
				m_core->resetType(ScVariantType::Float);
				m_core->m_float = value->getSerializeValueFloat();
				break;
			case SerializationValueType::Double:
				m_core->resetType(ScVariantType::Float);
				m_core->m_float = value->getSerializeValueDouble();
				break;
			case SerializationValueType::String:
				m_core->setString(value->getSerializeValueString());
				break;
			default:
				LN_UNREACHABLE();
				break;
			}
		}
		break;
	}
	case SerializationElementType::Array:
	{
		if (m_core == nullptr)
		{
			m_core = newObject<ScVariantCore>();
		}
		m_core->setList();
		int count = value->getSerializeElementCount();
		for (int i = 0; i < count; i++)
		{
			ScVariant v;
			v.loadInternal(value->getSerializeElement(i));
			m_core->m_list->add(v);
		}
		break;
	}
	case SerializationElementType::Object:
	{
		if (m_core == nullptr)
		{
			m_core = newObject<ScVariantCore>();
		}
		m_core->setMap();
		int count = value->getSerializeElementCount();
		for (int i = 0; i < count; i++)
		{
			ScVariant v;
			v.loadInternal(value->getSerializeElement(i));
			(*m_core->m_map)[value->getSerializeElementName(i)] = v;
		}
		break;
	}
	default:
		LN_UNREACHABLE();
		break;
	}
}



//==============================================================================
// Archive
//==============================================================================
const TCHAR* Archive::ClassNameKey = _T("_ln_class_name");
const TCHAR* Archive::ClassVersionKey = _T("_ln_class_version");
const TCHAR* Archive::ClassBaseDefaultNameKey = _T("_ln_class_base");

//------------------------------------------------------------------------------
RefPtr<ReflectionObject> Archive::createObject(const String& className, TypeInfo* requestedType)
{
	if (className == requestedType->getName())
	{
		// 格納したい変数の型と同じなら型情報を総検索する必要は無い
		return requestedType->createInstance();
	}
	else
	{
		auto* typeInfo = TypeInfo::findTypeInfo(className);
		return typeInfo->createInstance();
	}
}

////------------------------------------------------------------------------------
//Archive::Archive(const PathName& filePath, ArchiveMode mode)
//	: m_mode(mode)
//{
//	FileStreamPtr file;
//	switch (mode)
//	{
//	case ArchiveMode::Save:
//		file = FileStream::create(filePath, FileOpenMode::Write | FileOpenMode::Truncate);
//		break;
//	case ArchiveMode::Load:
//		file = FileStream::create(filePath, FileOpenMode::Read);
//		break;
//	default:
//		assert(0);
//		break;
//	}
//}

} // namespace tr
LN_NAMESPACE_END
