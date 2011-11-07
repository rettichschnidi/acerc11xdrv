/**
 * @file Data.h
 */

#ifndef USBDATA_H_
#define USBDATA_H_

#include <vector>
#include <stdlib.h>
#include <iostream>
#include <boost/noncopyable.hpp>
#include <boost/shared_array.hpp>

namespace acerc11xdrv {

	/**
	 * Abstract class for data which eventually will get written to an USB device
	 */
	class USBData: boost::noncopyable {
		public:
			typedef boost::shared_array<char> SaCharArray;

			virtual size_t getSize() const=0;
			virtual SaCharArray getDataAsCharArray() const=0;
			virtual ~USBData();
	};

	/**
	 * Abstract class to encapsulate all kind of picture representations, e.g. imagemagick images
	 */
	class ImageUSBData: public USBData {
		public:
			virtual size_t getSize() const=0;
			virtual SaCharArray getDataAsCharArray() const=0;
	};

	/**
	 * Abstract class for the widget data
	 *
	 * We call everyting a widget that gets transfered via USB to the device and ain't
	 * part of a JPEG file. This includes control frames (brightness, reset).
	 */
	class WidgetUSBData: public USBData {
		protected:
			typedef std::vector<char> CharVector;
			CharVector data;
			virtual ~WidgetUSBData();

		public:
			explicit WidgetUSBData();
			size_t getSize() const;
			SaCharArray getDataAsCharArray() const;

		private:
			virtual void init()=0;
			WidgetUSBData(const WidgetUSBData&);
			const WidgetUSBData& operator=(const WidgetUSBData&);
	};

	/**
	 * Class which holds data for the picture widgets
	 */
	class PictureWidgetUSBData: public WidgetUSBData {
		public:
			explicit PictureWidgetUSBData();
			PictureWidgetUSBData(size_t picture_size);
			void setPictureSize(size_t picture_size);
			virtual void init();
	};

	/**
	 * Class which holds data for the brightness widgets
	 */
	class BrightnessWidgetUSBData: public WidgetUSBData {
		public:
			enum Brightness {
				MAXIMUM, HIGH, MEDIUM, LOW
			};
			explicit BrightnessWidgetUSBData();
			explicit BrightnessWidgetUSBData(const Brightness& brightness);
			virtual void init();
			void setBrightness(const Brightness& brightness);
	};

	std::istream& operator>>(std::istream& in, BrightnessWidgetUSBData::Brightness& brightness);
}
#endif /* USBDATA_H_ */
