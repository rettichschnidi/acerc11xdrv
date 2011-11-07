/**
 * @file Projector.cpp
 */
#include "Projector.h"
#include "USB.h"
#include "Exception.h"

#include <sstream>
#include <string>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace DANGER_ZONE;

namespace acerc11xdrv {
	/**
	 * Default constructor for Projector. Subtype this class for concrete devices.
	 */
	template<typename ImageProcessorType, typename GeometryType, typename FilterType, typename ImageType>
	Projector<ImageProcessorType, GeometryType, FilterType, ImageType>::Projector() :
		max_resolution("800x480"), max_filesize(240000), vendorID(0), productID(0), current_resolution(max_resolution),
				desired_frames_per_second(60), bits_per_pixel(24), current_brightness(BrightnessWidgetUSBData::HIGH) {
		current_brightness = BrightnessWidgetUSBData::MEDIUM;
	}

	/**
	 * Destructor for Projector
	 */
	template<typename ImageProcessorType, typename GeometryType, typename FilterType, typename ImageType>
	Projector<ImageProcessorType, GeometryType, FilterType, ImageType>::~Projector() {
		//nothing to do in here
	}

	/**
	 * Claim the device and open up the endpoint so we can write to it afterwards.
	 *
	 * @throws Exception Description of the error
	 */
	template<typename ImageProcessorType, typename GeometryType, typename FilterType, typename ImageType>
	void Projector<ImageProcessorType, GeometryType, FilterType, ImageType>::initUSBSubsystem() {
		//make sure the derived class filled in a value for this fields
		assert(productID != 0);
		assert(vendorID != 0);
		DeviceID devID(vendorID, productID);
		DeviceIDList devIdList;
		devIdList.push_back(devID);
		std::list<Device *> devlist = usb.match(devIdList);
		if (devlist.empty()) {
			stringstream error;
			error << "There is not device with vendorId " << hex << "0x" << vendorID << " and productId 0x"
					<< productID << ".";
			throw Exception(error.str().c_str());
		}
		if (devlist.size() > 1) { // TODO
			throw Exception("This programm has never been tested with two devices"
				" and would just picks up the first one. Please fix this.");
		}

		device = SpDevice(devlist.front());
		config = SpConfiguration(device->firstConfiguration());
		interface = SpInterface(config->firstInterface());
		altsetting = SpAltSetting(interface->firstAltSetting());
		endPoint = SpEndpoint(altsetting->firstEndpoint());
		endPoint = SpEndpoint(altsetting->nextEndpoint()); // TODO: check if correct
		int r = interface->claim();
		if (r) {
			throw Exception("Failed to claim interface.");
		}
	}

	/**
	 * Starts the actual "beaming". Captures a screenshot and displays it on
	 * the beamer.
	 */
	template<typename ImageProcessorType, typename GeometryType, typename FilterType, typename ImageType>
	void Projector<ImageProcessorType, GeometryType, FilterType, ImageType>::run() {
		refreshPicture();
		writePicture();
		frame_counter.update();
	}

	template<typename ImageProcessorType, typename GeometryType, typename FilterType, typename ImageType>
	FrameCounter::TimeDuration Projector<ImageProcessorType, GeometryType, FilterType, ImageType>::getDurationPerFrame() {
		return frame_counter.getDurationPerFrame();
	}

	/**
	 * Updates to the most recent screenshot, resizes and transforms it.
	 */
	template<typename ImageProcessorType, typename GeometryType, typename FilterType, typename ImageType>
	void Projector<ImageProcessorType, GeometryType, FilterType, ImageType>::refreshPicture() {
		screenshotImport->updateScreenshot();
		if (imageData.use_count() == 0) {
			SpImage image = screenshotImport->getImage();
			imageData = SpImageProcessor(
					new ImageProcessor(image, current_resolution, resizeFilterType));
		} else {
			SpImage image = screenshotImport->getImage();
			imageData->updateImage(image);
		}
	}

	/**
	 * Write the most recent screenshot via USB to the beamer.
	 */
	template<typename ImageProcessorType, typename GeometryType, typename FilterType, typename ImageType>
	void Projector<ImageProcessorType, GeometryType, FilterType, ImageType>::writePicture() {
		int jpeg_size = imageData->getSize();
		int written;
		PictureWidgetUSBData widget(jpeg_size);
		int widget_size = widget.getSize();
		int retvalue = endPoint->bulkWrite((unsigned char*) widget.getDataAsCharArray().get(), widget_size, written); //FIXME: errorhandling
		assert(retvalue == 0);
		assert(written == widget_size);

		retvalue = endPoint->bulkWrite((unsigned char*) imageData->getDataAsCharArray().get(), jpeg_size, written); //FIXME: errorhandling
		assert(retvalue == 0);
		assert(written == jpeg_size);
	}

	/**
	 * Update the brightness to a new intensity.
	 *
	 * @param level New level of brightness
	 */
	template<typename ImageProcessorType, typename GeometryType, typename FilterType, typename ImageType>
	void Projector<ImageProcessorType, GeometryType, FilterType, ImageType>::setBrightness(Projector::Brightness level) {
		current_brightness = level;
		BrightnessWidgetUSBData widget(current_brightness);
		int written;
		int retvalue = endPoint->bulkWrite((unsigned char*) widget.getDataAsCharArray().get(), (int) widget.getSize(),
				written); //FIXME: errorhandling
		assert(retvalue == 0);
		assert((unsigned int)written == widget.getSize());
	}

	/**
	 * Query the current brightness level
	 *
	 * @return current brightness level
	 */
	template<typename ImageProcessorType, typename GeometryType, typename FilterType, typename ImageType>
	BrightnessWidgetUSBData::Brightness Projector<ImageProcessorType, GeometryType, FilterType, ImageType>::getBrightness() const {
		return current_brightness;
	}

	/**
	 * Update the used algorithm to resize pictures.
	 *
	 * @note Be careful which filter you select - the resulting
	 * quality and the processing speed may differ greatly.
	 * @param filterType Filtertype to be used for future processing
	 */
	template<typename ImageProcessorType, typename GeometryType, typename FilterType, typename ImageType>
	void Projector<ImageProcessorType, GeometryType, FilterType, ImageType>::setResizeFilter(Filters filterType) {
		resizeFilterType = filterType;
	}
}
