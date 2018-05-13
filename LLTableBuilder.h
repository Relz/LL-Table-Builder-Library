#ifndef LLPARSER_LLPARSER_H
#define LLPARSER_LLPARSER_H

#include "InputLibrary/Input.h"
#include "Table/Table.h"
#include <string>

class Symbol;

class LLTableBuilderLibrary
{
public:
	explicit LLTableBuilderLibrary(std::string const & fileName);
	Table const & GetTable() const;

private:
	static char const EMPTY_CHARACTER;
	static std::string const EMPTY_CHARACTER_STRING;
	static std::string const END_OF_LINE_STRING;

	Input m_input;

	unsigned int m_currentTableRowId = 0;
	Table m_table;

	std::vector<std::pair<std::string, unsigned int>> m_unresolvedNextIds;
	std::unordered_map<std::string, std::vector<unsigned int>> m_tableReferences;
	std::unordered_map<std::string, std::unordered_set<std::string>> m_referencingSets;

	void PrintParsingError(Input const & input, std::string const & message);
	void ParseNonterminalDeclaration(Input & input, std::string & nonterminal);
	bool TryToParseSymbol(
		std::string const & sequenceString, size_t & fromPos, char leftBorder, char rightBorder, Symbol & symbol);
	void SplitSequenceString(std::string const & sequenceString, std::vector<Symbol> & sequence);
	void ParseSequenceString(Input & input, std::string & sequenceString);
	void ParseReferencingSet(Input & input, std::unordered_set<std::string> & referencingSet);

};

#endif
