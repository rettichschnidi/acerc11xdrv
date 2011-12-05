/**
 * @file ProjectorC110.cpp
 */
#include "Projector.h"
#include "ProjectorC110.h"
#include "USB.h"
#include "ScreenshotImportX11.h"
#include "ImageMagickUSBData.h"
#include "Exception.h"

#include <sstream>
#include <string>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace DANGER_ZONE;

namespace acerc11xdrv {
	/**
	 * Default constructor for ProjectorC110.
	 */
	ProjectorC110::ProjectorC110() : ProjectorC110BaseProjector(Magick::BoxFilter) {
		vendorID = 0x1de1;
		productID = 0xc101;
		initUSBSubsystem();
		initScreeshotSubsystem();
	}

	/**
	 * Instantiates/Initializes the best available screenshot solution
	 */
	void ProjectorC110::initScreeshotSubsystem() {
		bool success = false;
		if (!success) {
			try {
				screenshotImport = SpScreenshotImporter(new ScreenshotImportX11SHM);
				success = true;
			} catch (Exception& error) {
				//success = false;
			}
		}
		if (!success) {
			try {
				screenshotImport = SpScreenshotImporter(new ScreenshotImportX11);
				success = true;
			} catch (Exception& error) {
				//success = false;
			}
		}
		if (success) {
			return;
		}
		throw Exception("No usable usable screenshot subsystem found");
	}
}
