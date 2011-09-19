/**
 * @file util.cpp
 */

#include <stdint.h>
#include <fstream>
#include <cassert>

#include <boost/filesystem.hpp>

#include "util.h"

using namespace std;
using namespace boost::filesystem3;

namespace acerc11xdrv {
	namespace util {

		/**
		 * Write data to a file
		 * @note When append and overwrite are true, append will "win" over the overwrite flag
		 * @param data Data to write
		 * @param filename Filename to write to
		 * @param append Wether to append data to a potentially existing file
		 * @param overwrite Wether to overwrite a potentially existing file
		 * @exception std::string
		 */
		void writeDataToFile(Data &data, string filename, bool append,
				bool overwrite) {
			ofstream out;
			if (exists(filename)) {
				if (append) {
					out.open(filename.c_str(), ios::app | ios::binary);
					out.seekp(0, ios::end);
				} else if (overwrite) {
					out.open(filename.c_str(), ios::trunc | ios::binary);
				} else {
					throw string("File " + filename + " already exists");
				}
			} else {
				out.open(filename.c_str(), ios::binary);
			}
			if (!out.is_open()) {
				throw string("Could not open file " + filename);
			}
			writeDataToFile(data, out);
		}

		/**
		 * @param data Data to write
		 * @param os Stream to write to
		 * @exception std::string
		 */
		void writeDataToFile(Data &data, ostream & os) {
			assert(os.good());
			char *d = data.getDataAsCharArray();
			os.write(d, data.getSize());
			delete d;
			if (os.bad()) {
				throw string("Could write data");
			}
		}

	}
}
