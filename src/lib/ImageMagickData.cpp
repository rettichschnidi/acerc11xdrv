/**
 * @file ImageMagickUSBData.cpp
 */
#include "ImageMagickUSBData.h"

#include <Magick++.h>
#include <cassert>
#include <string.h>
#include <boost/program_options.hpp>

using namespace Magick;

namespace acerc11xdrv {

	/**
	 * Constructor for ImageMagickUSBData.
	 *
	 * @param source_image Imagemagick/Magick++ image
	 * @param geometry Desired resolution
	 * @param filtertype Resize algorithm to be used for resizing the image
	 * @param max_filesize Maximal allowed filesize in bytes
	 */
	ImageMagickUSBData::ImageMagickUSBData(SpImage source_image, Geometry geometry, Magick::FilterTypes filtertype,
			size_t max_filesize) :
		source_image(source_image), image_geometry(geometry), max_filesize(max_filesize), filterType(filtertype) {
		generateJPEG();
	}

	/**
	 * Update the image with a new one. Delete the old one.
	 *
	 * @param new_source_image New image
	 */
	void ImageMagickUSBData::updateImage(SpImage new_source_image) {
		source_image = new_source_image;
		generateJPEG();
	}

	/**
	 * Convert the internal representation into a JPEG, ready to be
	 * retrieved with getDataAsCharArray().
	 */
	void ImageMagickUSBData::generateJPEG() {
		if (source_image->size() != image_geometry) {
			source_image->filterType(filterType);
			source_image->resize(image_geometry);
		}
		source_image->magick("JPEG");
		source_image->write(&finished_jpeg_image);
		assert(finished_jpeg_image.length() <= max_filesize); // TODO: handle this case
	}

	/**
	 * Get the size of the current JPEG.
	 *
	 * @return Size of the JPEG in Bytes
	 */
	size_t ImageMagickUSBData::getSize() const {
		return finished_jpeg_image.length();
	}

	/**
	 * Get the JPEG data as char array.
	 *
	 * @return JPEG data
	 */
	ImageMagickUSBData::SaCharArray ImageMagickUSBData::getDataAsCharArray() const {
		boost::shared_array<char> returnvalue(new char[getSize()]);
		memcpy(returnvalue.get(), finished_jpeg_image.data(), finished_jpeg_image.length());
		return returnvalue;
	}

	/**
	 * Set the resize algorithm to be used
	 *
	 * @param type Resize algorithm to be used
	 */
	void ImageMagickUSBData::setFilterType(Magick::FilterTypes type) {
		assert(type != UndefinedFilter);
		filterType = type;
	}

	/**
	 * Defines an input stream to read in a resolution level. For usage with boost's program_option.
	 *
	 * @note Following forms are allowed for resolution:
	 * 			Width and height: 1000x2000
	 * 			Width only: 1000x
	 * 			Height only: x200
	 * @param in Inputstream
	 * @param resolution Geometry object to update with the read values
	 * @return Inputstream which we read from
	 * @exception boost::program_options::invalid_option_value Errormessage
	 */
	std::istream& operator>>(std::istream& in, Magick::Geometry& resolution) {
		unsigned int width = 0, height = 0;
		char separator = 'f';
		char firstchar = in.peek();
		if(firstchar != 'x' && firstchar != 'X') {
			in >> width;
		}
		in >> separator;
		in >> height;
		if ((width != 0 || height != 0) && (separator == 'x' || separator == 'X')) {
			resolution.height(height);
			resolution.width(width);
			return in;
		} else {
			throw invalid_option_value("Invalid resolution value");
		}
	}
}
