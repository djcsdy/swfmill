#ifndef XSLT_STYLESHEET_AUTO_PTR_H
#define XSLT_STYLESHEET_AUTO_PTR_H

#include <libxslt/transform.h>
#include <stddef.h>

class XsltStylesheetAutoPtr {
	public:
		explicit XsltStylesheetAutoPtr(const xsltStylesheetPtr ptr=NULL);
		XsltStylesheetAutoPtr(const XsltStylesheetAutoPtr& other);
		~XsltStylesheetAutoPtr();
		XsltStylesheetAutoPtr& operator=(const XsltStylesheetAutoPtr& other);
		XsltStylesheetAutoPtr& operator=(xsltStylesheetPtr p);
		xsltStylesheetPtr operator->() const;
		operator xsltStylesheetPtr() const;
		xsltStylesheetPtr get() const;
		xsltStylesheetPtr release() const;

	private:
		xsltStylesheetPtr ptr;
		mutable bool owner;
};

#endif
