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
