/**
 * @file: test_data.cpp
 */

#include <boost/filesystem.hpp>
#include <string>

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "testing_helper.h"
#include "Data.h"

using namespace acerc11xdrv;

class TestableData: public Data {
	public:
		void addData(char c) {
			data.push_back(c);
		}
};

void test_Data() {
	char *teststring = (char*) "abc";
	TestableData d;

	//test getSize
	ASSERT_EQUALM("Data object has size of 0 after default construction", 0, d.getSize());

	d.addData(teststring[0]);
	ASSERT_EQUALM("Data object has size of 1 after adding one element", 1, d.getSize());

	d.addData(teststring[1]);
	d.addData(teststring[2]);
	ASSERT_EQUALM("Data object has size of 3 after adding three elements", 3, d.getSize());

	//test getDataAsCharArray
	char *ret = d.getDataAsCharArray();
	char *ret2 = d.getDataAsCharArray();
	ASSERTM("Returned data has the same value as the submitted data", (teststring[0] == ret[0] )
			&& (teststring[1] == ret[1] ) && (teststring[2] == ret[2] ));
	ASSERTM("Returned data is a copy of the submitted data", teststring != ret);
	ASSERTM("Data returns a fresh copy every time getDataAsCharArray gets called", ret2 != ret);
	delete ret;
	delete ret2;
}

void test_PictureWidgetData() {
	PictureWidgetData test_object;
	char expectedDefaultOrZero[] = {0x2, 0x0, 0x0, 0x0, 0x0, 0x10, 0x3e, 0x10, 0x01, 0x0, 0x0, 0x0, 0x20, 0x03, 0x0, 0x0, 0xe0, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	char * ret = test_object.getDataAsCharArray();
	for(PictureWidgetData::size_type i = 0; i < test_object.getSize(); i++) {
		// no need to test the non-static bytes
		if(i == 20 || i == 21 ||i == 22) {
			continue;
		}
		ASSERT(ret[i] == expectedDefaultOrZero[i]);
	}
	delete ret;

	test_object.setPictureSize(0);
	ret = test_object.getDataAsCharArray();
	for(PictureWidgetData::size_type i = 0; i < test_object.getSize(); i++) {
		ASSERT(ret[i] == expectedDefaultOrZero[i]);
	}
	delete ret;

	// test the maximum
	char expected0xffffff[] = {0x2, 0x0, 0x0, 0x0, 0x0, 0x10, 0x3e, 0x10, 0x01, 0x0, 0x0, 0x0, 0x20, 0x03, 0x0, 0x0, 0xe0, 0x01, 0x0, 0x0, 0xff, 0xff, 0xff, 0x0};
	test_object.setPictureSize(0xffffff);
	ret = test_object.getDataAsCharArray();
	for(PictureWidgetData::size_type i = 0; i < test_object.getSize(); i++) {
		ASSERT(ret[i] == expected0xffffff[i]);
	}
	delete ret;

	//black.jpg has a size of 6749 bytes
	char expected0x5d1a00[] = {0x2, 0x0, 0x0, 0x0, 0x0, 0x10, 0x3e, 0x10, 0x01, 0x0, 0x0, 0x0, 0x20, 0x03, 0x0, 0x0, 0xe0, 0x01, 0x0, 0x0, 0x5d, 0x1a, 0x00, 0x0};
	uint32_t filesize = boost::filesystem::file_size(std::string(PROJECT_SOURCE_DIR) + "/example-data/pictures/black.jpg");
	test_object.setPictureSize(filesize);
	ret = test_object.getDataAsCharArray();
	for(PictureWidgetData::size_type i = 0; i < test_object.getSize(); i++) {
		ASSERT(ret[i] == expected0x5d1a00[i]);
	}
	delete ret;
}

void test_BrightnessWidgetData() {
	BrightnessWidgetData test_object;
	char expectedDefaultOrMax[] = {0x04, 0x00, 0x0, 0x0, 0x0, 0xc, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	char * returnValue = test_object.getDataAsCharArray();
	for(PictureWidgetData::size_type i = 0; i < test_object.getSize(); i++) {
		ASSERT(returnValue[i] == expectedDefaultOrMax[i]);
	}
	delete returnValue;

	test_object.setBrightness(BrightnessWidgetData::MAXIMUM);
	returnValue = test_object.getDataAsCharArray();
	for(PictureWidgetData::size_type i = 0; i < test_object.getSize(); i++) {
		ASSERT(returnValue[i] == expectedDefaultOrMax[i]);
	}
	delete returnValue;

	test_object.setBrightness(BrightnessWidgetData::HIGH);
	char expectedHigh[] = {0x04, 0x00, 0x0, 0x0, 0x0, 0xc, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	returnValue = test_object.getDataAsCharArray();
	for(PictureWidgetData::size_type i = 0; i < test_object.getSize(); i++) {
		ASSERT(returnValue[i] == expectedHigh[i]);
	}
	delete returnValue;

	test_object.setBrightness(BrightnessWidgetData::MEDIUM);
	char expectedMedium[] = {0x04, 0x00, 0x0, 0x0, 0x0, 0xc, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	returnValue = test_object.getDataAsCharArray();
	for(PictureWidgetData::size_type i = 0; i < test_object.getSize(); i++) {
		ASSERT(returnValue[i] == expectedMedium[i]);
	}
	delete returnValue;

	test_object.setBrightness(BrightnessWidgetData::LOW);
	char expectedLow[] = {0x04, 0x00, 0x0, 0x0, 0x0, 0xc, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
	returnValue = test_object.getDataAsCharArray();
	for(PictureWidgetData::size_type i = 0; i < test_object.getSize(); i++) {
		ASSERT(returnValue[i] == expectedLow[i]);
	}
	delete returnValue;
}

void runSuite() {
	cute::suite s;
	s.push_back(CUTE(test_Data));
	s.push_back(CUTE(test_PictureWidgetData));
	s.push_back(CUTE(test_BrightnessWidgetData));
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "test_data");
}

int main() {
	runSuite();
	return 0;
}
