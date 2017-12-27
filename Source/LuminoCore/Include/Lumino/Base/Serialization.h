
#pragma once
#include <unordered_map>
#include "String.h"
#include "../Reflection/ReflectionObject.h"

#define LN_SC_SHARED_LIST

#ifdef LN_SC_SHARED_LIST
#include "SharedList.h"
#endif

LN_NAMESPACE_BEGIN
//template <class T> class Ref;

enum class SerializeClassFormat
{
	Default,
	String,
};

namespace detail
{

struct SerializeClassMetadata
{
	int						version = 0;
	SerializeClassFormat	format = SerializeClassFormat::Default;
};

struct SerializeClassMetadataMap
{
	std::unordered_map<std::size_t, SerializeClassMetadata> m_map;

	static SerializeClassMetadataMap & getInstance()
	{
		static SerializeClassMetadataMap t;
		return t;
	}

	void addVersion(std::size_t typeHash, std::uint32_t version)
	{
		SerializeClassMetadata& data = m_map[typeHash];
		data.version = version;
	}

	void addFormat(std::size_t typeHash, SerializeClassFormat format)
	{
		SerializeClassMetadata& data = m_map[typeHash];
		data.format = format;
	}

	bool getVersion(std::size_t typeHash, int* outVersion)
	{
		auto itr = m_map.find(typeHash);
		if (itr == m_map.end()) return false;
		*outVersion = itr->second.version;
		return true;
	}

	bool getFormat(std::size_t typeHash, SerializeClassFormat* outFormat)
	{
		auto itr = m_map.find(typeHash);
		if (itr == m_map.end()) return false;
		*outFormat = itr->second.format;
		return true;
	}
};

template <class T> struct SerializeClassVersionInfo
{
	static const int value = 0;
};

template <class T> struct SerializeClassFormatInfo
{
	static const SerializeClassFormat value = SerializeClassFormat::Default;
};

} // namespace detail

#define LN_SERIALIZE_CLASS_VERSION(type, version) \
	namespace ln { namespace detail { \
		template<> class SerializeClassVersionInfo<type> \
		{ \
			static const int value = version; \
		}; \
	} }

#define LN_SERIALIZE_CLASS_FORMAT(type, format)	\
	namespace ln { namespace detail { \
		template<> class SerializeClassFormatInfo<type> \
		{ \
			static const SerializeClassFormat value = format; \
		}; \
	} }

namespace tr {
//class ReflectionObject;
//class TypeInfo;
class Archive;

class ScVariant;
class ISerializeElement;

enum class ScVariantType
{
	Unknown,	// TODO: Null
	Bool,
	Int,
	Float,
	String,
	List,
	Map,
};

class ScVariantCore : public ln::Object
{
public:
	ScVariantCore()
		: m_type(ScVariantType::Unknown)
	{}

	virtual ~ScVariantCore();

	void setInt(int value);
	void setString(const StringRef& value);
	void setList();
	void setMap();


private:
	void resetType(ScVariantType type);
	void releaseValue();
	//void serialize(Archive& ar, int version);

	ScVariantType	m_type;
	union
	{
		bool										m_bool;
		int											m_int;
		float										m_float;
		ln::String*									m_string;
		ln::List<ScVariant>*						m_list;
		std::unordered_map<ln::String, ScVariant>*	m_map;
	};

	friend class ScVariant;
	friend class Archive;
};

class ScVariant
{
public:
	ScVariantType getType() const;

	void setInt(int value);
	int getInt() const;

	void setString(const StringRef& value);
	const String& getString() const;

	void saveInternal(ISerializeElement* value);
	void loadInternal(ISerializeElement* value);

	// strict equals
	bool equals(int value) const;

private:
	friend class Archive;
	ln::Ref<ScVariantCore>	m_core;
};

enum class ArchiveMode
{
	save,
	load,
};

/**
	@brief
*/
template<typename TRef>
class NameValuePair
{
public:
	const Char* name;
	TRef& value;
	//const TRef* defaultValue;

