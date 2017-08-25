#pragma once

#include "types.h"
#include "constants.h"
#include "JavaTypes.h"
#include "ClassHeap.h"

#include <vector>
#include <boost/enable_shared_from_this.hpp>

class Heap;

/**
 *
 */
class JavaClass : public ClassFile, public boost::enable_shared_from_this<JavaClass>
{
	public:
		JavaClass(void) : _heapClass(NULL), _objectFieldsCount(0) { }
		virtual ~JavaClass(void) { }

		CPBase * getConstantPoolObj(u1 *& p);
		bool getStringFromConstPool(u4 index, std::string & str);
		
		Variable loadConstantItem(u1 index, Heap * heap);

		bool isInheritFrom(const std::string & catch_type_name);
		
		//u2 getMethodID(const std::string & methodName, const std::string & methodDesc, boost::shared_ptr<JavaClass> & clazz);
        /*find method index, if can't find method index then try to find in his father*/
        u2 getMethodID(const std::string & methodName, const std::string & methodDesc, boost::shared_ptr<JavaClass> & clazz);

		Code_attribute * getMethodCodeAttribute(u2 m_index);
		u2 getFieldIndex(u2 strName, u2 strDesc);
		u1 getFieldType(u2 strDesc);
		virtual u4 getFieldCount(void);

		inline void setClassHeap(HeapClass* classHeap) { this->_heapClass = classHeap; }
		virtual u4 getObjectSize(void);
		
		std::string getThisClassName(void);
		std::string getSuperClassName(void);
		inline boost::shared_ptr<JavaClass> getSuperClass(void) { return _heapClass->getClass(getSuperClassName()); }

		/*this function resolves a name in constantPool*/
		boost::shared_ptr<JavaClass> resolveClassName(u2 index);

	private:
		std::string getClassName(u2 class_id);		
		u2	_objectFieldsCount;
		HeapClass * _heapClass;
};

/*TODO trocar por um enum o construtor*/
class JavaClassBasicType : public JavaClass
{
	public:
		JavaClassBasicType(u1 type) : _type(type) { }
		virtual ~JavaClassBasicType() { }

	private:
		u1 _type;
};
