#include "StdAfx.h"

#include "ClassHeap.h"
#include "JavaClass.h"
#include "FilePathManager.h"
#include "JavaClassParser.h"

#include <boost/shared_ptr.hpp>

HeapClass::HeapClass(void)
{
	pFilePathManager = new FilePathManager();
}

HeapClass::~HeapClass(void)
{
	if (pFilePathManager)
	{
		delete pFilePathManager;
		pFilePathManager = NULL;
	}
}

boost::shared_ptr<JavaClass> HeapClass::addClass(boost::shared_ptr<JavaClass> javaClass)
{
	std::string name(javaClass->getThisClassName());
	std::pair<classMap::iterator, bool> pair = _classMap.insert(std::make_pair(name, javaClass));
	if (pair.second == false)
		assert(false);

	return pair.first->second;
}

jClass HeapClass::getClass(const std::string & className)
{
	classMap::iterator it = _classMap.find(className);
	if (it != _classMap.end())
		return it->second;

	//need to load class
	boost::shared_ptr<JavaClass> java_class(new JavaClass());
	return this->loadClass(className);
}

/*TODO verify class in cache and return that*/
boost::shared_ptr<JavaClass> HeapClass::loadClass(const std::string & className)
{
	classMap::iterator it = _classMap.find(className); //TODO verificar o path completo, nao somente o nome da classe...
	if (it != _classMap.end())
		return it->second;

	std::string class_name;
	class_name = className;
	class_name += ".class";

	boost::filesystem::path p;
	p = pFilePathManager->getAbsolutePath(class_name);
	//if (p.empty())
	//	throw std::exception("path not found");

	jClass clazz(new JavaClass());
	JavaClassParser parser;
	parser.parseClass(p, clazz);

	clazz->setClassHeap(this);
	return addClass(clazz);
}
