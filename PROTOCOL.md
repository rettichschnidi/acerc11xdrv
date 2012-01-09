Protocol
========

The Acer C110 pico projector can operate in two modes:

* Mass storage mode
* "Beamer" mode

Mass Storage Mode
=================
When plugged in, the C110 registers itself as mass storage device with the following content:

    $ ls -lh
    total 2.3M
    -r-xr-xr-x 1 root root 144K 2011-05-05 05:45 AcerDetection.exe
    -r-xr-xr-x 1 root root 1.2M 2011-06-21 07:27 AcerDisplayPal_Setup.exe
    -r-xr-xr-x 1 root root   95 2011-03-03 14:57 AcerLaptopCheck.bat
    -r-xr-xr-x 1 root root  16K 2011-03-03 14:57 AcerLaptopCheck.exe
    -r-xr-xr-x 1 root root   37 2011-05-06 08:09 autorun.inf
    -r-xr-xr-x 1 root root 971K 2011-05-20 11:31 C110-EMEA-UG.pdf

Under Windows the user can install this drivers at this point.

The presented USB id is: 1de1:1101

Change To Beamer Mode
=====================
Under Windows the driver will send a special command to the C110 when it registers itself as mass storage device. The C110 will then change to the "beamer" mode.

Under Linux we can achieve this with the following command (usb_modeswitch required):

	usb_modeswitch --default-vendor 0x1de1 --default-product 0x1101 \
	--target-vendor 0x1de1 --target-product c101 --message-endpoint 0x01 \
	-M 555342430890d8f90000000000000cff020000000000000000000000000000


"Beamer" mode
=============

The presented USB id is: 1de1:c101

The C110 uses a fairly simple protocol. Per frame there is a picture with its 24 bytes long header, which we call widget.

Header/Widget
------

In front of every picture there is a simple 24 byte long widget:

	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	+02|00|00|00|00|10|3E|10|01|00|00|00|W2|W1|00|00|H2|H1|00|00|XX|YY|ZZ|00+
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Where XXYYZZ represents the size of the following JPEG image. The byte order is little endian.
W2W1 represents the width, H2H1 the height of the image (thanks Antonio). Again, little endian.

Example: 
A JPEG has a size of 59475 bytes (0xE853 in hex), the resolution is 800x480 (0x0320 and 0x01E0).

	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	+02|00|00|00|00|10|3E|10|01|00|00|00|20|03|00|00|E0|01|00|00|53|E8|00|00+
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Payload/Picture
---------------
Right after the widget the JPEG encoded picture follows. 

The restrictions are:

* JFIF standard 1.01
* resolutions between 800x480 and about 200x150 (known to work)
* resolution of 150x150 does not work
* resolution of 854x480 should work
* Setting a bigger resolution than the file that follows actually has, creates the effect that just the middle of the screen gets updated. Anything around keeps the content of the old picture.
* bigger resolutions than 854x480 will crash the device
* max filesize is about 245000 bytes


Special Header
--------------
Between any frame can be a special widget, which changes the brightness of the device.

maximum:

	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	+04|00|00|00|00|0c|ff|ff|00|00|00|00|00|00|00|00|00|00|00|00|00|00|00|00+
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

high:

	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	+04|00|00|00|00|0c|ff|ff|00|00|00|00|01|00|00|00|01|00|00|00|00|00|00|00+
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	                                    --^         --^
medium:

	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	+04|00|00|00|00|0c|ff|ff|00|00|00|00|01|00|00|00|00|00|00|00|00|00|00|00+
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	                                    --^
low:

	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	+04|00|00|00|00|0c|ff|ff|00|00|00|00|00|00|00|00|01|00|00|00|00|00|00|00+
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	                                                --^

