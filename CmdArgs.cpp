#include "CmdArgs.h"
#include "FileIO.h"
#include <iostream>

using namespace std;

CmdArgs::CmdArgs(int argc, char **argv)
{
    _success = true; // Unless proven otherwise
    _unordered = false; // By default, take chronological ordering into account

    if(argc <= 1)
    {
	this->dispHelp();
	_success = false;
	return;
    }
    
    for(int i=1; i<argc; i++)
    {
	string arg = argv[i];
	if(arg == "-g")
	{
	    i++;
	    if(i == argc)
	    {
		cout << "Missing graph file after -g argument." << endl;
		_success = false;
		continue;
	    }
	    _graphFname = argv[i];
	    if(FileIO::fileExists(_graphFname)==false)
	    {
		cout << "Graph file \"" << _graphFname << "\" does not exist, or cannot be opened." << endl;
		_success = false;		
	    }
	}
	else if(arg == "-q")
	{
	    i++;
	    if(i == argc)
	    {
		cout << "Missing query graph file after -q argument." << endl;
		_success = false;
		continue;
	    }
	    _queryFnames.push_back(argv[i]);
	    if(FileIO::fileExists(_queryFnames.back())==false)
	    {
		cout << "Query file \"" << _queryFnames.back() << "\" does not exist, or cannot be opened." << endl;
		_success = false;		
	    }
	}
        else if(arg == "-qf")
        {
           i++;
	    if(i == argc)
	    {
		cout << "Missing query graph folder after -qf argument." << endl;
		_success = false;
		continue;
	    }
           _queryFnames = FileIO::getFileNames(argv[i],".gdf");
           if(_queryFnames.empty())
           {
               cout << "No files with a *.gdf extension could be found in the given folder: " << argv[i] << endl;
               _success = false;
           }
        }
	else if(arg == "-o")
	{
	    i++;
	    if(i == argc)
	    {
		cout << "Missing query out file after -o argument." << endl;
		_success = false;
		continue;
	    }
	    _outFname = argv[i];
	}
	else if(arg == "-delta")
	{
	    i++;
	    if(i == argc)
	    {
		cout << "Missing delta value after -delta argument." << endl;
		_success = false;
		continue;
	    }
	    _deltaValues.push_back(parseDuration(argv[i]));
	    if(_deltaValues.back() <= 0)
	    {
		cout << "Delta value must be an integer >= 1." << endl;
		_success = false;
	    }
	}
	else
	{
	    cout << "Unknown command line argument: " << arg << endl;
	    _success = false;
	}
    }
            
    // Set default delta value, if necessary
    if(_deltaValues.empty())
        _deltaValues.push_back(60*60*24);

    if(_graphFname.empty())
    {
	cout << "Missing graph file. Specify with the -g argument." << endl;
	_success = false;
    }
    if(_queryFnames.empty())
    {
	cout << "Missing query file. Specify with the -q argument." << endl;
	_success = false;
    }
    if(_deltaValues.size() > 1 && _outFname.empty() == false)
    {
        cout << "If using multiple delta values, you need to use the computer generated output filenames" << endl;
        _success = false;
    }
    /*if(_outFname.empty())
    {
	cout << "Missing out file. Specify with the -o argument." << endl;
	_success = false;
    }
    /*if(_queryFnames.size() < _outFnames.size())
    {
        cout << "Inconsistent number of query files (" << _queryFnames.size() << ")";
        cout << " and output file names (" << _outFnames.size() << ")" << endl;
        _success = false;
    }*/
    /*if(_queryFnames.size() > _outFnames.size())
    {
        for(int i=_outFnames.size(); i<_queryFnames.size(); i++)
        {
            string fname = this->createOutFname(_graphFname, _queryFnames[i], _deltaValues[0]);
            _outFnames.push_back(fname);
        }
    }*/
}

void CmdArgs::dispHelp() const
{
    cout << endl;
    cout << "--- dynamo_search ---" << endl;
    cout << endl;
    cout << "Graph input arguments:" << endl;
    cout << "  -g [filename]" << endl;
    cout << "       Specifies the name of the GDF graph we want to search against." << endl;
    cout << endl;
    cout << "Query input arguments:" << endl;
    cout << "  -q [filename]" << endl;
    cout << "       Specifies the GDF subgraph we want to find in our graph." << endl;
    cout << endl;
    cout << "Output arguments:" << endl;
    cout << "  -o [filename]" << endl;
    cout << "       Filename of the GDF file to save the combined results to." << endl;
    cout << "       If no name is specified, then a default filename is used, based" << endl;
    cout << "       on the input filenames and the delta value used." << endl;
    cout << endl;
    cout << "Other arguments:" << endl;
    cout << "  -delta [t]" << endl;
    cout << "       Specifies max duration (in seconds) between matched temporal edges." << endl;
    cout << "       (Default is 24 hours)." << endl;
    cout << "NOTE:" << endl;
    cout << "   As a shortcut, instead of entering duration times in seconds, you can use" << endl;
    cout << "   the following abbreviations for different time scales:" << endl;
    cout << "      w = week, d = day, h = hour, m = minute" << endl;
    cout << "   So a delta duration of one hour can be represented as 1h instead of 3600." << endl;
    cout << endl;
}

int CmdArgs::parseDuration(string str) const
{
    if(str.empty())
	return 0;
    char c = str.back();
    // If the last value is a digit, return in seconds
    if(c >= '0' && c <= '9')
	return atoi(str.c_str());
    // Get substring of everything but last value
    str = str.substr(0,str.size()-1);
    int n = atoi(str.c_str());
    // Test last letter
    if(c == 's' || c == 'S')
	return n; // seconds
    // Minutes
    n *= 60;
    if(c == 'm' || c == 'M')
	return n;
    // Hours
    n *= 60;
    if(c == 'h' || c == 'H')
	return n;
    // Days
    n *= 24;
    if(c == 'd' || c == 'D')
	return n;
    // Weeks
    n *= 7;
    if(c == 'w' || c == 'W')
	return n;
    cerr << "ERROR. Unknown suffix on duration: " << str << c << endl;
    cerr << "Possible options are:" << endl;
    cerr << "  * s = seconds" << endl;
    cerr << "  * m = minutes" << endl;
    cerr << "  * h = hours" << endl;
    cerr << "  * d = days" << endl;
    cerr << "  * w = weeks" << endl;
    return 0;
}

string CmdArgs::createOutFname(const string &gFname, const string &hFname, time_t delta)
{
    string gRoot = FileIO::getFname(gFname);
    string hRoot = FileIO::getFname(hFname);
    size_t pos = gRoot.rfind('.');
    if(pos != string::npos && pos != 0)
        gRoot = gRoot.substr(0,pos);
    pos = hRoot.rfind('.');
    if(pos != string::npos && pos != 0)
        hRoot = hRoot.substr(0,pos);
    
    string newFname = "G_" + gRoot + "_Q_" + hRoot + "_D_" + std::to_string(delta) + ".gdf";
    return newFname;
}
