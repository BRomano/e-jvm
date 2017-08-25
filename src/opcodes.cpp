
#include "opcodes.h"
#include "types.h"
#include "JavaClass.h"
#include "ObjectHeap.h"
#include "jException.h"

u2 getu2_func(u1 ** ptr)
{
	u2 byte = (u2)(((*ptr)[0])<< 8 & 0x0000FF00 | ((*ptr)[1]));
	(*ptr) += 2;
	return byte;
}

bool compare(int value1, int value2, int op)
{
	switch(op)
	{
		case 0:
			return value1 == value2;
		case 1:
			return value1 != value2;
		case 2:
			return value1 < value2;
		case 3:
			return value1 >= value2;
		case 4:
			return value1 > value2;
		case 5:
			return value1 <= value2;

		default:
			assert(false);
			return false;
	}
}

bool f_fconst_0(u1 * bc, Frame * frame, JavaVM * arg)
{
	Variable var;
	var._var._primitive._float = (f4)bc[0] - JavaVM::STATIC_OPCODE_ARRAY[11]._value;
	frame->_operandStack.push(var);
	frame->_pc++;
	return true;
}

bool f_ASSERT(u1 * bc, Frame * frame, JavaVM * arg)
{
	assert(false);
	return false;
}

bool f_nop(u1 * bc, Frame * frame, JavaVM * arg)
{
	frame->_pc++;
	return true;
}

bool f_aconst_null(u1 * bc, Frame * frame, JavaVM * arg)
{
	Variable var;
	var._type = 'n';
	var._var._objectref = NULL;
	frame->_operandStack.push(var);
	frame->_pc++;
	return true;
}

bool f_iconst_m1(u1 * bc, Frame * frame, JavaVM * arg)
{
	Variable var;
	var._var._primitive._int = (u1)bc[0] - JavaVM::STATIC_OPCODE_ARRAY[3]._value;
	var._type = 'I';
	frame->_operandStack.push(var);
	frame->_pc++;
	return true;
}

bool f_bipush(u1 * bc, Frame * frame, JavaVM * arg)
{
	Variable var;
	var._var._primitive._int = (u1)bc[1];
	var._type = 'I';
	frame->_operandStack.push(var);
	frame->_pc += 2;
	return true;
}

/**
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-6.html#jvms-6.5.ldc
 */
bool f_ldc(u1 * bc, Frame * frame, JavaVM * arg)
{
	u1 index = bc[1];
	frame->_operandStack.push(frame->_clazz->loadConstantItem(index, arg->getObjectHeap()));
	frame->_pc += 2;
	return true;
}

bool f_ldc2_w(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }

bool f_iload(u1 * bc, Frame * frame, JavaVM * arg)
{
	bc++;
	u1 index = *bc;
	frame->_operandStack.push(frame->_localVariable[index]);
	frame->_pc += 2;
	return true;
}

