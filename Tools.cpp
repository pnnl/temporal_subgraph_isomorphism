#include "Tools.h"
#include <iostream>
#include <sstream>
#include <time.h>

using namespace std;

vector<string> Tools::split(const std::string &str, char delim)
{
    vector<string> vars;
    if(str.empty())
	return vars;

    size_t pos = 0;
    while(pos != string::npos)
    {
	size_t pos2 = str.find(delim, pos);
	if(pos2 == string::npos)
	{
	    vars.push_back(str.substr(pos));
	    break;
	}
	else
	{
	    vars.push_back(str.substr(pos, pos2-pos));
	    pos = pos2+1;
	}
    }
    return vars;
}

void Tools::dispDuration(int duration)
{
    int minute = 60;
    int hour = 60 * minute;
    int day = 24 * hour;
    int week = 7 * day;
    if(duration >= week)
    {
	cout << duration / week << "w ";
	duration -= (duration / week) * week;
    }
    if(duration >= day)
    {
	cout << duration / day << "d ";
	duration -= (duration / day) * day;
    }
    if(duration >= hour)
    {
	cout << duration / hour << "h ";
	duration -= (duration / hour) * hour;
    }
    if(duration >= minute)
    {
	cout << duration / minute << "m ";
	duration -= (duration / minute) * minute;
    }
    if(duration > 0)
    {
	cout << duration << "s ";
    }
}

string Tools::getDate(time_t date, bool useHyphen)
{
    struct tm *timeStruct;
    timeStruct = gmtime(&date);

    int month = timeStruct->tm_mon + 1; // Starts with 0
    int day = timeStruct->tm_mday;
    int year = timeStruct->tm_year + 1900;

    char sep = '/';
    if(useHyphen)
	sep = '-';

    stringstream ss;
    ss << month << sep << day << sep << year;
    return ss.str();
}

/*string Tools::getFname(const string &path)
{
    int pos = path.rfind('/');
    if(pos == string::npos)
	pos = path.rfind('\\');
    if(pos == string::npos)
	return path;
    return path.substr(pos+1);
}*/

unordered_map<int,int> Tools::count(string type, const vector<GraphMatch> &subgraphs, const CertGraph &g)
{
    unordered_map<int,int> nodeCounts;
    for(const GraphMatch &sg : subgraphs)
    {
	for(int v : sg.nodes())
	{
	    if(g.getNodeType(v) == type)
	    {
		if(nodeCounts.find(v)==nodeCounts.end())
		    nodeCounts[v] = 1;
		else
		    nodeCounts[v]++;
	    }
	}
    }
    return nodeCounts;
}

int Tools::findRanking(int v, const unordered_map<int,int> &nodeCounts)
{
    auto iter = nodeCounts.find(v);
    if(iter == nodeCounts.end())
	return 0;

    int vCount = iter->second;
    int ranking = 1; // Default (highest possible)
    // See how many nodes have higher counts than our node
    for(auto pair : nodeCounts)
    {
	int count = pair.second;
	// If another node has a better count, change the ranking by one
	if(count > vCount)
	    ranking++;
    }
    return ranking;
}