	NameValuePair(const Char* n, TRef& v) : name(n), value(v)/*, defaultValue(nullptr)*/ {}
	//NameValuePair(const Char* n, TRef& v, const TRef* d) : name(n), value(v), defaultValue(d) {}

private:
	NameValuePair & operator=(NameValuePair const &) = delete;
};

template<typename TRef>
NameValuePair<TRef> makeNVP(const Char* name, TRef& valueRef)
{
	return NameValuePair<TRef>(name, valueRef);
}
template<typename TRef>
NameValuePair<TRef> makeNVP(const Char* name, TRef& valueRef, const TRef& defaultValue)
{
	return NameValuePair<TRef>(name, valueRef, &defaultValue);
}



template<typename TThis>
class NameValuePairBaseObject
{
public:
	const Char* name;
	TThis* value;

	NameValuePairBaseObject(const Char* n, TThis* v) : name(n), value(v) {}

private:
	NameValuePairBaseObject & operator=(NameValuePairBaseObject const &) = delete;
};

///**
//	@brief
//*/
//class ISerializeArrayElement
//	: public ISerializeElement
//{
//public:
//	virtual int GetSerializeElementCount() const = 0;
//	virtual JsonElement2* GetSerializeElement(int index) const = 0;
//};


enum class SerializationElementType
{
	Value,
	Array,
	Object,
};

enum class SerializationValueType
{
	Null,
	Bool,
	Int8,
	Int16,
	Int32,
	Int64,
	UInt8,
	UInt16,
	UInt32,
	UInt64,
	Float,
	Double,
	String,
};


/**
	@brief
*/
class ISerializeElement
{
public:
	virtual SerializationElementType getSerializationElementType() const = 0;

	//virtual void SetValueDouble(const StringRef& name, int32_t value) = 0;
	virtual void setValueString(const StringRef& name, const String& value) = 0;

	virtual ISerializeElement* addObject(const StringRef& name) = 0;


	virtual bool tryGetValueInt32(const StringRef& name, int32_t* outValue) = 0;
	virtual bool tryGetValueString(const StringRef& name, String* outValue) = 0;

	virtual bool tryGetObject(const StringRef& name, ISerializeElement** outValue) = 0;

	virtual bool tryGetArray(const StringRef& name, ISerializeElement** outValue) = 0;



	// Value
	//virtual void setValueInt8(const StringRef& name, int32_t value) = 0;
	//virtual void setValueInt32(const StringRef& name, int32_t value) = 0;
	//virtual void setValueInt32(const StringRef& name, int32_t value) = 0;
	//virtual void setValueInt32(const StringRef& name, int32_t value) = 0;
	virtual SerializationValueType getSerializationValueType() const = 0;
	virtual bool getSerializeValueBool() const = 0;
	virtual int8_t getSerializeValueInt8() const = 0;
	virtual int16_t getSerializeValueInt16() const = 0;
	virtual int32_t getSerializeValueInt32() const = 0;
	virtual int64_t getSerializeValueInt64() const = 0;
	virtual uint8_t getSerializeValueUInt8() const = 0;
	virtual uint16_t getSerializeValueUInt16() const = 0;
	virtual uint32_t getSerializeValueUInt32() const = 0;
	virtual uint64_t getSerializeValueUInt64() const = 0;
	virtual float getSerializeValueFloat() const = 0;
	virtual double getSerializeValueDouble() const = 0;
	virtual String getSerializeValueString() const = 0;
	//virtual bool SetSerializeValueBool() const = 0;
	//virtual int8_t SetSerializeValueInt8() const = 0;
	//virtual int16_t SetSerializeValueInt16() const = 0;
	//virtual int32_t SetSerializeValueInt32() const = 0;
	//virtual int64_t SetSerializeValueInt64() const = 0;
	//virtual uint8_t SetSerializeValueUInt8() const = 0;
	//virtual uint16_t SetSerializeValueUInt16() const = 0;
	//virtual uint32_t SetSerializeValueUInt32() const = 0;
	//virtual uint64_t SetSerializeValueUInt64() const = 0;
	//virtual float SetSerializeValueFloat() const = 0;
	//virtual double SetSerializeValueDouble() const = 0;
	//virtual String SetSerializeValueString() const = 0;
	
