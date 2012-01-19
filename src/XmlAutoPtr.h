#ifndef XML_AUTO_PTR_H
#define XML_AUTO_PTR_H

#include <stddef.h>
#include <libxml/globals.h>


template <typename T>
class XmlAutoPtr {
	public:
		explicit XmlAutoPtr(T *ptr=NULL) {
			this->ptr = ptr;
		}

		~XmlAutoPtr() {
			if (this->ptr) {
				xmlFree(this->ptr);
			}
		}

		T* get() {
			return this->ptr;
		}

		XmlAutoPtr<T> operator=(T *ptr) {
			if (this->ptr) {
				xmlFree(this->ptr);
			}
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
