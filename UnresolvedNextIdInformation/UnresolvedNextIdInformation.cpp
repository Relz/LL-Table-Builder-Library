#include "UnresolvedNextIdInformation.h"

UnresolvedNextIdInformation::UnresolvedNextIdInformation(std::string sequenceString, unsigned int tableRowId, std::string actionName)
	: sequenceString(std::move(sequenceString))
	, tableRowId(tableRowId)
	, actionName(std::move(actionName))
{

}
