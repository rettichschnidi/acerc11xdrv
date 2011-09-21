/**
 * @file showpic.cpp
 * @brief Simply uploads a picture to the beamer.
 */

#include <iostream>
#include <fstream>
#include <cassert>

#define I_KNOW_THIS_IS_UNTESTED_CODE
#include "USB.h"

using namespace std;
using namespace DANGER_ZONE;

int main(int argc, char **argv) {

	USB usb;
	uint16_t vendor = 0x0483;
	uint16_t product = 0x2016;
	DeviceID devID(vendor, product);
	DeviceIDList devIdList;
	devIdList.push_back(devID);
	std::list<Device *> devlist = usb.match(devIdList);
	cout << "devlist.size(): " << devlist.size() << endl;
	cout << "usb.size(): " << usb.size() << endl;

	Device * dev = devlist.front();
	cout << "dev->numConfigurations(): " << (int)dev->numConfigurations() << endl;
	Configuration *config = dev->firstConfiguration();
	cout << "config->numInterfaces(): " << (int)config->numInterfaces() << endl;
	Interface *interface = config->firstInterface();
	cout << "interface->numAltSettings(): " << (int)interface->numAltSettings() << endl;
	AltSetting *altsetting = interface->firstAltSetting();
	cout << "altsetting->numEndpoints(): " << (int)altsetting->numEndpoints() << endl;
	for(AltSetting::const_iterator acsIter = altsetting->begin(); acsIter != altsetting->end(); acsIter++) {
		cout << "Endpoint: " << endl;
		(*acsIter)->dumpDescriptor();
	}

	Endpoint *endPoint = altsetting->firstEndpoint();

	if (argc != 2) {
		cout << "Usage: USB-LIBUSB INPUTFILE" << endl;
		return 1;
	}
//	libusb_device **devices;
//	libusb_context *ctx = NULL;
//	int ret = 0;
//	ssize_t cnt;
//	ret = libusb_init(&ctx);
//	if (ret < 0) {
//		cout << "FAIL @INIT: " << ret;
//		return false;
//	}
//	libusb_set_debug(ctx, 3);
//	cnt = libusb_get_device_list(ctx, &devices);
//	cout << "# of devices: " << cnt << endl;
//
//	libusb_device_handle *dev_handle = libusb_open_device_with_vid_pid(ctx,
//			0x1de1, 0xc101); //these are vendorID and productID I found for my usb device
//	if (dev_handle == NULL)
//		cout << "Cannot open device" << endl;
//	else
//		cout << "Device Opened" << endl;
//	libusb_free_device_list(devices, 1); //free the list, unref the devices in it


	ifstream payload(argv[1], ios::binary);

	payload.seekg(0, ios::end);
	int payload_size = payload.tellg();
	payload.seekg(0, ios::beg);
	char *payload_data = new char[payload_size];

	payload.read(payload_data, payload_size);
	assert(payload.gcount() == payload_size);

	int r = interface->claim();
	if(r) {
		cout << "Failed to claim interface: " << r << endl;
	}

	int wrote;
	r = endPoint->bulkWrite((unsigned char*)payload_data, payload_size, wrote);

	if(r) {
		cout << "Could not write data" << endl;
	} else if (wrote != payload_size)
		cout << "Error while writing PAYLOAD: Actual = " << wrote << endl;
	else {
		cout << "Writing PAYLOAD was successfully!" << endl;
	}
	interface->release();


//	int actual; //used to find out how many bytes were written
//	if (libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
//		cout << "Kernel Driver Active" << endl;
//		if (libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
//			cout << "Kernel Driver Detached!" << endl;
//		else {
//			cout << "Failed to remove the kernel driver" << endl;
//			return 1;
//		}
//	} else
//		cout << "No kernel driver is sitting on this device." << endl;
//
//	ret = libusb_claim_interface(dev_handle, 0);
//	if (ret < 0) {
//		cout << "Cannot Claim Interface" << endl;
//		return 1;
//	}
//	cout << "Claimed Interface" << endl;
//	cout << "Writing PAYLOAD Data..." << endl;
//
//	//-------------RESET------------------
//	ret = libusb_bulk_transfer(dev_handle, (1 | LIBUSB_ENDPOINT_OUT),
//			(unsigned char*) payload_data, playload_size, &actual, 0);
//	if (ret == 0 && actual == playload_size)
//		cout << "Writing PAYLOAD successful!" << endl;
//	else
//		cout << "Write PAYLOAD error: Actual = " << actual << endl;
//
//	ret = libusb_release_interface(dev_handle, 0); //release the claimed interface
//	if (ret != 0) {
//		cout << "Cannot Release Interface" << endl;
//		return 1;
//	}
//	cout << "Released Interface" << endl;
//
//	libusb_close(dev_handle); //close the device we opened
//	libusb_exit(ctx); //needs to be called to end the

	return true;
}
