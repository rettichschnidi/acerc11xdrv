#include "Exception.h"

#include <cstring>
#include <stdexcept>

namespace acerc11xdrv {

	Exception::Exception(const char *message) :
		message(NULL) {
		unsigned int len(strnlen(message, 1024));
		this->message = new char[len+1];
		this->message[len] = '\0';
		memcpy(this->message, message, strnlen(message, 1024));
	}

	const char* Exception::what() const throw () {
		return message;
	}

	Exception::~Exception() throw () {
		if (message != NULL) {
			delete message;
			message = NULL;
		}
	}
}
