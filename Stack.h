#ifndef Stack_H
#define Stack_H

#include<iostream>

using std::cout;
using std::endl;

template<class T>
class Stack
{
private:
	T *save;
	int now;
	int max;
	void bigger_size();
public:
	Stack(int size = 5);
	~Stack();
	T pop();
	void push(T& temp);
	int count();
	void visit(void(*function)(T&temp));
};


template<class T>
void Stack<T>::bigger_size()
{
	T* temp = new T[2 * max];
	max = max * 2;
	for (int i = 0; i < now; i++)
		temp[i] = save[i];
	delete[] save;
	save = temp;
}

template<class T>
Stack<T>::Stack(int size)
{
	if (size <= 0)
		cout << "ERROR: Size of Stack must bigger than zero!" << endl;
	else
	{
		now = 0;
		max = size;
		save = new T[max];
	}
}

template<class T>
Stack<T>::~Stack()
{
	now = max = 0;
	delete[] save;
}

template<class T>
T Stack<T>::pop()
{
	if (now == 0)
	{
		cout << "Error:Now Item in Stack,return something without meaning!" << endl;
		return T();
	}
	else
	{
		T temp = save[--now];
		return temp;
	}
}

template<class T>
void Stack<T>::push(T & temp)
{
	if (now == max)
		bigger_size();
	save[now++] = temp;
}

template<class T>
int Stack<T>::count()
{
	return now;
}

template<class T>
void Stack<T>::visit(void(*function)(T &temp))
{
	for (int i = 0; i < now; i++)
	{
		function(save[i]);
	}
}

#endif
