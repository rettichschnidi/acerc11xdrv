/**
 * @file USB.cpp
 * @brief Classes to access Universal Serial Bus devices
 *
 * The USB Namespace provides a number of classes to work
 * with Universal Serial Bus (USB) devices attached to the
 * system.
 *
 * @author Brad Hards, adapted for libusb-1.0: Reto Schneider
 */

#include "USB.h"

#include <errno.h>
#include <cstdlib>
#include <stdio.h>
#include <cassert>
//remove after debugging
#include <iostream>

#include <libusb-1.0/libusb.h>

namespace DANGER_ZONE {

	USB::USB(void) :
		ctx(NULL) {
		int r = libusb_init(&ctx);
		if (r < 0) {
			throw USBError("Could not init USB");
		}
		rescan();
	}

	USB::~USB() {
		libusb_free_device_list(devices, 1);
		libusb_exit(ctx);
	}

	void USB::rescan(void) {
		ssize_t devices_count = libusb_get_device_list(ctx, &devices);

		clear();

		for (ssize_t i = 0; i < devices_count; i++) {
			push_back(new Device(devices[i]));
		}
	}

	std::list<Device *> USB::match(u_int8_t class_code) {
		std::list<Device *> match_list;
		std::list<Device *>::const_iterator diter;

		for (diter = begin(); diter != end(); diter++) {
			if ((*diter)->devClass() == class_code) {
				match_list.push_back(*diter);
			}
		}
		return match_list;
	}

	std::list<Device *> USB::match(DeviceIDList devList) {
		std::list<Device *> match_list;
		std::list<Device *>::const_iterator diter;

		for (diter = begin(); diter != end(); diter++) {
			DeviceIDList::const_iterator deviceIdIterator;
			for (deviceIdIterator = devList.begin(); deviceIdIterator != devList.end(); deviceIdIterator++) {
				if ((deviceIdIterator->product() == (*diter)->idProduct()) && (deviceIdIterator->vendor()
						== (*diter)->idVendor())) {
					match_list.push_back(*diter);
				}
			}
		}
		return match_list;
	}

	Device::Device(libusb_device *device) :
		m_handle(NULL) {
		int r = libusb_get_device_descriptor(device, &m_descriptor);
		if (r < 0) {
			throw USBError("Could not get device descriptor");
		}
		r = libusb_open(device, &m_handle);
		if (r < 0) {
			throw USBError("Could not open device");
		}

		for (u_int8_t i = 0; i < numConfigurations(); i++) {
			libusb_config_descriptor *config_descriptor;
			r = libusb_get_config_descriptor(device, i, &config_descriptor);
			if (r < 0) {
				throw USBError("Could not open config descriptor");
			}
			push_back(new Configuration(config_descriptor, this));
		}
	}

	Device::~Device(void) {
		libusb_close(m_handle);
	}

	int Device::string(std::string &buf, int index, u_int16_t langID) { // FIXME: update the buf
		int retval;
		unsigned char tmpBuff[256];

		if (0 == langID) {
			/* we want the first lang ID available, so find out what it is */
			retval = libusb_get_string_descriptor(m_handle, 0, 0, tmpBuff, sizeof(tmpBuff));
			buf = std::string((const char*)tmpBuff);
			if (retval < 0)
				return retval;

			if (retval < 4 || tmpBuff[1] != LIBUSB_DT_STRING)
				return -EIO;

			langID = tmpBuff[2] | (tmpBuff[3] << 8);
		}

		retval = libusb_get_string_descriptor(m_handle, index, langID, tmpBuff, sizeof(tmpBuff));
		buf = std::string((const char*)tmpBuff);

		if (retval < 0)
			return retval;

		if (tmpBuff[1] != LIBUSB_DT_STRING)
			return -EIO;

		if (tmpBuff[0] > retval)
			return -EFBIG;

		return retval;
	}

	libusb_device_handle * Device::handle(void) {
		return m_handle;
	}

	int Device::reset(void) {
		return libusb_reset_device(m_handle);
	}

	int Device::setConfiguration(int configurationNumber) {
		return libusb_set_configuration(m_handle, configurationNumber);
	}

	u_int16_t Device::idVendor(void) {
		return m_descriptor.idVendor;
	}

