<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>
	<xsl:template name="writexml">
<xsl:document href="g{/format/@format}WriteXML.cpp" method="text">

#include "<xsl:value-of select="/format/@format"/>.h"
#include "base64.h"
#include &lt;cstring&gt;
#include &lt;errno.h&gt;
#include &lt;iconv.h&gt;

namespace <xsl:value-of select="/format/@format"/> {

xmlChar *toXmlChar(const Context *ctx, const char *from_str) {
	if (ctx-&gt;convertEncoding) {
		size_t len = strlen(from_str);
		iconv_t cd = iconv_open("UTF-8", ctx-&gt;swf_encoding);
		if (cd &lt; 0) {
			fprintf(stderr, "iconv_open failed.\n");
			return xmlCharStrdup("");
		}

		size_t buf_size = (len + 1) * 2;
		char *dst;
		for (;;) {
			dst = new char[buf_size];
			size_t inbytesleft = len;
			size_t outbytesleft = buf_size - 1;
			ICONV_CONST char *pin = (ICONV_CONST char*)from_str;
			char *pout = dst;
			bool expandbuf = false;

			while (inbytesleft &gt; 0) {
				size_t r = iconv(cd, &amp;pin, &amp;inbytesleft, &amp;pout, &amp;outbytesleft);
				if (r == (size_t)-1) {
					if (errno == E2BIG) {
						// buf_size shorten
						expandbuf = true;
					} else {
						//bad input charctor
						fprintf(stderr, "iconv failed: %s\n", from_str);
					}
					break;
				}
			}
			if (expandbuf) {
				delete[] dst;
				iconv(cd, 0, 0, 0, 0);
				buf_size *= 2;
				continue;
			}
                        *pout = '\0';
			break;
		}
		iconv_close(cd);
		xmlChar *ret = xmlCharStrdup(dst);
		delete[] dst;
		return ret;
	} else {
		return xmlCharStrdup(from_str);
	}
}

#define TMP_STRLEN 0xFF

<xsl:for-each select="type|tag|action|filter|style|stackitem|namespaceconstant|multinameconstant|trait|opcode">
void <xsl:value-of select="@name"/>::writeXML( xmlNodePtr xml, Context *ctx ) {
	char tmp[TMP_STRLEN];
	xmlNodePtr node = xml;
	xmlNodePtr node2;
	
	node = xmlNewChild( node, NULL, (const xmlChar *)"<xsl:value-of select="@name"/>", NULL );
	<xsl:apply-templates select="*[@prop]|flagged|if|context" mode="writexml"/>
	
	if( ctx->transientPropsToXML ) {
		snprintf( tmp, TMP_STRLEN, "%i", file_offset );
		xmlSetProp( node, (const xmlChar *)"file_offset", (const xmlChar *)tmp );
		<xsl:for-each select="*">
			<xsl:choose>
				<xsl:when test="@prop"/>
				<xsl:otherwise>
					<xsl:apply-templates select="." mode="writexml"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
	}
	
	<xsl:if test="@name='UnknownTag' or @name='UnknownAction' or @name='UnknownOpCode'">
		snprintf( tmp, TMP_STRLEN, "0x%02X", type );
		xmlSetProp( node, (const xmlChar *)"id", (const xmlChar *)tmp );
	</xsl:if>
}
</xsl:for-each>

}

</xsl:document>
	</xsl:template>


<xsl:template match="flagged" mode="writexml">
	if( <xsl:if test="@negative">!</xsl:if><xsl:value-of select="@flag"/>
		<xsl:if test="@signifier"> &amp; <xsl:value-of select="@signifier"/></xsl:if> ) {
		<xsl:apply-templates select="*[@prop]|flagged|if" mode="writexml"/>
	}
</xsl:template>


<xsl:template match="if" mode="writexml">
	if( <xsl:value-of select="@expression"/> ) {
		<xsl:apply-templates select="*[@prop]|flagged|if" mode="writexml"/>
	}
</xsl:template>

<xsl:template match="double|double2|half|float|fixedpoint|fixedpoint2" mode="writexml">
	snprintf(tmp,TMP_STRLEN,"%#.*g", 16, <xsl:value-of select="@name"/>);
	xmlSetProp( node, (const xmlChar *)"<xsl:value-of select="@name"/>", (const xmlChar *)&amp;tmp );
</xsl:template>

<xsl:template match="byte|word|byteOrWord|integer|bit|uint32|u30|s24|encodedu32" mode="writexml">
	snprintf(tmp,TMP_STRLEN,"<xsl:apply-templates select="." mode="printf"/>", <xsl:value-of select="@name"/>);
	xmlSetProp( node, (const xmlChar *)"<xsl:value-of select="@name"/>", (const xmlChar *)&amp;tmp );
</xsl:template>

<xsl:template match="string" mode="writexml">
	if( <xsl:value-of select="@name"/> ) {
		xmlChar *xmlstr = toXmlChar(ctx, <xsl:value-of select="@name"/>);
		xmlSetProp(node, (const xmlChar *)"<xsl:value-of select="@name"/>", xmlstr);
	}
</xsl:template>

<xsl:template match="object" mode="writexml">
	node2 = xmlNewChild( node, NULL, (const xmlChar *)"<xsl:value-of select="@name"/>", NULL );
	<xsl:value-of select="@name"/>.writeXML( node2, ctx );
</xsl:template>

<xsl:template match="list" mode="writexml">
	{
		node2 = xmlNewChild( node, NULL, (const xmlChar *)"<xsl:value-of select="@name"/>", NULL );
		<xsl:value-of select="@type"/> *item;
		ListItem&lt;<xsl:value-of select="@type"/>&gt;* i;
		i = <xsl:value-of select="@name"/>.first();
		while( i ) {
			item = i->data();
			if( item ) {
				item->writeXML( node2, ctx );
			}
			i = i->next();
		}
	}
</xsl:template>

<!-- FIXME: i dont really know how much memory to allocate for dst string.. -->
<xsl:template match="data" mode="writexml">
	{
		if( <xsl:value-of select="@size"/> &amp;&amp; <xsl:value-of select="@name"/> ) {
			char *tmp_data = (char *)<xsl:value-of select="@name"/>;
			int sz = <xsl:value-of select="@size"/>;
			char *tmpstr = new char[ (sz * 3) ];
			
			int l = base64_encode( tmpstr, tmp_data, sz );
			if( l > 0 ) {
				tmpstr[l] = 0;
				xmlNewTextChild( node, NULL, (const xmlChar *)"<xsl:value-of select="@name"/>", (const xmlChar *)tmpstr );
			}
			delete tmpstr;
		}
	}
</xsl:template>

<xsl:template match="xml" mode="writexml">
	{
		if(<xsl:value-of select="@name"/> ) {
			xmlDocPtr doc = xmlParseMemory(<xsl:value-of select="@name"/>, strlen(<xsl:value-of select="@name"/>));
			if( doc ) {
				xmlNodePtr child = doc->children;

				child = xmlDocCopyNode(child, node->doc, 1);
				xmlAddChild(node, child);

				xmlFreeDoc(doc);
			}
		}
	}
</xsl:template>

<xsl:template match="context" mode="writexml">
	ctx-><xsl:value-of select="@param"/> = <xsl:value-of select="@value"/>;
</xsl:template>

</xsl:stylesheet>
