﻿
#include "../Internal.h"
#include <Lumino/Base/String.h>
#include <Lumino/Json/JsonWriter.h>

LN_NAMESPACE_BEGIN
namespace tr {

//==============================================================================
// JsonWriter
//==============================================================================
//------------------------------------------------------------------------------
JsonWriter::JsonWriter(TextWriter* textWriter)
	: m_formatting(JsonFormatting::None)
	, m_textWriter(textWriter)
{
	if (LN_REQUIRE(m_textWriter != nullptr)) return;
	m_levelStack.reserve(32);
}

//------------------------------------------------------------------------------
JsonWriter::~JsonWriter()
{
}

//------------------------------------------------------------------------------
void JsonWriter::setFormatting(JsonFormatting formatting)
{
	m_formatting = formatting;
}

//------------------------------------------------------------------------------
void JsonWriter::writeStartObject()
{
	autoComplete(JsonToken::StartObject);
	m_levelStack.push(Level(false));
	onStartObject();
}

//------------------------------------------------------------------------------
void JsonWriter::writeEndObject()
{
	if (LN_REQUIRE(m_levelStack.getCount() >= 1)) return;
	if (LN_REQUIRE(!m_levelStack.getTop().inArray)) return;

	autoComplete(JsonToken::EndObject);
	m_levelStack.pop();
	onEndObject();

	if (!m_levelStack.isEmpty())	// ルート要素のクローズに備える
	{
		m_levelStack.getTop().valueCount++;
		m_levelStack.getTop().justSawContainerEnd = true;
	}
}

//------------------------------------------------------------------------------
void JsonWriter::writeStartArray()
{
	autoComplete(JsonToken::StartArray);
	m_levelStack.push(Level(true));
	onStartArray();
}

//------------------------------------------------------------------------------
void JsonWriter::writeEndArray()
{
	if (LN_REQUIRE(m_levelStack.getCount() >= 2)) return;
	if (LN_REQUIRE(m_levelStack.getTop().inArray)) return;

	autoComplete(JsonToken::EndArray);
	m_levelStack.pop();
	onEndArray();
	m_levelStack.getTop().valueCount++;
	m_levelStack.getTop().justSawContainerEnd = true;
}

//------------------------------------------------------------------------------
void JsonWriter::writePropertyName(const Char* str, int length)
{
	if (LN_REQUIRE(m_levelStack.getCount() >= 1)) return;
	length = (length <= -1) ? (int)StringTraits::tcslen(str) : length;

	autoComplete(JsonToken::PropertyName);
	onKey(str, length);
	m_levelStack.getTop().justSawKey = true;
}

//------------------------------------------------------------------------------
void JsonWriter::writeNull()
{
	if (LN_REQUIRE(m_levelStack.getCount() >= 1)) return;

	autoComplete(JsonToken::Null);
	onNull();
	m_levelStack.getTop().valueCount++;
}

//------------------------------------------------------------------------------
void JsonWriter::writeBool(bool value)
{
	if (LN_REQUIRE(m_levelStack.getCount() >= 1)) return;

	autoComplete(JsonToken::Boolean);
	onBool(value);
	m_levelStack.getTop().valueCount++;
}

//------------------------------------------------------------------------------
void JsonWriter::writeInt32(int32_t value)
{
	if (LN_REQUIRE(m_levelStack.getCount() >= 1)) return;
	autoComplete(JsonToken::Int32);
	onInt32(value);
	m_levelStack.getTop().valueCount++;
}

//------------------------------------------------------------------------------
void JsonWriter::writeInt64(int64_t value)
{
	if (LN_REQUIRE(m_levelStack.getCount() >= 1)) return;
	autoComplete(JsonToken::Int64);
	onInt64(value);
	m_levelStack.getTop().valueCount++;
}

//------------------------------------------------------------------------------
void JsonWriter::writeFloat(float value)
{
	if (LN_REQUIRE(m_levelStack.getCount() >= 1)) return;
	autoComplete(JsonToken::Float);
	onFloat(value);
	m_levelStack.getTop().valueCount++;
}

//------------------------------------------------------------------------------
void JsonWriter::writeDouble(double value)
{
	if (LN_REQUIRE(m_levelStack.getCount() >= 1)) return;

	autoComplete(JsonToken::Double);
	onDouble(value);
	m_levelStack.getTop().valueCount++;
}

//------------------------------------------------------------------------------
void JsonWriter::writeString(const Char* str, int length)	// TODO: StringRef
{
	if (LN_REQUIRE(m_levelStack.getCount() >= 1)) return;
	length = (length <= -1) ? (int)StringTraits::tcslen(str) : length;

	autoComplete(JsonToken::String);
	onString(str, length);
	m_levelStack.getTop().valueCount++;
}

//------------------------------------------------------------------------------
bool JsonWriter::isComplete() const
{
	return m_levelStack.isEmpty();
}

//------------------------------------------------------------------------------
void JsonWriter::autoComplete(JsonToken token)
{
	if (!m_levelStack.isEmpty())
	{
		Level& level = m_levelStack.getTop();

		if (token != JsonToken::EndObject && token != JsonToken::EndArray)
		{
			if (level.justSawKey) {
				onPrefix(PrefixType_Key, level.valueCount);
			}
			else if (level.valueCount > 0)
			{
				if (level.inArray) {
					onPrefix(PrefixType_Array, level.valueCount);
				}
				else {
					onPrefix(PrefixType_Object, level.valueCount);
				}
			}
		}

		if (m_formatting == JsonFormatting::Indented)
		{
			if (level.justSawKey)
			{
				m_textWriter->write(_LT(' '));	// : after space
			}

			if (!level.justSawKey)
			{
				if (token == JsonToken::StartObject || token == JsonToken::StartArray || token == JsonToken::PropertyName)
				{
					m_textWriter->writeLine();
					for (int i = 0; i < m_levelStack.getCount(); i++)
					{
						m_textWriter->write(_LT(' '));
						m_textWriter->write(_LT(' '));
					}
				}
				else if (token == JsonToken::EndObject || (token == JsonToken::EndArray && level.justSawContainerEnd))
				{
					m_textWriter->writeLine();
					for (int i = 0; i < m_levelStack.getCount() - 1; i++)
					{
						m_textWriter->write(_LT(' '));
						m_textWriter->write(_LT(' '));
					}
				}
			}
		}

		level.justSawKey = false;
		level.justSawContainerEnd = false;
	}
}

//------------------------------------------------------------------------------
void JsonWriter::onPrefix(PrefixType type, int valueCount)
{
	if (type == PrefixType_Array || type == PrefixType_Object) {
		if (valueCount > 0) {
			m_textWriter->write(_LT(','));
		}
	}
	else {
		m_textWriter->write(_LT(':'));
	}
}

//------------------------------------------------------------------------------
void JsonWriter::onStartObject()
{
	m_textWriter->write(_LT('{'));
}

//------------------------------------------------------------------------------
void JsonWriter::onEndObject()
{
	m_textWriter->write(_LT('}'));
}

//------------------------------------------------------------------------------
void JsonWriter::onStartArray()
{
	m_textWriter->write(_LT('['));
}

//------------------------------------------------------------------------------
void JsonWriter::onEndArray()
{
	m_textWriter->write(_LT(']'));
}

//------------------------------------------------------------------------------
void JsonWriter::onKey(const Char* str, int length)
{
	onString(str, length);
}

//------------------------------------------------------------------------------
void JsonWriter::onNull()
{
	m_textWriter->write(_LT("null"), 4);
}

//------------------------------------------------------------------------------
void JsonWriter::onBool(bool value)
{
	if (value) {
		m_textWriter->write(_LT("true"), 4);
	}
	else {
		m_textWriter->write(_LT("false"), 5);
	}
}

//------------------------------------------------------------------------------
void JsonWriter::onInt32(int32_t value)
{
	m_textWriter->write(value);
}

//------------------------------------------------------------------------------
void JsonWriter::onInt64(int64_t value)
{
	m_textWriter->write(value);
}

//------------------------------------------------------------------------------
void JsonWriter::onFloat(float value)
{
	m_textWriter->write(value);
}

//------------------------------------------------------------------------------
void JsonWriter::onDouble(double value)
{
	m_textWriter->write(value);
}

//------------------------------------------------------------------------------
void JsonWriter::onString(const Char* str, int length)
{
	m_textWriter->write(_TT("\""), 1);
	m_textWriter->write(str, length);
	m_textWriter->write(_TT("\""), 1);
}

//------------------------------------------------------------------------------
//void JsonWriter::OnIndent(int level)
//{
//	m_textWriter->WriteLine();
//	while (level > 0)
//	{
//		m_textWriter->Write(_T(' '));
//		--level;
//	}
//}

} // namespace tr
LN_NAMESPACE_END
