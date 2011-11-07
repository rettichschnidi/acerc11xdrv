/**
 * @file USB.h
 * @brief Classes to access Universal Serial Bus devices
 *
 * The USB Namespace provides a number of classes to work
 * with Universal Serial Bus (USB) devices attached to the
 * system.
 *
 * @author Brad Hards, adapted for libusb-1.0: Reto Schneider
 */
#ifndef USB_H_
#define USB_H_

#include <string>
#include <list>
#include <cstring>
#include <stdexcept>

#include <libusb-1.0/libusb.h>

namespace DANGER_ZONE {

	class Device;

	/**
	 * @brief Class representing a device endpoint
	 *
	 * This class represents a device endpoint. You need this class to
	 * perform bulk reads and writes.
	 *
	 */
	class Endpoint {
		public:
		Endpoint();
		Endpoint(libusb_endpoint_descriptor, uint8_t number, Device*);

		/**
		 * @brief Bulk write
		 *
		 * This method performs a bulk transfer to the endpoint.
		 *
		 * @param message is the message to be sent.
		 * @param size is the number of bytes to be sent
		 * @param wrote will be updated with the sent bytes
		 * @param timeout is the USB transaction timeout in milliseconds
		 *
		 * @return 0 on success, anything else marks an error
		 */
		int bulkWrite(unsigned char *message, int size, int &wrote, unsigned int timeout = 100);

		/**
		 * @brief Bulk read
		 *
		 * This method performs a bulk transfer from the endpoint.
		 *
		 * @param length is the maximum data transfer required.
		 * @param message is the message that was received.
		 * @param read is the number of bytes which where received.
		 * @param timeout is the USB transaction timeout in milliseconds
		 *
		 * @return the number of bytes received, or a negative value on
		 * failure
		 */
		int bulkRead(unsigned int length, unsigned char *message, int &read, int timeout = 100);

		/**
		 * @brief Reset endpoint
		 *
		 * This method resets the endpoint.
		 */
		//	int reset(void);

		/**
		 * @brief Clear halt
		 *
		 * This method clears a halt (stall) on the endpoint.
		 */
		int clearHalt(void);

		/**
		 * @brief Endpoint descriptor information output
		 *
		 * This method dumps out the various characteristics
		 * of the endpoint to standard output.
		 *
		 * It is mostly useful for debugging.
		 */
		void dumpDescriptor(void) const;

		const libusb_endpoint_descriptor& endpointDescriptor();

		private:
		libusb_endpoint_descriptor m_endpoint_descriptor;
		uint8_t m_number;
		Device *m_parent;
	}
	;

	class AltSetting: public std::list<Endpoint *> {
		public:
		AltSetting();
		AltSetting(libusb_interface_descriptor descriptor, Device * device);

		u_int8_t numEndpoints(void);

		/**
		 * @brief AltSetting descriptor information output
		 *
		 * This method dumps out the various characteristics
		 * of the alternate setting to standard output.
		 *
		 * It is mostly useful for debugging.
		 */
		void dumpDescriptor(void);

		Endpoint *firstEndpoint(void);
		Endpoint *nextEndpoint(void);
		Endpoint *lastEndpoint(void);

		private:
		std::list<Endpoint *>::const_iterator iter;

		libusb_interface_descriptor m_interface_descriptor;
	};

	/**
	 * @brief Class representing an interface of a Device
	 *
	 * The Interface class represents a USB interface
	 * for a device attached to a Universal Serial Bus.
	 *
	 * Interfaces are the main element of the USB class
	 * structure.
	 *
	 * @author Brad Hards
	 */
	class Interface: public std::list<AltSetting *> {
		public:
		Interface();
		Interface(libusb_interface interface, uint8_t number, Device * device);
		~Interface();

		/**
		 * @brief Claim this interface
		 *
		 * This method claims the interface. You have to claim the
		 * interface before performing any operations on the interface (or
		 * on endpoints that are part of the interface).
		 *
		 * @return 0 on success or negative number on error.
		 */
		int claim(void);

