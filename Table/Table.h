#ifndef LLPARSER_TABLE_H
#define LLPARSER_TABLE_H

#include <unordered_map>
#include <unordered_set>

class TableRow;

class Table
{
public:
	void AddRow(unsigned int id, TableRow * row);
	TableRow * GetRow(unsigned int id) const;

private:
	std::unordered_map<unsigned int, TableRow *> m_rows = std::unordered_map<unsigned int, TableRow *>();
};

#endif
