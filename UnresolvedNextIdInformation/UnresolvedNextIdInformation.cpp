#include "UnresolvedNextIdInformation.h"

UnresolvedNextIdInformation::UnresolvedNextIdInformation(std::string sequenceString, unsigned int tableRowId, std::vector<std::string> actionNames)
	: sequenceString(std::move(sequenceString))
	, tableRowId(tableRowId)
	, actionNames(std::move(actionNames))
{
}
