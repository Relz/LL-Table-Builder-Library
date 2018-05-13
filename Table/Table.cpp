#include <iostream>
#include <vector>
#include <algorithm>
#include "TableRow/TableRow.h"
#include "Table.h"

void Table::AddRow(unsigned int id, TableRow * row)
{
	m_rows.emplace(id, row);
}

TableRow * Table::GetRow(unsigned int id) const
{
	return m_rows.find(id) == m_rows.end() ? nullptr : m_rows.at(id);
}

void Table::Print(std::ostream & ostream) const
{
	std::vector<std::pair<unsigned int, TableRow *>> rows(m_rows.begin(), m_rows.end());
	std::sort(rows.begin(), rows.end());

	size_t referencingSetColumnWidth = 14;
	for (auto const & tableElement : rows)
	{
		TableRow * tableRow = tableElement.second;

		referencingSetColumnWidth =
			std::max(referencingSetColumnWidth, SetToString(tableRow->referencingSet).length());
	}

	ostream << "|Id|";
	ostream.width(referencingSetColumnWidth);
	ostream << "ReferencingSet";
	ostream << "|NextId|DoShift|PushToStack|IsError|IsEnd|\n";
	ostream << std::string(referencingSetColumnWidth, '-');
	ostream << "----------------------------------------------\n";

	for (auto const & tableElement : rows)
	{
		unsigned int tableRowId = tableElement.first;
		TableRow * tableRow = tableElement.second;

		ostream << "|";
		ostream.width(2);
		ostream << tableRowId;
		ostream.width(1);
		ostream << "|";
		ostream.width(referencingSetColumnWidth);
		ostream << SetToString(tableRow->referencingSet);
		ostream.width(1);
		ostream << "|";
		ostream.width(6);
		ostream << (tableRow->nextId == 0 ? "-" : std::to_string(tableRow->nextId));
		ostream.width(1);
		ostream << "|";
		ostream.width(7);
		ostream << (tableRow->doShift ? "+" : "-");
		ostream.width(1);
		ostream << "|";
		ostream.width(11);
		ostream << (tableRow->pushToStack == 0 ? "-" : std::to_string(tableRow->pushToStack));
		ostream.width(1);
		ostream << "|";
		ostream.width(7);
		ostream << (tableRow->isError ? "+" : "-");
		ostream.width(1);
		ostream << "|";
		ostream.width(5);
		ostream << (tableRow->isEnd ? "+" : "-");
		ostream.width(1);
		ostream << "|\n";
	}
}

std::string Table::SetToString(std::unordered_set<std::string> const & set) const
{
	if (set.empty())
	{
		return "";
	}
	std::string result;
	for (std::string const & setElement : set)
	{
		result += setElement;
		result.push_back(',');
	}
	result.erase(result.end() - 1);

	return result;
}
