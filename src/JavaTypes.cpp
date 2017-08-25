#include "JavaTypes.h"
#include "JavaClass.h"
#include "JavaClassParser.h"
#include <boost/shared_ptr.hpp>

/*
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.3
 * This code extract the elements from info to this class.
 */
void Code_attribute::parse(JavaClassParser * parser, boost::shared_ptr<JavaClass> clazz, u1 * info)
{
	this->max_stack = getu2(info);
	this->max_locals = getu2(info);
	this->code_length = getu4(info);
	info += 4;

	if (this->code_length > 0)
	{
		this->code = new u1[this->code_length];
		memcpy(this->code, info, this->code_length);
		info += this->code_length;
	}
	else
	{
		this->code = NULL;
	}
	
	u2 exception_table_length = getu2(info);
	this->_exception_table.resize(exception_table_length);
//	this->_exception_table = new Exception_table[this->exception_table_length];
	for (std::size_t i = 0; i < this->_exception_table.size(); i++)
	{
		this->_exception_table[i] = new Exception_table();
		this->_exception_table[i]->start_pc = getu2(info);
		this->_exception_table[i]->end_pc = getu2(info);
		this->_exception_table[i]->handler_pc = getu2(info);
		this->_exception_table[i]->catch_type = getu2(info);
	}

	u2 attributes_count = getu2(info);
	this->_attributes.resize(attributes_count);
	parser->parseAttributes(this->_attributes, clazz, info);
}

/**
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.5
 */
void Exceptions_attribute::parse(JavaClassParser * parser, boost::shared_ptr<JavaClass> clazz, u1 * info)
{
	u2 number_of_exceptions = getu2(info);
	//this->exception_index_table = new u2[this->number_of_exceptions];
	_exception_index_table.resize(number_of_exceptions);
	for(std::size_t i = 0; i < this->_exception_index_table.size(); i++)
		this->_exception_index_table[i] = getu2(info);
}

/**
 * 
 */
void SourceFile_attribute::parse(JavaClassParser * parser, boost::shared_ptr<JavaClass> clazz, u1 * info)
{
	this->sourcefile_index = getu2(info);
}

void Dummy_attribute::parse(JavaClassParser * parser, boost::shared_ptr<JavaClass> clazz, u1 * info)
{
}