		/**
		 * @brief Release this interface
		 *
		 * This method releases the interface. You should release the
		 * interface after all operations on it (and any lower level
		 * endpoints) are completed.
		 *
		 * @return 0 on success or negative number on error.
		 */
		int release(void);

		/**
		 * @brief Set interface alternate setting
		 *
		 * This method sets the interface to a particular AltSetting.
		 *
		 * @param altSettingNumber the AltSetting that the interface
		 * should be changed to.
		 *
		 * @return 0 on success, or a negative number in case of error.
		 */
		int setAltSetting(int altSettingNumber);

		/**
		 * @brief Number of Alternative Settings that this interface has
		 *
		 * This is a simple accessor method that specifies the number
		 * alternative settings that this device interface has.
		 */
		u_int8_t numAltSettings(void);

		/**
		 * @brief First AltSetting for the Interface
		 *
		 * This method returns a pointer to the first AltSetting
		 * for the Interface.
		 *
		 * See nextAltSetting() for an example of how it might be
		 * used.
		 *
		 * @see nextAltSetting(), lastAltSetting(), numAltSettings()
		 */
		AltSetting *firstAltSetting(void);

		/**
		 * @brief Next AltSetting for the Interface
		 *
		 * This method returns a pointer to the next AltSetting
		 * for the Interface.
		 *
		 * If you want to iterate through each AltSetting on
		 * a device, you can use something like the following:
		 * @code
		 * USB::Configuration *this_Configuration;
		 * this_Configuration = device->firstConfiguration();
		 * for (i=0; i < device->numConfigurations(); i++) {
		 *	 this_Configuration->dumpDescriptor();
		 *   USB::Interface *this_Interface;
		 *   this_Interface = this_Configuration->firstInterface();
		 *   for (j=0; j < this_Configuration->numInterfaces(); j++) {
		 *     USB::AltSetting *this_AltSetting;
		 *     this_AltSetting = this_Interface->firstAltSetting();
		 *     for (k=0; k < this_Interface->numAltSettings(); k++) {
		 *       // do something with this_AltSetting
		 *       this_AltSetting = this_Interface->nextAltSetting();
		 *     }
		 *   this_Interface = this_Configuration->nextInterface();
		 *   }
		 *   this_Configuration = device->nextConfiguration();
		 * }
		 * @endcode
		 *
		 * @see firstAltSetting(), lastAltSetting(), numAltSettings()
		 */
		AltSetting *nextAltSetting(void);

		/**
		 * @brief Last AltSetting for the Interface
		 *
		 * This method returns a pointer to the last AltSetting
		 * for the Interface.
		 *
		 * @see firstAltSetting(), nextAltSetting(), numAltSettings()
		 */

		AltSetting *lastAltSetting(void);

		private:
		std::list<AltSetting *>::const_iterator iter;

		u_int8_t m_numAltSettings;
		Device *m_parent;

		/* index representing the interface, in this configuration */
		int m_interfaceNumber;
	};

	/**
	 * @brief Class representing a configuration of a Device
	 *
	 * The Configuration class represents a single configuration
	 * of a device attached to a Universal Serial Bus.
	 *
	 * @author Brad Hards
	 */
	class Configuration: public std::list<Interface *> {
		public:
		Configuration();
		Configuration(libusb_config_descriptor *, Device * device);
		~Configuration();

		/**
		 * @brief Configuration descriptor information output
		 *
		 * This method dumps out the various characteristics
		 * of the configuration to standard output.
		 *
		 * It is mostly useful for debugging.
		 */
		void dumpDescriptor(void);

		/**
		 * @brief Number of Interfaces that this device has
		 *
		 * This is a simple accessor method that specifies the number
		 * Interfaces that this device configuration has.
		 */
		u_int8_t numInterfaces(void);

		/**
		 * @brief First Interface for the Configuration
		 *
		 * This method returns a pointer to the first Interface
		 * for the Configuration.
		 *
		 * See nextInterface() for an example of how it might be
		 * used.
		 *
		 * @see nextInterface(), lastInterface(), numInterfaces()
		 */
		Interface *firstInterface(void);

