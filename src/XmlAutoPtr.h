#ifndef XML_AUTO_PTR_H
#define XML_AUTO_PTR_H

#include <stddef.h>
#include <libxml/globals.h>


template <typename T>
class XmlAutoPtr {
	public:
		explicit XmlAutoPtr(T* p=NULL)
			: owner(p), ptr(p) { }

		XmlAutoPtr(const XmlAutoPtr& other)
			: owner(other.owner), ptr(other.release()) { }

		~XmlAutoPtr() {
			if (owner) {
				xmlFree(ptr);
			}
		}

		XmlAutoPtr& operator=(const XmlAutoPtr& other) {
			if ((void*)&other != (void*)this) {
				if (owner) {
					xmlFree(ptr);
				}
				owner = other.owner;
				ptr = other.release();
			}
		}

		T* operator->() const {
			return this->ptr;
		}

		operator T*() const {
			return this->ptr;
		}

		T* get() const {
			return this->ptr;
		}

	private:
		T* ptr;
		mutable bool owner;

		T* release() const {
			owner = false;
			return ptr;
		}
};

#endif
