/*
	Dynamic array template

	Maintains a dynamic array with members of type T
	Wonderful for ints, floats, pointers etc.
	Load of rubbish for large structures & object instances.
*/

#ifndef _INCLUDED_DYNARRAY_H
#define _INCLUDED_DYNARRAY_H

#include <memory.h>
#include <iostream.h>

template <class T> class dynarray
{
private:
	// static "compare" function for type T, used for qsort
	static int cmp_elem(const void* p, const void* q)
	{
		T *a = (T*)p, *b = (T*)q;
		return (*a < *b) ? -1 : ((*a > *b) ? 1 : 0);
	}

protected:
	T *array;
	int _size, _used, _step;

public:
	// construct an empty dynarray with growth step of 10
	dynarray() : _size(0), _used(0), _step(10), array(NULL) { }

	// construct an empty dynarray with growth step of 10, preallocate 'alloc' entries
	dynarray(int alloc) : _size(alloc), _used(0), _step(alloc)
	{
		dynarray<T>(alloc, 10);
	}

	// construct an empty dynarray with growth step of 'step', preallocate 'alloc' entries
	dynarray(int alloc, int step) :	_size(alloc), _used(0), _step(step)
	{
		array = new T[_size];
	}

	// destructor
	~dynarray() { delete [] array; }

	// construct a dynarray from the contents of another
	dynarray(dynarray<T> &a) :
		_size(a._used), _used(a._used), _step(a._step)
	{
		array = new T[_size];
		memcpy(array, a.array, _size * sizeof(T));
	}

	// assignment copies entire contents; relatively expensive
	dynarray<T>& operator=(dynarray<T> &a)
	{
		_used = 0; return add(a.array, a._used);
	}

	// add an element, allocating more space if necessary (cheap)
	dynarray &add(const T a)
	{
		int e = _used;
		if (_used >= _size) pack(_used + _step);
		array[e] = a;
		_used++;
		return *this;
	}

	// add an array of elements, allocating more space if necessary (cheap)
	dynarray &add(const T* a, int len)
	{
		int e = _used;
		if ((_used+len) > _size) pack(_used + len);
		memcpy(&array[e], a, len * sizeof(T));
		_used += len;
		return *this;
	}

	// concatenate anoter dynarray to the end of this one (relatively cheap)
	dynarray &concatenate(dynarray &a) { return add(a.array, a._used); }

	// insert an element at a position (expensive)
	dynarray &insert(const T a, int e)
	{
		if (_used >= _size) pack(_used + _step);
		memcpy(&array[e+1], &array[e], (_used-e) * sizeof(T));
		array[e] = a;
		_used++;
		return *this;
	}

	// insert an array of elements at a given positon (expensive)
	dynarray &insert(const T* a, int len, int e)
	{
		if ((_used+len) > _size) pack(_used + len);
		memcpy(&array[e+len], &array[e], (_used-e) * sizeof(T));
		memcpy(&array[e], a, len * sizeof(T));
		_used += len;
		return *this;
	}

	// remove the given element (expensive)
	dynarray &remove(int e)
	{
		if (e < 0 || e >= _used) return *this;
		_used--;
		memcpy(&array[e], &array[e+1], (_used-e) * sizeof(T));
		return *this;
	}

	// remove a number of elements (expensive)
	dynarray &remove(int e, int n)
	{
		if (e < 0) return *this;
		if ((e+n) > _used)
		{
			n = _used - e;
			if (n <= 0) return *this;
		}
		_used -= n;
		memcpy(&array[e], &array[e+n], (_used-e) * sizeof(T));
		return *this;
	}

	// Element indexing [] operator, returns reference to an element
#ifdef _DEBUG
	const T& operator[](int e) {
		if (e < 0 || e >= _used) throw "dynarray out of range";
		else return array[e];
	}
#else
	inline const T& operator[](int e) { return array[e]; }
#endif

	// allocate at least this much memory
	dynarray &pack(int new_size)
	{
		if (new_size < _used) new_size = _used;
		
		T *temp = new T[new_size];
		memcpy(temp, array, sizeof(T) * _used);
		delete array;
		array = temp;
		_size = new_size;
		return *this;
	}

	// allocate exactly the right amount of memory
	dynarray &pack() { return pack(_used); }

	// the number of elements in the array
	inline int length() { return _used; }
	// the amount of space allocated for the array
	inline int allocated() { return _size; }
	
	// set the size by which the array grows
	inline int alloc_step(int step) { if (step > 0) _step = step; return _step; }
	// the size by which the array grows
	inline int alloc_step() { return step; }

	// quicksort the array using the ANSI C standard 'qsort' function
	inline dynarray &qsort() { ::qsort(array, _used, sizeof(T), cmp_elem); return *this; }
};

template<class T> ostream& operator<< (ostream &s, dynarray<T> &a)
{
	s << '[';
	for (int i = a.length(), p = 0; i; i--, p++)
	{
		s << a[p];
		if (i > 1) s << ',';
	}
	s << "]\n";
	return s;
}

#endif