		/**
		 * @brief Next Interface for the Configuration
		 *
		 * This method returns a pointer to the next Interface
		 * for the Configuration.
		 *
		 * If you want to iterate through each Interface on
		 * a device, you can use something like the following:
		 * @code
		 * USB::Configuration *this_Configuration;
		 * this_Configuration = device->firstConfiguration();
		 * for (i=0; i < device->numConfigurations(); i++) {
		 *	 this_Interface = this_Configuration->firstInterface();
		 *	 for (j=0; j < this_Configuration->numInterfaces(); j++) {
		 *     // do something with this_Interface
		 *     this_Interface = this_Configuration->nextInterface();
		 *   }
		 *   this_Configuration->nextConfiguration();
		 * }
		 * @endcode
		 *
		 * @see firstInterface(), lastInterface(), numInterfaces()
		 */
		Interface *nextInterface(void);

		/**
		 * @brief Last Interface for the Configuration
		 *
		 * This method returns a pointer to the last Interface
		 * for the Configuration.
		 *
		 * @see firstInterface(), nextInterface(), numInterfaces()
		 */
		Interface *lastInterface(void);

		private:
		std::list<Interface *>::const_iterator iter;

		libusb_config_descriptor *m_config_descriptor;
	};

	/**
	 * @brief Class representing a Device on the Bus
	 *
	 * The Device class represents a single device
	 * attached to a Universal Serial Bus.
	 *
	 * @author Brad Hards
	 */
	class Device: public std::list<Configuration *> {
		public:
		Device() {
		}

		Device(libusb_device *);
		~Device();

		/**
		 * @brief The vendor ID number, as provided by the device.
		 *
		 * This method returns a number containing the vendor
		 * (manufacturer) identification number. These are allocated
		 * by the USB Implementers Forum, and you can construct a
		 * lookup based on the number to get the manufacturer's name,
		 * even if the device does not contain a vendor string.
		 *
		 * @see Vendor()
		 */
		u_int16_t idVendor(void);

		/**
		 * @brief The product ID number, as provided by the device.
		 *
		 * This method returns a number containing the product
		 * identification number. These are allocated
		 * by the manufacturer, and should be different on each device.
		 *
		 * @see Product()
		 */
		u_int16_t idProduct(void);

		/**
		 * @brief The product's revision ID, as provided by the device.
		 *
		 * This method returns a number containing the product's revision.
		 * This revision level is nominally binary coded decimal, but
		 * hexadecimal revision levels are not uncommon. The binary coded
		 * decimal version nominally has a major version in the high byte,
		 * and a minor version in the low byte.
		 */
		u_int16_t idRevision(void);

		/**
		 * @brief The device's USB class, as provided by the device.
		 *
		 * This method returns a number containing the device's class.
		 * These are defined by the USB Implementer's Forum.
		 *
		 * A code of Zero is special (and common) - it means that the
		 * class is found in the Interface descriptor, rather than in the
		 * Device descriptor.
		 *
		 * A code of 0xFF is also special (and far too common) - it means
		 * that the manufacturer didn't conform to one of the defined
		 * class specifications, and chose to implement a vendor specified
		 * protocol.
		 *
		 */
		u_int8_t devClass(void);

		/**
		 * @brief The device's USB subclass, as provided by the device.
		 *
		 * This method returns a number containing the device's subclass.
		 * These subclasses are defined by the USB Implementer's Forum,
		 * and only have meaning in the context of a specified class.
		 */
		u_int8_t devSubClass(void);

		/**
		 * @brief The device's USB protocol, as provided by the device.
		 *
		 * This method returns a number containing the device's protocol.
		 * These protocols are defined by the USB Implementer's Forum, and
		 * only have meaning in the context of a specified class and
		 * subclass.
		 */
		u_int8_t devProtocol(void);

		/**
		 * @brief Number of Configurations that this device has
		 *
		 * This is a simple accessor method that specifies the number
		 * configurations that this device has.
		 */
		u_int8_t numConfigurations(void);

