/**
 * @file ScreenshotImportX11.h
 **/

#ifndef SCREENSHOTIMPORTX11_H_
#define SCREENSHOTIMPORTX11_H_

#include "ScreenshotImport.h"

#include <X11/Xlib.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <Magick++.h>
// needed by Eclipse for syntax completition, remove when done
#include <Magick++/Blob.h>
#include <Magick++/Image.h>
#include <boost/noncopyable.hpp>
#include <boost/smart_ptr.hpp>

namespace acerc11xdrv {
	typedef ScreenshotImport<Magick::Image> ScreenshotImportX11BaseScreenshotImport;

	class ScreenshotImportX11: public ScreenshotImportX11BaseScreenshotImport{
		public:
			typedef ScreenshotImportX11BaseScreenshotImport::SpImage SpImage;

			ScreenshotImportX11();
			virtual ~ScreenshotImportX11();
			virtual void updateScreenshot();
			virtual SpImage getImage();

		protected:
			XImage *image;
			Display *display;
	};

	class ScreenshotImportX11SHM: public ScreenshotImportX11 {
		public:
			ScreenshotImportX11SHM();
			virtual ~ScreenshotImportX11SHM();
			virtual void updateScreenshot();

		protected:
			XShmSegmentInfo shminfo;
	};
}

#endif /* SCREENSHOTIMPORTX11_H_ */
