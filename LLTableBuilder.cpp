#include "Table/TableRow/TableRow.h"
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

			std::unordered_set<std::string> referencingSet;
			ParseReferencingSet(m_input, referencingSet);

			++m_currentTableRowId;

			TableRow * tableRow = new TableRow();
			tableRow->referencingSet = referencingSet;

			if (m_tableReferences.find(nonterminal) != m_tableReferences.end())
			{
				for (unsigned int tableRowId : m_tableReferences.at(nonterminal))
				{
					m_table.GetRow(tableRowId)->isError = false;
				}
			}

			m_unresolvedNextIds.emplace_back(std::make_pair(sequenceString, m_currentTableRowId));
			m_tableReferences[nonterminal].emplace_back(m_currentTableRowId);
			m_referencingSets[nonterminal].insert(referencingSet.begin(), referencingSet.end());

			tableRow->doShift = false;
			tableRow->pushToStack = 0;
			tableRow->isError = true;
			tableRow->isEnd = false;

			m_table.AddRow(m_currentTableRowId, tableRow);

			if (sequenceString == std::string(1, Symbol::NONTERMINAL_LEFT_BORDER) + EMPTY_CHARACTER + Symbol::NONTERMINAL_RIGHT_BORDER)
			{
				m_referencingSets[EMPTY_CHARACTER_STRING].insert(referencingSet.begin(), referencingSet.end());
			}

			m_input.SkipLine();
		}
	}
	catch (std::runtime_error const & e)
	{
		PrintParsingError(m_input, e.what());
		throw std::runtime_error("Parsing error. Please check " + fileName + " for errors");
	}

	try
	{
		for (auto const & unresolvedNextId : m_unresolvedNextIds)
		{
			std::string const & sequenceString = unresolvedNextId.first;
			unsigned int const & tableRowId = unresolvedNextId.second;

			std::vector<Symbol> sequence;
			SplitSequenceString(sequenceString, sequence);

			m_table.GetRow(tableRowId)->nextId = m_currentTableRowId + 1;

			for (size_t i = 0; i < sequence.size(); ++i)
			{
				Symbol const & sequenceElement = sequence.at(i);

				++m_currentTableRowId;

				TableRow * tableRow = new TableRow();

				std::string symbolValue = sequenceElement.GetValue();

				if (sequenceElement.IsTerminal())
				{
					tableRow->referencingSet = std::unordered_set<std::string>({ symbolValue });
					if (i == sequence.size() - 1)
					{
						tableRow->nextId = 0;
					}
					else
					{
						tableRow->nextId = m_currentTableRowId + 1;
					}
					if (symbolValue == END_OF_LINE_STRING)
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
				else if (sequenceElement.IsNonterminal())
				{
					tableRow->referencingSet = m_referencingSets.at(symbolValue);
					if (symbolValue == EMPTY_CHARACTER_STRING)
					{
						tableRow->nextId = 0;
					}
					else
					{
						tableRow->nextId = m_tableReferences.at(symbolValue).front();
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

				tableRow->isError = true;

				m_table.AddRow(m_currentTableRowId, tableRow);
			}
		}
	}
	catch (std::runtime_error const & e)
	{
		std::cout << e.what();
		throw std::runtime_error("Parsing error. Please check " + fileName + " for errors");
	}
}

Table const & LLTableBuilder::GetTable() const
{
	return m_table;
}

char const LLTableBuilder::EMPTY_CHARACTER = 'e';
std::string const LLTableBuilder::EMPTY_CHARACTER_STRING = std::string(1, LLTableBuilder::EMPTY_CHARACTER);

std::string const LLTableBuilder::END_OF_LINE_STRING = "#";

void LLTableBuilder::PrintParsingError(Input const & m_input, std::string const & message)
{
	std::cout << m_input.GetFileName()
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
		for (size_t j = fromPos; j < sequenceString.size(); ++j)
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
					std::string("Symbol parsing: ") + sequenceString + ": " + "from " + std::to_string(fromPos) + ": "
							+ "'" + std::string(1, rightBorder) + "'" + " expected"
			);
		}
		symbol = std::move(possibleSymbol);
		fromPos += symbol.size() - 1;
		return true;
	}
	return false;
}

void LLTableBuilder::SplitSequenceString(std::string const & sequenceString, std::vector<Symbol> & sequence)
{
	for (size_t i = 0; i < sequenceString.size(); ++i)
	{
		sequence.emplace_back(Symbol());
		if (!TryToParseSymbol(
				sequenceString, i, Symbol::NONTERMINAL_LEFT_BORDER, Symbol::NONTERMINAL_RIGHT_BORDER, sequence.back())
				&& !TryToParseSymbol(
						sequenceString, i, Symbol::TERMINAL_LEFT_BORDER, Symbol::TERMINAL_RIGHT_BORDER, sequence.back()))
		{
			throw std::runtime_error(
					"Sequence string splitting: "
							+ std::string(1, '"') + sequenceString + std::string(1, '"') + "(" + std::to_string(i + 1) + ")" + ": "
							+ "'" + std::string(1, sequenceString.at(i)) + "'" + " not expected"
			);
		}
	}
}

void LLTableBuilder::ParseSequenceString(Input & m_input, std::string & sequenceString)
{
	std::string possibleSequenceString;
	m_input.ReadUntilCharacters({ '/', '\n' }, possibleSequenceString);
	if (m_input.IsEndOfLine())
	{
		throw std::runtime_error("Referencing set expected");
	}
	m_input.SkipArgument<char>();
	sequenceString = std::move(possibleSequenceString);
}

void LLTableBuilder::ParseReferencingSet(Input & m_input, std::unordered_set<std::string> & referencingSet)
{
	std::string reference;
	while (m_input.ReadUntilCharacters({ ',', '\n' }, reference) && !reference.empty())
	{
		referencingSet.emplace(reference);
		if (!m_input.IsEndOfLine())
		{
			m_input.SkipArgument<char>();
		}
	}
}