		/**
		 * @brief fetch an arbitrary string from the device
		 *
		 * @param string the string from the device. You can typically
		 * pass in an empty string for this.
		 * @param index the index of the string required
		 * @param lang the language ID to use. Defaults to using the
		 * first language ID.
		 *
		 * @return length of string, or 0 on error.
		 */
		int string(std::string &buf, int index, u_int16_t lang = 0);

		/**
		 * @brief First Configuration for the Device
		 *
		 * This method returns a pointer to the first Configuration
		 * for the Device.
		 *
		 * See nextConfiguration() for an example of how it might be
		 * used.
		 */
		Configuration *firstConfiguration(void);

		/**
		 * @brief Next Configuration for the Device
		 *
		 * This method returns a pointer to the next Configuration
		 * for the Device.
		 *
		 * If you want to iterate through each Configuration on
		 * a device, you can use something like the following:
		 * @code
		 * USB::Configuration *this_Configuration;
		 * this_Configuration = device->firstConfiguration();
		 * for (i=0; i < device->numConfigurations(); i++) {
		 *   // do something with this_Configuration
		 *   this_Configuration->nextConfiguration();
		 * }
		 * @endcode
		 */
		Configuration *nextConfiguration(void);

		/**
		 * @brief Last Configuration for the Device
		 *
		 * This method returns a pointer to the last Configuration
		 * for the Device.
		 *
		 */
		Configuration *lastConfiguration(void);

		/**
		 * @brief USB control transfer
		 *
		 * This method performs a standard control transfer to the default
		 * endpoint. See the USB specification for more details on this.
		 *
		 * @param requestType corresponds to the bmRequestType field
		 * in the transfer
		 * @param request corresponds to the bRequest field in the
		 * transfer
		 * @param value corresponds to the wValue field in the transfer
		 * @param index corresponds to the wIndex field in the transfer
		 * @param length corresponds to the wLength field in the transfer
		 * @param payload corresponds to the data phase of a control
		 * transfer
		 * @param timeout is the timeout period for the control transfer,
		 * in milliseconds
		 *
		 * @return number of bytes sent or received, or a negative number
		 * in case of error.
		 */
		int controlTransfer(u_int8_t requestType, u_int8_t request,
				u_int16_t value, u_int16_t index, u_int16_t length,
				unsigned char *payload, int timeout = 100);

		/**
		 * @brief USB device reset
		 *
		 * This method performs a device reset - see USB Specification
		 * 9.1 for how this changes the device state to the Default state.
		 *
		 * @return 0 on success, or a negative number in case of error.
		 */
		int reset(void);

		/**
		 * @brief Set device configuration
		 *
		 * This method sets the device to a particular Configuration.
		 *
		 * @param configurationNumber the configuration that the device
		 * should be changed to.
		 *
		 * @return 0 on success, or a negative number in case of error.
		 */
		int setConfiguration(int configurationNumber);

		libusb_device_handle *handle(); // FIXME: this is fugly, should be private

		private:
		std::list<Configuration *>::const_iterator iter;

		libusb_device_handle *m_handle;
		libusb_device_descriptor m_descriptor;
	};

	///**
	// * @brief Class representing a single bus on the machine
	// *
	// * This class is essentially a list of Device class instances
	// */
	//class Bus: public std::list<Device *> {
	//	/**
	//	 * Busses is a friend because it fills in the directory name
	//	 * information on initialisation and rescan.
	//	 */
	//	friend class Busses;
	//	public:
	//	Bus() {
	//	}
	//	;
	//	/**
	//	 * @brief OS representation of directory name for this Bus
	//	 *
	//	 * libusb++ provides a uniform way of accessing USB
	//	 * busses irrespective of the underlying Operation System
	//	 * representation. If you want to map the libusb++ representation
	//	 * to the Operating System representation, you can do this
	//	 * with directory name().
	//	 *
	//	 * On Linux, the directoryname is usually something like 003, which
	//	 * represents the third bus on the host.
	//	 *
	//	 * @see Directory::filename()
	//	 */
	//	std::string directoryName(void);
	//	private:
	//	std::list<Device *>::const_iterator iter;
	//
	//	void setDirectoryName(std::string);
	//	std::string m_directoryName;
	//};

