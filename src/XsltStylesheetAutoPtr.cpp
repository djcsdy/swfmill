#include "XsltStylesheetAutoPtr.h"

XsltStylesheetAutoPtr::XsltStylesheetAutoPtr(const xsltStylesheetPtr p)
	: owner(p), ptr(p) { }

XsltStylesheetAutoPtr::XsltStylesheetAutoPtr(const XsltStylesheetAutoPtr& other)
	: owner(other.owner), ptr(other.release()) { }

XsltStylesheetAutoPtr::~XsltStylesheetAutoPtr() {
	if (owner) {
		xsltFreeStylesheet(this->ptr);
	}
}

XsltStylesheetAutoPtr& XsltStylesheetAutoPtr::operator=(const XsltStylesheetAutoPtr& other) {
	if ((void*)&other != (void*)this) {
		if (owner) {
			xsltFreeStylesheet(ptr);
		}
		owner = other.owner;
		ptr = other.release();
	}
}

XsltStylesheetAutoPtr& XsltStylesheetAutoPtr::operator=(xsltStylesheetPtr p) {
	if (p != ptr) {
		if (owner) {
			xsltFreeStylesheet(ptr);
		}
		owner = p;
		ptr = p;
	}
}

xsltStylesheetPtr XsltStylesheetAutoPtr::operator->() const {
	return this->ptr;
}

XsltStylesheetAutoPtr::operator xsltStylesheetPtr() const {
	return this->ptr;
}

xsltStylesheetPtr XsltStylesheetAutoPtr::get() const {
	return this->ptr;
}

xsltStylesheetPtr XsltStylesheetAutoPtr::release() const {
	owner = false;
	return ptr;
}
