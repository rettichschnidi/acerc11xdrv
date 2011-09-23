/**
 * @file ImageMagickData.h
 */
#ifndef IMAGEMAGICKDATA_H_
#define IMAGEMAGICKDATA_H_

#include <Magick++.h>

#include "Data.h"

namespace acerc11xdrv {

	class ImageMagickData: public ImageData {
		public:
			ImageMagickData();
			virtual ~ImageMagickData();
	};

}

#endif /* IMAGEMAGICKDATA_H_ */
