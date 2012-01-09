/**
 * @file: test_data.cpp
 */

#include <boost/filesystem.hpp>
#include <string>

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include <testing_helper.h>
#include "USBData.h"

using namespace acerc11xdrv;

class TestableWidgetData: public WidgetUSBData {
	public:
		void addData(char c) {
			data.push_back(c);
		}
		void init() {
			// dummy
		};
};

void test_WidgetData() {
	char *teststring = (char*) "abc";
	TestableWidgetData d;

	//test getSize
	ASSERT_EQUALM("Data object has size of 0 after default construction", 0, d.getSize());

	d.addData(teststring[0]);
	ASSERT_EQUALM("Data object has size of 1 after adding one element", 1, d.getSize());

	d.addData(teststring[1]);
	d.addData(teststring[2]);
	ASSERT_EQUALM("Data object has size of 3 after adding three elements", 3, d.getSize());

	//test getDataAsCharArray
	USBData::SaCharArray ret = d.getDataAsCharArray();
	USBData::SaCharArray ret2 = d.getDataAsCharArray();
	ASSERTM("Returned data has the same value as the submitted data", (teststring[0] == ret[0] )
			&& (teststring[1] == ret[1] ) && (teststring[2] == ret[2] ));
	ASSERTM("Returning individual copy on ever call to getDataAsCharArray()", ret.get() != ret2.get());
}

void test_PictureWidgetData() {
	PictureWidgetUSBData test_object;
	char expectedDefaultOrZero[] = {0x2, 0x0, 0x0, 0x0, 0x0, 0x10, 0x3e, 0x10, 0x01, 0x0, 0x0, 0x0, 0x20, 0x03, 0x0, 0x0, 0xe0, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	USBData::SaCharArray ret = test_object.getDataAsCharArray();
	for(size_t i = 0; i < test_object.getSize(); i++) {
		// no need to test the non-static bytes
		if(i == 20 || i == 21 ||i == 22) {
			continue;
		}
		ASSERT(ret[i] == expectedDefaultOrZero[i]);
	}

	test_object.setPictureSize(0);
	ret = test_object.getDataAsCharArray();
	for(size_t i = 0; i < test_object.getSize(); i++) {
		ASSERT(ret[i] == expectedDefaultOrZero[i]);
	}

	// test the maximum
	char expected0xffffff[] = {0x2, 0x0, 0x0, 0x0, 0x0, 0x10, 0x3e, 0x10, 0x01, 0x0, 0x0, 0x0, 0x20, 0x03, 0x0, 0x0, 0xe0, 0x01, 0x0, 0x0, 0xff, 0xff, 0xff, 0x0};
	test_object.setPictureSize(0xffffff);
	ret = test_object.getDataAsCharArray();
	for(size_t i = 0; i < test_object.getSize(); i++) {
		ASSERT(ret[i] == expected0xffffff[i]);
	}

	//black.jpg has a size of 6749 bytes
	char expected0x5d1a00[] = {0x2, 0x0, 0x0, 0x0, 0x0, 0x10, 0x3e, 0x10, 0x01, 0x0, 0x0, 0x0, 0x20, 0x03, 0x0, 0x0, 0xe0, 0x01, 0x0, 0x0, 0x5d, 0x1a, 0x00, 0x0};
	uint32_t filesize = boost::filesystem::file_size(std::string(PROJECT_SOURCE_DIR) + "/example-data/pictures/black.jpg");
	test_object.setPictureSize(filesize);
	ret = test_object.getDataAsCharArray();
	for(size_t i = 0; i < test_object.getSize(); i++) {
		ASSERT(ret[i] == expected0x5d1a00[i]);
	}
}

void test_BrightnessWidgetData() {
	BrightnessWidgetUSBData test_object;
	char expectedDefaultOrMax[] = {0x04, 0x00, 0x0, 0x0, 0x0, 0xc, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	USBData::SaCharArray returnValue = test_object.getDataAsCharArray();
	for(size_t i = 0; i < test_object.getSize(); i++) {
		ASSERT(returnValue[i] == expectedDefaultOrMax[i]);
	}

	test_object.setBrightness(BrightnessWidgetUSBData::MAXIMUM);
	returnValue = test_object.getDataAsCharArray();
	for(size_t i = 0; i < test_object.getSize(); i++) {
		ASSERT(returnValue[i] == expectedDefaultOrMax[i]);
	}

	test_object.setBrightness(BrightnessWidgetUSBData::HIGH);
	char expectedHigh[] = {0x04, 0x00, 0x0, 0x0, 0x0, 0xc, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	returnValue = test_object.getDataAsCharArray();
	for(size_t i = 0; i < test_object.getSize(); i++) {
		ASSERT(returnValue[i] == expectedHigh[i]);
	}

	test_object.setBrightness(BrightnessWidgetUSBData::MEDIUM);
	char expectedMedium[] = {0x04, 0x00, 0x0, 0x0, 0x0, 0xc, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	returnValue = test_object.getDataAsCharArray();
	for(size_t i = 0; i < test_object.getSize(); i++) {
		ASSERT(returnValue[i] == expectedMedium[i]);
	}

	test_object.setBrightness(BrightnessWidgetUSBData::LOW);
	char expectedLow[] = {0x04, 0x00, 0x0, 0x0, 0x0, 0xc, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	returnValue = test_object.getDataAsCharArray();
	for(size_t i = 0; i < test_object.getSize(); i++) {
		ASSERT(returnValue[i] == expectedLow[i]);
	}
}

void runSuite() {
	cute::suite s;
	s.push_back(CUTE(test_WidgetData));
	s.push_back(CUTE(test_PictureWidgetData));
	s.push_back(CUTE(test_BrightnessWidgetData));
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "test_data");
}

int main() {
	runSuite();
	return 0;
}
