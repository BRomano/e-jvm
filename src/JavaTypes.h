#pragma once

#include "types.h"
#include <cstring>
#include <vector>
#include <map>
#include <stack>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

class JavaClassParser;
//class JavaClass;
//typedef boost::shared_ptr<JavaClass> jClass;

/**
 * I change these values. All these structs are declared different on Oracle JVM specification.
 */
//constant_pool base

/*
 * To use sizeof instead plus size in number
 */

#pragma pack(push, 1)
struct CPBase
{
	u1 tag;
	virtual ~CPBase() { } //I can't use this
};

struct CONSTANT_Utf8_info : public CPBase
{
	u2 length;
	u1 * bytes;//[length];

	CONSTANT_Utf8_info(u1 * bc)
	{
		this->tag = *bc;
		bc++;
		this->length = getu2(bc);
		bytes = new u1[this->length];
		std::memcpy(this->bytes, bc, this->length);
	}

	virtual ~CONSTANT_Utf8_info()
	{
		delete[] bytes;
	}
};

/**
 * the inheritance doesn't work good because I can't use memcpy, the data read is BE I need change these values when I read, using getu2, getu4...
 * then I prefer use what jvm specification use a cp_info with two attributes a tag and a array...
 * and use cast in code, this is a little ugly but what can I do ?...
 */
struct CONSTANT_Class_info : public CPBase
{
	u2 name_index;

	CONSTANT_Class_info(u1 * bc)
	{
		this->tag = *bc;
		bc++;
		this->name_index = getu2(bc);
	}
};

struct CONSTANT_Class_info_resolve : public CONSTANT_Class_info
{
	CONSTANT_Utf8_info * _name_index;

	CONSTANT_Class_info_resolve(u1 * bc) : CONSTANT_Class_info(bc), _name_index(NULL)
	{
		//TODO resolve a link object
	}
};

struct CONSTANT_NameAndType_info : public CPBase
{
	u2 name_index;
	u2 descriptor_index;

	CONSTANT_NameAndType_info(u1 * bc)
	{
		this->tag = *bc;
		bc++;
		this->name_index = getu2(bc);
		this->descriptor_index = getu2(bc);
	}
};

struct CONSTANT_NameAndType_info_resolve : public CONSTANT_NameAndType_info
{
	CONSTANT_Utf8_info * _name_index;
	CONSTANT_Utf8_info * _descriptor_index;

	CONSTANT_NameAndType_info_resolve(u1 * bc) : CONSTANT_NameAndType_info(bc), _name_index(NULL), _descriptor_index(NULL)
	{
	}
};

struct CONSTANT_Fieldref_info : public CPBase
{
	u2 class_index;
	u2 name_and_type_index;

	CONSTANT_Fieldref_info(u1 * bc)
	{
		this->tag = *bc;
		bc++;
		this->class_index = getu2(bc);
		this->name_and_type_index = getu2(bc);
	}
};

struct CONSTANT_Fieldref_info_resolve : public CONSTANT_Fieldref_info
{
	CONSTANT_Class_info_resolve * _class_index;
	CONSTANT_NameAndType_info_resolve * _name_and_type_index;

	CONSTANT_Fieldref_info_resolve(u1 * bc) : CONSTANT_Fieldref_info(bc), _class_index(NULL), _name_and_type_index(NULL)
	{
	}
};

struct CONSTANT_Methodref_info : public CPBase
{
	u2 class_index;
	u2 name_and_type_index;

	CONSTANT_Methodref_info(u1 * bc)
	{
		this->tag = *bc;
		bc++;
		this->class_index = getu2(bc);
		this->name_and_type_index = getu2(bc);
	}
};

struct CONSTANT_Methodref_info_resolve : public CONSTANT_Methodref_info
{
	CONSTANT_Class_info_resolve * _class_index;
	CONSTANT_NameAndType_info_resolve * _name_and_type_index;

	CONSTANT_Methodref_info_resolve(u1 * bc) : CONSTANT_Methodref_info(bc), _class_index(NULL), _name_and_type_index(NULL)
	{
	}
};

struct CONSTANT_InterfaceMethodref_info : public CPBase
{
	u2 class_index;
	u2 name_and_type_index;

	CONSTANT_InterfaceMethodref_info(u1 * bc)
	{
		this->tag = *bc;
		bc++;
		this->class_index = getu2(bc);
		this->name_and_type_index = getu2(bc);
	}
};

