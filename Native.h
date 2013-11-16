#pragma once

#include "JavaVM.h"
#include <boost/function.hpp>
#include <map>
#include <vector>
#include <string>

typedef boost::function<Variable (JavaVM *, Object *, std::vector<Variable> *)> nativeFunction;

class Native
{
	private:
		std::map<std::string, nativeFunction> _nativeFunctions;

	public:
		Native(void);
		~Native(void);

		bool loadDLL()
		{
/*
			HMODULE library = LoadLibrary("SDL.dll");
			if (library == NULL)
			{
		
			}
*/
		}

		bool invokeNativeMethod(Frame * frameStack)
		{
			std::string method_name;
			method_name = frameStack->_clazz->getThisClassName();

			std::string aux;
			frameStack->_clazz->getStringFromConstPool(frameStack->_method->name_index, aux);
			method_name += aux;

			frameStack->_clazz->getStringFromConstPool(frameStack->_method->descriptor_index, aux);
			method_name += aux;

			std::map<std::string, nativeFunction>::iterator func = _nativeFunctions.find(method_name);
			if (func == _nativeFunctions.end())
			{
				//TODO find this method and insert it into _nativeFunctions
			}

			//TODO passar JavaVM...
			//get object from operandStack
			func->second(NULL, frameStack->_operandStack.top()._var._objectref, NULL);
			return 0;
		}
};

