/**
 * @file util.h
 */

#include <stdint.h>
#include <string>
#include <iostream>

#include "Data.h"

namespace acerc11xdrv {
	namespace util {

		void writeDataToFile(Data &data, std::string filename,
				bool append = false, bool overwrite = false);
		void writeDataToFile(Data &data, std::ostream & os);
	}
}
