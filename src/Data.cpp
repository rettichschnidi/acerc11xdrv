/**
 * @file Data.cpp
 */

#include <string>
#include <boost/assign.hpp>
#include <boost/program_options.hpp>


#include "Data.h"

using namespace std;
using namespace boost::program_options;

namespace acerc11xdrv {

	/**
	 * Default constructor for Data
	 */
	Data::Data() {
		// TODO Auto-generated constructor stub
	}

	/**
	 * Destructor for Data
	 */
	Data::~Data() {
		// TODO Auto-generated destructor stub
	}

	/**
	 * @return Size of the data in bytes
	 */
	Data::size_type Data::getSize() const {
		return data.size();
	}

	/**
	 * Get the data as char-pointer array
	 *
	 * @note Caller has to delete the array
	 * @return A copy of the data as char-pointer array
	 */
	char * Data::getDataAsCharArray() const {
		size_type size = data.size();
		char *c = new char[size];
		for (size_type i = 0; i < size; i++) {
			c[i] = data[i];
		}
		return c;
	}

	/**
	 * Default constructor for WidgetData
	 */
	WidgetData::WidgetData() {
		// TODO Auto-generated destructor stub
	}

	/**
	 * Destructor for WidgetData
	 */
	WidgetData::~WidgetData() {
		// TODO Auto-generated destructor stub
	}

	/**
	 * Default constructor for PictureWidgetData
	 */
	PictureWidgetData::PictureWidgetData() {
		init();
	}

	/**
	 * Constructor which takes the size of the picture and calls setPictureSize()
	 * @param picture_len Size of the pricture in bytes
	 */
	PictureWidgetData::PictureWidgetData(uint32_t picture_size) {
		init();
		setPictureSize(picture_size);
	}

	/**
	 * Sets the default data-value for PictureWidgetData (shared by all constructors)
	 */
	void PictureWidgetData::init() {
		using namespace boost::assign;
		data += 0x2, 0x0, 0x0, 0x0, 0x0, 0x10, 0x3e, 0x10, 0x01, 0x0, 0x0, 0x0, 0x20, 0x03, 0x0, 0x0, 0xe0, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0;
		assert(data.size() == 24);
	}

	/**
	 * Manipulates the data according to the pictures size
	 * @param picture_size Size of the pricture in bytes
	 */
	void PictureWidgetData::setPictureSize(uint32_t picture_size) {
		data[20] = picture_size & 0xff;
		picture_size = picture_size >> 8;
		data[21] = picture_size & 0xff;
		picture_size = picture_size >> 8;
		data[22] = picture_size & 0xff;
	}

	PictureWidgetData::~PictureWidgetData() {
		// TODO Auto-generated destructor stub
	}

	BrightnessWidgetData::BrightnessWidgetData() {
		init();
	}

	BrightnessWidgetData::~BrightnessWidgetData() {
		// TODO Auto-generated destructor stub
	}

	/**
	 * Sets the default data-value for BrightnessWidgetData (shared by all constructors)
	 */
	void BrightnessWidgetData::init() {
		using namespace boost::assign;
		data += 0x04, 0x00, 0x0, 0x0, 0x0, 0xc, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0;
		assert(data.size() == 24);
	}

	void BrightnessWidgetData::setBrightness(const Brightness &brightness) {
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

	std::istream& operator>>(std::istream& in,
			BrightnessWidgetData::Brightness& brightness) {
		string token;
		in >> token;
		if (token == "maximum") {
			brightness = BrightnessWidgetData::MAXIMUM;
		} else if (token == "high") {
			brightness = BrightnessWidgetData::HIGH;
		} else if (token == "medium") {
			brightness = BrightnessWidgetData::MEDIUM;
		} else if (token == "low") {
			brightness = BrightnessWidgetData::LOW;
		} else {
			throw invalid_option_value("Invalid brightness value");
		}
		return in;
	}

}