	u_int16_t Device::idProduct(void) {
		return m_descriptor.idProduct;
	}

	u_int16_t Device::idRevision(void) {
		return m_descriptor.bcdDevice;
	}

	u_int8_t Device::devClass(void) {
		return m_descriptor.bDeviceClass;
	}

	u_int8_t Device::devSubClass(void) {
		return m_descriptor.bDeviceSubClass;
	}

	u_int8_t Device::devProtocol(void) {
		return m_descriptor.bDeviceProtocol;
	}

	u_int8_t Device::numConfigurations(void) {
		return m_descriptor.bNumConfigurations;
	}

	Configuration *
	Device::firstConfiguration(void) {
		iter = begin();
		return *iter++;
	}

	Configuration *
	Device::nextConfiguration(void) {
		if (iter == end())
			return NULL;

		return *iter++;
	}

	Configuration *
	Device::lastConfiguration(void) {
		return back();
	}

	int Device::controlTransfer(u_int8_t requestType, u_int8_t request, u_int16_t value, u_int16_t index,
			u_int16_t length, unsigned char *payload, int timeout) {
		return libusb_control_transfer(m_handle, requestType, request, value, index, payload, length, timeout);
	}

	Configuration::Configuration() :
		m_config_descriptor(NULL) {
	}

	Configuration::~Configuration() {
		if (m_config_descriptor)
			libusb_free_config_descriptor(m_config_descriptor);
	}

	Configuration::Configuration(libusb_config_descriptor *config_descriptor, Device * device) {
		m_config_descriptor = config_descriptor;
		for (uint8_t i = 0; i < m_config_descriptor->bNumInterfaces; i++) {
			push_back(new Interface(m_config_descriptor->interface[i], i, device));
		}
	}

	u_int8_t Configuration::numInterfaces(void) {
		return m_config_descriptor->bNumInterfaces;
	}

	void Configuration::dumpDescriptor(void) {
		printf("  wTotalLength:         %d\n", m_config_descriptor->wTotalLength);
		printf("  bNumInterfaces:       %d\n", m_config_descriptor->bNumInterfaces);
		printf("  bConfigurationValue:  %d\n", m_config_descriptor->bConfigurationValue);
		printf("  iConfiguration:       %d\n", m_config_descriptor->iConfiguration);
		printf("  bmAttributes:         %02xh\n", m_config_descriptor->bmAttributes);
		printf("  MaxPower:             %d\n", m_config_descriptor->MaxPower);
	}

	Interface *
	Configuration::firstInterface(void) {
		iter = begin();
		return *iter++;
	}

	Interface *
	Configuration::nextInterface(void) {
		if (iter == end())
			return NULL;

		return *iter++;
	}

	Interface *
	Configuration::lastInterface(void) {
		return back();
	}

	Interface::Interface() :
		m_numAltSettings(0), m_interfaceNumber(0) {
	}

	Interface::Interface(libusb_interface interface, u_int8_t number, Device * device) {
		m_numAltSettings = interface.num_altsetting;
		m_interfaceNumber = number;
		m_parent = device;
		for (int i = 0; i < m_numAltSettings; i++) {
			push_back(new AltSetting(interface.altsetting[i], device));
		}
	}

	Interface::~Interface() {

	}

	int Interface::claim(void) {
		return libusb_claim_interface(m_parent->handle(), m_interfaceNumber);
	}

	int Interface::release(void) {
		return libusb_release_interface(m_parent->handle(), m_interfaceNumber);
	}

	int Interface::setAltSetting(int altSettingNumber) {
		return libusb_set_interface_alt_setting(m_parent->handle(), m_interfaceNumber, altSettingNumber);
	}

	u_int8_t Interface::numAltSettings(void) {
		return m_numAltSettings;
	}

	AltSetting *
	Interface::firstAltSetting(void) {
		iter = begin();
		return *iter++;
	}

	AltSetting *
	Interface::nextAltSetting(void) {
		if (iter == end())
			return NULL;

		return *iter++;
	}

	AltSetting *
	Interface::lastAltSetting(void) {
		return back();
	}

