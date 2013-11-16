#pragma once

#include "types.h"
#include "JavaTypes.h"
#include "ClassHeap.h"

#include <boost/shared_ptr.hpp>

class DummyAllocator
{
	public:
		DummyAllocator() : _id(0) { }
		~DummyAllocator() { }
		u4 allocateObj(std::string clazz, Object *& object, u4 fields);
		u4 allocatePrimitiveArr(u1 type, Object *& object, u4 count);

		void deallocateObj(u4 id, Object * obj);
		void deallocatePrimitiveArr(u4 id, Object * obj, u4 count);

	public:
		std::size_t _id;
};

class Heap
{
	public:
		virtual Object * createObject(boost::shared_ptr<JavaClass> clazz) = 0;
		virtual Object * createStringObject(HeapClass * classHeap) = 0;
		virtual Object * createPrimitiveArray(u1 type, i4 count) = 0;
		//virtual Object * createObjectArray(boost::shared_ptr<JavaClass> clazz, i4 count) = 0;
		virtual void destroyObject(u4 id, Object * obj) = 0;

		void unreferenceAttributes(Object * object)
		{
			for (int i = 0; i < object->_countFields; i++)
			{
				if (object->_objFields[i]._value._type == 'L')
				{
					this->unreferenceAttributes(object->_objFields[i]._value._var._objectref);
					object->_objFields[i]._value._var._objectref->_reference--;
				}
			}
		}

		void garbageCollector()
		{

			for (std::map<u4, Object *>::iterator objects = _objectMap.begin(); objects != _objectMap.end(); objects++)
			{
				if (objects->second->_reference > 0)
					continue;

				//invokeFinalize();
				this->unreferenceAttributes(objects->second);
				destroyObject(objects->first, objects->second);
			}
		}

	protected:
		std::map<u4, Object *> _objectMap;
};

template <typename Allocator>
class HeapObject : public Heap
{
	public:
		HeapObject(void) : _alloc(new Allocator()) { }
		HeapObject(Allocator * alloc) : _alloc(alloc) { }

		virtual ~HeapObject(void)
		{
			//TODO correct clean for array and objects.
			for (std::map<u4, Object *>::iterator it = _objectMap.begin(); it != _objectMap.end(); it++)
				delete it->second;
		}

		Variable * getObject(Object object);
		virtual Object * createObject(boost::shared_ptr<JavaClass> clazz)
		{
			u4 fields = clazz->getFieldCount();
			Object * object = NULL;
			u4 id = _alloc->allocateObj(clazz->getThisClassName(), object, fields);

			if (id == 0)
				assert(false); // throw outofmemoryexception

			object->_class = clazz;
			object->_countFields = fields;

			boost::shared_ptr<JavaClass> virtualClass(clazz);
			int index = 0;
			while (index < object->_countFields)
			{
				for (int i = 0; i < virtualClass->_fields.size() /*fields_count*/ ; i++)
				{
					object->_objFields[i + index]._field_index = /*clazz or virtualClass ??*/ virtualClass->getFieldIndex(virtualClass->_fields[i]->name_index, virtualClass->_fields[i]->descriptor_index);
					object->_objFields[i + index]._value._type = virtualClass->getFieldType(virtualClass->_fields[i]->descriptor_index);
				}
				//initialize objects from my father
				index += virtualClass->_fields.size();// fields_count;
				if (index < object->_countFields)
					virtualClass = virtualClass->getSuperClass();
			}

			_objectMap.insert(std::make_pair(id, object));
			return object;
		}

		virtual Object * createStringObject(HeapClass * classHeap)
		{
			Object * object = NULL;//new Object();
			if (classHeap == NULL)
				throw std::exception();

			boost::shared_ptr<JavaClass> clazz(classHeap->getClass(std::string("java/lang/String")));
			if (clazz.get() == NULL) //incorrect
				throw std::exception();

			object = createObject(clazz);
			object->_objFields[0]._value._var._primitive._ptrValue = new std::string();
			object->_objFields[0]._value._type = 'N'; //native pointer
			return object;
		}

		virtual Object * createPrimitiveArray(u1 type, i4 count)
		{
			Object * object = NULL;
			u4 id = _alloc->allocatePrimitiveArr(type, object, count);
			object->_countFields = count;
			//object->_class = ; TODO array class

			for (int i = 0; i < object->_countFields; i++)
				object->_objFields[i]._value._type = type;

			_objectMap.insert(std::make_pair(id, object));
			return object;
		}

		virtual void destroyObject(u4 id, Object * obj)
		{
			_alloc->deallocateObj(id, obj);
		}

		/*
		virtual Object * createObjectArray(boost::shared_ptr<JavaClass> clazz, i4 count)
		{
			Object * object = NULL;
			u4 fields = clazz->getFieldCount();
			u4 id = _alloc->allocateObjArr(clazz->getThisClassName(), object, count, fields);

			object->_countFields = count;
			for (std::size_t i = 0; i < object->_countFields; i++)
			{
				object->_objFields[i]._value._type = 'L'; //array type
				object->_objFields[i]._value._var._objectref->_class = clazz;
			}

			_objectMap.insert(std::make_pair(id, object));
			return object;
		}
		*/
	protected:
		Allocator * _alloc;
};
