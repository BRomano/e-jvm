
#include "JavaVM.h"
#include "JavaClass.h"
#include "types.h"
#include "opcodes.h"
#include "ClassHeap.h"
#include "ObjectHeap.h"
#include "jException.h"

#include <time.h>

bool Frame::catchException()
{
	bool ret = false;
	for (std::size_t i = 0; i < this->_code->_exception_table.size(); i++)
	{
		u2 start_pc = this->_code->_exception_table[i]->start_pc;
		u2 end_pc = this->_code->_exception_table[i]->end_pc;
		u2 catch_type = this->_code->_exception_table[i]->catch_type;

		if (start_pc >= end_pc)
			ASSERT(false);

		if (this->_pc < start_pc || this->_pc > end_pc)
			continue;
		
		CONSTANT_Class_info_resolve * resolve = reinterpret_cast<CONSTANT_Class_info_resolve *>(this->_clazz->_constantPool[catch_type]);
		std::string catch_type_name;
		catch_type_name.assign((char *)resolve->_name_index->bytes, resolve->_name_index->length);
		if (this->_operandStack.top()._var._objectref->_class->isInheritFrom(catch_type_name))
		{
			ret = true;
			this->_pc += this->_code->_exception_table[i]->handler_pc - this->_pc;
			this->_hasException = false; // now we will process this exception
		}
	}

	return ret;
}

boost::shared_ptr<StackFrame> JavaVM::createStack()
{
	boost::shared_ptr<StackFrame> stack(new StackFrame());
	return stack;
}

void JavaVM::addNewThread(Frame * frame)
{
	//TODO asio, post
	StackFrame * stack = new StackFrame();

	stack->push(frame);
	_threads.push_back(stack);
}

Variable JavaVM::invokeMethod(Object * object, const std::string & name, const std::string & desc)
{
	boost::shared_ptr<JavaClass> virtual_class(object->_class);
	u2 method_id = object->_class->getMethodID(name, desc, virtual_class);
	
	Code_attribute * code = virtual_class->getMethodCodeAttribute(method_id);
	Frame * new_f = new Frame();
	new_f->setCode(code);
	new_f->_clazz = virtual_class;
	new_f->_method = virtual_class->_methods[method_id];

	Variable obj;
	obj._type = 'L';
	obj._var._objectref = object;
	new_f->_localVariable.push_back(obj);

	boost::shared_ptr<StackFrame> stack;
	stack = createStack();
	stack->push(new_f);

	Variable ret;
	execThread(stack.get(), 2000);
	ret = stack->top()->_operandStack.top();
	return ret;
}

/**
 * The unsigned indexbyte1 and indexbyte2 are used to construct an index into the runtime constant pool of the current class (§2.6),
 * where the value of the index is (indexbyte1 << 8) | indexbyte2. The runtime constant pool item at that index must be a symbolic reference to a method (§5.1),
 * which gives the name and descriptor (§4.3.3) of the method as well as a symbolic reference to the class in which the method is to be found.
 * The named method is resolved (§5.4.3.3). Finally, if the resolved method is protected (§4.6), and it is a member of a superclass of the current class,
 * and the method is not declared in the same runtime package (§5.3) as the current class,
 * then the class of objectref must be either the current class or a subclass of the current class.
 */
