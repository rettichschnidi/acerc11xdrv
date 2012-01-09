/**
 * @file ScreenshotImportX11.cpp
 **/

#include "ScreenshotImportX11.h"

#include <string>
#include <iostream>
#include <stdint.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <Magick++.h>

using namespace std;
using namespace Magick;

namespace acerc11xdrv {
	/**
	 * Default constructor
	 *
	 * @throws Exception Errormessage
	 */
	ScreenshotImportX11::ScreenshotImportX11() :
		image(NULL), display(NULL) {
		display = XOpenDisplay(0);
		if (display == NULL) {
			throw Exception("XOpenDisplay failed");
		}
	}

	/**
	 * Destructor for ScreenshotImportX11
	 */
	ScreenshotImportX11::~ScreenshotImportX11() {
		if (image != NULL) {
			XDestroyImage(image);
			image = NULL;
		}
		if (display != NULL) {
			XCloseDisplay(display);
			display = NULL;
		}
	}

	/**
	 * Update the internally stored image with a fresh screenshot, using
	 * the default XGetImage() function.
	 *
	 * @throw Exception Errormessage
	 */
	void ScreenshotImportX11::updateScreenshot() {
		if (image != NULL) {
			XDestroyImage(image);
		}
		image = XGetImage(display, RootWindow(display, DefaultScreen(display)), 0, 0,
				DisplayWidth(display, DefaultScreen(display)), DisplayHeight(display, DefaultScreen(display)),
				AllPlanes, ZPixmap);
		if (!image) {
			throw Exception("Could not get a screenshot with XGetImage()");
		}
	}

	/**
	 * Return the latest screenshot as an imagemagick/Magick++ image
	 * @return imagemagick/Magick++ image
	 */
	ScreenshotImportX11::SpImage ScreenshotImportX11::getImage() {
		if (!image) {
			updateScreenshot();
		}
		SpImage magickImage = SpImage(new Image(Geometry(image->width, image->height), "White"));
		int x = 0, y = 0, dx = image->width, dy = image->height;
		int offset = 0;
		PixelPacket *pixel;

		magickImage->modifyImage();
		pixel = magickImage->getPixels(x, y, dx, dy);

		int calculated_offset = 0;
		unsigned int v = 0;
		for (int y = 0; y < image->height; y++) {
			calculated_offset = offset;
			offset += image->bytes_per_line;
			for (int x = 0; x < image->width; x++) {
				// FIXME: 	breaks on many systems?
				calculated_offset += 4;
				v = *((uint32_t *) (image->data + calculated_offset));
				pixel->red = (v >> 8) & 0xffff;
				pixel->green = (v) & 0xffff;
				pixel->blue = (v << 8) & 0xffff;
				pixel++;
			}
		}
		magickImage->syncPixels();
		return magickImage;
	}

	/**
	 * Default constructor for ScreenshotImportX11SHM
	 *
	 * @throw Exception Errormessage
	 */
	ScreenshotImportX11SHM::ScreenshotImportX11SHM() {
		if (!XShmQueryExtension(display)) {
			throw Exception("SHM-Extension not available");
		}
		image = XShmCreateImage(display, DefaultVisual(display, 0), 32, ZPixmap, NULL, &shminfo,
				DisplayWidth(display, DefaultScreen(display)), DisplayHeight(display, DefaultScreen(display)));
		if (image == NULL) {
			throw Exception("XShmCreateImage failed");
		}
		/* Get the shared memory and check for errors */
		shminfo.shmid = shmget(IPC_PRIVATE, image->bytes_per_line * image->height, IPC_CREAT | 0777);
		if (shminfo.shmid < 0) {
			throw Exception("shmget() failed");
		}

		/* attach, and check for errrors */
		shminfo.shmaddr = image->data = (char *) shmat(shminfo.shmid, 0, 0);
		if (shminfo.shmaddr == (char *) -1) {
			throw Exception("shmat() failed");
		}

		/* set as read/write, and attach to the display */
		shminfo.readOnly = False;
		XShmAttach(display, &shminfo);
	}

	/**
	 * Destructor for ScreenshotImportX11SHM
	 */
	ScreenshotImportX11SHM::~ScreenshotImportX11SHM() {
		XShmDetach(display, &shminfo);
		shmdt(shminfo.shmaddr);
		if (image != NULL) {
			XDestroyImage(image);
			image = NULL;
		}
		if (display != NULL) {
			XCloseDisplay(display);
			display = NULL;
		}
	}

	/**
	 * Update the internally stored image with a fresh screenshot, using
	 * the default SHM extension.
	 *
	 * @throw Exception Errormessage
	 */
	void ScreenshotImportX11SHM::updateScreenshot() {
		XShmGetImage(display, RootWindow(display, DefaultScreen(display)), image, 0, 0, AllPlanes);
		if (image == NULL) {
			throw Exception("Could not get a screenshot with XShmGetImage().");
		}
	}
}
