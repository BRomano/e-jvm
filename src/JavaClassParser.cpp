#include "shlwapi.h"

#include "JavaClassParser.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

bool JavaClassParser::loadClassFromFile(const boost::filesystem::path & path) throw (std::exception)
{
	if (!boost::filesystem::exists(path))
	{
		assert(false);
	}

	boost::filesystem::ifstream ifs;
	ifs.open(path, std::ios::in);
	if (!ifs)
	{
		assert(false);
	}
	_byteCodeLength = boost::filesystem::file_size(path);
	_byteCode = new u1[_byteCodeLength + 2];
	ifs.read((char *)_byteCode, _byteCodeLength);
	if (ifs.eof())
		ifs.close();

	FILE * file = fopen(path.string().c_str(), "rb");
	if (file == NULL)
		throw std::runtime_error("FUCK FILE");
//	fseek(file, 0, SEEK_END);
//   _byteCodeLength = ftell(file);
//	if (_byteCodeLength <= 0)
//		throw std::exception("FUCK FILE");
//	rewind(file);
	size_t size = fread(_byteCode, 1, _byteCodeLength, file);
	ASSERT(size == _byteCodeLength);
	fclose(file);

	
	ASSERT(_byteCode != NULL);

//	USES_CONVERSION;
//	CString fname(A2W(filePath.c_str()));
//	CFile cfile(fname, CFile::modeRead);
//	UINT size = cfile.Read(_byteCode, _byteCodeLength);	

	if (_byteCode)
	{
		_byteCode[_byteCodeLength] = 0;
		return true;
	}

	return false;
}


bool JavaClassParser::parseClass(const boost::filesystem::path & path, boost::shared_ptr<JavaClass> & clazz)
{
	try
	{
		this->loadClassFromFile(path);
	}
	catch(...)
	{
		ASSERT(FALSE);
	}
	printf("Parsing class %s\n", path.string().c_str());

	_byteCodeOffset = _byteCode;
	clazz->magic = getu4(_byteCodeOffset);
	_byteCodeOffset += 4;

	ASSERT(clazz->magic == 0xCAFEBABE);

	clazz->minor_version = getu2(_byteCodeOffset); //short
	clazz->major_version = getu2(_byteCodeOffset); //short

	parseConstantPool(clazz->_constantPool, clazz);

    clazz->access_flags = getu2(_byteCodeOffset);
    clazz->this_class = getu2(_byteCodeOffset);
//	printf("Parsing class %s\n", clazz->getThisClassName().c_str());

    clazz->super_class = getu2(_byteCodeOffset);
	
	parseInterfaces(clazz->_interfaces, clazz);
	/*I can parse method and field using the same method*/
	parseFields(clazz->_fields, clazz);
	parseFields(clazz->_methods, clazz);

	u2 attributes_count = getu2(_byteCodeOffset);
	clazz->_attributes.resize(attributes_count);
	parseAttributes(clazz->_attributes, clazz, _byteCodeOffset);

	/*here is a small trick to make debug easy, and optimize some search*/
	resolveConstPool(clazz);

	//TODO create a check Class
	//This check class needs to validate if exception catch_type inherit from Throwable

	if (_byteCode)
		delete[] _byteCode;
	return 0;
}

bool JavaClassParser::parseInterfaces(std::vector<u2> & interfaces, boost::shared_ptr<JavaClass> & clazz)
{
	u2 interface_count = getu2(_byteCodeOffset);
	interfaces.resize(interface_count);
	if (interface_count <= 0)
		return false;

	std::string interface_name;
	for(std::size_t i = 0; i < interfaces.size(); i++)
	{
		interfaces[i] = getu2(_byteCodeOffset);
		clazz->getStringFromConstPool(interfaces[i], interface_name);
		printf("Interface at %s\n", interface_name.c_str());
	}

	return true;
}

bool JavaClassParser::parseConstantPool(std::vector<CPBase *> & constantPool, boost::shared_ptr<JavaClass> & clazz)
{
	u2 constant_pool_count = getu2(_byteCodeOffset);
	constantPool.resize(constant_pool_count);
	if (constant_pool_count <= 0)
		return false;

	//Where index value 1 refers to the first constant in the table (index value 0 is invalid)
	for(size_t i = 1; i < constantPool.size(); i++)
	{
		constantPool[i] = getConstantPoolObj();
		//printf("Constant pool %d type %d\n", i, (u1)constantPool[i]->tag); //for debug

		/* Additionally, two types of constants (longs and doubles) take up two consecutive 
			slots in the table, although the second such slot is a phantom index that is never directly used */
		if (constantPool[i]->tag == CONSTANT_Long || constantPool[i]->tag == CONSTANT_Double)
		{
			constantPool[i + 1] = NULL;
			i++;
		}
	}

	return true;
}

