
#ifndef FASTREADER_H
#define FASTREADER_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

/** Faster CSV reader, that has slightly less flexibility, due to 
 set limits on the number of vars and the size of the vars. */
class FastReader
{
public:
    static constexpr int MAX_VARS = 100;
    static constexpr int MAX_VAR_SIZE = 200;
    
    FastReader(char delim = ',');
    FastReader(const std::string &fname, char delim = ',');
    ~FastReader();
    void open(const std::string &fname, char delim = ',');
    bool good() const;
    char ** next();
    char ** row() const;
    int rowSize() const;
    int varSize(int var) const;
    void close();
    void dispRow() const;
private:
    char _delim;    
    char **_row;
    //std::vector<char> _buf;
    //int _bufIndex;
    int _rowSize;
    int _lineNum;
    std::ifstream _ifs;
};

#endif /* FASTREADER_H */

