#include <stdexcept>
#include <string>

using std::string;
using std::runtime_error;

namespace KwazCollections
{
	class NoMoreElementsError : public runtime_error
	{	
		public:
			NoMoreElementsError(string& arg)
				: runtime_error(arg) {}
	};
}
