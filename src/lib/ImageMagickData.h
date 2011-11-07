/**
 * @file ImageMagickUSBData.h
 */
#ifndef IMAGEMAGICKDATA_H_
#define IMAGEMAGICKDATA_H_

#include "USBData.h"

#include <Magick++.h>
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>

using namespace boost::program_options;

namespace acerc11xdrv {
	class ImageMagickUSBData: public ImageUSBData {
		public:
			typedef Magick::Image Image;
			typedef boost::shared_ptr<Image> SpImage;

			ImageMagickUSBData(SpImage source_image, Magick::Geometry geometry,
					Magick::FilterTypes filtertype = Magick::BoxFilter, size_t max_filesize = 240000);
			void updateImage(SpImage new_source_image);
			virtual size_t getSize() const;
			virtual SaCharArray getDataAsCharArray() const;
			virtual void setFilterType(Magick::FilterTypes type);

		private:
			void generateJPEG();
			Magick::Blob finished_jpeg_image;
			SpImage source_image;
			Magick::Geometry image_geometry;
			size_t max_filesize;
			Magick::FilterTypes filterType;
	};
	std::istream& operator>>(std::istream& in, Magick::Geometry& resolution);
}

#endif /* IMAGEMAGICKDATA_H_ */
