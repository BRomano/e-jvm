
#include "ObjectHeap.h"
#include "types.h"
#include "ClassHeap.h"
#include "JavaClass.h"


u4 DummyAllocator::allocateObj(std::string clazz, Object *& object, u4 fields)
{
	object = new Object();
	object->_objFields = new ObjectFields[fields];
	return ++_id; //object identifier
}

u4 DummyAllocator::allocatePrimitiveArr(u1 type, Object *& object, u4 count)
{
	object = new Object();
	object->_objFields = new ObjectFields[count];
	return ++_id; //object identifier
}

void DummyAllocator::deallocateObj(u4 id, Object * obj)
{
	delete[] obj->_objFields;
	delete obj;
}

void DummyAllocator::deallocatePrimitiveArr(u4 id, Object * obj, u4 count)
{
	delete[] obj->_objFields;
	delete obj;
}

