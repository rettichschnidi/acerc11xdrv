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
			typedef std::vector<char> type_t;
			typedef type_t::size_type size_type;

			Data();
			size_type getSize() const;
			char * getDataAsCharArray() const;
			virtual ~Data()=0;
		protected:
			type_t data;

	};

	/**
	 * Abstract class for the widget data
	 */
	class WidgetData: public Data {
		public:
			WidgetData();
			virtual ~WidgetData();
		private:
			virtual void init()=0;
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

	std::istream& operator>>(std::istream& in,
			BrightnessWidgetData::Brightness& brightness);
}
#endif /* DATA_H_ */
