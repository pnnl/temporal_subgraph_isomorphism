#include "Roles.h"
#include "FastReader.h"

using namespace std;

void Roles::load(const string &fname)
{
    FastReader csv(fname);
    csv.next(); // Header
    while(csv.good())
    {
	char **vars = csv.next();
	if(csv.rowSize() < 4)
	    continue;
	_userRoles[vars[1]] = vars[2];
    }
    csv.close();
}

const string &Roles::getRole(const string &user) const
{
    auto pair = _userRoles.find(user);
    if(pair == _userRoles.end())
	return UNKNOWN;
    else
	return pair->second;
}
