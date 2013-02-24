/****************************************************************************************/
/*											*/
/* This program is free software; you can redistribute it and/or modify it under the	*/
/* terms of the GNU General Public License as published by the Free Software		*/
/* Foundation; either version 2 of the License, or (at your option) any later version.	*/
/*											*/
/* This program is distributed in the hope that it will be useful, but WITHOUT ANY	*/
/* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A	*/
/* PARTICULAR PURPOSE. See the GNU General Public License for more details.		*/
/*											*/
/* You should have received a copy of the GNU General Public License along with this	*/
/* program; (See "COPYING"). If not, If not, see <http://www.gnu.org/licenses/>.        */
/*											*/
/*--------------------------------------------------------------------------------------*/
/*											*/
/*  Copyright   Joerg Anders, TU Chemnitz, Fakultaet fuer Informatik, GERMANY           */
/*		ja@informatik.tu-chemnitz.de						*/
/*											*/
/*											*/
/****************************************************************************************/

/* Actually I wanted to use GArray and GPtrArray but this
 * leads to random crashes in malloc on X86_64. A Glib-Bug ???
 */

#ifndef DYN_ARRAY_H

#define DYN_ARRAY_H
#include <gtk/gtk.h>

#define CHECK_SIZES

#define MAX_LEN 8192
#define DELAULT_LEN 10
#define SEG_LEN 10


template <typename T>
class NedDynArray {
	public:
		NedDynArray();
		~NedDynArray<T>();
		void insert(unsigned int idx, T element);
		void append(T element);
		T getAt(unsigned int idx);
		T* getPointerAt(unsigned int idx);
		unsigned int getLen()  {return m_len;}
		void setLen(unsigned int len, bool zero);
		void empty(bool zero);
		void zeroArray();
		void sort(int (*comfunc)(const void *, const void *));
		T* getFirst();
		T* getNext();
		T* skip(int len);
		T* getFirst2(int len);
		T* getNext2();
		T* getFirst3();
		T* getNext3();
		T* getArray();

	private:
		void allocateArray();
#ifdef CHECK_SIZES
		void dynarray_abort(const char *s);
#endif
		unsigned long m_len;
		unsigned long m_allocLen;
		T *m_array;
		T *m_int_ptr, *m_end_ptr;
		T *m_int_ptr2, *m_end_ptr2;
		T *m_int_ptr3;
		unsigned long m_element_size;
};