	// Array and Object
	virtual int getSerializeElementCount() const = 0;
	virtual ISerializeElement* getSerializeElement(int index) const = 0;
	virtual const String& getSerializeElementName(int index) const = 0;

	// Array
	virtual void addSerializeItemValue(SerializationValueType type, const void* value) = 0;
	virtual ISerializeElement* addSerializeItemNewArray() = 0;
	virtual ISerializeElement* addSerializeItemNewObject() = 0;

	// Object
	virtual ISerializeElement* findSerializeElement(const StringRef& name) const = 0;
	virtual void addSerializeMemberValue(const StringRef& name, SerializationValueType type, const void* value) = 0;
	virtual ISerializeElement* addSerializeMemberNewArray(const StringRef& name) = 0;
	virtual ISerializeElement* addSerializeMemberNewObject(const StringRef& name) = 0;


//#define LN_DEFINE_ValueAccessor(type, cppType) \
//	virtual cppType GetSerializeValue##type() const = 0; \
//	virtual cppType AddSerializeValueItem##type(cppType value) = 0; \
//	virtual cppType AddSerializeValueMember##type(const StringRef& name, cppType value) = 0;
//
//	LN_DEFINE_ValueAccessor(Bool, bool);
//	LN_DEFINE_ValueAccessor(Int8, int8_t);
//	LN_DEFINE_ValueAccessor(Int16, int16_t);
//	LN_DEFINE_ValueAccessor(Int32, int32_t);
//	LN_DEFINE_ValueAccessor(Int64, int64_t);
//	LN_DEFINE_ValueAccessor(UInt8, uint8_t);
//	LN_DEFINE_ValueAccessor(UInt16, uint16_t);
//	LN_DEFINE_ValueAccessor(UInt32, uint32_t);
//	LN_DEFINE_ValueAccessor(UInt64, uint64_t);
//	LN_DEFINE_ValueAccessor(Float, float);
//	LN_DEFINE_ValueAccessor(Double, double);
//	LN_DEFINE_ValueAccessor(String, String);
//
//#undef LN_DEFINE_ValueAccessor
};

/**
	@brief
*/
class ISerializationeStore
{
public:
	virtual ISerializeElement* getRootObject() = 0;
};


/**
	@brief
	@details	�ǂݍ��݂ł́A�����J�n�O�� DOM ���I���������ɑS�ēW�J���܂��B
				���ɑ傫���f�[�^�̓ǂݏ����ɂ� BinaryReader �� BinaryWriter ���g�p���Ă��������B
*/
class Archive
{
	struct KeyInfo
	{
		StringRef name;
		bool callBase;
	};

public:
	static const int ArchiveVersion;
	static const Char* ArchiveVersionKey;
	static const Char* ArchiveRootObjectKey;
	static const Char* ClassNameKey;
	static const Char* ClassVersionKey;
	static const Char* ClassBaseDefaultNameKey;
	//Archive(const PathName& filePath, ArchiveMode mode);

	//template<typename T> Archive& operator&(T && arg)
	//{
	//	Process(std::forward<T>(arg));
	//	return *self;
	//}

	Archive(ISerializationeStore* stream, ArchiveMode mode, bool refrectionSupported)
		: m_mode(mode)
		, m_stream(stream)
		, m_refrectionSupported(refrectionSupported)
	{
		m_currentObject = m_stream->getRootObject();
	}

	template<class TRef>
	Archive& operator & (const NameValuePair<TRef>& nvp)
	{
		process(KeyInfo{ nvp.name, false }, nvp.value);
		return *this;
	}

	template<class TThis>
	Archive& operator & (const NameValuePairBaseObject<TThis>& nvp)
	{
		process(KeyInfo{ nvp.name, true }, Ref<TThis>(nvp.value));
		return *this;
	}

	//template<class T>
	//void serialize(const )
	//{

	//}

	ISerializeElement* getCurrentSerializeElement() const
	{
		return m_currentObject;
	}

