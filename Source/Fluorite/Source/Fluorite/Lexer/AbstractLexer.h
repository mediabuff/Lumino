
#pragma once
#include <Fluorite/Token.h>

namespace fl {
class InputFile;
class DiagnosticsItemSet;
	
/**
	@brief
*/
class AbstractLexer
	: public Object
{
public:
	struct Range
	{
		const char* pos;
		const char* end;

		Range()
		{
		}

		Range(const char* begin, const char* end_)
		{
			pos = begin;
			end = end_;
		}
	};

public:
	AbstractLexer();
	virtual ~AbstractLexer();

	ResultState Tokenize(InputFile* file);


protected:
	virtual int ReadToken(const Range& buffer) = 0;
	virtual void PollingToken(SourceToken* newToken);

	void AddToken(TokenGroup group, const char* bufBegin, const char* bufEnd, int tokenType = 0);
	SourceToken* GetLastToken();
	bool EqualsString(SourceToken* token, const char* str, int length) const;
	DiagnosticsItemSet* getDiag() const { return m_diag; }

	static AlphaNumTypeFlags GetAlphaNumType(int ch);

	int ReadNewLine(const Range& buffer);
	static int IsNewLine(const Range& buffer);

	int ReadMBSSequence(const Range& buffer);

	virtual void onStart();

private:
	ResultState TokenizeInternal(const char* code, int length, TokenList* outTokenList, DiagnosticsItemSet* diag);

	InputFile*			m_inputFile;
	const char*			m_inputBuffer;
	//TokenList*			m_tokenList;
	DiagnosticsItemSet*	m_diag;
	int					m_currentLineNumber;	// 0�`
	int					m_currentColumn;
};

} // namespace fl


