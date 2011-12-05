/**
 * @file Projector.h
 */
#ifndef PROJECTOR_H_
#define PROJECTOR_H_

#include "USB.h"
#include "USBData.h"
#include "FrameCounter.h"
#include "ScreenshotImport.h"

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace acerc11xdrv {

	/**
	 * Abstract base class for all projector devices.
	 */
	template<typename ImageProcessorType, typename GeometryType, typename FilterType, typename ImageType>
	class Projector {
		public:
			typedef BrightnessWidgetUSBData::Brightness Brightness;
			typedef FrameCounter::TimeDuration TimeDuration;
			typedef FilterType Filters;
			typedef GeometryType Geometry;
			typedef ImageProcessorType ImageProcessor;
			typedef ImageType Image;
			typedef boost::shared_ptr<ImageType> SpImage;

			Projector(Filters resizeFilterType);
			virtual ~Projector();
			void run();
			void setBrightness(Brightness level);
			Brightness getBrightness() const;
			TimeDuration getDurationPerFrame();

		protected:
			typedef boost::shared_ptr<ImageProcessor> SpImageProcessor;
			typedef boost::shared_ptr<ScreenshotImport<ImageType> > SpScreenshotImporter;
			typedef boost::shared_ptr<DANGER_ZONE::Device> SpDevice;
			typedef boost::shared_ptr<DANGER_ZONE::Configuration> SpConfiguration;
			typedef boost::shared_ptr<DANGER_ZONE::Interface> SpInterface;
			typedef boost::shared_ptr<DANGER_ZONE::AltSetting> SpAltSetting;
			typedef boost::shared_ptr<DANGER_ZONE::Endpoint> SpEndpoint;

			void initUSBSubsystem();
			virtual void initScreeshotSubsystem()=0;

			Geometry max_resolution;
			size_t max_filesize;
			Filters resizeFilterType;
			uint16_t vendorID;
			uint16_t productID;
			DANGER_ZONE::USB usb;
			SpDevice device;
			SpConfiguration config;
			SpEndpoint endPoint;
			SpInterface interface;
			SpAltSetting altsetting;
			SpScreenshotImporter screenshotImport;
			SpImageProcessor imageData;

		private:
			void refreshPicture();
			void writePicture();
			void setResizeFilter(Filters filterType);

			Geometry currentResolution;
			unsigned int desiredFramesPerSecond;
			unsigned int bitsPerPixel;
			Brightness currentBrightness;
			FrameCounter frameCounter;
	};
}

#include "Projector.tpp"

#endif /* PROJECTOR_H_ */