	bool isSaving() const { return m_mode == ArchiveMode::save; }

	bool isLoading() const { return m_mode == ArchiveMode::load; }

	template<typename T>
	void save(T& value)
	{
		m_currentObject = saveArchiveHeaderElement(m_currentObject);
		doSaveObjectType(value, false);
	}
	void save(ScVariant& value)
	{
		if (LN_ENSURE(value.getType() == ScVariantType::Map)) return;
		m_currentObject = saveArchiveHeaderElement(m_currentObject);
		doSaveObjectType(*value.m_core->m_map, false);
	}


	template<typename T>
	void load(T& value)
	{
		m_currentObject = loadArchiveHeaderElement(m_currentObject);
		tryGetValue(m_currentObject, &value, false);
	}
	void load(ScVariant& value)
	{
		m_currentObject = loadArchiveHeaderElement(m_currentObject);
		tryGetValue(m_currentObject, &value, false);
	}


protected:
	virtual Ref<ReflectionObject> createObject(const String& className, TypeInfo* requestedType);

private:
	ISerializeElement* saveArchiveHeaderElement(ISerializeElement* element);
	ISerializeElement* loadArchiveHeaderElement(ISerializeElement* element);



	template<typename T> void process(const KeyInfo& key, T && value)
	{
		switch (m_mode)
		{
		case ArchiveMode::save:
			processWrite(key, value);
			break;
		case ArchiveMode::load:
			processRead(key, value);
			break;
		default:
			assert(0);
			break;
		}
	}

	template<typename T>
	void processWrite(const KeyInfo& key, T && value)
	{
		addMemberValue(key, value);
	}

	template<typename T>
	void processRead(const KeyInfo& key, T && value)
	{
		ISerializeElement* element = m_currentObject->findSerializeElement(key.name);
		if (element)	// fuzzy mode
		{
			//if (LN_REQUIRE_KEY(element, key.name.data())) return;
			tryGetValue(element, &value, key.callBase);
		}
		else
		{
		}
	}


	//-----------------------------------------------------------------------------
	// Save
	//		���݂� Object(m_currentObject) �ցA�w�肵�����O�ƒl�̃y�A��ǉ�����B

