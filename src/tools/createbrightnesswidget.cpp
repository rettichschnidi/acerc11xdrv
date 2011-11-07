/**
 * @file createpicturewidget.cpp
 */

#include "util.h"
#include "USBData.h"
#include "Exception.h"

#include <fstream>
#include <string>
#include <stdexcept>
#include <cassert>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem3;
using namespace acerc11xdrv;
using namespace acerc11xdrv::util;

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
				("level,l",boost::program_options::value<BrightnessWidgetUSBData::Brightness>(),"Brightness level. {maximum,high,medium,low}")
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
		BrightnessWidgetUSBData data;
		data.setBrightness(variablesMap["level"].as<BrightnessWidgetUSBData::Brightness>());
		acerc11xdrv::util::writeDataToFile(data, destination, false, overwrite);
	} catch (filesystem_error &e) {
		cerr << "Error: " << e.what() << endl;
		exit(1);
	} catch (Exception &e) {
		cerr << "Error: " << e.what() << endl;
		exit(1);
	}

	return 0;
}
