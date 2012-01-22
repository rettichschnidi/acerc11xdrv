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

Under Linux we can achieve this with the following command (usb_modeswitch required, current version of usb_modeswitch already comes with this udev-rule):

	usb_modeswitch --default-vendor 0x1de1 --default-product 0x1101 \
	--target-vendor 0x1de1 --target-product c101 --message-endpoint 0x01 \
	-M 555342430890d8f90000000000000cff020000000000000000000000000000


"Beamer" mode
=============

The presented USB id is: 1de1:c101

The C110 uses a fairly simple protocol. Per frame there is a picture (JPEG or YUV) with its 24 bytes long header, which we call widget.

Header/Widget
------

In front of every picture there is a simple 24 byte long widget:

	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	+02|00|00|00|00|10|3E|10|FF|00|00|00|W0|W1|W2|W3|H0|H1|H2|H3|S0|S1|S2|S3+
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The first byte (0x02) tells us that this is an image widget.
FF specifies the following picture type. 0x01 for JPEG, 0x02 for a YUV picture.
S0S1S2S3 represents the size of the next JPEG image. The byte order is little endian.
W0W1W2W3 represents the width, H0H1H2H3 the height of the image (thanks Antonio). Again, little endian.

Examples: 
A JPEG has a size of 59475 bytes (0x0000E853 in hex), the resolution is 800x480 (0x00000320 and 0x000001E0).

	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	+02|00|00|00|00|10|3E|10|01|00|00|00|20|03|00|00|E0|01|00|00|53|E8|00|00+
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

A YUV picture has a size of 576000 bytes (800x480x12Bit, 0x0008CA00 in hex), the resolution is 800x480 (0x00000320 and 0x000001E0).

	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	+02|00|00|00|00|10|3E|10|02|00|00|00|20|03|00|00|E0|01|00|00|00|CA|08|00+
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


Payload/Picture
---------------
Right after the widget the JPEG or the YUV encoded picture follows. 

The restrictions for JPEG pictures are:

* JFIF standard 1.01
* resolutions between 854x480 and about 200x150 (known to work)
* resolutions of 150x150 and smaller do not work and creates funky colors (tested)
* bigger resolutions than 854x480 will crash the device, re-plugging required
* Announcing a bigger resolution than the file that follows actually has creates the effect that just the middle of the screen gets updated. Anything around keeps the content of the old picture.
* max filesize is about 245000 bytes

The restrictions for YUV pictures are:

* NV12 encodingt - have a look at this link: http://www.fourcc.org/yuv.php#NV12
* resolutions between 800x480 and about 200x150 (known to work)
* resolutions of 150x150 and smaller do not work and creates funky colors (tested)
* bigger resolutions than 800x480 creates funky colors

Special Header
--------------
Between any frame can be a special widget, which changes the brightness of the device.
The first byte (0x04) tells us that this is a brightness widget.

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