	void addMemberValue(const KeyInfo& key, bool value) { m_currentObject->addSerializeMemberValue(key.name, SerializationValueType::Bool, &value); }
	void addMemberValue(const KeyInfo& key, int8_t value) { m_currentObject->addSerializeMemberValue(key.name, SerializationValueType::Int8, &value); }
	void addMemberValue(const KeyInfo& key, int16_t value) { m_currentObject->addSerializeMemberValue(key.name, SerializationValueType::Int16, &value); }
	void addMemberValue(const KeyInfo& key, int32_t value) { m_currentObject->addSerializeMemberValue(key.name, SerializationValueType::Int32, &value); }
	void addMemberValue(const KeyInfo& key, int64_t value) { m_currentObject->addSerializeMemberValue(key.name, SerializationValueType::Int64, &value); }
	void addMemberValue(const KeyInfo& key, uint8_t value) { m_currentObject->addSerializeMemberValue(key.name, SerializationValueType::UInt8, &value); }
	void addMemberValue(const KeyInfo& key, uint16_t value) { m_currentObject->addSerializeMemberValue(key.name, SerializationValueType::UInt16, &value); }
	void addMemberValue(const KeyInfo& key, uint32_t value) { m_currentObject->addSerializeMemberValue(key.name, SerializationValueType::UInt32, &value); }
	void addMemberValue(const KeyInfo& key, uint64_t value) { m_currentObject->addSerializeMemberValue(key.name, SerializationValueType::UInt64, &value); }
	void addMemberValue(const KeyInfo& key, float value) { m_currentObject->addSerializeMemberValue(key.name, SerializationValueType::Float, &value); }
	void addMemberValue(const KeyInfo& key, double value) { m_currentObject->addSerializeMemberValue(key.name, SerializationValueType::Double, &value); }
	void addMemberValue(const KeyInfo& key, String& value) { m_currentObject->setValueString(key.name, value); }
	template<typename T> void addMemberValue(const KeyInfo& key, T& obj)	 // non�]intrusive Object
	{
		auto* old = m_currentObject;
		m_currentObject = m_currentObject->addObject(key.name);
		doSaveObjectType(obj, false);
		m_currentObject = old;
	}
	template<typename T> void addMemberValue(const KeyInfo& key, Ref<T>& value)	 // non�]intrusive Object
	{
		auto* old = m_currentObject;
		m_currentObject = m_currentObject->addObject(key.name);
		doSaveObjectType(value, key.callBase);
		m_currentObject = old;
	}
	template<typename T> void addMemberValue(const KeyInfo& key, List<T>& obj)	 // non�]intrusive Object
	{
		ISerializeElement* ary = m_currentObject->addSerializeMemberNewArray(key.name);
		for (int i = 0; i < obj.getCount(); i++)
		{
			addItemValue(ary, obj[i]);
		}
	}
	template<typename T> void addMemberValue(const KeyInfo& key, std::unordered_map<String, T>& obj)	 // non�]intrusive Object
	{
		ISerializeElement* e = m_currentObject->addSerializeMemberNewObject(key.name);
		auto* old = m_currentObject;
		m_currentObject = e;
		doSaveObjectType(obj, key.callBase);
		m_currentObject = old;
	}
	void addMemberValue(const KeyInfo& key, ScVariant& value)	 // non�]intrusive Object
	{
		if (value.m_core == nullptr || value.m_core->m_type == ScVariantType::Unknown)
		{
			LN_NOTIMPLEMENTED();
		}
		else
		{
			switch (value.m_core->m_type)
			{
			case ScVariantType::Bool:
				addMemberValue(key, value.m_core->m_bool);
				break;
			case ScVariantType::Int:
				addMemberValue(key, value.m_core->m_int);
				break;
			case ScVariantType::Float:
				//AddMemberValue(key, value.m_core->m_float);
				break;
			case ScVariantType::String:
				//AddMemberValue(key, *value.m_core->m_string);
				break;
			case ScVariantType::List:
				addMemberValue(key, *value.m_core->m_list);
				break;
			case ScVariantType::Map:
				addMemberValue(key, *value.m_core->m_map);
				break;
			default:
				LN_UNREACHABLE();
				break;
			}
		}
	}

	// DoSaveObjectType()
	//		m_currentObject ������ Object �^�ցA�w�肵�����O�ƒl�̃y�A��ǉ�����
	template<typename T> void doSaveObjectType(T& value, bool baseCall)
	{
		value.serialize(*this, 0);
	}
	template<typename T> void doSaveObjectType(Ref<T>& value, bool baseCall)
	{
		callSave(*value, baseCall);
	}
	template<typename T> void doSaveObjectType(std::unordered_map<String, T>& value, bool baseCall)
	{
		for (auto& pair : value)
		{
			addMemberValue({ pair.first, baseCall }, pair.second);
		}
	}


#define LN_DEFINE_addItemValue(type, cppType)  \
	static void addItemValue(ISerializeElement* arrayElement, cppType value) \
	{ \
		arrayElement->addSerializeItemValue(SerializationValueType::type, &value); \
	}

	LN_DEFINE_addItemValue(Bool, bool);
	LN_DEFINE_addItemValue(Int8, int8_t);
	LN_DEFINE_addItemValue(Int16, int16_t);
	LN_DEFINE_addItemValue(Int32, int32_t);
	LN_DEFINE_addItemValue(Int64, int64_t);
	LN_DEFINE_addItemValue(UInt8, uint8_t);
	LN_DEFINE_addItemValue(UInt16, uint16_t);
	LN_DEFINE_addItemValue(UInt32, uint32_t);
	LN_DEFINE_addItemValue(UInt64, uint64_t);
	LN_DEFINE_addItemValue(Float, float);
	LN_DEFINE_addItemValue(Double, double);
	LN_DEFINE_addItemValue(String, String);

#undef LN_DEFINE_addItemValue

