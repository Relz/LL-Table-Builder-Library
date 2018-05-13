#ifndef LLPARSER_TABLEROW_H
#define LLPARSER_TABLEROW_H

#include <unordered_set>

class TableRow
{
public:
	std::unordered_set<std::string> referencingSet = std::unordered_set<std::string>();
	unsigned int nextId = 0;
	bool doShift = false;
	unsigned int pushToStack = 0;
	bool isError = false;
	bool isEnd = false;
};

#endif
