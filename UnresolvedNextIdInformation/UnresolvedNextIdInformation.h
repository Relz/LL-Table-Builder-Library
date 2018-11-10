#ifndef LLTABLEVISUALIZER_UNRESOLVEDNEXTIDINFORMATION_H
#define LLTABLEVISUALIZER_UNRESOLVEDNEXTIDINFORMATION_H

#include <vector>
#include <string>

class UnresolvedNextIdInformation
{
public:
	UnresolvedNextIdInformation(std::string sequenceString, unsigned int tableRowId);

	std::string sequenceString;
	unsigned int tableRowId;
};

#endif //LLTABLEVISUALIZER_UNRESOLVEDNEXTIDINFORMATION_H
