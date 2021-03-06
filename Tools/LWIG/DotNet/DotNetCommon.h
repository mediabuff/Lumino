#pragma once
#include <unordered_map>
#include "../Global.h"

class DotNetCommon
{
public:
	static void initialize();
	static String MakePInvokeTypeName(TypeInfoPtr typeInfo);
	static String MakeTypeName(TypeInfoPtr typeInfo);
	static String MakeParamTypeName(ParameterInfoPtr paramInfo);
	static String MakeXmlDocument(DocumentInfoPtr doc);
	static String MakeLiteral(ConstantInfoPtr constantInfo);

	static std::unordered_map<TypeInfoPtr, String>	primitiveTypesMap;

};
