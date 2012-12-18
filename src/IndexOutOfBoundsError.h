#include <stdexcept>
#include <string>

using std::string;
using std::runtime_error;

class IndexOutOfBoundsError : public runtime_error {
	
	public:
		IndexOutOfBoundsError(string& arg) : runtime_error(arg) {}

};
