#include "FastReader.h"
#include <iostream>

using namespace std;

FastReader::FastReader(char delim) : _delim(delim) 
{
    // Initialize 
    _lineNum = 0;
    _row = new char*[MAX_VARS];
    for(int i=0; i<MAX_VARS; i++)
    {
        _row[i] = new char[MAX_VAR_SIZE];
        _row[i][0] = '\0';
    }
}

FastReader::FastReader(const std::string& fname, char delim) : _delim(delim)
{
    // Initialize
    _lineNum = 0;
    _row = new char*[MAX_VARS];
    for(int i=0; i<MAX_VARS; i++)
    {
        _row[i] = new char[MAX_VAR_SIZE];
        _row[i][0] = '\0';
    }
    // Open file
    this->open(fname);
}

FastReader::~FastReader()
{
    this->close();
    if(_row != 0)
    {
        for(int i=0; i<MAX_VARS; i++)
        {
            if(_row[i] != 0)
            {
                delete [] _row[i];
                _row[i] = 0;
            }
        }    
        delete [] _row;
        _row = 0;
    }
}

void FastReader::open(const std::string& fname, char delim)
{    
    if(_ifs.is_open())
        _ifs.close();
    
    _delim = delim;
    _ifs.open(fname);
    /*_ifs.open(fname, ios::binary | ios::ate);
    streamsize size = _ifs.tellg();
    _ifs.seekg(0, ios::beg);
    _buf.resize(size);
    _ifs.read(_buf.data(), size);
    _bufIndex = 0;*/
}

void FastReader::close()
{
    _ifs.close();
    //_buf.clear();
}

bool FastReader::good() const 
{ 
    //return _bufIndex < _buf.size();
    return _ifs.good(); 
}

char **FastReader::row() const
{
    return _row;
}

char **FastReader::next()
{        
    _rowSize = 0;
    int varSize = 0;        
    char *var = _row[_rowSize];
    bool inQuote = false;
    _lineNum++;
    //const int bufSize = _buf.size();
    var[0] = '\0';
    bool inText = false;
    bool badVar = false;
    
    while(true)
    {       
        char c = _ifs.get();
        //char c = _buf[_bufIndex];
        //_bufIndex++;
        
        if(!_ifs.good())
        //if(_bufIndex >= bufSize)
        {   
            //cout << "[EOF]" << endl;
            var[varSize] = '\0';
            varSize = 0;
            _rowSize++;
            break;
        }
        
        if(c == '"')
        {
            inQuote = !inQuote;
            inText = inQuote;
            continue;
        }
        
        // For DOS vs Unix return lines
        if(c == '\r')
            c = _ifs.get();
        
        if(c == '\n')
        {            
            if(!inQuote)
            {
                //cout << endl;
                var[varSize] = '\0';
                varSize = 0;
                _rowSize++;
                break;
            }
            else
            {
                cout << "Line Number: " << _lineNum << endl;
                cout << "Row: ";
                this->dispRow();
                throw "Line break found inside quotation marks.";
            }
        }
        
        if(c == _delim && !inQuote)
        {
            inText = false;
	    badVar = false;
            //cout << "|" << flush;
            _rowSize++;
            var[varSize] = '\0';
            if(_rowSize >= MAX_VARS)
            {
                _rowSize = MAX_VARS;
                cout << "Line Number: " << _lineNum << endl;
                cout << "Row: ";
                this->dispRow();
                throw "Number of columns in CSV file exceeded number available for FastReader.";
            }
            var = _row[_rowSize];
            var[0] = '\0';
            varSize = 0;
        }
        else if(inText || c != ' ')
        {
            inText = true;
            //cout << c << flush;
	    if(!badVar)
	    {
                var[varSize] = c;
                varSize++;
	    }
            else if(varSize >= MAX_VAR_SIZE)
            {
                var[MAX_VAR_SIZE-1] = '\0';
                cout << "Line Number: " << _lineNum << endl;
                cout << "Row: ";
                this->dispRow();
                cout << "Variable size = " << varSize << endl;
                cout << "Max variable size = " << MAX_VAR_SIZE << endl;
                //throw "Variable size exceeded amount available for CSV FastReader.";
		cout << "Skipping remainding content of variable" << endl;
		badVar = true;
            }
        }
    }
    
    return _row;
}

int FastReader::rowSize() const { return _rowSize; }

void FastReader::dispRow() const
{
    for(int i=0; i<_rowSize-1; i++)
    {
        cout << _row[i] << ",";
    }
    cout << _row[_rowSize-1] << endl;
}