bool f_lload(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_aload(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_iload_0(u1 * bc, Frame * frame, JavaVM * arg)
{
	int index = (int)bc[0] - JavaVM::STATIC_OPCODE_ARRAY[26]._value;
	frame->_operandStack.push(frame->_localVariable[index]);
	frame->_pc++;
	return true;
}

bool f_lload_0(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_lload_1(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_lload_2(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_lload_3(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }

bool f_fload_0(u1 * bc, Frame * frame, JavaVM * arg)
{
	int index = (int)bc[0] - JavaVM::STATIC_OPCODE_ARRAY[34]._value;
	frame->_operandStack.push(frame->_localVariable[index]);
	frame->_pc++;
	return true;
}

bool f_aload_0(u1 * bc, Frame * frame, JavaVM * arg)
{
	int index = (int)bc[0] - JavaVM::STATIC_OPCODE_ARRAY[42]._value;
	frame->_operandStack.push(frame->_localVariable[index]);
	frame->_pc++;
	return true;
}

bool f_iaload(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_aaload(u1 * bc, Frame * frame, JavaVM * arg)
{
	Variable arrayref = frame->_operandStack.top();
	frame->_operandStack.pop();
	int index = frame->_operandStack.top()._var._primitive._int;

	if (index >= arrayref._var._objectref->_objFields[0]._field_index)
		assert(false); //TODO throws Arrayindexofboundexception....

	Variable var = ((Variable *)arrayref._var._objectref->_objFields[0]._value._var._primitive._ptrValue)[index];
	frame->_operandStack.push(var);
	frame->_pc++;
	return true;
}

bool f_istore(u1 * bc, Frame * frame, JavaVM * arg)
{
	bc++;
	u1 index = *bc;
	frame->_localVariable[index] = frame->_operandStack.top();
	frame->_operandStack.pop();
	frame->_pc += 2;
	return true;
}

bool f_astore(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_istore_0(u1 * bc, Frame * frame, JavaVM * arg)
{
	int index = (int)bc[0] - JavaVM::STATIC_OPCODE_ARRAY[59]._value;
	frame->_localVariable[index] = frame->_operandStack.top();
	frame->_operandStack.pop();
	frame->_pc++;
	return true;
}

bool f_lstore_0(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_lstore_1(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_lstore_2(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_lstore_3(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }

bool f_fstore_0(u1 * bc, Frame * frame, JavaVM * arg)
{
	int index = (int)bc[0] - JavaVM::STATIC_OPCODE_ARRAY[67]._value;
	frame->_localVariable[index] = frame->_operandStack.top();
	frame->_operandStack.pop();
	frame->_pc++;
	return true;
}

bool f_astore_0(u1 * bc, Frame * frame, JavaVM * arg)
{
	if (frame->_localVariable[(u1)bc[0] - JavaVM::STATIC_OPCODE_ARRAY[75]._value]._type == 'L')
		frame->_localVariable[(u1)bc[0] - JavaVM::STATIC_OPCODE_ARRAY[75]._value]._var._objectref->_reference--;

	frame->_localVariable[(u1)bc[0] - JavaVM::STATIC_OPCODE_ARRAY[75]._value] = frame->_operandStack.top();
	frame->_operandStack.pop();
	
	if (frame->_localVariable[(u1)bc[0] - JavaVM::STATIC_OPCODE_ARRAY[75]._value]._type == 'L')
		frame->_localVariable[(u1)bc[0] - JavaVM::STATIC_OPCODE_ARRAY[75]._value]._var._objectref->_reference++;

	frame->_pc++;
	return true;
}

bool f_iastore(u1 * bc, Frame * frame, JavaVM * arg)
{
	Variable value = frame->_operandStack.top();
	frame->_operandStack.pop();
	int index = frame->_operandStack.top()._var._primitive._int;
	frame->_operandStack.pop();
	Variable arrayref = frame->_operandStack.top();
	frame->_operandStack.pop();

	((Variable*)arrayref._var._objectref->_objFields[0]._value._var._primitive._ptrValue)[index] = value;
	frame->_pc++;
	return true;
}

bool f_aastore(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_pop(u1 * bc, Frame * frame, JavaVM * arg)
{
	frame->_operandStack.pop();
	frame->_pc++;
	return true;
}

bool f_dup(u1 * bc, Frame * frame, JavaVM * arg)
{
	frame->_operandStack.push(frame->_operandStack.top());
	frame->_pc++;
	return true;
}

bool f_dup_x1(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_dup_x2(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_iadd(u1 * bc, Frame * frame, JavaVM * arg)
{
	jint value = frame->_operandStack.top()._var._primitive._int;
	frame->_operandStack.pop();
	frame->_operandStack.top()._var._primitive._int += value;
	frame->_pc++;
	return true;
}

bool f_fadd(u1 * bc, Frame * frame, JavaVM * arg)
{
	jfloat value = frame->_operandStack.top()._var._primitive._float;
	frame->_operandStack.pop();
	frame->_operandStack.top()._var._primitive._float += value;
	frame->_pc++;
	return true;
}

bool f_ladd(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_isub(u1 * bc, Frame * frame, JavaVM * arg)
{
	int value = frame->_operandStack.top()._var._primitive._int;
	frame->_operandStack.pop();
	frame->_operandStack.top()._var._primitive._int -= value;
	frame->_pc++;
	return true;
}

bool f_imul(u1 * bc, Frame * frame, JavaVM * arg)
{
	int value = frame->_operandStack.top()._var._primitive._int;
	frame->_operandStack.pop();
	frame->_operandStack.top()._var._primitive._int *= value;
	frame->_pc++;
	return true;
}

bool f_iinc(u1 * bc, Frame * frame, JavaVM * arg)
{
	bc++;
	u1 index = *bc;
	bc++;
	char _const = *bc;
	frame->_localVariable[index]._var._primitive._int += _const;
	frame->_pc += 3;
	return true;
}

bool f_int2float(u1 * bc, Frame * frame, JavaVM * arg)
{
	frame->_operandStack.top()._type = 'F';
	frame->_operandStack.top()._var._primitive._float = (float) frame->_operandStack.top()._var._primitive._int;
	frame->_pc++;
	return true;
}

bool f_float2int(u1 * bc, Frame * frame, JavaVM * arg)
{
	frame->_operandStack.top()._type = 'I';
	frame->_operandStack.top()._var._primitive._int = (int) frame->_operandStack.top()._var._primitive._float;
	frame->_pc++;
	return true;
}

bool f_fcmp(u1 * bc, Frame * frame, JavaVM * arg)
{
	jfloat value2 = frame->_operandStack.top()._var._primitive._float;
	frame->_operandStack.pop();
	jfloat value1 = frame->_operandStack.top()._var._primitive._float;
	
	//http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-6.html#jvms-6.5.fcmp_op
	frame->_operandStack.top()._type = 'I';

	//at least one of value1' or value2' is NaN. The fcmpg instruction pushes the int value 1 onto the operand stack and the fcmpl instruction pushes the int value -1 onto the operand stack. 
	if(false) //TODO, analize if it is NaN
	{
		if (*bc == 150) //fcmpg, TODO change it to a define or something like this.
			frame->_operandStack.top()._var._primitive._int = 1;
		else //fcmpl
			frame->_operandStack.top()._var._primitive._int = -1;
	}
	else if (value1 > value2)
		frame->_operandStack.top()._var._primitive._int = 1;
	else if(value1 == value2)
		frame->_operandStack.top()._var._primitive._int = 0;
	else if (value1 < value2)
		frame->_operandStack.top()._var._primitive._int = -1;
	else
		ASSERT(false);

	frame->_pc++;
	return true;
}

bool f_ifeq(u1 * bc, Frame * frame, JavaVM * arg)
{
	int value = frame->_operandStack.top()._var._primitive._int;
	frame->_operandStack.pop();

	if (compare(value, 0, (bc[0] - JavaVM::STATIC_OPCODE_ARRAY[153]._value)))
	{
		bc++;
		u4 branch = geti2(bc);
		frame->_pc += branch;
	}
	else
		frame->_pc += 3;

	return true;
}

bool f_if_icmpeq(u1 * bc, Frame * frame, JavaVM * arg)
{
	int value_2 = frame->_operandStack.top()._var._primitive._int;
	frame->_operandStack.pop();
	int value_1 = frame->_operandStack.top()._var._primitive._int;
	frame->_operandStack.pop();

	if (compare(value_2, value_1, (bc[0] - JavaVM::STATIC_OPCODE_ARRAY[159]._value)))
	{
		bc++;
		u4 branch = geti2(bc);
		frame->_pc += branch;
	}
	else
		frame->_pc += 3;
	
	return true;
}

bool f_goto(u1 * bc, Frame * frame, JavaVM * arg)
{
	bc++;
	i2 branch = geti2(bc);
	frame->_pc += branch;
	return true;
}

bool f_return(u1 * bc, Frame * frame, JavaVM * arg)
{
	return false;
}

bool f_getfield(u1 * bc, Frame * frame, JavaVM * arg)
{
	bc++;
	u2 index = getu2(bc);
	Variable objectref = frame->_operandStack.top();
	frame->_operandStack.pop();
	
	ASSERT(objectref._var._objectref->_class->magic == 0xCAFEBABE);
	CONSTANT_Fieldref_info_resolve * resolve = reinterpret_cast<CONSTANT_Fieldref_info_resolve *>(frame->_clazz->_constantPool[index]);
	for (size_t i = 0; i < objectref._var._objectref->_countFields; i++)
	{
		if (objectref._var._objectref->_objFields[i]._field_index == index)
		{
			frame->_operandStack.push(objectref._var._objectref->_objFields[i]._value);
			break;
		}
	}

	frame->_pc +=3;
	return true;
}

bool f_putfield(u1 * bc, Frame * frame, JavaVM * arg)
{
	/*The runtime constant pool item at that index must be a symbolic reference to a field*/
	/*
	 * Get value and objectref from stack.
	 * Get Index from byte code, this index is a reference to a field in constantPool.
	 * Then Set value in object
	 */
	bc++;
	u2 index = getu2(bc);
	Variable value = frame->_operandStack.top();
	frame->_operandStack.pop();
	Variable objectref = frame->_operandStack.top();
	frame->_operandStack.pop();
	ASSERT(objectref._type == 'L');

	CONSTANT_Fieldref_info_resolve * resolve = reinterpret_cast<CONSTANT_Fieldref_info_resolve *>(frame->_clazz->_constantPool[index]);
	for (u2 i = 0; i < objectref._var._objectref->_countFields; i++)
	{
		if (objectref._var._objectref->_objFields[i]._field_index == index)
		{
			//discount reference for previous object
			if (objectref._var._objectref->_objFields[i]._value._type == 'L')
				objectref._var._objectref->_objFields[i]._value._var._objectref->_reference--;

			objectref._var._objectref->_objFields[i]._value = value;

			if (objectref._var._objectref->_objFields[i]._value._type == 'L')
				objectref._var._objectref->_objFields[i]._value._var._objectref->_reference++;
			break;
		}
	}

	frame->_pc += 3;
	return true;
}

bool f_invokevirtual(u1 * bc, Frame * frame, JavaVM * arg)
{
//	arg->pushNewFrame(frame); //push a new frame on top of stack
	frame->_pc += 3;
	return true;
}

bool f_invokespecial(u1 * bc, Frame * frame, JavaVM * arg)
{
//	arg->pushNewFrame(frame);
	frame->_pc += 3;
	return true;
}
bool f_invokestatic(u1 * bc, Frame * frame, JavaVM * arg)
{
//	arg->pushNewFrame(frame);
	frame->_pc += 3;
	return true;
}

bool f_new(u1 * bc, Frame * frame, JavaVM * arg)
{
	/* http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-6.html#jvms-6.5.new */
	bc++;
	u2 index = getu2(bc);
	boost::shared_ptr<JavaClass> java_type = frame->_clazz->resolveClassName(index);
	Object * objectref = arg->getObjectHeap()->createObject(java_type);
	if (objectref == NULL)
		ASSERT(false); //TODO throw exception
	
	Variable var;
	var._var._objectref = objectref;
	var._type = 'L';
	frame->_operandStack.push(var);
	frame->_pc += 3;
	return true;
}

bool f_newarray(u1 * bc, Frame * frame, JavaVM * arg)
{
	int count = frame->_operandStack.top()._var._primitive._int;
	frame->_operandStack.pop();
	bc++;
	u1 atype = bc[0];
	
	Object * objectref = arg->getObjectHeap()->createPrimitiveArray(atype, count);
	if (objectref == NULL)
		ASSERT(false); //TODO throw exception
	
	Variable var;
	var._var._objectref = objectref;
	frame->_operandStack.push(var);
	frame->_pc += 2;
	return true;
}

bool f_anewarray(u1 * bc, Frame * frame, JavaVM * arg)
{
	bc++;
	u2 index = getu2(bc);
	u4 count = frame->_operandStack.top()._var._primitive._int;
	frame->_operandStack.pop();

	boost::shared_ptr<JavaClass> java_type = frame->_clazz->resolveClassName(index);
	Object * objectref = arg->getObjectHeap()->createPrimitiveArray('L', count);
	if (objectref == NULL)
		ASSERT(false); //TODO throw exception
	
	Variable var;
	var._var._objectref = objectref;
	frame->_operandStack.push(var);
	frame->_pc += 3;
	return true;
}

bool f_arraylength(u1 * bc, Frame * frame, JavaVM * arg)
{
	Variable arrayref = frame->_operandStack.top();
	frame->_operandStack.pop();
	
	Variable size;
	size._var._primitive._int = arrayref._var._objectref->_countFields;
	frame->_operandStack.push(size);
	frame->_pc++;
	return true;
}

bool f_athrow(u1 * bc, Frame * frame, JavaVM * arg)
{
	return false;
}

bool f_checkcast(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }
bool f_instanceof(u1 * bc, Frame * frame, JavaVM * arg) { return f_ASSERT(bc, frame, arg); }

bool f_monitorenter(u1 * bc, Frame * frame, JavaVM * arg)
{
	if (arg->lockMonitor(frame->_operandStack.top()._var._objectref))
	{
		frame->_objOwner.push(frame->_localVariable[0]._var._objectref);
	}
	else
	{
		frame->_waitingObj = frame->_operandStack.top()._var._objectref;
		frame->_lock = true;
		arg->wait(frame->_waitingObj, boost::bind(&Frame::unlock, frame));
	}
	return true;
}

bool f_monitorexit(u1 * bc, Frame * frame, JavaVM * arg)
{
	if (arg->unlockMonitor(frame->_operandStack.top()._var._objectref))
	{
		arg->notify(frame->_operandStack.top()._var._objectref);
		//TODO remover a referencia do objeto da lista de objtos locked
	}

	return true;
}