/* ------------------------------------------- Implementation --------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>

static void erroraus(const char *s) {
	fprintf(stderr, "%s\n", s);
}

#ifdef CHECK_SIZES
template <typename T>
void NedDynArray<T>::dynarray_abort(const char *s) {
	//fprintf(stderr, s);
	erroraus(s);
	exit(-1);
}
#endif

template <typename T>
NedDynArray<T>::NedDynArray() : m_len(0), m_allocLen(0), m_array(NULL)  {
	m_element_size = sizeof(T);
}


template <typename T>
NedDynArray<T>::~NedDynArray() {
	if (m_len > 0) {
		g_free(m_array);
	}
}

template <typename T>
void NedDynArray<T>::allocateArray() {
	unsigned int newsize, newalloclen;
	if (m_len == 0) return;
	newsize = m_element_size * m_len;
	if (newsize >= m_allocLen) {
		newalloclen = m_allocLen + m_element_size * SEG_LEN;
		if (newalloclen < newsize) {
			newalloclen = newsize;
		}
		m_allocLen = newalloclen;
		if (m_array == NULL) {
			if ((m_array = (T*) g_try_malloc(m_allocLen)) == NULL) {
				dynarray_abort("NedDynArray<T>::allocateArray: malloc error\n");
			}
		}
		else {
			if ((m_array = (T*) g_try_realloc(m_array, m_allocLen)) == NULL) {
				dynarray_abort("NedDynArray<T>::allocateArray: realloc error\n");
			}
		}
	}
}

template <typename T>
void NedDynArray<T>::insert(unsigned int idx, T element) {
#ifdef CHECK_SIZES
	if (idx >= m_len) {
		dynarray_abort("NedDynArray<T>::insert\n");
	}
#endif
	m_array[idx] = element;
	
}

template <typename T>
void NedDynArray<T>::append(T element) {
#ifdef CHECK_SIZES
	if (m_len + 1 > MAX_LEN) {
		dynarray_abort("NedDynArray<T>::append\n");
	}
#endif
	m_len++;
	allocateArray();
	m_array[m_len-1] = element;
}

template <typename T>
T NedDynArray<T>::getAt(unsigned int idx) {
#ifdef CHECK_SIZES
	if (idx >= m_len) {
		printf("idx = %u, m_len = %lu\n", idx, m_len);
		dynarray_abort("NedDynArray<T>::insert\n");
	}
#endif
	return m_array[idx];
}

template <typename T>
T* NedDynArray<T>::getPointerAt(unsigned int idx) {
#ifdef CHECK_SIZES
	if (idx >= m_len) {
		printf("idx = %u, m_len = %lu\n", idx, m_len);
		dynarray_abort("NedDynArray<T>::insert\n");
	}
#endif
	return m_array + idx;
}

template <typename T>
T* NedDynArray<T>::getFirst() {
	m_int_ptr = m_array;
	m_end_ptr = m_array + m_len;
	if (m_len < 1) {
		return NULL;
	}
	return m_int_ptr;
}

template <typename T>
T* NedDynArray<T>::getNext() {
	m_int_ptr++;
	if (m_int_ptr >= m_end_ptr) {
		return NULL;
	}
	return m_int_ptr;
}

template <typename T>
T* NedDynArray<T>::skip(int len) {
	m_int_ptr += len;
	if (m_int_ptr >= m_end_ptr) {
		return NULL;
	}
	return m_int_ptr;
}

template <typename T>
T* NedDynArray<T>::getFirst2(int len) {
#ifdef CHECK_SIZES
	if (m_len < 1) {
		dynarray_abort("NedDynArray<T>::getFirst2(1)\n");
	}
	if (m_int_ptr >= m_end_ptr) {
		dynarray_abort("NedDynArray<T>::getFirst2(2)\n");
	}
	if (m_int_ptr + len  > m_end_ptr) {
		dynarray_abort("NedDynArray<T>::getFirst2(2)\n");
	}
#endif
	m_int_ptr2 = m_int_ptr;
	m_end_ptr2 = m_int_ptr2 + len;
	return m_int_ptr2;
}

template <typename T>
T* NedDynArray<T>::getNext2() {
	m_int_ptr2++;
	if (m_int_ptr2 >= m_end_ptr2) {
		return NULL;
	}
	return m_int_ptr2;
}

template <typename T>
T* NedDynArray<T>::getFirst3() {
#ifdef CHECK_SIZES
	if (m_len < 1) {
		dynarray_abort("NedDynArray<T>::getFirst3(1)\n");
	}
	if (m_int_ptr2 >= m_end_ptr) {
		dynarray_abort("NedDynArray<T>::getFirst3(2)\n");
	}
#endif
	m_int_ptr3 = m_int_ptr2 + 1;
	if (m_int_ptr3 >= m_end_ptr2) return NULL;
	return m_int_ptr3;
}

template <typename T>
T* NedDynArray<T>::getNext3() {
	m_int_ptr3++;
	if (m_int_ptr3 >= m_end_ptr2) {
		return NULL;
	}
	return m_int_ptr3;
}

template <typename T>
T* NedDynArray<T>::getArray() {
#ifdef CHECK_SIZES
	if (m_len < 1) {
		dynarray_abort("NedDynArray<T>::getArray\n");
	}
#endif
	return m_array;
}



template <typename T>
void NedDynArray<T>::setLen(unsigned int len, bool zero) {
#ifdef CHECK_SIZES
	if (len > MAX_LEN) {
		printf("len = %d, MAX_LEN = %d\n", len, MAX_LEN); fflush(stdout);
		dynarray_abort("NedDynArray::setLen\n");
	}
#endif
	m_len = len;
	allocateArray();
	if (zero) {
		memset(m_array, 0, m_allocLen);
	}
}

template <typename T>
void NedDynArray<T>::sort(int (*comfunc)(const void *, const void *)) {
	if (m_len < 2) return;

	qsort(m_array, m_len, m_element_size, comfunc);
}

template <typename T>
void NedDynArray<T>::empty(bool zero) {
	if (zero && m_len > 0) {
		memset(m_array, 0, m_allocLen);
	}
	m_len = 0;
}

template <typename T>
void NedDynArray<T>::zeroArray() {
	if (m_len > 0) {
		memset(m_array, 0, m_allocLen);
	}
}


#endif /*  DYN_ARRAY_H */
