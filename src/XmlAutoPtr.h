#ifndef XML_AUTO_PTR_H
#define XML_AUTO_PTR_H

#include <libxml/globals.h>


template <typename T>
class XmlAutoPtr {
	public:
		explicit XmlAutoPtr(T *ptr=0) {
			this->ptr = ptr;
		}

		~XmlAutoPtr() {
			xmlFree(this->ptr);
		}

		T* get() {
			return this->ptr;
		}

		XmlAutoPtr<T> operator=(T *ptr) {
			xmlFree(this->ptr);
			this->ptr = ptr;
		}

		T* operator->() {
			return this->ptr;
		}

		operator T*() {
			return this->ptr;
		}

	private:
		T* ptr;
};

#endif
