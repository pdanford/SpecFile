// SpecFile.cpp : implementation of TSpecFile class
// Copyright (c) 2013 Peter Danford
///////////////////////////////////////////////////

#include "SpecFile.h"
#include <iomanip>

TSpecFile::TSpecFile()
{
}

// **************************************************************************
TSpecFile::TSpecFile(const std::string& specFileNameWithPath)
{
    LoadFile(specFileNameWithPath);
}

// **************************************************************************
TSpecFile::TSpecFile(TSpecFile& obj)
{
    // Note that this initializes this's members twice,
    // but that is ok in this case.
    *this = obj;
}

// **************************************************************************
TSpecFile& TSpecFile::operator= (TSpecFile& rhs)
{
    // The pointers in _parameterMap are not owned by this object.
    std::map<std::string, std::stringstream*>::iterator parameterMapIter = rhs._parameterMap.begin();

    while(parameterMapIter != rhs._parameterMap.end())
        {
        // Create new stringstream object for parameter about added.
        std::stringstream* stringStreamPtr = new std::stringstream(parameterMapIter->second->str());

        // Set default format for output numbers.
        // Setting precision to 20 (doubles hold max 15 digits) will essentially keep automatic scientific
        // notation from begin written out but still retain decision process for
        // writing out integer or floating point format.
        stringStreamPtr->precision(20);

        // Add name and its value(s) to _parameterValue_Map.
        std::string paramName = parameterMapIter->first;
        std::pair<std::map<std::string, std::stringstream*>::iterator, bool> insertStatusPair1;
        insertStatusPair1 = _parameterMap.insert( std::make_pair(paramName, stringStreamPtr) );

        ++parameterMapIter;
        }

    _parameterTagLineNumberMap = rhs._parameterTagLineNumberMap;
    _commentLineNumberVec = rhs._commentLineNumberVec;
    _currentlyLoadedSpecFileNameWithPath = rhs._currentlyLoadedSpecFileNameWithPath;

    return *this;
}

// **************************************************************************
TSpecFile::~TSpecFile()
{
    Reset();
}

// **************************************************************************
void TSpecFile::Reset()
{
    // Release memory
    _parameterMapIter = _parameterMap.begin();

    while(_parameterMapIter != _parameterMap.end())
        {
        delete (*_parameterMapIter).second;

        ++_parameterMapIter;
        }

    _parameterMap.clear();
    _parameterTagLineNumberMap.clear();
    _commentLineNumberVec.clear();
}

// **************************************************************************
void TSpecFile::LoadFile(const std::string& specFileNameWithPath)
{
    Reset();

    _currentlyLoadedSpecFileNameWithPath = specFileNameWithPath;

    // Open input parameter file
    std::ifstream specFile(specFileNameWithPath.c_str(), std::ios::binary | std::ios::in);

    int tries = 3;  // Try to open up to 3 more times
    while(!specFile)
        {
        specFile.clear();
        specFile.open(specFileNameWithPath.c_str(), std::ios::binary | std::ios::in);

        --tries;
        if(tries == 0 && !specFile)
            {
            // If file cannot be opened
            std::string error_string = "TSpecFile::LoadFile(): Invalid file name for spec file > " + specFileNameWithPath;
            throw std::invalid_argument(error_string);
            }
        }
   
    // Working vars for reading ASCII file.
    std::string inputLine;

    // Read in line from file and init stringStream with prepared version of line.
    while( std::getline(specFile, inputLine) )
        {
        // Cull CR from windows created data so input can be parsed on OSX/Linux.
        size_t cr_idx = inputLine.find('\r', 0);
        if (cr_idx != std::string::npos)
            {
            inputLine.erase(cr_idx, 1);
            }
        
        // Trim any comments from inputLine (but comments are saved in the string comment).
        std::string comment;

            {
            // ***********************************************
            // ** General purpose string stream preparation **
            // ***********************************************

            std::string::size_type idx = inputLine.find("// ");
            if(idx != std::string::npos)
                {
                // Include comment's leading whitespace.
                while(idx > 0)
                    {
                    --idx;
                    if(inputLine[idx] != ' ' &&
                       inputLine[idx] != '\t')
                        {
                        ++idx;
                        break;
                        }
                    }

                // Copy comment portion to comment string;
                comment.assign(inputLine, idx, std::string::npos);

                // Truncate rest of line.
                inputLine.erase(idx);
                }

            // Trim any leading whitespaces from inputLine (primarily to detect blank lines).
            std::string::iterator pos = inputLine.begin();
            while(pos != inputLine.end())
                {
                if(*pos == ' ' || *pos == '\t')
                    {
                    pos++;
                    continue;
                    }
                break;
                }
            inputLine = std::string(pos, inputLine.end());
            }

        // Parse tag name of parameter from inputLine (if unless line is blank or comment only).
        std::string paramName;

        if(inputLine != "")
            {
            // Create string stream with read in line from file.
            std::stringstream stringStream(inputLine);

            stringStream >> paramName;

            // Remove paramName from inputLine.
            inputLine.erase(0, paramName.size());

            // Trim any leading whitespaces from inputLine (values for paramName).
            std::string::iterator pos = inputLine.begin();
            while(pos != inputLine.end())
                {
                if(*pos == ' ' || *pos == '\t')
                    {
                    pos++;
                    continue;
                    }
                break;
                }
            inputLine = std::string(pos, inputLine.end());
            }

        AddEntry(paramName, inputLine, comment);
        }
}

