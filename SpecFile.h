//SpecFile.h
//Copyright (c) 2013 Peter Danford
//////////////////////////////////////////

#if !defined(TSpecFile_h)
#define TSpecFile_h

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>

class TSpecFile
{
public:
    //specFileNameWithPath is file to parse.
    //Trims any comments (lines beginning with //)
    //Stores parameters associated with their names (tags).
    //Throws if specFileNameWithPath does not exist.
    TSpecFile(const std::string& specFileNameWithPath);

    //This version is here so this class can easily be used as a member.
    //LoadFile() will have to be called if created with this.
    TSpecFile();
    TSpecFile(TSpecFile&);
    TSpecFile& operator= (TSpecFile& right);

    virtual ~TSpecFile();

    //Returns a istringstream parameter value(s) corresponding to tagName.
    //The returned istringstream reference may contain multiple vaules to be extracted.
    //Note that the returned stream is a reference and thus can be modified. Also note
    //that the returned stream is set to the beginning of the stream at each call.
    //Throws invalid_argument if tagName is not in file.
    //**NOTE: This is not thread safe because seeks are done on the return val and
    //        also two threads cannot use the return value in parallel.
    std::stringstream& GetParamValueStreamRefForTag(const std::string& tagName);

    //Returns name of parameter (tag) associated with tagPositionNumber. Position starts at 0.
    //tagPositionNumber refers to the order the tags were encountered in specFileNameWithPath.
    //throws if tagPositionNumber is out of bounds.
    std::string& GetTagInPosition(int tagPositionNumber);

    //Returns true if paramName exists as a parameter in specFileNameWithPath.
    bool DoesParameterExistInFile(const std::string& paramName);

    //Returns total number of parameters loaded from specFileNameWithPath.
    int GetNumberOfParametersSpecifiedInFile();

    //May be called multiple times. State is completely reinitialized each time with new file.
    //Throws if specFileNameWithPath does not exist.
    //Uses AddEntry() which also can throw.
    void LoadFile(const std::string& specFileNameWithPath);

    //Saves current _parameterValue_Map under _currentlyLoadedSpecFileNameWithPath unless
    //specFileNameWithPath != "" which will override _currentlyLoadedSpecFileNameWithPath.
    //If saveComments == false, no comments from original file are written out. In both
    //cases, some minor formatting is performed to make output uniform. Note that if
    //saved with a new specFileNameWithPath, that becomes the active spec file.
    void Save(const std::string& specFileNameWithPath = "", bool saveComments = true);

    //Add new parameter name (tag) and its associated value to _parameterValue_Map (if not "").
    //comment is append at end of line. Throws if tag already exists. If paramName == "", only a
    //comment is entered for the current line number (Note that comments should start with //).
    void AddEntry(std::string paramName, std::string value_list, std::string comment = "");

    //Releases allocated memory and puts object in clean state.
    void Reset();

private:
    //Stores parameters and associated value streams.
    std::map<std::string, std::stringstream*> _parameterMap;
    std::map<std::string, std::stringstream*>::iterator _parameterMapIter;

    //Used to preserve order tags are read in as well as line numbers they occur on.
    std::map<int, std::string> _parameterTagLineNumberMap;

    //Used to record all comments in file (even blank ones).
    std::vector<std::string> _commentLineNumberVec;

    std::string _currentlyLoadedSpecFileNameWithPath;
};

#endif