struct CONSTANT_InterfaceMethodref_info_resolve : public CONSTANT_InterfaceMethodref_info
{
	CONSTANT_Class_info_resolve * _class_index;
	CONSTANT_NameAndType_info_resolve * _name_and_type_index;

	CONSTANT_InterfaceMethodref_info_resolve(u1 * bc) : CONSTANT_InterfaceMethodref_info(bc), _class_index(NULL), _name_and_type_index(NULL)
	{
	}
};

struct CONSTANT_String_info : public CPBase
{
	u2 string_index;

	CONSTANT_String_info(u1 * bc)
	{
		this->tag = *bc;
		bc++;
		this->string_index = getu2(bc);
	}
};

struct CONSTANT_String_info_resolve : public CONSTANT_String_info
{
	CONSTANT_Utf8_info * _string_info;

	CONSTANT_String_info_resolve(u1 * bc) : CONSTANT_String_info(bc), _string_info(NULL)
	{
	}
};

struct CONSTANT_Integer_info : public CPBase
{
	u4 bytes;
	
	CONSTANT_Integer_info(u1 * bc)
	{
		this->tag = *bc;
		bc++;
		this->bytes = getu4((char *)bc);
	}
};

struct CONSTANT_Float_info : public CPBase
{
	u4 bytes;

	CONSTANT_Float_info(u1 * bc)
	{
		this->tag = *bc;
		bc++;
		this->bytes = getu4((char *)bc);
	}
};

struct CONSTANT_Long_info : public CPBase
{
	u4 high_bytes;
	u4 low_bytes;

	CONSTANT_Long_info(u1 * bc)
	{
		this->tag = *bc;
		bc++;
		this->high_bytes = getu4((char *)bc);
		bc += 4;
		this->low_bytes = getu4((char *)bc);
	}
};

struct CONSTANT_Double_info : public CPBase
{
	u4 high_bytes;
	u4 low_bytes;

	CONSTANT_Double_info(u1 * bc)
	{
		this->tag = *bc;
		bc++;
		this->high_bytes = getu4((char *)bc);
		bc += 4;
		this->low_bytes = getu4((char *)bc);
	}
};

struct u2_resolved
{
	u2 index;
	CONSTANT_Utf8_info * _string_info;
};

#pragma pack(pop)

class JavaClass;
/*
 * The Java virtual machine does not mandate any particular internal structure for objects.
 * In some of Oracle�s implementations of the Java virtual machine,
 * a reference to a class instance is a pointer to a handle that is itself a pair of pointers: 
 * one to a table containing the methods of the object and a pointer to the Class object that represents 
 * the type of the object, and the other to the memory allocated from the heap for the object data. 
 */

/**
	B 	byte 	signed byte
	C 	char 	Unicode character code point in the Basic Multilingual Plane, encoded with UTF-16
	D 	double 	double-precision floating-point value
	F 	float 	single-precision floating-point value
	I 	int 	integer
	J 	long 	long integer
	L ClassName ; 	reference 	an instance of class ClassName
	S 	short 	signed short
	Z 	boolean 	true or false
	[ 	reference 	one array dimension

T_BOOLEAN 	4
T_CHAR 	5
T_FLOAT 	6
T_DOUBLE 	7
T_BYTE 	8
T_SHORT 	9
T_INT 	10
T_LONG 	11

 */

struct Object;

union PrimitiveType
{
	jchar _char;
	jshort _short;
	jint _int;
	jfloat _float;
	void * _ptrValue;
};

/**
 * _type is a char and his value can be these below
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.2
 *
 * Standard types
 * 'B' byte
 * 'C' char
 * 'D' double
 * 'F' floar
 * 'I' int
 * 'J' long
 * 'S' short
 * 'Z' boolean
 * 'L' object reference
 * '[' array dimension
 *
 * Non standard types
 * 'n' null
 * 'N' native pointer
 */
struct Variable
{
	Variable() : _type('n') { }
	union
	{
		PrimitiveType _primitive;
		Object * _objectref;
	} _var;

	u1 _type;
};

struct ObjectFields
{
	ObjectFields() : _field_index(0)
	{
		_value._var._primitive._float = 0;
		_value._var._objectref = NULL;
	}