void JavaVM::pushNewFrame(StackFrame * stackFrame)
{
	Frame * frame = stackFrame->top();
	//get a process counter and get a next instruction
	//invoke instance method on object objectref, where the method is identified by method reference index in constant pool (indexbyte1 << 8 + indexbyte2)
	u1 * ptr = &frame->_code->code[frame->_pc + 1];
	u2 method_index = getu2(ptr);
	CPBase * pConstPool = frame->_clazz->_constantPool[method_index];

	//get method
	ASSERT(pConstPool->tag == CONSTANT_Methodref);
	CONSTANT_Methodref_info_resolve * method_ref = reinterpret_cast<CONSTANT_Methodref_info_resolve *>(pConstPool);

	/*get class name*/
	std::string class_name;
	class_name.assign((const char *)method_ref->_class_index->_name_index->bytes, method_ref->_class_index->_name_index->length);
	boost::shared_ptr<JavaClass> clazz(_classHeap->getClass(class_name));

	/*get method*/
	std::string method_name, method_desc;
	frame->_clazz->getStringFromConstPool(method_ref->_name_and_type_index->name_index, method_name);
	frame->_clazz->getStringFromConstPool(method_ref->_name_and_type_index->descriptor_index, method_desc);

	printf("invoke %s.%s:%s\n", class_name.c_str(), method_name.c_str(), method_desc.c_str());
	boost::shared_ptr<JavaClass> virtual_clazz(clazz);
	u2 nIndex = clazz->getMethodID(method_name, method_desc, virtual_clazz);
	
	Frame * new_f = new Frame();
	new_f->_clazz = virtual_clazz;
	new_f->_method = virtual_clazz->_methods[nIndex];
	if (!(virtual_clazz->_methods[nIndex]->getAccessFlags() & ACC_NATIVE)) // there is no code
	{
		Code_attribute * code = virtual_clazz->getMethodCodeAttribute(nIndex);
		new_f->setCode(code);
	}
	else
	{
		Code_attribute * code = new Code_attribute();
		code->max_locals = 1;
		code->max_stack = 0;
		code->code = NULL;
		new_f->setCode(code);
	}

	/*
	 * The Java virtual machine uses local variables to pass parameters on method invocation
	 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-2.html#jvms-2.6.1
	 */
	std::string params = countMethodStack(method_desc);
	for (std::size_t i = params.length(), j = i; i >= 0; i--, j--)
	{
		/** 
		  * The objectref and the argument values are consecutively made the values of local variables of the new frame,
		  * with objectref in local variable 0,
		  * arg1 in local variable 1 (or, if arg1 is of type long or double, in local variables 1 and 2), and so on
		  */
		new_f->_localVariable[i] = frame->_operandStack.top();
		frame->_operandStack.pop();
		
		//TODO melhorar
		if (i == 0)
			break;

		/* if it is long or double I need to add 1 in local variable */
		if(params.size() > 0 && (params[j-1] =='J' || params[j-1] =='D'))
			i--;
	}

	if (new_f->_method->getAccessFlags() & ACC_SYNCHRONIZED)
	{
		if (lockMonitor(new_f->_localVariable[0]._var._objectref))
		{
			
		}
		else
		{
			new_f->_lock = true;
			new_f->_waitingObj = new_f->_localVariable[0]._var._objectref;
			this->wait(new_f->_waitingObj, boost::bind(&Frame::unlock, new_f));
		}
	}

	//if this method has ACC_SYNCHRONIZED then I need to do monitorenter, it is implicit
	//I need to get new_f->_localVariable[0], this location in array is objref, I need to lock this object
	stackFrame->push(new_f);
}

//arrumar o modo de achar a classe e o metodo =/
/**
 * return true of false, true if this method is finished, false if it doesnt finished yet.
 */
#define athrow 191
boost::logic::tribool JavaVM::executionEngine(StackFrame * stack)
{
	Frame * f = stack->top();
	bool ret = false;
	if (f->_hasException)
	{
		ret = f->catchException();
		if (ret == false) return boost::logic::tribool::indeterminate_value; //this method doesnt know what happens.
		return false;// Program thinks, "Everythink is OK, I have control", but he only thinks it ;-P
	}

	u1 * byte_code = f->_code->code;
	printf("[%d]:[%s] \n", f->_pc, _OpcodeArray[byte_code[f->_pc]]._name);

	if (_OpcodeArray[byte_code[f->_pc]]._value == 182 || _OpcodeArray[byte_code[f->_pc]]._value == 183)
	{
		this->pushNewFrame(stack);
	}

	ret = _OpcodeArray[byte_code[f->_pc]].do_opcode(&byte_code[f->_pc], f, this);
	if (ret == false && _OpcodeArray[byte_code[f->_pc]]._value == athrow)
	{
		ret = f->catchException();
		if (ret == false) return boost::logic::tribool::indeterminate_value; //this method doesnt know what happens.
		return false;// O programa pensa que esta tudo bem, mas ele soh acha isso ;-P
	}
	else if (ret == false)
	{
		return true; /* this method's done */
	}

	return false;
}

/**
 * TODO put this method on Frame class
 */