/**
 * This method can parse fields and methods, because they are similar
 */
bool JavaClassParser::parseFields(std::vector<method_and_field_info *> & fields, boost::shared_ptr<JavaClass> & clazz)
{
	u2 field_count = getu2(_byteCodeOffset);
	fields.resize(field_count);
	if (field_count <= 0)
		return false;

	for (std::size_t i = 0; i < fields.size(); i++)
	{
		fields[i] = new method_and_field_info();
		u2 acc = getu2(_byteCodeOffset);
		fields[i]->setAccessFlags(acc);
		fields[i]->name_index = getu2(_byteCodeOffset);
		fields[i]->descriptor_index = getu2(_byteCodeOffset);
		//fields[i]->attributes_count = getu2(_byteCodeOffset);
		
		u2 attribute_count = getu2(_byteCodeOffset);
		fields[i]->_attributes.resize(attribute_count);
		this->parseAttributes(fields[i]->_attributes, clazz, _byteCodeOffset);
		
		std::string name, desc;
		clazz->getStringFromConstPool(fields[i]->name_index, name);
		clazz->getStringFromConstPool(fields[i]->descriptor_index, desc);
		printf("Parsed -> %s:%s\n", name.c_str(), desc.c_str());
	}

	return true;
}

/**
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7
 */
bool JavaClassParser::parseAttributes(std::vector<attribute_info *> & attributes, boost::shared_ptr<JavaClass> & clazz, u1 *& bytecode)
{
	if (attributes.size() <= 0)
		return false;

	std::string name;
	for(size_t i = 0; i < attributes.size(); i++)
	{
		u2 attribute_name_index = getu2(bytecode);
		u4 attribute_length = getu4(bytecode);
		bytecode += 4;

		clazz->getStringFromConstPool(attribute_name_index, name);
		if (name.compare("Code") == 0)
			attributes[i] = new Code_attribute();
		else if (name.compare("Exceptions") == 0)
			attributes[i] = new Exceptions_attribute();
		else if (name.compare("SourceFile") == 0)
			attributes[i] = new SourceFile_attribute();
		else if (name.compare("LineNumberTable") == 0)
			attributes[i] = new Dummy_attribute();
		else if (name.compare("StackMapTable") == 0)
			attributes[i] = new Dummy_attribute();
		else
			ASSERT(FALSE); //unknow

		attributes[i]->attribute_name_index = attribute_name_index;
		attributes[i]->attribute_length = attribute_length;
		attributes[i]->parse(this, clazz->shared_from_this(), bytecode);
		bytecode += attributes[i]->attribute_length;
		
		printf("Attribute name is %s\n", name.c_str());
	}

	return true;
}

//metodo horrivel, modificar o design dele
CPBase * JavaClassParser::getConstantPoolObj()
{
	u1 tag = *_byteCodeOffset;

	switch(tag)
	{
		case CONSTANT_Class:
		{
			CONSTANT_Class_info_resolve * r = new CONSTANT_Class_info_resolve(_byteCodeOffset);
			_byteCodeOffset += 3;
			return r;
		}
		case CONSTANT_Fieldref:
		{
			CONSTANT_Fieldref_info_resolve * r = new CONSTANT_Fieldref_info_resolve(_byteCodeOffset);
			_byteCodeOffset += 5;
			return r;
		}
		case CONSTANT_Methodref:
		{
			CONSTANT_Methodref_info_resolve * r = new CONSTANT_Methodref_info_resolve(_byteCodeOffset);
			_byteCodeOffset += 5;
			return r;
		}
		case CONSTANT_InterfaceMethodref:
		{
			CONSTANT_InterfaceMethodref_info_resolve * r = new CONSTANT_InterfaceMethodref_info_resolve(_byteCodeOffset);
			_byteCodeOffset += 5;
			return r;
		}
		case CONSTANT_String:
		{
			CONSTANT_String_info_resolve * r = new CONSTANT_String_info_resolve(_byteCodeOffset);
			_byteCodeOffset += 3;
			return r;
		}
		case CONSTANT_Integer:
		{
			CONSTANT_Integer_info * r = new CONSTANT_Integer_info(_byteCodeOffset);
			_byteCodeOffset += 5;
			return r;
		}
		case CONSTANT_Float:
		{
			CONSTANT_Float_info * r = new CONSTANT_Float_info(_byteCodeOffset);
			_byteCodeOffset += 5;
			return r;
		}
		case CONSTANT_Long:
		{
			CONSTANT_Long_info * r = new CONSTANT_Long_info(_byteCodeOffset);
			_byteCodeOffset += 9;
			return r;
		}
		case CONSTANT_Double:
		{
			CONSTANT_Double_info * r = new CONSTANT_Double_info(_byteCodeOffset);
			_byteCodeOffset += 9;
			return r;
		}
		case CONSTANT_NameAndType:
		{
			CONSTANT_NameAndType_info_resolve * r = new CONSTANT_NameAndType_info_resolve(_byteCodeOffset);
			_byteCodeOffset += 5;
			return r;
		}
		case CONSTANT_Utf8:
		{
			CONSTANT_Utf8_info * utf8 = new CONSTANT_Utf8_info(_byteCodeOffset);
			_byteCodeOffset += (3 + utf8->length);
			return utf8;
		}
		default:
		{
			ASSERT(FALSE);
			break;
		}
	}
	
	return NULL;
}

