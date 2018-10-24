#ifndef LLTABLEVISUALIZER_UNRESOLVEDNEXTIDINFORMATION_H
#define LLTABLEVISUALIZER_UNRESOLVEDNEXTIDINFORMATION_H

#include <string>

class UnresolvedNextIdInformation
{
public:
	UnresolvedNextIdInformation(std::string sequenceString, unsigned int tableRowId, std::string actionName);

	std::string sequenceString;
	unsigned int tableRowId;
	std::string actionName;
};

#endif //LLTABLEVISUALIZER_UNRESOLVEDNEXTIDINFORMATION_H
