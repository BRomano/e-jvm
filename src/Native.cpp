
#include "Native.h"


Native::Native(void)
{
}


Native::~Native(void)
{
}

/**
	std::string method_name;
	method_name = frameStack->_clazz->getThisClassName();

	std::string aux;
	frameStack->_clazz->getStringFromConstPool(frameStack->_method->name_index, aux);
	method_name += aux;

	frameStack->_clazz->getStringFromConstPool(frameStack->_method->descriptor_index, aux);
	method_name += aux;

	HMODULE library = LoadLibrary(".dll");
	if (library == NULL)
	{
		
	}
	return 0;
}
*/