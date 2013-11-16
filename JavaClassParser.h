#pragma once

/**
	This class is responsable to parse .class files.
	.class file has a specific strutcture
	I use these references

	http://en.wikipedia.org/wiki/Java_class_file


*/
#include "types.h"
#include "JavaTypes.h"
#include "JavaClass.h"
#include <boost/shared_ptr.hpp>
#include <string>

#include <boost/filesystem.hpp>

class JavaClassParser
{
	public:
		JavaClassParser(void) {}
		~JavaClassParser(void) {}
		bool parseClass(const boost::filesystem::path & filePath, boost::shared_ptr<JavaClass> & clazz);

		/**
		 * Parse all objects, from constantPool
		 * Constant Pool is an important part of .class files and jvm structure
		 * for more information see the official documentation 
		 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4
		 */
		bool parseConstantPool(std::vector<CPBase *> & constantPool, boost::shared_ptr<JavaClass> & clazz);
		
		/**
		 * This method parses interfaces.
		 */
		bool parseInterfaces(std::vector<u2> & interfaces, boost::shared_ptr<JavaClass> & clazz);
		
		/**
		 * it can parse methods and fields.
		 */
		bool parseFields(std::vector<method_and_field_info *> & fields, boost::shared_ptr<JavaClass> & clazz);
		
		/**
		 * it parses all kind of attributes.
		 * We need to give support for all kind of attributes ;-P
		 */
		bool parseAttributes(std::vector<attribute_info *> & attributes, boost::shared_ptr<JavaClass> & clazz, u1 *& bytecode);

	private:
		bool loadClassFromFile(const boost::filesystem::path & filePath) throw (std::exception);
		CPBase * getConstantPoolObj();
		void resolveConstPool(boost::shared_ptr<JavaClass> & clazz);

		size_t _byteCodeLength;
		u1 * _byteCode;
		u1 * _byteCodeOffset;
};
