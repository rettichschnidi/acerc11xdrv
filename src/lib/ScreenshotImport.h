/**
 * @file ScreenshotImport.h
 **/

#ifndef SCREENSHOTIMPORT_H_
#define SCREENSHOTIMPORT_H_

#include <boost/noncopyable.hpp>
#include <boost/smart_ptr.hpp>

namespace acerc11xdrv {
	template<typename ImageType>
	class ScreenshotImport: boost::noncopyable {
		public:
			typedef ImageType Image;
			typedef boost::shared_ptr<Image> SpImage;

			virtual ~ScreenshotImport()=0;
			virtual void updateScreenshot()=0;
			virtual SpImage getImage()=0;
	};
}

#include "ScreenshotImport.tpp"

#endif /* SCREENSHOTIMPORT_H_ */