	// for other struct type
	template<typename T>
	void addItemValue(ISerializeElement* arrayElement, T& value)
	{
		ISerializeElement* objectElement = arrayElement->addSerializeItemNewObject();

		auto* old = m_currentObject;
		m_currentObject = objectElement;
		value.serialize(*this, 0);
		//CallSave(value);
		m_currentObject = old;
	}
	// for ln::Object type
	template<typename T>
	void addItemValue(ISerializeElement* arrayElement, Ref<T>& value)
	{
		if (value == nullptr)
		{
			arrayElement->addSerializeItemValue(SerializationValueType::Null, nullptr);
		}
		else
		{
			ISerializeElement* objectElement = arrayElement->addSerializeItemNewObject();
			auto* old = m_currentObject;
			m_currentObject = objectElement;
			//value->serialize(*this, T::lnsl_GetClassVersion());
			callSave(*value, false);
			m_currentObject = old;
		}
	}
	template<typename T> void addItemValue(ISerializeElement* arrayElement, List<T>& obj)	 // non�]intrusive Object
	{
		ISerializeElement* ary = arrayElement->addSerializeItemNewArray();
		for (int i = 0; i < obj.getCount(); i++)
		{
			addItemValue(ary, obj[i]);
		}
	}
	void addItemValue(ISerializeElement* arrayElement, ScVariant& value)
	{
		if (value.m_core == nullptr || value.m_core->m_type == ScVariantType::Unknown)
		{
			LN_NOTIMPLEMENTED();
		}
		else
		{
			switch (value.m_core->m_type)
			{
			case ScVariantType::Bool:
				addItemValue(arrayElement, value.m_core->m_bool);
				break;
			case ScVariantType::Int:
				addItemValue(arrayElement, value.m_core->m_int);
				break;
			case ScVariantType::Float:
				addItemValue(arrayElement, value.m_core->m_float);
				break;
			case ScVariantType::String:
				addItemValue(arrayElement, *value.m_core->m_string);
				break;
			case ScVariantType::List:
				addItemValue(arrayElement, *value.m_core->m_list);
				break;
			case ScVariantType::Map:
				addItemValue(arrayElement, *value.m_core->m_map);
				break;
			default:
				LN_UNREACHABLE();
				break;
			}
		}
	}
	template<typename T> void addItemValue(ISerializeElement* arrayElement, std::unordered_map<String, T>& obj)	 // non�]intrusive Object
	{
		ISerializeElement* objectElement = arrayElement->addSerializeItemNewObject();
		auto* old = m_currentObject;
		m_currentObject = objectElement;
		doSaveObjectType(obj, false);
		m_currentObject = old;
	}


	//-----------------------------------------------------------------------------
	// Load

	static bool tryGetIntagerValue(ISerializeElement* element, int64_t* outValue)
	{
		assert(element);
		assert(outValue);
		SerializationValueType type = element->getSerializationValueType();
		switch (type)
		{
		case SerializationValueType::Int8:
			*outValue = static_cast<int64_t>(element->getSerializeValueInt8());
			return true;
		case SerializationValueType::Int16:
			*outValue = static_cast<int64_t>(element->getSerializeValueInt16());
			return true;
		case SerializationValueType::Int32:
			*outValue = static_cast<int64_t>(element->getSerializeValueInt32());
			return true;
		case SerializationValueType::Int64:
			*outValue = static_cast<int64_t>(element->getSerializeValueInt64());
			return true;
		case SerializationValueType::UInt8:
			*outValue = static_cast<int64_t>(element->getSerializeValueUInt8());
			return true;
		case SerializationValueType::UInt16:
			*outValue = static_cast<int64_t>(element->getSerializeValueUInt16());
			return true;
		case SerializationValueType::UInt32:
			*outValue = static_cast<int64_t>(element->getSerializeValueUInt32());
			return true;
		case SerializationValueType::UInt64:
			*outValue = static_cast<int64_t>(element->getSerializeValueUInt64());
			return true;
		default:
			return false;
		}
	}

