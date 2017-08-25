#pragma once

#include <boost/filesystem.hpp>

class FilePathManager
{
	public:
		FilePathManager(void);
		~FilePathManager(void);
	public:
		boost::filesystem::path getAbsolutePath(const std::string & className);
};
