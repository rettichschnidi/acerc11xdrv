#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <stdexcept>

namespace acerc11xdrv {
	class Exception: public std::exception {
		public:
			explicit Exception(const char *message);
			virtual const char* what() const throw();
			virtual ~Exception() throw ();
		private:
			char *message;
	};
}

#endif /* EXCEPTION_H_ */