	static bool tryGetDecimalValue(ISerializeElement* element, double* outValue)
	{
		assert(element);
		assert(outValue);
		SerializationValueType type = element->getSerializationValueType();
		switch (type)
		{
		case SerializationValueType::Float:
			*outValue = static_cast<double>(element->getSerializeValueFloat());
			return true;
		case SerializationValueType::Double:
			*outValue = static_cast<double>(element->getSerializeValueDouble());
			return true;
		default:
			return false;
		}
	}

#define LN_DEFINE_tryGetValue_Intager(type, cppType) \
	static bool tryGetValue(ISerializeElement* element, cppType* value, bool) \
	{ \
		if (element->getSerializationValueType() == SerializationValueType::type) \
		{ \
			*value = element->getSerializeValue##type(); \
			return true; \
		} \
		int64_t fuzzyValue; \
		if (tryGetIntagerValue(element, &fuzzyValue)) \
		{ \
			*value = static_cast<cppType>(fuzzyValue); \
			return true; \
		} \
		return false; \
	}

	LN_DEFINE_tryGetValue_Intager(Int8, int8_t);
	LN_DEFINE_tryGetValue_Intager(Int16, int16_t);
	LN_DEFINE_tryGetValue_Intager(Int32, int32_t);
	LN_DEFINE_tryGetValue_Intager(Int64, int64_t);
	LN_DEFINE_tryGetValue_Intager(UInt8, uint8_t);
	LN_DEFINE_tryGetValue_Intager(UInt16, uint16_t);
	LN_DEFINE_tryGetValue_Intager(UInt32, uint32_t);
	LN_DEFINE_tryGetValue_Intager(UInt64, uint64_t);

#undef LN_DEFINE_tryGetValue_Intager

#define LN_DEFINE_tryGetValue(type, cppType)\
	static bool tryGetValue(ISerializeElement* element, cppType* value, bool) \
	{ \
		if (element->getSerializationValueType() == SerializationValueType::type) \
		{ \
			*value = element->getSerializeValue##type(); \
			return true; \
		} \
		return false; \
	}

	LN_DEFINE_tryGetValue(Bool, bool);
	LN_DEFINE_tryGetValue(String, String);

#undef LN_DEFINE_TryGetValue

#define LN_DEFINE_tryGetValue_Decimal(type, cppType)\
	static bool tryGetValue(ISerializeElement* element, cppType* value, bool) \
	{ \
		if (element->getSerializationValueType() == SerializationValueType::type) \
		{ \
			*value = element->getSerializeValue##type(); \
			return true; \
		} \
		double fuzzyValue; \
		if (tryGetDecimalValue(element, &fuzzyValue)) \
		{ \
			*value = static_cast<cppType>(fuzzyValue); \
			return true; \
		} \
		return false; \
	}

	LN_DEFINE_tryGetValue_Decimal(Float, float);
	LN_DEFINE_tryGetValue_Decimal(Double, double);

#undef LN_DEFINE_tryGetValue_Decimal


	template<typename T>
	bool tryGetValue(ISerializeElement* element, Ref<T>* value, bool callBase)
	{
		auto* old = m_currentObject;
		m_currentObject = element;
		callLoad(value, callBase);
		m_currentObject = old;
		return true;
	}
	template<typename T>
	bool tryGetValue(ISerializeElement* arrayElement, List<T>* value, bool)
	{
		int count = arrayElement->getSerializeElementCount();
		for (int i = 0; i < count; i++)
		{
			auto* element = arrayElement->getSerializeElement(i);
			if (element->getSerializationElementType() == SerializationElementType::Value &&
				element->getSerializationValueType() == SerializationValueType::Null)
			{
				value->add(T());	// null
			}
			else
			{
				T raw;
				if (tryGetValue(element, &raw, false))
				{
					value->add(raw);
				}
				else
				{
					LN_NOTIMPLEMENTED();
				}
			}

		}
		return true;
	}
	template<typename T>
	bool tryGetValue(ISerializeElement* element, T* value, bool callBase)
	{
		auto* old = m_currentObject;
		m_currentObject = element;
		value->serialize(*this, 0);
		m_currentObject = old;
		return true;
	}
	bool tryGetValue(ISerializeElement* element, ScVariant* value, bool callBase)
	{
		value->loadInternal(element);
		return true;
	}