bool JavaVM::checkFrame(Frame * f)
{
	if (f->_code == NULL)
	{
		return false;
	}
	else if (f->_lock)
	{
		return false; /* or break, this thread can't execute, go on */
	}
	else if (f->_waitingObj == NULL && f->_lock == false)
	{
		/* I'm owner for this execution*/
		return true;
	}
	else
	{
		//this object is unlocked, I'm locking it
		if (this->lockMonitor(f->_waitingObj))//->_monitor == 0
		{
			//f->_waitingObj->_monitor++;
			f->_objOwner.push(f->_waitingObj);
			f->_waitingObj = NULL; //I'm not waiting nothing more
			return true; //Noew this object is mine
		}
		else
		{
			//this object is locked, I'm waiting for it.
			f->_lock = true;
			this->wait(f->_waitingObj, boost::bind(&Frame::unlock, f));
			//This object was unlocked but someone, lock it before me.
			return false;
		}
	}
	return false;
}

/**
 *
 */
bool JavaVM::execThread(StackFrame * currentThread, int timeEscalonator)
{
	try
	{
		while(time(NULL) + timeEscalonator > time(NULL))
		{
			Frame * f = currentThread->top(); //here I get the top frame, if a new method was stacked, I got him.
			if (!checkFrame(f))
				break;

			if (f->_method->getAccessFlags() & ACC_NATIVE)
			{
				invokeNativeMethod(f); //exceptions need to set _hasException on Frame and exception needs to be on top _operandStack
				continue;
			}

			//return true, false or undefined, true if this method is finished, false if it doesnt finished yet, undefined if it thorws an exception
			boost::logic::tribool ret = this->executionEngine(currentThread);
			if (!ret) { /* not finished yet*/ }
			else if (ret)
			{
				if (f->_method->getAccessFlags() & ACC_SYNCHRONIZED)
				{
					f->_localVariable[0]._var._objectref->_monitor--;
					this->notify(f->_localVariable[0]._var._objectref);
					//if this method has ACC_SYNCHRONIZED I need to do monitorexit
					//I need to sinalize I have unlock this object
				}

				Variable ret;
				if (currentThread->methodSDone(&ret))
					currentThread->top()->_operandStack.push(ret);
			}
			else //exception
			{
				currentThread->setException();
			}
		}
	}
	catch(std::exception & ex)
	{
//		throw ex;
		return false;
	}
	return true;
}

/**
 * this is an escalonator, operator() para o asio
 */
void JavaVM::execThreads() //Each thread needs to have his stack
{
	StackFrame * currentThread = NULL;
	while(true) //TODO while this program is alive, for asio remove it
	{
		currentThread = popThread();
		if(currentThread == NULL)
			continue; //need to be async, e criar isso sendo algo para o boost::asio

		if (execThread(currentThread, 2000))
		{
			pushThread(currentThread);
		}
		else
		{
			//this thread's done
		}
	}
}

/**
 * This method counts how much param methodDesc has.
 */
std::string JavaVM::countMethodStack(const std::string & methodDesc)
{
	std::string stack_count;
	for(std::size_t i = 1; i < methodDesc.length(); i++)
	{
		//cLass =)
		if(methodDesc[i] =='L')
		{
			stack_count += 'L';
			while(methodDesc[i] !=';')
				i++;
		}

		if(methodDesc[i] ==')')
			break;
		if(methodDesc[i] =='[')
			continue;

		stack_count += methodDesc[i];
	}
	return stack_count;
}


u4 JavaVM::invokeNativeMethod(Frame * nativeMethod)
{
	assert(false);
	return 0;
}

StackFrame * JavaVM::popThread()
{
	/*LOCK*/
	if (_threads.size() == 0)
		return NULL;

	StackFrame * t = _threads.back();
	_threads.pop_back();
	return t;
	/*UNLOCK*/
}

void JavaVM::wait(Object * objectref, notify_fn f)
{
	std::map<Object *, std::vector<notify_fn> >::iterator it = _notify.find(objectref);
	if (it == _notify.end())
	{
		std::vector<notify_fn> vec_notify;
		vec_notify.push_back(f);
		_notify.insert(std::make_pair(objectref, vec_notify));
	}
	else
	{
		it->second.push_back(f);
	}
}

void JavaVM::notify(Object * objectref)
{
	std::map<Object *, std::vector<notify_fn> >::iterator it = _notify.find(objectref);
	if (it == _notify.end())
		return;

	for (std::size_t i = 0; i < it->second.size(); i++)
		it->second[i]();
}

