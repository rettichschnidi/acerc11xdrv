/**
 * @file util.h
 *
 * @brief Just some helper functions which do not really fit anywhere else.
 */

#ifndef UTIL_H_
#define UTIL_H_

#include "USBData.h"
#include "Magick++.h"

#include <stdint.h>
#include <string>
#include <iostream>

namespace acerc11xdrv {
	namespace util {
		void writeDataToFile(USBData &data, std::string filename,
				bool append = false, bool overwrite = false);
		void writeDataToFile(USBData &data, std::ostream & os);
	}
}

#endif /* UTIL_H_ */
