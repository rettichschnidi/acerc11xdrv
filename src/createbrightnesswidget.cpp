/**
 * @file createpicturewidget.cpp
 */

#include <fstream>
#include <string>
#include <stdexcept>
#include <cassert>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "util.h"
#include "Data.h"

using namespace std;
using namespace boost::filesystem3;
using namespace acerc11xdrv;

int main(int argc, char **argv) {
	bool overwrite;
	string destination;

	// Process command line
	// ***********************
	// Check for needed command line arguments and process supported command line options
	boost::program_options::variables_map variablesMap;
	boost::program_options::options_description desc("Allowed options");

	try {
		desc.add_options()
				("destination,d",boost::program_options::value<string>(&destination)->default_value("widget"),"Outputfile. Default is \"widget\".")
				("level,l",boost::program_options::value<BrightnessWidgetData::Brightness>(),"Brightness level. {maximum,high,medium,low}")
				("overwriteIfExisting,o", "Try to overwrite the destination file if existing")
				("help,h","Shows this information");

		boost::program_options::positional_options_description posOpt;
		posOpt.add("level", 1).add("destination", 1).add("overwriteDestinationIfExisting", 1);

		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc),	variablesMap);
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(posOpt).run(), variablesMap);
		boost::program_options::notify(variablesMap);
	} catch (std::exception & e) {
		std::cerr << "Error in arguments parsing: " << e.what() << std::endl;
		exit(1);
	}

	if (variablesMap.count("help")) {
		cerr << desc;
		exit(0);
	}

	if (!variablesMap.count("level")) {
		cerr << desc;
		exit(1);
	}

	overwrite = variablesMap.count("overwriteIfExisting");

	try {
		BrightnessWidgetData data;
		data.setBrightness(variablesMap["level"].as<BrightnessWidgetData::Brightness>());
		acerc11xdrv::util::writeDataToFile(data, destination, false, overwrite);
	} catch (filesystem_error &e) {
		cerr << "Error: " << e.what() << endl;
		exit(1);
	} catch (string &e) {
		cerr << "Error: " << e << endl;
		exit(1);
	}

	return 0;
}
