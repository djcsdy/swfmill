#include "XmlDocAutoPtr.h"

XmlDocAutoPtr::XmlDocAutoPtr(const xmlDocPtr p)
	: owner(p), ptr(p) { }

XmlDocAutoPtr::XmlDocAutoPtr(const XmlDocAutoPtr& other)
	: owner(other.owner), ptr(other.release()) { }

XmlDocAutoPtr::~XmlDocAutoPtr() {
	if (owner) {
		xmlFreeDoc(this->ptr);
	}
}

XmlDocAutoPtr& XmlDocAutoPtr::operator=(const XmlDocAutoPtr& other) {
	if ((void*)&other != (void*)this) {
		if (owner) {
			xmlFreeDoc(ptr);
		}
		owner = other.owner;
		ptr = other.release();
	}
}

XmlDocAutoPtr& XmlDocAutoPtr::operator=(xmlDocPtr p) {
	if (p != ptr) {
		if (owner) {
			xmlFreeDoc(ptr);
		}
		owner = p;
		ptr = p;
	}
}

xmlDocPtr XmlDocAutoPtr::operator->() const {
	return this->ptr;
}

XmlDocAutoPtr::operator xmlDocPtr() const {
	return this->ptr;
}

xmlDocPtr XmlDocAutoPtr::get() const {
	return this->ptr;
}

xmlDocPtr XmlDocAutoPtr::release() const {
	owner = false;
	return ptr;
}
