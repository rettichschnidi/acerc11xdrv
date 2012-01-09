/**
 * @file getScreenshot.cpp
 **/
#include "ScreenshotImportX11.h"
#include "ImageMagickUSBData.h"
#include "util.h"
#include "USBData.h"
#include "USB.h"
#include "Exception.h"

#include <iostream>
#include <string>
#include <iomanip>

#include <boost/program_options.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <sys/shm.h>
#include <X11/Xutil.h>
#include <Magick++.h>

#define NUMBER_OF_FILTERS_AS_THE_MAGICK_SOURCECODES_STATES 22

using namespace std;
using namespace acerc11xdrv;
using namespace acerc11xdrv::util;
using namespace Magick;
using namespace DANGER_ZONE;

int main(int argc, char **argv) {

	// Process command line
	// ***********************
	// Check for needed command line arguments and process supported command line options
	boost::program_options::variables_map variablesMap;
	boost::program_options::options_description desc("Allowed options");

	unsigned int frames;
	string filenameprefix;
	string mode; //TODO: needs a validator
	string imageformat; //TODO: needs a validator
	Magick::Geometry resolution;
	bool nofurtherproceeding;
	bool noresize;
	bool nowrite;
	bool inmemoryencoding;
	bool dobenchmark;
	bool dofullbenchmark;

	try {
		desc.add_options()
				("frames,f", boost::program_options::value<unsigned int>(&frames)->default_value(1), "Number of pictures to take, must be greater than zero.")
				("mode,m", boost::program_options::value<string>(&mode)->default_value("shm"),"Specify \"X11\" to use XGetImage() or \"shm\" to use the MIT-SHM extension to retrieve a screenshot")
				("prefix,p", boost::program_options::value<string>(&filenameprefix)->default_value("screenshot"), "Specify a prefix for the filenames")
				("resolution,r", boost::program_options::value<Magick::Geometry>(&resolution)->default_value(Magick::Geometry("800x480"), "800x480"), "Specify the resolution")
				("imageformat,i", boost::program_options::value<string>(&imageformat)->default_value("JPEG"), "Specify the desired compression format")
				("nofurtherproceeding,c", "Do not resize nor save the screenshot. Usefull to \"benchmark\" the screenshot module.")
				("noresize,e", "Do not resize the screenshot.")
				("nowrite,w", "Do not write out the screenshot.")
				("inmemoryencoding,n", "Encode image in memory. Does not apply if image gets written to disc.")
				("dobenchmark,b", "Test default imagefilter and print out much time it requires.")
				("dofullbenchmark", "Test all available imagefilters and print out how much time each requires.")
				("help,h","Shows this information");

		boost::program_options::positional_options_description posOpt;

		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), variablesMap);
		boost::program_options::store(
				boost::program_options::command_line_parser(argc, argv).options(desc).positional(posOpt).run(),
				variablesMap);
		boost::program_options::notify(variablesMap);
	} catch (std::exception & e) {
		std::cerr << "Error in arguments parsing: " << e.what() << std::endl;
		exit(1);
	}

	if (variablesMap.count("help") || frames == 0) {
		cerr << desc;
		exit(0);
	}

	nofurtherproceeding = variablesMap.count("nofurtherproceeding");
	noresize = variablesMap.count("noresize");
	nowrite = variablesMap.count("nowrite");
	inmemoryencoding = variablesMap.count("inmemoryencoding");
	dobenchmark = variablesMap.count("dobenchmark");
	dofullbenchmark = variablesMap.count("dofullbenchmark");

	ScreenshotImportX11* import;
	if(mode == "SHM" || mode == "shm") { // TODO: this _really_ needs a a better solution
		try {
			import = new ScreenshotImportX11SHM;
		} catch (acerc11xdrv::Exception &e) {
			cout << "Error: " << e.what() << endl;
			exit(1);
		}
	}
	else {
		try {
			import = new ScreenshotImportX11;
		} catch (acerc11xdrv::Exception &e) {
			cout << "Error: " << e.what() << endl;
			exit(1);
		}
	}

	boost::shared_ptr<Magick::Image> image;
	Blob blob;

	FilterTypes currentFilter = BoxFilter;

	boost::posix_time::time_duration duration;
	boost::posix_time::ptime start, end, lastTimestamp;

	try {
		while(currentFilter < NUMBER_OF_FILTERS_AS_THE_MAGICK_SOURCECODES_STATES) {
			start = boost::posix_time::microsec_clock::local_time();
			for (unsigned int i = 0; i < frames; i++) {
				lastTimestamp = boost::posix_time::microsec_clock::local_time();
				stringstream filename;
				filename << filenameprefix << "-" << setw(5) << setfill('0') << i << "." << imageformat;
				if(dobenchmark || dofullbenchmark) {
					lastTimestamp = boost::posix_time::microsec_clock::local_time();
				}
				import->updateScreenshot();
				if(dobenchmark || dofullbenchmark) {
					cout << "updateScreenshot: " << boost::posix_time::microsec_clock::local_time() - lastTimestamp << endl;
					lastTimestamp = boost::posix_time::microsec_clock::local_time();
				}
				if (!nofurtherproceeding) {
					image = import->getImage();
					if(dobenchmark || dofullbenchmark) {
						cout << "getimage: " << boost::posix_time::microsec_clock::local_time() - lastTimestamp << endl;
						lastTimestamp = boost::posix_time::microsec_clock::local_time();
					}
					if(!noresize) {
						image->filterType(currentFilter);
						image->resize(resolution);
						if(dobenchmark || dofullbenchmark) {
							cout << "resize: " << boost::posix_time::microsec_clock::local_time() - lastTimestamp << endl;
							lastTimestamp = boost::posix_time::microsec_clock::local_time();
						}
					}
					if(!nowrite) {
						image->magick(imageformat);
						image->write(filename.str());
					}
					if(inmemoryencoding && nowrite) {
						if(dobenchmark || dofullbenchmark) {
							lastTimestamp = boost::posix_time::microsec_clock::local_time();
						}
						image->magick(imageformat);
						image->write(&blob);
						if(dobenchmark || dofullbenchmark) {
							cout << "encoding: " << boost::posix_time::microsec_clock::local_time() - lastTimestamp << endl;
							lastTimestamp = boost::posix_time::microsec_clock::local_time();
						}
					}
				}
			}
			end = boost::posix_time::microsec_clock::local_time();
			duration = end - start;
			if(dobenchmark || dofullbenchmark) {
				cout << "FilterType #" << (int)currentFilter << ": " << duration/frames << " <--\n";
				currentFilter = static_cast<FilterTypes>(currentFilter+1);
			}
			if(!dofullbenchmark) {
				break;
			}
		}
	} catch(acerc11xdrv::Exception &e) {
		cout << "Error: " << e.what() << endl;
		exit(2);
	}
	cout << endl;
	delete import;
	return 0;
}
