#ifndef CMDARGS_H
#define CMDARGS_H

#include <string>
#include <vector>
#include <time.h>

/**
 * Our custom command line argument parsing class.
 */
class CmdArgs
{
public:
    CmdArgs(int argc, char **argv);
    //const std::string &queryFname() const { return _queryFname; }    
    //time_t delta() const { return _delta; }
    const std::string &graphFname() const { return _graphFname; }
    const std::vector<std::string> &queryFnames() const { return _queryFnames; }
    const std::string &outFname() const { return _outFname; }
    //const std::vector<std::string> &outFnames() const { return _outFnames; }
    const std::vector<time_t> &deltaValues() const { return _deltaValues; }    
    bool success() const { return _success; }
    bool unordered() const { return _unordered; }
    void dispHelp() const;
     /** Parses the time in the string as seconds. Makes putting in long durations
     * less painful on the command line.
     * @param str  String containing time. 
     * If it's just a number (or ends with 's'), treat it as seconds.
     * If it ends with 'm', treat it as minutes.
     * If it ends with 'h', treat it as hours.
     * If it ends with 'd', treat it as days.
     * If it ends with 'w', treat it as weeks.*/
    int parseDuration(std::string str) const;
    /**
     * Creates a default output file name based on the input filenames and the delta used.
     * @param gFname  The data graph we are searching against.
     * @param hFname  The query graph we are looking for.
     * @param delta  Time in seconds that the query graph must take place over.
     * @return New unique filename based on the input parameters.
     */
    std::string createOutFname(const std::string &gFname, const std::string &hFname, time_t delta);
private:
    std::string _graphFname, _outFname; // _queryFname
    std::vector<std::string> _queryFnames; // _outFnames;
    std::vector<time_t> _deltaValues;
    time_t _delta;
    bool _success, _unordered;
};

#endif
