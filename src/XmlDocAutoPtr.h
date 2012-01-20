#ifndef XML_DOC_AUTO_PTR_H
#define XML_DOC_AUTO_PTR_H

#include <libxml/tree.h>
#include <stddef.h>

class XmlDocAutoPtr {
	public:
		explicit XmlDocAutoPtr(const xmlDocPtr ptr=NULL);
		XmlDocAutoPtr(const XmlDocAutoPtr& other);
		~XmlDocAutoPtr();
		XmlDocAutoPtr& operator=(const XmlDocAutoPtr& other);
		XmlDocAutoPtr& operator=(xmlDocPtr p);
		xmlDocPtr operator->() const;
		operator xmlDocPtr() const;
		xmlDocPtr get() const;

	private:
		xmlDocPtr ptr;
		mutable bool owner;
		xmlDocPtr release() const;
};

#endif
