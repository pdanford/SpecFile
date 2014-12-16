
#include "SpecFile.h"

#include <iostream>

int main(int argc, const char * argv[])
{
    try
    {
        //Load config file.
        TSpecFile configFile("example.cfg");

        //Read in a tag who has a single value (or a tag that has multiple values but you only want the first).
        int port_number_int;
        std::string port_number_str;
        configFile.GetParamValueStreamRefForTag("PORT") >> port_number_int; //Read in as int.
        configFile.GetParamValueStreamRefForTag("PORT") >> port_number_str; //Read in as string.
        std::cout << "int version: " << port_number_int << std::endl;
        std::cout << "str version: " << port_number_str << std::endl;
        
        //Read in a tag's multiple values - numbers mixed with strings.
        int lowerBound;
        int upperBound;
        std::string units;
        configFile.GetParamValueStreamRefForTag("planet_size_range") >> lowerBound >> upperBound >> units;
        std::cout << lowerBound << " " << upperBound << " " << units << std::endl;

        //Read in a tag with a variable length parameter list.
        std::stringstream& numberStream = configFile.GetParamValueStreamRefForTag("list_of_numbers");
        while(numberStream.good())
        {
            double number;
            numberStream >> number;
            std::cout << number << " ";
        }
        std::cout << std::endl;
        
        //Read in a tag's value, increment it, save it back out.
        int currentEventCount;
        std::stringstream& currentValueSStream = configFile.GetParamValueStreamRefForTag("currentEventCount");
        currentValueSStream >> currentEventCount;
        currentValueSStream.clear();
        currentValueSStream.str("");
        currentValueSStream << ++currentEventCount;
        configFile.Save();                          //Save it back out to same file with comments intact.
        configFile.Save("new_example1.cfg");        //Or save it to a new filename with comments.
        configFile.Save("new_example2.cfg", false); //Or save it to a new filename without comments.
        
        //Add a new tag twiceAsDeliciousNumber to memory copy of new_example2.cfg and write back out with
        //a comment as new_example3.cfg.
        configFile.AddEntry("twiceAsDeliciousNumber", "6.28318530", "  //WOW!");
        configFile.Save("new_example3.cfg");
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