	/**
	 * @brief A vendor/product ID pair
	 *
	 * DeviceID provides a list of (vendor, product) identification
	 * pairs. It is intended for use in a list of device numbers to
	 * search for, but there is no reason why it couldn't be used for a
	 * general purpose (vendor,product) tuple if you had some reason for
	 * this.
	 *
	 * The description for Busses::match() provides an example of how
	 * this class might be used.
	 *
	 * @see DeviceIDList, Busses::match()
	 */
	class DeviceID {
		public:
		DeviceID() {
		}
		;
		/**
		 * @brief Standard constructor
		 *
		 * This constructor takes (vendor, product) tuple, which are
		 * stored away.
		 *
		 * @param vendor the 16 bit vendor number for the device
		 * @param product the 16 bit product number for the device
		 */
		DeviceID(u_int16_t vendor, u_int16_t product);

		/**
		 * @brief vendor number for the device
		 *
		 * This method returns the 16 bit vendor number.
		 */
		u_int16_t vendor(void) const;

		/**
		 * @brief product number for the device
		 *
		 * This method returns the 16 bit product number.
		 */
		u_int16_t product(void) const;

		private:
		u_int16_t m_vendor;
		u_int16_t m_product;
	};

	/**
	 * @brief A list of vendor/product pairs
	 *
	 * DeviceIDList provides a list of DeviceID classes, which is
	 * essentially a list of (vendor, product) identification pairs.
	 *
	 * @see DeviceID
	 */
	typedef std::list<DeviceID> DeviceIDList;

	/**
	 * @brief Class representing all the busses on the machine
	 *
	 * This class is essentially a list of Bus class instances
	 */
	class USB: public std::list<Device *> {
		public:
		USB();
		~USB();
		/**
		 * @brief Update method
		 *
		 * This method can be called to rescan the various devices
		 * attached to the various busses. You should use it to
		 * update if things change. Unfortunately there is no
		 * way to automatically detect this change in a portable way,
		 * so worst case is that you need to call this using some
		 * kind of timer in the background.
		 */
		void rescan(void);

		/**
		 * @brief find all devices with matching device class designator
		 *
		 * This method searches every device on every bus, and returns a
		 * list of pointers to the devices that have a matching device
		 * class code
		 */
		std::list<Device *> match(u_int8_t Class);

		/**
		 * @brief find all devices with matching device IDs
		 *
		 * This method searches every device on every bus, and returns a
		 * list of pointers to the devices that have a matching device
		 * ID. That is, if the (vendor, product) tuple of a device matches
		 * one of the tuples on the list, then the device will be added to
		 * the list of matches.
		 *
		 * An example of usage is shown below:
		 * @code
		 * USB::Busses buslist;
		 * USB::Device *device;
		 * std::list<USB::Device> miceFound;
		 * USB::DeviceIDList mouseList;
		 *
		 * mouseList.append(USB::DeviceID(VENDOR_LOGITECH, 0xC00E)); // Wheel Mouse Optical
		 * mouseList.append(USB::DeviceID(VENDOR_LOGITECH, 0xC012)); // MouseMan Dual Optical
		 * mouseList.append(USB::DeviceID(VENDOR_LOGITECH, 0xC506)); // MX700 Optical Mouse
		 *
		 * miceFound = buslist.match(mouseList);
		 *
		 * for ( device = miceFound.first(); device; device = miceFound.next() ) {
		 *     // do something with each mouse that matched
		 * }
		 * FIXME: This is incorrect now
		 * @endcode
		 */
		std::list<Device *> match(DeviceIDList);

		private:
		libusb_device **devices;
		std::list<Device *>::const_iterator iter;
		libusb_context *ctx;
	};

	class USBError: public std::exception {
		public:
			USBError(const char *message);
			virtual const char* what() const throw();
			virtual ~USBError() throw ();
		private:
			char *message;
	};

}
#endif /* USB_H_ */

