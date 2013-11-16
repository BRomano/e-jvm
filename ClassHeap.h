#pragma once

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

class JavaClass;
class FilePathManager;

/**
 * this class is a class to store all classes from JVM
 */
class HeapClass
{
	private:
		typedef std::map<std::string, boost::shared_ptr<JavaClass> > classMap;
		classMap _classMap; //TODO put full class name
		FilePathManager * pFilePathManager;

	public:
		HeapClass(void);
		virtual ~HeapClass(void);

	public:
		boost::shared_ptr<JavaClass> addClass(boost::shared_ptr<JavaClass> javaClass);
		boost::shared_ptr<JavaClass> getClass(const std::string & className);
		boost::shared_ptr<JavaClass> loadClass(const std::string & className);
};
