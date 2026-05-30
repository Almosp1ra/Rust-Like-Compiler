#include <iostream>
#include "SourceLocation.h"

ostream& operator<<(ostream& o, SourceLocation location)
{
	o << "(Line: " << location.line << ", Colume: " << location.column << ")";

	return o;
}