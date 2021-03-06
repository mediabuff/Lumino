
#pragma once
#include "../OutputBuffer.h"
class SymbolDatabase;

/** C言語の用の API を出力する */
class WrapperIFGenerator
{
public:
	void generate(SymbolDatabase* database);

private:
	String GenerateOverrideCallerFuncPtrs();
	String GenerateWrapperIFClasses();

	StringA MakeInstanceParamName(TypeInfoPtr info);
	StringA MakeMethods(TypeInfoPtr typeInfo);
	StringA MakeMethod(TypeInfoPtr typeInfo, MethodInfoPtr methodInfo, bool virtualBase);
	StringA MakeCApiParamTypeName(MethodInfoPtr methodInfo, ParameterInfoPtr paramInfo);
	StringA MakeCppTypeName(TypeInfoPtr typeInfo);
	String MakeDocumentComment(DocumentInfoPtr doc);

	String MakeEventWrapperMemberVariableName(MethodInfoPtr connectMethod);

	SymbolDatabase*	m_database;

	OutputBuffer	m_eventWrapperConnectExps;
	OutputBuffer	m_eventWrappers;
};

