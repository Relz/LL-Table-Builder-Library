#include "Table/TableRow/TableRow.h"
#include "TokenLibrary/Token.h"
#include "TokenLibrary/TokenExtensions/TokenExtensions.h"
#include "Symbol/Symbol.h"
#include "LLTableBuilder.h"
#include <stack>

LLTableBuilder::LLTableBuilder(std::string const & fileName)
	: m_input(fileName)
{
	try
	{
		while (!m_input.IsEndOfStream())
		{
			std::string nonterminal;
			ParseNonterminalDeclaration(m_input, nonterminal);

			std::string sequenceString;
			ParseSequenceString(m_input, sequenceString);

			bool isEmptyCharacterSequence = sequenceString.rfind(EMPTY_CHARACTER_STRING_BORDERED, 0) != std::string::npos;

			std::unordered_set<Token> referencingSet;
			ParseReferencingSet(m_input, referencingSet);

			if (isEmptyCharacterSequence)
			{
				sequenceString.insert(sequenceString.begin() + 2, nonterminal.begin(), nonterminal.end());
				m_referencingSets[EMPTY_CHARACTER + nonterminal].insert(referencingSet.begin(), referencingSet.end());
			}

			++m_currentTableRowId;

			TableRow * tableRow = new TableRow();
			tableRow->referencingSet = referencingSet;

			UpdateIsErrorOfAlternatives(nonterminal);

			m_unresolvedNextIds.emplace_back(UnresolvedNextIdInformation(sequenceString, m_currentTableRowId));
			m_tableReferences[nonterminal].emplace_back(m_currentTableRowId);
			m_referencingSets[nonterminal].insert(referencingSet.begin(), referencingSet.end());

			tableRow->doShift = false;
			tableRow->pushToStack = 0;
			tableRow->isError = true;
			tableRow->isEnd = false;

			m_table.AddRow(m_currentTableRowId, tableRow);

			m_input.SkipLine();
		}
	}
	catch (std::runtime_error const & e)
	{
		PrintParsingError(m_input, e.what());
		std::cerr << "\n";
		std::cerr << "Parsing error. Please check " << fileName << " for errors.\n";
		return;
	}

	try
	{
		for (UnresolvedNextIdInformation const & unresolvedNextId : m_unresolvedNextIds)
		{
			std::vector<Symbol> sequence;
			SplitSequenceString(unresolvedNextId.sequenceString, sequence);

			m_table.GetRow(unresolvedNextId.tableRowId)->nextId = m_currentTableRowId + 1;

			for (size_t i = 0; i < sequence.size(); ++i)
			{
				Symbol const & sequenceElement = sequence.at(i);
				++m_currentTableRowId;

				TableRow * tableRow = new TableRow();
				tableRow->isError = true;

				std::string const & symbolValueString = sequenceElement.GetValue();
				if (sequenceElement.IsTerminal())
				{
					Token symbolValue;
					if (!TokenExtensions::CreateFromString(symbolValueString, symbolValue))
					{
						throw std::runtime_error("Cannot create token from string: " + symbolValueString);
					}
					tableRow->referencingSet = std::unordered_set<Token>({ symbolValue });
					if (i == sequence.size() - 1)
					{
						tableRow->nextId = 0;
					}
					else
					{
						tableRow->nextId = m_currentTableRowId + 1;
					}
					if (symbolValue == Token::END_OF_FILE)
					{
						tableRow->doShift = false;
						tableRow->isEnd = true;
					}
					else
					{
						tableRow->doShift = true;
						tableRow->isEnd = false;
					}
					tableRow->pushToStack = 0;
				}
				else if (sequenceElement.IsActionName())
				{
					tableRow->referencingSet = std::unordered_set<Token>({ Token::UNKNOWN });
					if (i == sequence.size() - 1)
					{
						tableRow->nextId = 0;
					}
					else
					{
						tableRow->nextId = m_currentTableRowId + 1;
					}
					tableRow->doShift = false;
					tableRow->isEnd = false;
					tableRow->pushToStack = 0;
					tableRow->actionName = std::move(symbolValueString);
					tableRow->isError = false;
					TableRow * prevTableRow = m_table.GetRow(m_currentTableRowId - 1);
					if (prevTableRow != nullptr && prevTableRow->isEnd)
					{
						prevTableRow->isEnd = false;
						tableRow->isEnd = true;
					}
				}
				else if (sequenceElement.IsNonterminal())
				{
					tableRow->referencingSet = m_referencingSets.at(symbolValueString);
					if (symbolValueString.at(0) == EMPTY_CHARACTER)
					{
						tableRow->nextId = 0;
					}
					else
					{
						tableRow->nextId = m_tableReferences.at(symbolValueString).front();
					}
					tableRow->doShift = false;
					if (i == sequence.size() - 1)
					{
						tableRow->pushToStack = 0;
					}
					else
					{
						tableRow->pushToStack = m_currentTableRowId + 1;
					}
					tableRow->isEnd = false;
				}

				m_table.AddRow(m_currentTableRowId, tableRow);
			}
		}
	}
	catch (std::runtime_error const & e)
	{
		std::cerr << "Resolving next id error: " << e.what() << "\n";
	}
}