//this->addForNotify(Object * objectref, boost::bind(Frame::unlock, f));
bool JavaVM::lockMonitor(Object * objectref)
{
	/*LOCK*/
	if (objectref->_monitor == 0)
		objectref->_monitor++;
	else
		return false;
	return true;
	/*UNLOCK*/
}

bool JavaVM::unlockMonitor(Object * objectref)
{
	objectref->_monitor--;
	if (objectref->_monitor == 0)
		return true;
	return false;
}


/**OPCODE manipulation*/

void JavaVM::replaceOPCodeFunction(u2 opCode, bool (*do_opcode)(u1 *, Frame *, JavaVM *))
{
	_OpcodeArray[opCode].do_opcode = do_opcode;
}

void JavaVM::initializeOPCodes()
{
	for(int i = 0; i < 256; i++)
	{
		_OpcodeArray[i] = STATIC_OPCODE_ARRAY[i];
	}
}
/**
 * put this array inside JAVAVM class
 * then I can create hooks for each instruction. :-)
 */
const struct opCode JavaVM::STATIC_OPCODE_ARRAY[256] =
{
	{0,  "nop",					f_nop},
	{1,  "aconst_null",			f_aconst_null},
	{2,  "iconst_m1",			f_iconst_m1},
	{3,  "iconst_0",			f_iconst_m1},
	{4,  "iconst_1",			f_iconst_m1},
	{5,  "iconst_2",			f_iconst_m1},
	{6,  "iconst_3",			f_iconst_m1},
	{7,  "iconst_4",			f_iconst_m1},
	{8,  "iconst_5",			f_iconst_m1},
	{9,  "lconst_0",			f_ASSERT},
	{10, "lconst_1",			f_ASSERT},
	{11, "fconst_0",			f_fconst_0},
	{12, "fconst_1",			f_fconst_0},
	{13, "fconst_2",			f_fconst_0},
	{14, "NULL",				f_ASSERT},
	{15, "NULL",				f_ASSERT},
	{16, "bipush",				f_bipush},
	{17, "sipush",				f_ASSERT},
	{18, "ldc",					f_ldc},
	{19, "NULL",				f_ASSERT},
	{20, "ldc2_w",				f_ldc2_w},
	{21, "iload",				f_iload},
	{22, "lload",				f_lload},
	{23, "NULL",				f_ASSERT},
	{24, "NULL",				f_ASSERT},
	{25, "aload",				f_aload},
	{26, "iload_0",				f_iload_0},
	{27, "iload_1",				f_iload_0},
	{28, "iload_2",				f_iload_0},
	{29, "iload_3",				f_iload_0},
	{30, "lload_0",				f_lload_0},
	{31, "lload_1",				f_lload_1},
	{32, "lload_2",				f_lload_2},
	{33, "lload_3",				f_lload_3},
	{34, "fload_0",				f_fload_0},
	{35, "fload_1",				f_fload_0},
	{36, "fload_2",				f_fload_0},
	{37, "fload_3",				f_fload_0},
	{38, "NULL",				f_ASSERT},
	{39, "NULL",				f_ASSERT},
	{40, "NULL",				f_ASSERT},
	{41, "NULL",				f_ASSERT},
	{42, "aload_0",				f_aload_0},
	{43, "aload_1",				f_aload_0},
	{44, "aload_2",				f_aload_0},
	{45, "aload_3",				f_aload_0},
	{46, "iaload",				f_iaload},
	{47, "NULL",				f_ASSERT},
	{48, "NULL",				f_ASSERT},
	{49, "NULL",				f_ASSERT},
	{50, "aaload",				f_aaload},
	{51, "NULL",				f_ASSERT},
	{52, "NULL",				f_ASSERT},
	{53, "NULL",				f_ASSERT},
	{54, "istore",				f_istore},
	{55, "NULL",				f_ASSERT},
	{56, "NULL",				f_ASSERT},
	{57, "NULL",				f_ASSERT},
	{58, "astore",				f_astore},
	{59, "istore_0",			f_istore_0},
	{60, "istore_1",			f_istore_0},
	{61, "istore_2",			f_istore_0},
	{62, "istore_3",			f_istore_0},
	{63, "lstore_0",			f_lstore_0},
	{64, "lstore_1",			f_lstore_1},
	{65, "lstore_2",			f_lstore_2},
	{66, "lstore_3",			f_lstore_3},
	{67, "fstore_0",			f_fstore_0},
	{68, "fstore_1",			f_fstore_0},
	{69, "fstore_2",			f_fstore_0},
	{70, "fstore_3",			f_fstore_0},
	{71, "NULL",				f_ASSERT},
	{72, "NULL",				f_ASSERT},
	{73, "NULL",				f_ASSERT},
	{74, "NULL",				f_ASSERT},
	{75, "astore_0",			f_astore_0},
	{76, "astore_1",			f_astore_0},
	{77, "astore_2",			f_astore_0},
	{78, "astore_3",			f_astore_0},
	{79, "iastore",				f_iastore},
	{80, "NULL",				f_ASSERT},
	{81, "NULL",				f_ASSERT},
	{82, "NULL",				f_ASSERT},
	{83, "aastore",				f_aastore},
	{84, "NULL",				f_ASSERT},
	{85, "NULL",				f_ASSERT},
	{86, "NULL",				f_ASSERT},
	{87, "_pop",				f_pop},
	{88, "NULL",				f_ASSERT},
	{89, "dup",					f_dup},
	{90, "dup_x1",				f_dup_x1},
	{91, "dup_x2",				f_dup_x2},
	{92, "NULL",				f_ASSERT},
	{93, "NULL",				f_ASSERT},
	{94, "NULL",				f_ASSERT},
	{95, "NULL",				f_ASSERT},
	{96, "iadd",				f_iadd},
	{97, "ladd",				f_ladd},
	{98, "fadd",				f_fadd},
	{99, "NULL",				f_ASSERT},
	{100, "isub",				f_isub},
	{101, "NULL",				f_ASSERT},
	{102, "NULL",				f_ASSERT},
	{103, "NULL",				f_ASSERT},
	{104, "imul",				f_imul},
	{105, "NULL",				f_isub},
	{106, NULL,					f_ASSERT},
	{107, NULL,					f_ASSERT},
	{108, NULL,					f_ASSERT},
	{109, NULL,					f_ASSERT},
	{110, NULL,					f_ASSERT},
	{111, NULL,					f_ASSERT},
	{112, NULL,					f_ASSERT},
	{113, NULL,					f_ASSERT},
	{114, NULL,					f_ASSERT},
	{115, NULL,					f_ASSERT},
	{116, NULL,					f_ASSERT},
	{117, NULL,					f_ASSERT},
	{118, NULL,					f_ASSERT},
	{119, NULL,					f_ASSERT},
	{120, NULL,					f_ASSERT},
	{121, NULL,					f_ASSERT},
	{122, NULL,					f_ASSERT},
	{123, NULL,					f_ASSERT},
	{124, NULL,					f_ASSERT},
	{125, NULL,					f_ASSERT},
	{126, NULL,					f_ASSERT},
	{127, NULL,					f_ASSERT},
	{128, NULL,					f_ASSERT},
	{129, NULL,					f_ASSERT},
	{130, NULL,					f_ASSERT},
	{131, NULL,					f_ASSERT},
	{132, "iinc",				f_iinc},
	{133, NULL,					f_ASSERT},
	{134, "i2f",				f_int2float},
	{135, NULL,					f_ASSERT},
	{136, NULL,					f_ASSERT},
	{137, NULL,					f_ASSERT},
	{138, NULL,					f_ASSERT},
	{139, "f2i",				f_float2int},
	{140, NULL,					f_ASSERT},
	{141, NULL,					f_ASSERT},
	{142, NULL,					f_ASSERT},
	{143, NULL,					f_ASSERT},
	{144, NULL,					f_ASSERT},
	{145, NULL,					f_ASSERT},
	{146, NULL,					f_ASSERT},
	{147, NULL,					f_ASSERT},
	{148, NULL,					f_ASSERT},
	{149, "fcmpl",				f_fcmp},
	{150, "fcmpg",				f_fcmp},
	{151, NULL,					f_ASSERT},
	{152, NULL,					f_ASSERT},
	{153, "ifeq",				f_ifeq},
	{154, "ifne",				f_ifeq},
	{155, "iflt",				f_ifeq},
	{156, "ifge",				f_ifeq},
	{157, "ifgt",				f_ifeq},
	{158, "ifle",				f_ifeq},
	{159, "if_icmpeq",			f_if_icmpeq},
	{160, "if_icmpne",			f_if_icmpeq},
	{161, "if_icmplt",			f_if_icmpeq},
	{162, "if_icmpge",			f_if_icmpeq},
	{163, "if_icmpgt",			f_if_icmpeq},
	{164, "if_icmple",			f_if_icmpeq},
	{165, "if_acmpeq",			f_ASSERT},
	{166, "if_acmpne",			f_ASSERT},
	{167, "_goto",				f_goto},
	{168, NULL,					f_ASSERT},
	{169, NULL,					f_ASSERT},
	{170, NULL,					f_ASSERT},
	{171, NULL,					f_ASSERT},
	{172, "ireturn",			f_return},
	{173, NULL,					f_ASSERT},
	{174, "freturn",			f_return},
	{175, NULL,					f_ASSERT},
	{176, NULL,					f_ASSERT},
	{177, "_return",			f_return},
	{178, NULL,					f_ASSERT},
	{179, NULL,					f_ASSERT},
	{180, "getfield",			f_getfield},
	{181, "putfield",			f_putfield},
	{182, "invokevirtual",		f_invokevirtual},
	{183, "invokespecial",		f_invokespecial},
	{184, "invokestatic",		f_invokestatic},
	{185, NULL,					f_ASSERT},
	{186, NULL,					f_ASSERT},
	{187, "_new",				f_new},
	{188, "newarray",			f_newarray},
	{189, "anewarray",			f_anewarray},
	{190, "arraylength",		f_arraylength},
	{191, "athrow",				f_athrow},
	{192, "checkcast",			f_checkcast},
	{193, "instanceof",			f_instanceof},
	{194, "monitorenter",		f_monitorenter},
	{195, "monitorexit",		f_monitorexit},
	{196, NULL,					f_ASSERT},
	{197, NULL,					f_ASSERT},
	{198, NULL,					f_ASSERT},
	{199, NULL,					f_ASSERT},
	{200, NULL,					f_ASSERT},
	{201, NULL,					f_ASSERT},
	{202, NULL,					f_ASSERT},
	{203, NULL,					f_ASSERT},
	{204, NULL,					f_ASSERT},
	{205, NULL,					f_ASSERT},
	{206, NULL,					f_ASSERT},
	{207, NULL,					f_ASSERT},
	{208, NULL,					f_ASSERT},
	{209, NULL,					f_ASSERT},
	{210, NULL,					f_ASSERT},
	{211, NULL,					f_ASSERT},
	{212, NULL,					f_ASSERT},
	{213, NULL,					f_ASSERT},
	{214, NULL,					f_ASSERT},
	{215, NULL,					f_ASSERT},
	{216, NULL,					f_ASSERT},
	{217, NULL,					f_ASSERT},
	{218, NULL,					f_ASSERT},
	{219, NULL,					f_ASSERT},
	{220, NULL,					f_ASSERT},
	{221, NULL,					f_ASSERT},
	{222, NULL,					f_ASSERT},
	{223, NULL,					f_ASSERT},
	{224, NULL,					f_ASSERT},
	{225, NULL,					f_ASSERT},
	{226, NULL,					f_ASSERT},
	{227, NULL,					f_ASSERT},
	{228, NULL,					f_ASSERT},
	{229, NULL,					f_ASSERT},
	{230, NULL,					f_ASSERT},
	{231, NULL,					f_ASSERT},
	{232, NULL,					f_ASSERT},
	{233, NULL,					f_ASSERT},
	{234, NULL,					f_ASSERT},
	{235, NULL,					f_ASSERT},
	{236, NULL,					f_ASSERT},
	{237, NULL,					f_ASSERT},
	{238, NULL,					f_ASSERT},
	{239, NULL,					f_ASSERT},
	{240, NULL,					f_ASSERT},
	{241, NULL,					f_ASSERT},
	{242, NULL,					f_ASSERT},
	{243, NULL,					f_ASSERT},
	{244, NULL,					f_ASSERT},
	{245, NULL,					f_ASSERT},
	{246, NULL,					f_ASSERT},
	{247, NULL,					f_ASSERT},
	{248, NULL,					f_ASSERT},
	{249, NULL,					f_ASSERT},
	{250, NULL,					f_ASSERT},
	{251, NULL,					f_ASSERT},
	{252, NULL,					f_ASSERT},
	{253, NULL,					f_ASSERT},
	{254, NULL,					f_ASSERT},
	{255, NULL,					f_ASSERT},
};
