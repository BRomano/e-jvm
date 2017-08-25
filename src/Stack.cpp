#include "Stack.h"
#include "JavaClass.h"

bool StackFrame::methodSDone(Variable * ret)
{
	Frame * f = this->top();
	this->pop();
	std::string method_desc;
	f->_clazz->getStringFromConstPool(f->_method->descriptor_index, method_desc);
	if (method_desc.find(")V") == std::string::npos)
		(*ret) = f->_operandStack.top();

	delete f;

	if (method_desc.find(")V") == std::string::npos)
		return true;

	return false;
}

void StackFrame::setException()
{
	Frame * f = this->top();
	this->pop();
	this->top()->_operandStack.push(f->_operandStack.top()); //stack up exception
	this->top()->_hasException = true;
	/* remove all notify for this frame */
	delete f;
}
