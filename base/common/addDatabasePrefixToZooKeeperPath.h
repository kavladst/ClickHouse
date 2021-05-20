#pragma once
#include <string>

namespace DB
{

class Context;

bool addDatabasePrefixToZooKeeperPath(std::string & path, const std::string & database);
bool removeDatabasePrefixFromZooKeeperPath(std::string & path);

}