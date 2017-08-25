//
// Created by Romano on 24/08/2017.
//

#include <string>
#include <boost/shared_ptr.hpp>
#include <iostream>

#include "src/JavaClass.h"
#include "src/types.h"
#include "src/constants.h"
#include "src/JavaVM.h"
#include "src/JavaClassParser.h"

class DummyAllocator;

void Execute(std::string className)
{
	boost::shared_ptr<JavaClass> clazz;
	boost::shared_ptr<JavaClass> object_class;

	DummyAllocator d;
	JavaVM * jvm = new JavaVM(&d);
	try
	{
		clazz = jvm->loadClass(className);
		object_class = jvm->loadClass(className);
	}
	catch(...)
	{
		assert(false);
	}

//	Object * obj = jvm->getObjectHeap()->createObject(virtual_class);
//	jvm->invokeMethod(obj, "main", "()I");


	u2 m_index = clazz->getMethodID("main", "([Ljava/lang/String;)V", clazz); //initialize class
	Code_attribute * code = clazz->getMethodCodeAttribute(m_index);
	Frame * frame = new Frame();
	frame->setCode(code);
	frame->_clazz = clazz;
	frame->_method = clazz->_methods[m_index];

	jvm->addNewThread(frame);
	jvm->execThreads();
	delete frame;
}

int main(int argc, char * argv[])
{
	if (argc < 2)
    {
        std::cout << "Rodou";
        return -1;
    }

	Execute(argv[1]);
	return 0;
}