/**
 * @file ProjectorC110.h
 */
#ifndef PROJECTORC110_H_
#define PROJECTORC110_H_

#include "USB.h"
#include "USBData.h"
#include "ScreenshotImport.h"
#include "ImageMagickUSBData.h"
#include "FrameCounter.h"
#include "Projector.h"

#include <Magick++.h>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace acerc11xdrv {
	/**
	 * Projector class which supports the Acer C110
	 */
	typedef Projector<ImageMagickUSBData, Magick::Geometry, Magick::FilterTypes, Magick::Image>
			ProjectorC110BaseProjector;

	class ProjectorC110: public ProjectorC110BaseProjector {
		public:
			ProjectorC110();

		protected:
			virtual void initScreeshotSubsystem();
	};
}

#endif /* PROJECTORC110_H_ */
