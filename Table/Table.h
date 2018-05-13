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
	void Print(std::ostream & ostream) const;

private:
	std::unordered_map<unsigned int, TableRow *> m_rows = std::unordered_map<unsigned int, TableRow *>();

	std::string SetToString(std::unordered_set<std::string> const & set) const;
};

#endif
