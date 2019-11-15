#ifndef ROLES__H
#define ROLES__H

#include <string>
#include <unordered_map>

class Roles
{
public:
    Roles() {}
    Roles(const std::string &fname) { this->load(fname); }
    void load(const std::string &fname);
    const std::string &getRole(const std::string &user) const;
private:
    std::unordered_map<std::string,std::string> _userRoles;
    const std::string UNKNOWN = "UNKNOWN"; // Used when requesting a user with an unknown role
};

#endif
