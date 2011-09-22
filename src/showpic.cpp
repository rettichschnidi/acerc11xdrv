/**
 * @file showpic.cpp
 * @brief Simply uploads a picture to the beamer.
 */

#include <iostream>
#include <fstream>
#include <cassert>
#include <boost/program_options.hpp>

#define I_KNOW_THIS_IS_UNTESTED_CODE
#include "USB.h"

using namespace std;
using namespace DANGER_ZONE;

int main(int argc, char **argv) {
	// Process command line
	// ***********************
	// Check for needed command line arguments and process supported command line options
	boost::program_options::variables_map variablesMap;
	boost::program_options::options_description desc("Allowed options");

	try {
		desc.add_options()("source,s", boost::program_options::value<string>(), "JPEG file to read")
				("help,h", "Shows this information");

		boost::program_options::positional_options_description posOpt;
		posOpt.add("source", 1);

		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), variablesMap);
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(posOpt).run(),variablesMap);
		boost::program_options::notify(variablesMap);
	} catch (std::exception & e) {
		std::cerr << "Error in arguments parsing: " << e.what() << std::endl;
		exit(1);
	}

	if (variablesMap.count("help")) {
		cerr << desc;
		exit(0);
	}

	if (!variablesMap.count("source")) {
		cerr << desc;
		exit(1);
	}

	USB usb;
	uint16_t vendor = 0x1de1;
	uint16_t product = 0xc101;
	DeviceID devID(vendor, product);
	DeviceIDList devIdList;
	devIdList.push_back(devID);
	std::list<Device *> devlist = usb.match(devIdList);
	if (devlist.empty()) {
		cerr << "There is not device with vendorId 0x1de1 and productId 0xc101." << endl;
		exit(1);
	}
	if (devlist.size() > 1) {
		cerr << "This programm has never been tested with two devices. Be warned." << endl;
	}

	Device * dev = devlist.front();
	Configuration *config = dev->firstConfiguration();
	Interface *interface = config->firstInterface();
	AltSetting *altsetting = interface->firstAltSetting();
	Endpoint *endPoint = altsetting->firstEndpoint();
	endPoint = altsetting->nextEndpoint();

	ifstream payload(argv[1], ios::binary);

	payload.seekg(0, ios::end);
	int payload_size = payload.tellg();
	payload.seekg(0, ios::beg);
	char *payload_data = new char[payload_size];

	payload.read(payload_data, payload_size);
	assert(payload.gcount() == payload_size);

	int r = interface->claim();
	if (r) {
		cout << "Failed to claim interface: " << r << endl;
	}

	int wrote;
	r = endPoint->bulkWrite((unsigned char*) payload_data, payload_size, wrote);

	if (r) {
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