	AltSetting::AltSetting() {
	}
	;
	AltSetting::AltSetting(libusb_interface_descriptor descriptor, Device * device) {
		m_interface_descriptor = descriptor;
		for (uint8_t i = 0; i < m_interface_descriptor.bNumEndpoints; i++) {
			push_back(new Endpoint(m_interface_descriptor.endpoint[i], i, device));
		}
	}

	void AltSetting::dumpDescriptor(void) {
		printf("    bInterfaceNumber:   %d\n", m_interface_descriptor.bInterfaceNumber);
		printf("    bAlternateSetting:  %d\n", m_interface_descriptor.bAlternateSetting);
		printf("    bNumEndpoints:      %d\n", m_interface_descriptor.bNumEndpoints);
		printf("    bInterfaceClass:    %d\n", m_interface_descriptor.bInterfaceClass);
		printf("    bInterfaceSubClass: %d\n", m_interface_descriptor.bInterfaceSubClass);
		printf("    bInterfaceProtocol: %d\n", m_interface_descriptor.bInterfaceProtocol);
		printf("    iInterface:         %d\n", m_interface_descriptor.iInterface);
	}

	Endpoint *
	AltSetting::firstEndpoint(void) {
		iter = begin();
		return *iter++;
	}

	Endpoint *
	AltSetting::nextEndpoint(void) {
		if (iter == end())
			return NULL;

		return *iter++;
	}

	Endpoint *
	AltSetting::lastEndpoint(void) {
		return back();
	}

	u_int8_t AltSetting::numEndpoints(void) {
		return m_interface_descriptor.bNumEndpoints;
	}

	Endpoint::Endpoint() :
		m_parent(NULL) {

	}

	Endpoint::Endpoint(libusb_endpoint_descriptor endpoint_descriptor, uint8_t number, Device* parent) :
		m_parent(NULL) {
		m_endpoint_descriptor = endpoint_descriptor;
		m_parent = parent;
		m_number = number;
	}

	int Endpoint::bulkWrite(unsigned char *message, int size, int &wrote, unsigned int timeout) {
		int ret = libusb_bulk_transfer(m_parent->handle(), (m_number | LIBUSB_ENDPOINT_OUT), message, size, &wrote,
				timeout);
		return ret;
	}

	int Endpoint::bulkRead(unsigned int length, unsigned char *message, int &read, int timeout) { // FIXME: what about size?
		int res;

		assert(length <= sizeof(message));
		res = libusb_bulk_transfer(m_parent->handle(), (m_number | LIBUSB_ENDPOINT_IN), message, length, &read, timeout);

		if (res > 0) {
			assert(false); // FIXME: errorhandling missing
		}

		return res;
	}

	//	int Endpoint::reset(void) {
	//		return libusb_clear_halt(m_parent->handle(), m_EndpointAddress);
	//	}

	int Endpoint::clearHalt(void) {
		return libusb_clear_halt(m_parent->handle(), m_number);
	}

	const libusb_endpoint_descriptor& Endpoint::endpointDescriptor() {
		return m_endpoint_descriptor;
	}

	void Endpoint::dumpDescriptor(void) const {
		printf("      bEndpointAddress: %02xh\n", m_endpoint_descriptor.bEndpointAddress);
		printf("      bmAttributes:     %02xh\n", m_endpoint_descriptor.bmAttributes);
		printf("      wMaxPacketSize:   %d\n", m_endpoint_descriptor.wMaxPacketSize);
		printf("      bInterval:        %d\n", m_endpoint_descriptor.bInterval);
		printf("      bRefresh:         %d\n", m_endpoint_descriptor.bRefresh);
		printf("      bSynchAddress:    %d\n", m_endpoint_descriptor.bSynchAddress);
	}

	DeviceID::DeviceID(u_int16_t vendor, u_int16_t product) {
		m_vendor = vendor;
		m_product = product;
	}

	u_int16_t DeviceID::vendor(void) const {
		return m_vendor;
	}

	u_int16_t DeviceID::product(void) const {
		return m_product;
	}

	USBError::USBError(const char *message) :
		message(NULL) {
		unsigned int len(strnlen(message, 1024));
		this->message = new char[len+1];
		this->message[len] = '\0';
		memcpy(this->message, message, len);
	}

	const char* USBError::what() const throw(){
		return message;
	}

	USBError::~USBError() throw() {
		if (message != NULL) {
			delete message;
			message = NULL;
		}
	}
}
