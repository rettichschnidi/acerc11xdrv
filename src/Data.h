/**
 * @file Data.h
 */

#ifndef DATA_H_
#define DATA_H_

#include <vector>
#include <stdint.h>

namespace acerc11xdrv {

	/**
	 * Abstract class for a data container
	 */
	class Data {
		public:
			Data();
			virtual size_t getSize() const=0;
			virtual char * getDataAsCharArray() const=0;
			virtual ~Data()=0;
	};

	/**
	 * Abstract class to store all kind of pictures (JPEG, RAW/XLIB)
	 */
	class PictureData: public Data {
		public:
			unsigned int getHeight() const=0;
			unsigned int getWidth() const=0;

			virtual ~PictureData()=0;
	};

	/**
	 * Abstract class for the widget data
	 */
	class WidgetData: public Data {
		public:
			typedef std::vector<char> type_t;
			typedef type_t::size_type size_type;

			WidgetData();
			virtual ~WidgetData();
			virtual size_t getSize() const;
			virtual char * getDataAsCharArray() const;
		private:
			virtual void init()=0;
		protected:
			type_t data;
	};

	/**
	 * Class which holds data for the picture widgets
	 */
	class PictureWidgetData: public WidgetData {
		public:
			PictureWidgetData();
			PictureWidgetData(uint32_t picture_size);
			virtual ~PictureWidgetData();
			void setPictureSize(uint32_t picture_size);
			virtual void init();
	};

	/**
	 * Class which holds data for the brightness widgets
	 */
	class BrightnessWidgetData: public WidgetData {
		public:
			enum Brightness {
				MAXIMUM, HIGH, MEDIUM, LOW
			};
			BrightnessWidgetData();
			virtual ~BrightnessWidgetData();
			virtual void init();
			void setBrightness(const Brightness &brightness);
	};

	std::istream& operator>>(std::istream& in, BrightnessWidgetData::Brightness& brightness);
}
#endif /* DATA_H_ */
