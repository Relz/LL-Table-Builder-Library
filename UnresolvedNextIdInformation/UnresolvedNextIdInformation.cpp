#include "UnresolvedNextIdInformation.h"

UnresolvedNextIdInformation::UnresolvedNextIdInformation(std::string sequenceString, unsigned int tableRowId)
	: sequenceString(std::move(sequenceString))
	, tableRowId(tableRowId)
{
}
