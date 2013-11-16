// kjvm.cpp : Defines the entry point for the console application.
//

/**
 * The best link I use to understand and create this JVM version.
 * This link is a specification for JVM, this is the most important link, all I've needed is there.
 * But I need more to understand then google is the best friend.
 * http://docs.oracle.com/javase/specs/jvms/se7/html/index.html
 *
 * It's good to understand how jvm process method, thread.
 * http://www.cubrid.org/blog/dev-platform/understanding-jvm-internals/
 */

/*
 * This is the process to load and initialize a class in JVM. I need create these 5 steps to improve the JVM.
 *
 * Loading: A class is obtained from a file and loaded to the JVM memory.
 --> This process is too sample, only create a class to find file in path (CLASSPATH) and read all file into memory.
 *
 * Verifying: Check whether or not the read class is configured as described in the Java Language Specification and JVM specifications. 
 * This is the most complicated test process of the class load processes, and takes the longest time. 
 * Most cases of the JVM TCK test cases are to test whether or not a verification error occurs by loading wrong classes.
 --> This is a little more complicate, need to validate all class read before.
 *
 * Preparing: Prepare a data structure that assigns the memory required by classes and indicates the fields, methods, and interfaces defined in the class.
 --> I can do this step together with before step, It's too sample.
 *
 * Resolving: Change all symbolic references in the constant pool of the class to direct references.
 --> I do.
	This process is not complicate but I need create this to improve speed for load classes in runtime execution. 
	If constant Pool references a class then I need load this, before I need to use that.
 *
 * Initializing: Initialize the class variables to proper values. Execute the static initializers and initialize the static fields to the configured values.
 --> Today I don't do this.
 */


/**
 * TODOs
 * 1- Some instruction are specific for a type, e.g iadd this instruction must add two int.
 *   Today any instruction are validantig their parameters.
 */

#include "stdafx.h"
#include "JavaClass.h"
#include "types.h"
#include "constants.h"
#include "JavaVM.h"
#include "JavaClassParser.h"

#include <string>
#include <boost/shared_ptr.hpp>

class DummyAllocator;

void Execute(std::string className)
{
	jClass clazz;
	jClass object_class;
	
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
	
	jClass virtual_class(clazz);
//	Object * obj = jvm->getObjectHeap()->createObject(virtual_class);
//	jvm->invokeMethod(obj, "main", "()I");
	

	u2 m_index = clazz->getMethodID("main", "([Ljava/lang/String;)V", virtual_class); //initialize class
	Code_attribute * code = virtual_class->getMethodCodeAttribute(m_index);
	Frame * frame = new Frame();
	frame->setCode(code);
	frame->_clazz = virtual_class;
	frame->_method = virtual_class->_methods[m_index];

	jvm->addNewThread(frame);
	jvm->execThreads();
	delete frame;
}

int main(int argc, char * argv[])
{
	if (argc < 2)
		return -1;

	Execute(argv[1]);
	return 0;
}

/**
 *
 * boost::asio::io_service io_service;
 * JVM<basic_thread<thread>> jvm(io_service);
 * jvm.async_exec(method, callback);
 * jvm.async_exec(method2, callback2);
 * io_service.run();
 *
 * Para executar desta forma, preciso colocar o basic_thread sendo o escalonador, e o JVM sendo a JavaVM
 * agora o thread precisa ser uma classe que executa o executionEnginer, ainda nao sei bem ao certo 
 * o que este cara pode ser.
 */
