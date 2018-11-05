#ifndef LLPARSER_TABLEROW_H
#define LLPARSER_TABLEROW_H

#include <unordered_set>
#include <vector>
#include <string>
#include "../../TokenLibrary/Token.h"

class TableRow
{
public:
	std::unordered_set<Token> referencingSet;
	unsigned int nextId = 0;
	bool doShift = false;
	unsigned int pushToStack = 0;
	bool isError = false;
	bool isEnd = false;
	std::vector<std::string> actionNames;
};

#endif
