SpecFile - Robust specification/configuration file parsing and modification in C++.
-----------------------------------------------------------------------------------

A TSpecFile instance reads in a specification/configuration file and makes available parameter streams with one or more possibly mixed type values by tag name. Also, parameter values may be modified and saved as well as new tags added. See SpecFile.h for more detail. Note that some TSpecFile members may throw (such as the ctor or LoadFile()), so they should be used inside try/catch blocks.

Example specification file:

	deliciousNumber  3.14159265  //I like pie.

	//Process port to listen on
	//(specify a cool port number):
	PORT  1337

	//Regular market trading hours.
	marketTimeRange	    09:30 16:00   //Eastern Time

	//Comfortable planet sizes (3 parameters for planet_size_range tag).
	planet_size_range  11000 13500 km //lowerBound upperBound units

	//Counter to keep track of the number of times The Event happens.
	currentEventCount  3

	//Variable parameter list.
	list_of_numbers  1 2 3 4.4 5.123 15

Example use (see example.cpp for more):

	//Load config file.
	TSpecFile configFile("example.cfg");

	//Read in a tag who has a single value (or a tag that has multiple values but you only want the first).
	int port_number_int;
	std::string port_number_str;
	configFile.GetParamValueStreamRefForTag("PORT") >> port_number_int; //Read in as int.
	configFile.GetParamValueStreamRefForTag("PORT") >> port_number_str; //Read in as string.

	//Add a new tag twiceAsDeliciousNumber to memory copy and save().
	configFile.AddEntry("twiceAsDeliciousNumber", "6.28318530", "  //WOW!");
	configFile.Save();

pdanford@pdanford.com - Feb, 2013
