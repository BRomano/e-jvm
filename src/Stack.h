#pragma once

#include "JavaTypes.h"

class StackFrame
{
	private:
		std::stack<Frame *> _frames;

	public:
		StackFrame()
		{
			Frame * base_frame = new Frame();
			this->push(base_frame);
		}
		
		~StackFrame()
		{
			Frame * f = this->top();
			this->pop();
			delete f;
		}

		void push(Frame * f) { _frames.push(f); }
		void pop() { _frames.pop(); }

		Frame * top()
		{
			if (_frames.empty())
				throw std::exception();
			return _frames.top();
		}

		void setException();
		
		/**
		 * return true if this method is not void, otherwise it will return false.
		 */
		bool methodSDone(Variable * ret);
};