	//template<typename T>
	//void DoLoadObjectType(T* value, bool callBase)
	//{
	//	value->serialize(*this, 0);
	//}
	//void DoLoadObjectType(ScVariant* value, bool callBase)
	//{
	//	
	//}



	template<typename T>
	void callSave(T& obj, bool callBase)
	{
		int version = ::ln::detail::SerializeClassVersionInfo<T>::value;

		if (m_refrectionSupported)
		{
			TypeInfo* typeInfo;

			if (callBase)
			{
				typeInfo = TypeInfo::getTypeInfo<T>();
			}
			else
			{
				typeInfo = TypeInfo::getTypeInfo(&obj);
			}

			//int version = 0;
			if (typeInfo != TypeInfo::getTypeInfo<Object>())
			{
				String typeName = typeInfo->getName();
				//int version = typeInfo->getSerializeClassVersion();
				m_currentObject->addSerializeMemberValue(ClassNameKey, SerializationValueType::String, &typeName);
				m_currentObject->addSerializeMemberValue(ClassVersionKey, SerializationValueType::Int32, &version);
			}
			else
			{
				// "Object" �̌^��񂵂��Ƃ�Ȃ��Ƃ������Ƃ́AT �� TypeInfo ����`����Ă��Ȃ��B
				// �ۑ����Ă������ł��Ȃ��̂ŁA�^���͕ۑ������v���[���ȏ��ɂ���B
			}

			if (callBase)
				obj.T::serialize(*this, version);
			else
				obj.serialize(*this, version);
		}
		else
		{
			if (version > 0)
			{
				m_currentObject->addSerializeMemberValue(ClassVersionKey, SerializationValueType::Int32, &version);
			}


			obj.serialize(*this, version);
		}
	}

	template<typename T>
	void callLoad(Ref<T>* value, bool callBase)
	{
		bool loaded = false;
		if (m_refrectionSupported)
		{
			ISerializeElement* classNameElement = m_currentObject->findSerializeElement(ClassNameKey);
			ISerializeElement* classVersionElement = m_currentObject->findSerializeElement(ClassVersionKey);
			if (classNameElement != nullptr && classVersionElement != nullptr)
			{
				String className;
				int classVersion;
				if (tryGetValue(classNameElement, &className, false) && tryGetValue(classVersionElement, &classVersion, false))
				{
					TypeInfo* typeInfo = TypeInfo::getTypeInfo<T>();
					if (callBase)
					{
						(*value)->T::serialize(*this, classVersion);
					}
					else
					{
						(*value) = Ref<T>::staticCast(createObject(className, typeInfo));
						(*value)->serialize(*this, classVersion);
					}
					loaded = true;
				}
			}
		}
		
		if (!loaded)
		{
			(*value) = newObject<T>();
			(*value)->serialize(*this, 0);
		}
	}

	ArchiveMode					m_mode;
	ISerializationeStore*		m_stream;
	ISerializeElement*			m_currentObject;			//�@serialize() ���� & ���ꂽ�l�̕ۑ���A�܂��͕������ƂȂ�e Object(Map)�Bserialize() �ɓ���Ƃ��A�ɃZ�b�g�����B
	bool						m_refrectionSupported;

};

	
	
	template<typename TThis>
	NameValuePairBaseObject<TThis> makeNVPBaseObject(TThis* value)
	{
		return NameValuePairBaseObject<TThis>(Archive::ClassBaseDefaultNameKey, value);
	}

//#define LN_SERIALIZE(ar, version, classVersion) \
//	static const int lnsl_GetClassVersion() { return classVersion; } \
//	virtual void serialize(tr::Archive& ar, int version)

} // namespace tr

#define LN_NVP(var)		ln::tr::makeNVP(_LT(#var), var)
//#define LN_NVP(var, ...)	ln::tr::makeNVP(_LT(#var), var, ##__VA_ARGS__)

LN_NAMESPACE_END