	u2 _field_index; //this is a index_name in constant_pool.
	Variable _value; // this is a value
};

struct Object
{
	Object() : _objFields(NULL), _countFields(0), _monitor(0), _reference(0) { }
	boost::shared_ptr<JavaClass> _class; //pointer to Java class
	ObjectFields * _objFields;
	int _countFields; //size of _objFields
	int _monitor; //monitor for monitorenter and monitorexit
	int _reference; //how much reference this object has, for gc
};

/**
 * Attributes are used in the ClassFile (�4.1), field_info (�4.5), method_info (�4.6) and Code_attribute (�4.7.3) structures of the class file format. 
 * All attributes have the following general format: 
 *
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.21
 * see this link to more information about attributes.
 * I made an inherit relationalship here.
 */
struct attribute_info
{
	//TODO create a link for this attribute_name_index in constantPool
    u2 attribute_name_index;
    u4 attribute_length;
//    u1 * info;//[attribute_length]; //TODO link enum for point propertie
	 /**
	  * May be something like below
			ConstantValue (�4.7.2) 	1.0.2 	45.3
			Code (�4.7.3) 	1.0.2 	45.3
			StackMapTable (�4.7.4) 	6 	50.0
			Exceptions (�4.7.5) 	1.0.2 	45.3
			InnerClasses (�4.7.6) 	1.1 	45.3
			EnclosingMethod (�4.7.7) 	5.0 	49.0
			Synthetic (�4.7.8) 	1.1 	45.3
			Signature (�4.7.9) 	5.0 	49.0
			SourceFile (�4.7.10) 	1.0.2 	45.3
			SourceDebugExtension (�4.7.11) 	5.0 	49.0
			LineNumberTable (�4.7.12) 	1.0.2 	45.3
			LocalVariableTable (�4.7.13) 	1.0.2 	45.3
			LocalVariableTypeTable (�4.7.14) 	5.0 	49.0
			Deprecated (�4.7.15) 	1.1 	45.3
			RuntimeVisibleAnnotations (�4.7.16) 	5.0 	49.0
			RuntimeInvisibleAnnotations (�4.7.17) 	5.0 	49.0
			RuntimeVisibleParameterAnnotations (�4.7.18) 	5.0 	49.0
			RuntimeInvisibleParameterAnnotations (�4.7.19) 	5.0 	49.0
			AnnotationDefault (�4.7.20) 	5.0 	49.0
			BootstrapMethods (�4.7.21) 	7 	51.0
	  */

	 virtual void parse(JavaClassParser * parser, boost::shared_ptr<JavaClass> clazz, u1 * info) = 0;
	 virtual ~attribute_info() { }
};

struct Exception_table
{
	u2 start_pc;
	u2 end_pc;
	u2 handler_pc;
	u2 catch_type;
};

class Code_attribute : public attribute_info
{
public:
//	u2						attribute_name_index;
//	u4						attribute_length;
	u2						max_stack;
	u2						max_locals;
	u4						code_length;
	u1 *					code;//[code_length];
//	u2						exception_table_length; _exception_table.size()
//	u2						attributes_count; _attributes.size()

	std::vector<Exception_table *> _exception_table;//[exception_table_length];
	std::vector<attribute_info *>  _attributes;//[attributes_count];

	virtual void parse(JavaClassParser * parser, boost::shared_ptr<JavaClass> clazz, u1 * info);

	Code_attribute() : code(NULL) { }
	virtual ~Code_attribute()
	{
		for (std::size_t i = 0; i < _exception_table.size(); i++)
			delete _exception_table[i];

		for (std::size_t i = 0; i < _attributes.size(); i++)
			delete _attributes[i];

		if (code)
		{
			delete[] code;
			code = NULL;
		}
	}
};

class Exceptions_attribute : public attribute_info
{
//	u2 attribute_name_index;
//	u4 attribute_length;
//	u2 number_of_exceptions;

	/**
	 * Each value in the exception_index_table array must be a valid index into the constant_pool table.
	 * The constant_pool entry referenced by each table item must be a CONSTANT_Class_info structure (�4.4.1) representing a class type that this method is declared to throw.
	 */
	std::vector<u2> _exception_index_table;//[number_of_exceptions];
	virtual void parse(JavaClassParser * parser, boost::shared_ptr<JavaClass> clazz, u1 * info);
	virtual ~Exceptions_attribute() { }
};

