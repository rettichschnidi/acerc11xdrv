/**
 * @file util.cpp
 */

#include "util.h"
#include "Exception.h"

#include <stdint.h>
#include <fstream>
#include <cassert>
#include <string>

#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem3;

namespace acerc11xdrv {
	namespace util {

		/**
		 * Write data to a file
		 * @note When append and overwrite are true, append will supersede the overwrite flag
		 * @param data Data to write
		 * @param filename Filename to write to
		 * @param append Wether to append data to a potentially existing file
		 * @param overwrite Wether to overwrite a potentially existing file
		 * @exception Exception
		 */
		void writeDataToFile(USBData &data, string filename, bool append, bool overwrite) {
			ofstream out;
			if (exists(filename)) {
				if (append) {
					out.open(filename.c_str(), ios::app | ios::binary);
					out.seekp(0, ios::end);
				} else if (overwrite) {
					out.open(filename.c_str(), ios::trunc | ios::binary);
				} else {
					string error("File " + filename + " already exists");
					throw Exception(error.c_str());
				}
			} else {
				out.open(filename.c_str(), ios::binary);
			}
			if (!out.is_open()) {
				string error("Could not open file " + filename);
				throw Exception(error.c_str());
			}
			writeDataToFile(data, out);
		}

		/**
		 * @param data Data to write
		 * @param os Stream to write to
		 * @exception Exception Errormessage
		 */
		void writeDataToFile(USBData &data, ostream & os) {
			if(os.bad()) {
				throw Exception("Bad ostream");
			}
			USBData::SaCharArray d = data.getDataAsCharArray();
			os.write(d.get(), data.getSize());
			if (os.bad()) {
				throw Exception("Could not write data");
			}
		}
	}
}
