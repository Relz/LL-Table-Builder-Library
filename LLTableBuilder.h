#ifndef LLPARSER_LLPARSER_H
#define LLPARSER_LLPARSER_H

#include "InputLibrary/Input.h"
#include "Table/Table.h"
#include "UnresolvedNextIdInformation/UnresolvedNextIdInformation.h"
#include <string>

enum class Token;
class Symbol;

class LLTableBuilder
{
public:
	explicit LLTableBuilder(std::string const & fileName);
	Table const & GetTable() const;

private:
	static char const EMPTY_CHARACTER;
	static std::string const EMPTY_CHARACTER_STRING;
	static std::string const EMPTY_CHARACTER_STRING_BORDERED;

	Input m_input;

	unsigned int m_currentTableRowId = 0;
	Table m_table;

	std::vector<UnresolvedNextIdInformation> m_unresolvedNextIds;
	std::unordered_map<std::string, std::vector<unsigned int>> m_tableReferences;
	std::unordered_map<std::string, std::unordered_set<Token>> m_referencingSets;

	void PrintParsingError(Input const & input, std::string const & message);
	void ParseNonterminalDeclaration(Input & input, std::string & nonterminal);
	bool TryToParseSymbol(
		std::string const & sequenceString, size_t & fromPos, char leftBorder, char rightBorder, Symbol & symbol);
	void SplitSequenceString(std::string const & sequenceString, std::vector<Symbol> & sequence);
	void ParseSequenceString(Input & input, std::string & sequenceString);
	void ParseReferencingSet(Input & input, std::unordered_set<Token> & referencingSet);
	bool TryParseActionNames(Input & m_input, std::vector<std::string> & actionNames);
	void UpdateIsErrorOfAlternatives(std::string const & nonterminal);
};

#endif