class SourceFile_attribute : public attribute_info
{
//    u2 attribute_name_index;
//    u4 attribute_length;
    u2 sourcefile_index;
	virtual void parse(JavaClassParser * parser, boost::shared_ptr<JavaClass> clazz, u1 * info);
	virtual ~SourceFile_attribute() {}
};

class Dummy_attribute : public attribute_info
{
//    u2 attribute_name_index;
//    u4 attribute_length;
	virtual void parse(JavaClassParser * parser, boost::shared_ptr<JavaClass> clazz, u1 * info);
	virtual ~Dummy_attribute() { }
};


struct method_and_field_info
{
public:
	method_and_field_info() { }
	virtual ~method_and_field_info()
	{
		for (std::size_t i = 0; i < _attributes.size(); i++)
			delete _attributes[i];
	}

	u2 name_index;
	u2 descriptor_index;
//	u2 attributes_count;
	std::vector<attribute_info *> _attributes;//[attributes_count];

	u2 getAccessFlags() { return _access_flags; }
	void setAccessFlags(u2 access) { this->_access_flags = access; }

private:
	u2 _access_flags;
};

/**
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
 * I got this struct from the link above.
 * The cp_info type is a const_pool info.
 */
struct ClassFile
{
	u4					magic;
	u2					minor_version;
	u2					major_version;
	u2					access_flags;
	u2					this_class;
	u2					super_class;

//	u2					_constantPoolCount; This value is _constantPool.size()
//	u2					interfaces_count; This value is _interfaces.size()
//	u2					fields_count; This value is _fields.size()	
//	u2					methods_count;
//	u2					attributes_count;

	std::vector<CPBase *> _constantPool;//[constant_pool_count-1];
	std::vector<u2>		_interfaces;//[interfaces_count];
	std::vector<method_and_field_info *> _fields;//[fields_count];
	std::vector<method_and_field_info *> _methods;//[methods_count];
	std::vector<attribute_info *> _attributes;//[attributes_count];

	ClassFile() { }

	virtual ~ClassFile()
	{
		for (size_t i = 0; i < _constantPool.size(); i++)
			delete _constantPool[i]; //TODO ERROR se o _constantPool nao for virtual

		for (size_t i = 0; i < _fields.size(); i++)
			delete _fields[i];

		for (size_t i = 0; i < _methods.size(); i++)
			delete _methods[i];

		for (size_t i = 0; i < _attributes.size(); i++)
			delete _attributes[i];
	}
};

struct ConstantValue_attribute
{
	u2 attribute_name_index;
	u4 attribute_length;
	u2 constantvalue_index;
};

class JavaClass;


/**
 * This Frame is incorrect, the PC field needs to go to a struct to represent a thread and SP stay here, to represent a heap for methods.
 */
class Frame
{
	public:
		u4	_pc; //process counter
		/* http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-2.html#jvms-2.6.1 */
		std::vector<Variable> _localVariable;
		int _localVariableSize;

		/* http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-2.html#jvms-2.6.2 */
		std::stack<Variable> _operandStack;
		int _operandStackSize;

		/*reference to the runtime constant pool*/
		boost::shared_ptr<JavaClass> _clazz; // here I have my constPool, maybe put here a pointer for this constpool
		method_and_field_info * _method;
		Code_attribute * _code; // optimize
		bool _hasException; // if this frame needs to process an exception
		/*This stack is to put how many objects I have locked*/
		std::stack<Object *> _objOwner; //This attribute is for monitorenter and monitorexit

		//this bool is to notify if I'm blocked or not
		bool _lock;
		//This object is what object I'm waiting for.
		Object * _waitingObj;

		Frame() : _pc(0), _localVariableSize(0), _operandStackSize(0), _hasException(false), _lock(false), _waitingObj(NULL), _code(NULL) { }
		void setCode(Code_attribute * code)
		{
			_localVariableSize = code->max_locals;
			_localVariable.resize(_localVariableSize);
			_operandStackSize = code->max_stack;
			_code = code;
		}

		void unlock() {	_lock = false; }
		bool catchException();
};

class JavaVM;
struct opCode
{
	u2 _value;
	const char * _name;
	bool (*do_opcode)(u1 *, Frame *, JavaVM *);
};
