#pragma once

#include "StdAfx.h"
#include "JavaClass.h"
#include "shlwapi.h"
#include "ClassHeap.h"
#include "ObjectHeap.h"
#include "jException.h"

#include <string>
#include <boost/weak_ptr.hpp>

bool JavaClass::getStringFromConstPool(u4 index, std::string & str)
{
	if (index < 1 || index >= (u4)_constantPool.size())
		return false;

	if (_constantPool[index]->tag != CONSTANT_Utf8)
		return false;

	CONSTANT_Utf8_info * utf8 = reinterpret_cast<CONSTANT_Utf8_info *>(_constantPool[index]);
	str.assign((char *)utf8->bytes, utf8->length);
	return true;
}

std::string JavaClass::getClassName(u2 classId)
{
	std::string retVal;
	if(_constantPool[classId]->tag != CONSTANT_Class)
		return retVal; //TODO throw exception

	CONSTANT_Class_info_resolve * resolve = reinterpret_cast<CONSTANT_Class_info_resolve *>(_constantPool[classId]);
	retVal.assign((char *)resolve->_name_index->bytes, resolve->_name_index->length);
	return retVal;
}

std::string JavaClass::getThisClassName(void)
{
	return this->getClassName(this_class);
}

std::string JavaClass::getSuperClassName(void)
{
	std::string super_class_name;
	if (super_class < 1)
		throw std::exception("There is no class");

	return this->getClassName(super_class);
}

Code_attribute * JavaClass::getMethodCodeAttribute(u2 m_index)
{
	std::string name;
	if (m_index > this->_methods.size())
		ASSERT(false);

	for (std::size_t i = 0; i < this->_methods[m_index]->_attributes.size(); i++)
	{
		u2 index = this->_methods[m_index]->_attributes[i]->attribute_name_index;
		if (!this->getStringFromConstPool(index, name))
			ASSERT(false); //this attribute is not a code

		if (name == "Code")
			return reinterpret_cast<Code_attribute *>(this->_methods[m_index]->_attributes[i]);
	}
	throw jException("No such method");
}

/*find method index, if can't find method index then try to find in his father*/
u2 JavaClass::getMethodID(const std::string & methodName, const std::string & methodDesc, jClass & clazz)
{
	if (_methods.size() == 0)
		throw std::exception("There are no methods");

	int i = 0;
	clazz = this->shared_from_this();
	
	while (true)
	{
		for (i = 0; i < (u2)clazz->_methods.size(); i++)
		{
			std::string name, desc;
			clazz->getStringFromConstPool(clazz->_methods[i]->name_index, name);
			if (name != methodName)
				continue;

			clazz->getStringFromConstPool(clazz->_methods[i]->descriptor_index, desc);
			if (desc == methodDesc)
				break;
		}

		try
		{
			if (i < (u2)clazz->_methods.size())
				break;
			else
				clazz = clazz->getSuperClass(); //needs find method in father
		}
		catch(std::exception & ex) // can't find class
		{
			ASSERT(false); //TODO throw nosuchclassexception, throw jException
			i = -1;
			break;
		}
	}

	return i;
}

u2 JavaClass::getFieldIndex(u2 u2Name, u2 u2Desc)
{
	for (std::size_t i = 1; i < this->_constantPool.size(); i++)
	{
		if (this->_constantPool[i]->tag != CONSTANT_Fieldref)
			continue;
		
		CONSTANT_Fieldref_info * fieldref = reinterpret_cast<CONSTANT_Fieldref_info *>(this->_constantPool[i]);
		if (fieldref->class_index != this->this_class)
			continue; // I'm not sure
		
		CONSTANT_NameAndType_info * name_type = reinterpret_cast<CONSTANT_NameAndType_info *>(this->_constantPool[fieldref->name_and_type_index]);
		if (name_type->name_index != u2Name)
			continue;

		if (name_type->descriptor_index != u2Desc)
			continue;
		
		return i;
	}

	//TODO
	return 0; //nosuchfield
}

/**
 * 
 */
u1 JavaClass::getFieldType(u2 u2Desc)
{
	std::string desc;
	this->getStringFromConstPool(u2Desc, desc);
	if (desc[0] == 'L') //putfield uses field type to increment or decrement reference
		return 'n';

	return desc[0];
}

u4 JavaClass::getObjectSize(void)
{
	u4 size = _fields.size() * sizeof(Variable);
	boost::shared_ptr<JavaClass> super_class;
	
	try
	{
		super_class = _heapClass->getClass(getSuperClassName());
	}
	catch(std::exception & ex)
	{
		return size;
	}

	size += super_class->getObjectSize();
	return size;
}

u4 JavaClass::getFieldCount(void)
{
	u4 count = _fields.size();
	boost::shared_ptr<JavaClass> super_class;

	try
	{
		super_class = _heapClass->getClass(getSuperClassName());
	}
	catch(std::exception & ex)
	{
		return count;
	}

	count += super_class->getFieldCount();
	return count;
}

boost::shared_ptr<JavaClass> JavaClass::resolveClassName(u2 index)
{
	CPBase * cp = this->_constantPool[index];
	if (cp->tag != CONSTANT_Class)
		/*http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-5.html#jvms-5.4.3.1*/
		ASSERT(false); // TODO throw exception 

	CONSTANT_Class_info_resolve * cci_resolve = reinterpret_cast<CONSTANT_Class_info_resolve *>(cp);

	std::string class_name;
	class_name.assign((char *)cci_resolve->_name_index->bytes, cci_resolve->_name_index->length);
	printf("Creating new object of class [%s]\n", class_name.c_str());
	boost::shared_ptr<JavaClass> clazz(_heapClass->getClass(class_name));

	return clazz;
}

/**
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.4
 */
Variable JavaClass::loadConstantItem(u1 index, Heap * heap)
{
	Variable v;
	v._var._primitive._float = 0;
	CPBase * cp = this->_constantPool[index];

	switch(cp->tag)
	{
		case CONSTANT_Integer:
		{
			CONSTANT_Integer_info * cii = reinterpret_cast<CONSTANT_Integer_info *>(cp);
			v._var._primitive._int = (int)cii->bytes;
			v._type = 'I';
			break;
		}

		case CONSTANT_Float:
		{
			/*
			 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-2.html#jvms-2.3.2
			 * float has a specific type.
			 */
			CONSTANT_Float_info * cfi = reinterpret_cast<CONSTANT_Float_info *>(cp);
			v._var._primitive._int = cfi->bytes;
			v._type = 'F';
			break;
		}
		case CONSTANT_String:
		{
			CONSTANT_String_info * csi = reinterpret_cast<CONSTANT_String_info *>(cp);
			std::string str;
			this->getStringFromConstPool(csi->string_index, str);
			Object * object = heap->createStringObject(_heapClass);
			((std::string *)object->_objFields[0]._value._var._primitive._ptrValue)->append(str);
			v._var._objectref = object;
			v._type = 'L';
			break;
		}

	case CONSTANT_Double:
		break;

	case CONSTANT_Long:
		break;		
	}

	return v;
}

/*TODO test if this_class extends from superClass*/
bool JavaClass::isInheritFrom(const std::string & catch_type_name)
{
	std::string class_name;
	class_name = this->getThisClassName();
	if (class_name.compare(catch_type_name) == 0)
		return true;

	return this->getSuperClass()->isInheritFrom(catch_type_name);
}