void JavaClassParser::resolveConstPool(boost::shared_ptr<JavaClass> & clazz)
{
	for (size_t i = 1; i < clazz->_constantPool.size(); i++)
	{
		switch(clazz->_constantPool[i]->tag)
		{
			case CONSTANT_Class:
			{
				CONSTANT_Class_info_resolve * resolve = reinterpret_cast<CONSTANT_Class_info_resolve *>(clazz->_constantPool[i]);
				resolve->_name_index = reinterpret_cast<CONSTANT_Utf8_info *>(clazz->_constantPool[resolve->name_index]);
				break;
			}
			case CONSTANT_Fieldref:
			{
				CONSTANT_Fieldref_info_resolve * resolve = reinterpret_cast<CONSTANT_Fieldref_info_resolve *>(clazz->_constantPool[i]);
				resolve->_class_index = reinterpret_cast<CONSTANT_Class_info_resolve *>(clazz->_constantPool[resolve->class_index]);
				resolve->_name_and_type_index = reinterpret_cast<CONSTANT_NameAndType_info_resolve *>(clazz->_constantPool[resolve->name_and_type_index]);
				break;
			}
			case CONSTANT_Methodref:
			{
				CONSTANT_Methodref_info_resolve * resolve = reinterpret_cast<CONSTANT_Methodref_info_resolve *>(clazz->_constantPool[i]);
				resolve->_class_index = reinterpret_cast<CONSTANT_Class_info_resolve *>(clazz->_constantPool[resolve->class_index]);
				resolve->_name_and_type_index = reinterpret_cast<CONSTANT_NameAndType_info_resolve *>(clazz->_constantPool[resolve->name_and_type_index]);
				break;
			}
			case CONSTANT_InterfaceMethodref:
			{
				CONSTANT_InterfaceMethodref_info_resolve * resolve = reinterpret_cast<CONSTANT_InterfaceMethodref_info_resolve *>(clazz->_constantPool[i]);
				resolve->_class_index = reinterpret_cast<CONSTANT_Class_info_resolve *>(clazz->_constantPool[resolve->class_index]);
				resolve->_name_and_type_index = reinterpret_cast<CONSTANT_NameAndType_info_resolve *>(clazz->_constantPool[resolve->name_and_type_index]);
				break;
			}
			case CONSTANT_String:
			{
				CONSTANT_String_info_resolve * resolve = reinterpret_cast<CONSTANT_String_info_resolve *>(clazz->_constantPool[i]);
				resolve->_string_info = reinterpret_cast<CONSTANT_Utf8_info *>(clazz->_constantPool[resolve->string_index]);
				break;
			}
			case CONSTANT_Integer:
			{
				break;
			}
			case CONSTANT_Float:
			{
				break;
			}
			case CONSTANT_Long:
			{
				break;
			}
			case CONSTANT_Double:
			{
				break;
			}
			case CONSTANT_NameAndType:
			{
				CONSTANT_NameAndType_info_resolve * resolve = reinterpret_cast<CONSTANT_NameAndType_info_resolve *>(clazz->_constantPool[i]);
				resolve->_name_index = reinterpret_cast<CONSTANT_Utf8_info *>(clazz->_constantPool[resolve->name_index]);
				resolve->_descriptor_index = reinterpret_cast<CONSTANT_Utf8_info *>(clazz->_constantPool[resolve->descriptor_index]);
				break;
			}
			case CONSTANT_Utf8:
			{
				break;
			}
			default:
			{
				ASSERT(FALSE);
				break;
			}
		}
	}
}
