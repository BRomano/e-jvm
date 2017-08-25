#include <string>

class jException
{
	public:
		jException(int bc) : _bc(bc) { }
		jException(const char * str) : _error(str) { }

		~jException(void) { }

	private:
		int _bc;
		std::string _error;
};

