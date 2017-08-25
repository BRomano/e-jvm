#include "FilePathManager.h"
//#include "shlwapi.h"
//#pragma comment(lib, "shlwapi.lib")

#include <boost/filesystem.hpp>

FilePathManager::FilePathManager(void)
{

}

FilePathManager::~FilePathManager(void)
{

}

boost::filesystem::path FilePathManager::getAbsolutePath(const std::string & className)
{
	boost::filesystem::path full_path(boost::filesystem::current_path());
	full_path /= "lib";
	full_path /= className;

	if (!boost::filesystem::exists(full_path) && !boost::filesystem::is_regular_file(full_path))
	{
		//TODO throw nosuchClass
		assert(false);
		throw std::exception();
		//return std::string("");
	}

	//full_path = boost::filesystem::canonical(full_path);
	//full_path.native();
	//std::string a = full_path.string();
	//full_path.normalize();
	return full_path;
}