// **************************************************************************
void TSpecFile::AddEntry(std::string paramName, std::string value_list, std::string comment)
{
    // Only add to _parameterMap and _parameterTagLineNumberMap if there is an actual paramName to add.
    // This is to support adding comment and blank lines only.
    if(paramName != "")
        {
        // Create new stringstream object for parameter about added.
        std::stringstream* stringStreamPtr = new std::stringstream(value_list);

        // Set default format for output numbers.
        // Setting precision to 20 (doubles hold max 15 digits) will essentially keep automatic scientific
        // notation from begin written out but still retain decision process for
        // writing out integer or floating point format.
        stringStreamPtr->precision(20);

        // Add name and its value(s) to _parameterValue_Map.
        std::pair<std::map<std::string, std::stringstream*>::iterator, bool> insertStatusPair1;
        insertStatusPair1 = _parameterMap.insert( std::make_pair(paramName, stringStreamPtr) );

        if(!insertStatusPair1.second)
            {
            delete stringStreamPtr;
            throw std::runtime_error(std::string("TSpecFile::AddEntry(): Duplicate tag found -> ") + paramName);
            }

        // Save line number and order info. _commentLineNumberVec.size() is current total number of lines that exist (e.g. read in) so far.
        std::pair<std::map<int, std::string>::iterator, bool> insertStatusPair2;
        insertStatusPair2 = _parameterTagLineNumberMap.insert( std::make_pair((int)_commentLineNumberVec.size(), paramName) );

        if(!insertStatusPair2.second)
            {
            throw std::runtime_error(std::string("TSpecFile::AddEntry(): Insert failed for _parameterTagLineNumberMap"));
            }
        }

    // Add comment corresponding to this line. There will always be a comment added - even if "".
    _commentLineNumberVec.push_back(comment);
}

// **************************************************************************
std::stringstream& TSpecFile::GetParamValueStreamRefForTag(const std::string& tagName)
{
    _parameterMapIter = _parameterMap.find(tagName);

    if(_parameterMapIter == _parameterMap.end())
        {
        std::string error_string = "TSpecFile::GetParamValueStreamRefForTag(): tag " + tagName +
                                   " not in in parameter file: " + _currentlyLoadedSpecFileNameWithPath;
        throw std::invalid_argument(error_string);
        }

    // Reset positions.
    (*((*_parameterMapIter).second)).clear();  // Clear so seeks will work
    (*((*_parameterMapIter).second)).seekg(0);
    (*((*_parameterMapIter).second)).seekp(0);
    (*((*_parameterMapIter).second)).clear();  // Clear again because some libs may not remain clear after seek.

    return *((*_parameterMapIter).second);
}

// **************************************************************************
std::string& TSpecFile::GetTagInPosition(int tagPositionNumber)
{
    if(tagPositionNumber > (int)_parameterTagLineNumberMap.size()-1 || tagPositionNumber < 0)
        {
        throw std::out_of_range("TSpecFile::GetTagInPosition(): tagPositionNumber number out of bounds.");
        }

    std::map<int, std::string>::iterator _parameterTagLineNumberMapIter = _parameterTagLineNumberMap.begin();
    for(int i = 0; i < tagPositionNumber; ++i)
        {
        ++_parameterTagLineNumberMapIter;
        }

    return (*_parameterTagLineNumberMapIter).second;
}

// **************************************************************************
bool TSpecFile::DoesParameterExistInFile(const std::string& paramName)
{
    _parameterMapIter = _parameterMap.find(paramName);

    if(_parameterMapIter == _parameterMap.end())
        {
        return false;
        }

    return true;    // Exists
}

// **************************************************************************
int TSpecFile::GetNumberOfParametersSpecifiedInFile()
{
    return (int)_parameterMap.size();
}

// **************************************************************************
void TSpecFile::Save(const std::string& specFileNameWithPath, bool saveComments)
{
    if(specFileNameWithPath != "")
        {
        _currentlyLoadedSpecFileNameWithPath = specFileNameWithPath;
        }

    std::ofstream outputFile(_currentlyLoadedSpecFileNameWithPath.c_str(), std::ios::trunc | std::ios::binary | std::ios::out);

    int tries = 3;  // Try to open up to 3 more times
    while(!outputFile)
        {
        outputFile.clear();
        outputFile.open(_currentlyLoadedSpecFileNameWithPath.c_str(), std::ios::trunc | std::ios::binary | std::ios::out);

        --tries;
        if(tries == 0 && !outputFile)
            {
            // If file cannot be opened
            std::string error("TSpecFile::Save(): Can't open spec output file: " + _currentlyLoadedSpecFileNameWithPath);
            throw std::invalid_argument(error);
            }
        }

    // Get the length for the longest tag for formatting below (pretty!)
    int maxTagSize = 0;

    for(int i = 0; i < GetNumberOfParametersSpecifiedInFile(); ++i)
        {
        std::string tag = GetTagInPosition(i);

        if((int)tag.size() > maxTagSize)
            {
            maxTagSize = (int)tag.size();
            }
        }

    // Iterate through number of lines read in (i.e. all lines in _commentLineNumberVec).
    for(int i = 0; i < (int)_commentLineNumberVec.size(); ++i)
        {
        // See if there is a tag and value for this line.
        std::map<int, std::string>::iterator _parameterTagLineNumberMapIter = _parameterTagLineNumberMap.find(i);

        if(_parameterTagLineNumberMapIter != _parameterTagLineNumberMap.end())
            {
            // Output tag and its associated value.
            outputFile << std::setw(maxTagSize+2) << std::left << _parameterTagLineNumberMapIter->second << GetParamValueStreamRefForTag(_parameterTagLineNumberMapIter->second).str();

            if(!saveComments)
                {
                // endl if no comment is to follow.
                outputFile << std::endl;
                }
            }

        // This is here so comment only lines will be saved also.
        if(saveComments)
            {
            // Save any comment that may exist.
            outputFile << _commentLineNumberVec[i] << std::endl;
            }
        }
}
