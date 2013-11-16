#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/logic/tribool.hpp>
#include <list>
#include <stack>
#include <string>

#include "types.h"
#include "JavaTypes.h"
#include "ObjectHeap.h"
#include "ClassHeap.h"
#include "JavaClass.h"
#include <boost/function.hpp>
#include "ObjectHeap.h"
#include "Stack.h"

class HeapClass;

typedef boost::function<void(void)> notify_fn;

class JavaVM
{
	private:
		HeapClass * _classHeap;
		Heap * _objectHeap;

		std::list< StackFrame * > _threads;
		//std::list< std::stack<Frame *> * > _threads;
		std::map<Object *, std::vector<notify_fn> > _notify;
	
	public:

		template <typename ObjectAllocator>
		JavaVM(ObjectAllocator * alocator) : _classHeap(NULL), _objectHeap(NULL)
		{
			_classHeap = new HeapClass();
			_objectHeap = new HeapObject<ObjectAllocator>(alocator);
			initializeOPCodes();
		}

		JavaVM() : _classHeap(NULL), _objectHeap(NULL)
		{
			_classHeap = new HeapClass();
			_objectHeap = new HeapObject<DummyAllocator>();
			initializeOPCodes();
		}

		virtual ~JavaVM(void)
		{
			delete _objectHeap;
			delete _classHeap;
		}

		jClass loadClass(const std::string & className) { return getClassHeap()->loadClass(className); }
		HeapClass * getClassHeap() { return _classHeap; }
		Heap * getObjectHeap() { return _objectHeap; }
		StackFrame * popThread();

		void pushThread(StackFrame * thread)
		{
			/*LOCK*/
			_threads.push_front(thread);
			/*UNLOCK*/
		}

		void execThreads();
		bool execThread(StackFrame * currentThread, int timeEscalonator);
		void addNewThread(Frame * frame);
		boost::shared_ptr<StackFrame> JavaVM::createStack();

		bool checkFrame(Frame * f);

		void notify(Object * objectref);
		void wait(Object * objectref, notify_fn f);
		bool lockMonitor(Object * objectref);
		bool unlockMonitor(Object * objectref);

		/**
		  * This method can invoke a specific java method.
		  * @param object: invokeMethod will invoke this method from this object
		  * @param name: it is the method name
		  * @param desc: it is the method description
		  * @return if this method has a return value it is variable, if this method doesn't have return 
		  *     then null is returned, description indicates if there is a return or not ()V is void, 
		  *     otherwise it returns a value.
		  */
		Variable invokeMethod(Object * object, const std::string & name, const std::string & desc);
		
		/**
		  * static method with default opcode
		  */
		const static struct opCode STATIC_OPCODE_ARRAY[256];
		/*Helpfull methods*/
		/**
		  * you can switch an especific opcode from another, becarefull when you do it.
		  */
		void replaceOPCodeFunction(u2 opCode, bool (*do_opcode)(u1 *, Frame *, JavaVM *));

	private:
		boost::logic::tribool executionEngine(StackFrame * stack);

		/*Return if has java exception*/
		void pushNewFrame(StackFrame * frameStack);
		std::string countMethodStack(const std::string & methodDesc);
		u4 invokeNativeMethod(Frame * nativeMethod);

		struct opCode _OpcodeArray[256];
		void initializeOPCodes();
};