Table const & LLTableBuilder::GetTable() const
{
	return m_table;
}

char const LLTableBuilder::EMPTY_CHARACTER = 'e';
std::string const LLTableBuilder::EMPTY_CHARACTER_STRING = std::string(1, LLTableBuilder::EMPTY_CHARACTER);
std::string const LLTableBuilder::EMPTY_CHARACTER_STRING_BORDERED =
	std::string(1, Symbol::NONTERMINAL_LEFT_BORDER) + EMPTY_CHARACTER + Symbol::NONTERMINAL_RIGHT_BORDER;

void LLTableBuilder::PrintParsingError(Input const & m_input, std::string const & message)
{
	std::cerr << m_input.GetFileName()
		<< "(" << m_input.GetPosition().GetLine() << ", " << m_input.GetPosition().GetColumn() << ")"
		<< ": " << message;
}

void LLTableBuilder::ParseNonterminalDeclaration(Input & m_input, std::string & nonterminal)
{
	if (!m_input.ReadUntilCharacters({ '-' }, nonterminal))
	{
		throw std::runtime_error(
				nonterminal.empty()
						? "Nonterminal declaration expected"
						: "Character '-' expected after nonterminal declaration"
		);
	}
	m_input.SkipArgument<char>();
}

bool LLTableBuilder::TryToParseSymbol(
	std::string const & sequenceString, size_t & fromPos, char leftBorder, char rightBorder, Symbol & symbol)
{
	Symbol possibleSymbol;
	if (sequenceString.at(fromPos) == leftBorder)
	{
		size_t j = fromPos;
		for (j; j < sequenceString.length(); ++j)
		{
			possibleSymbol.push_back(sequenceString.at(j));
			if (sequenceString.at(j) == rightBorder)
			{
				break;
			}
		}
		if (possibleSymbol.back() != rightBorder)
		{
			throw std::runtime_error(
				std::string("Symbol parsing: ")
				+ '"' + sequenceString + '"' + ": "
				+ "(" + std::to_string(fromPos) + '-' + std::to_string(j) + ")" + ": "
				+ "'" + std::string(1, rightBorder) + "'" + " expected");
		}
		symbol = std::move(possibleSymbol);
		fromPos += symbol.size() - 1;
		return true;
	}
	return false;
}

void LLTableBuilder::SplitSequenceString(std::string const & sequenceString, std::vector<Symbol> & sequence)
{
	for (size_t i = 0; i < sequenceString.length(); ++i)
	{
		sequence.emplace_back(Symbol());
		if (!TryToParseSymbol(
				sequenceString, i, Symbol::NONTERMINAL_LEFT_BORDER, Symbol::NONTERMINAL_RIGHT_BORDER, sequence.back())
			&& !TryToParseSymbol(
				sequenceString, i, Symbol::TERMINAL_LEFT_BORDER, Symbol::TERMINAL_RIGHT_BORDER, sequence.back())
			&& !TryToParseSymbol(
				sequenceString, i, Symbol::ACTION_NAME_LEFT_BORDER, Symbol::ACTION_NAME_RIGHT_BORDER, sequence.back()))
		{
			throw std::runtime_error(
				std::string("Sequence string splitting: ")
					+ '"' + sequenceString + '"' + "(" + std::to_string(i + 1) + ")" + ": "
					+ "'" + std::string(1, Symbol::NONTERMINAL_LEFT_BORDER) + "'" + " or "
					+ "'" + std::string(1, Symbol::TERMINAL_LEFT_BORDER) + "'" + " or "
					+ "'" + std::string(1, Symbol::ACTION_NAME_LEFT_BORDER) + "'"
					+ " expected");
		}
	}
}

void LLTableBuilder::ParseSequenceString(Input & m_input, std::string & sequenceString)
{
	m_input.ReadUntilCharacters({ '/', '\n' }, sequenceString);
	if (m_input.IsEndOfLine())
	{
		throw std::runtime_error("Referencing set expected");
	}
}

void LLTableBuilder::ParseReferencingSet(Input & m_input, std::unordered_set<Token> & referencingSet)
{
	char nextCharacter;
	if (!m_input.GetNextCharacter(nextCharacter) || nextCharacter != '/')
	{
		throw std::runtime_error(R"(Referencing set must starts with "/" character)");
	}
	m_input.SkipArgument<char>();
	std::string referenceString;
	while (m_input.ReadUntilCharacters({ ',', '\n' }, referenceString))
	{
		Token reference;
		if (!TokenExtensions::CreateFromString(referenceString, reference))
		{
			throw std::runtime_error("Cannot create token from string: " + referenceString);
		}
		referencingSet.emplace(reference);
		if (m_input.IsEndOfLine())
		{
			break;
		}
		else
		{
			m_input.SkipArgument<char>();
		}
	}
}

void LLTableBuilder::UpdateIsErrorOfAlternatives(std::string const & nonterminal)
{
	if (m_tableReferences.find(nonterminal) != m_tableReferences.end())
	{
		for (unsigned int tableRowId : m_tableReferences.at(nonterminal))
		{
			m_table.GetRow(tableRowId)->isError = false;
		}
	}
}
