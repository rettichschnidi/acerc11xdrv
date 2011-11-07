/**
 * @file USBData.cpp
 */

#include "USBData.h"

#include <stdlib.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <boost/assign.hpp>
#include <boost/program_options.hpp>

using namespace std;
using namespace boost::program_options;

namespace acerc11xdrv {

	/**
	 * Empty destructor. Avoids compiler warning, that's it.
	 */
	USBData::~USBData() {
	}

	/**
	 * Default constructor for WidgetUSBData.
	 */
	WidgetUSBData::WidgetUSBData() :
		data(0) {
	}

	/**
	 * Deconstructor for WidgetUSBData.
	 */
	WidgetUSBData::~WidgetUSBData() {
	}

	/**
	 * @return Size of the data in bytes
	 */
	size_t WidgetUSBData::getSize() const {
		return data.size();
	}

	/**
	 * Get the data as char-pointer array
	 *
	 * @return A copy of the data as char-pointer array
	 */
	USBData::SaCharArray WidgetUSBData::getDataAsCharArray() const {
		SaCharArray retvalue = SaCharArray(new char[data.size()]);
		for (unsigned int i = 0; i < data.size(); i++) {
			retvalue.get()[i] = data[i];
		}
		return retvalue;
	}

	/**
	 * Default constructor for PictureWidgetUSBData
	 */
	PictureWidgetUSBData::PictureWidgetUSBData() {
		init();
	}

	/**
	 * Constructor which takes the size of the picture and sets the size of the picture
	 *
	 * @param picture_len Size of the pricture in bytes
	 */
	PictureWidgetUSBData::PictureWidgetUSBData(size_t picture_size) {
		init();
		setPictureSize(picture_size);
	}

	/**
	 * Sets the default data-value for PictureWidgetData (shared by all constructors)
	 */
	void PictureWidgetUSBData::init() {
		using namespace boost::assign;
		data += 0x2, 0x0, 0x0, 0x0, 0x0, 0x10, 0x3e, 0x10, 0x01, 0x0, 0x0, 0x0, 0x20, 0x03, 0x0, 0x0, 0xe0, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0;
		assert(data.size() == 24);
	}

	/**
	 * Manipulates the data according to the pictures size
	 *
	 * @param picture_size Size of the pricture in bytes
	 */
	void PictureWidgetUSBData::setPictureSize(size_t picture_size) {
		data[20] = picture_size & 0xff;
		picture_size = picture_size >> 8;
		data[21] = picture_size & 0xff;
		picture_size = picture_size >> 8;
		data[22] = picture_size & 0xff;
	}

	/**
	 * Default constructor for BrightnessWidgetUSBData
	 */
	BrightnessWidgetUSBData::BrightnessWidgetUSBData() {
		init();
	}

	/**
	 * Constructor for BrightnessWidgetUSBData. Sets the brightness level to the given one.
	 */
	BrightnessWidgetUSBData::BrightnessWidgetUSBData(const Brightness &brightness) {
		init();
		setBrightness(brightness);
	}

	/**
	 * Sets the default data-value for BrightnessWidgetData (shared by all constructors)
	 *
	 * So far I do not know what the values 0-11, 13-15, 17-19 and 23 represent.
	 */
	void BrightnessWidgetUSBData::init() {
		using namespace boost::assign;
		data += 0x04, 0x00, 0x0, 0x0, 0x0, 0xc, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0;
		assert(data.size() == 24);
	}

	/**
	 * Set the inner state to the requested brightness level
	 *
	 * For an Acer C110 projector, bytes 12 and 16 have to be adjusted for this.
	 *
	 * 				byte 12	|	byte 16
	 * maximum:		  0		|	  0
	 * high:		  1		|	  1
	 * medium:		  1		|	  0
	 * low:			  0		|	  1
	 */
	void BrightnessWidgetUSBData::setBrightness(const Brightness &brightness) {
		switch (brightness) {
			case MAXIMUM:
				data[12] = 0;
				data[16] = 0;
				break;
			case HIGH:
				data[12] = 1;
				data[16] = 1;
				break;
			case MEDIUM:
				data[12] = 1;
				data[16] = 0;
				break;
			case LOW:
				data[12] = 0;
				data[16] = 1;
				break;
		}
	}

	/**
	 * Defines an input stream to read brightness level. For usage with boost's program_option.
	 *
	 * @param in Inputstream
	 * @param resolution Brightness object to update with the read values
	 * @return Inputstream which we read from
	 * @exception boost::program_options::invalid_option_value Errormessage
	 */
	std::istream& operator>>(std::istream& in, BrightnessWidgetUSBData::Brightness& brightness) { //TODO: allow short forms like max/min/med/l
		using namespace acerc11xdrv;
		using namespace std;
		string token;
		in >> token;
		if (token == "maximum") {
			brightness = BrightnessWidgetUSBData::MAXIMUM;
		} else if (token == "high") {
			brightness = BrightnessWidgetUSBData::HIGH;
		} else if (token == "medium") {
			brightness = BrightnessWidgetUSBData::MEDIUM;
		} else if (token == "low") {
			brightness = BrightnessWidgetUSBData::LOW;
		} else {
			throw invalid_option_value("Invalid brightness value");
		}
		return in;
	}
}
