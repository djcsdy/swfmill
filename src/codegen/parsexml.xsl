<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>
	<xsl:template name="parsexml">
<xsl:document href="g{/format/@format}ParseXML.cpp" method="text">

#include "<xsl:value-of select="/format/@format"/>.h"
#include &lt;cstring&gt;
#include &lt;cctype&gt;
#include &lt;cstdlib&gt;
#include "base64.h"
#include &lt;errno.h&gt;
#include &lt;iconv.h&gt;

using namespace std;

namespace <xsl:value-of select="/format/@format"/> {

char *fromXmlChar(const Context *ctx, const xmlChar *from_str) {
	if (ctx-&gt;convertEncoding) {
		size_t len = strlen((const char *)from_str);
		iconv_t cd = iconv_open(ctx-&gt;swf_encoding, "UTF-8");
		if (cd &lt; 0) {
			fprintf(stderr, "iconv_open failed.\n");
			char *buf = new char[1];
			buf[0] = '\0';
			return buf;
		}
		size_t buf_size = (len + 1) * 2;
		for (;;) {
			char * const dst = new char[buf_size];
			size_t inbytesleft = len;
			size_t outbytesleft = buf_size - 1; // reserve 1 byte for '\0'
			ICONV_CONST char *pin = (ICONV_CONST char*)from_str;
			char *pout = dst;
			bool expandbuf = false;

			while (inbytesleft &gt; 0) {
				size_t r = iconv(cd, &amp;pin, &amp;inbytesleft, &amp;pout, &amp;outbytesleft);
				if (r == (size_t)-1) {
					if (errno == E2BIG) {
						expandbuf = true;
					} else {
						// bad input charctor
						fprintf(stderr, "iconv failed: %s\n", from_str);
					}
					break;
				}
			}
			if (expandbuf) {
				iconv(cd, 0, 0, 0, 0);
				delete[] dst;
				buf_size *= 2;
				continue;
			}
			*pout = '\0';
			iconv_close(cd);
			return dst;
		}
	} else {
		size_t len = strlen((const char *)from_str) + 1;
		char *buf = new char[len];
		strcpy(buf, (const char *)from_str);
		return buf;
	}
}

char *strdupx(const char *src) {
	char *t = new char[strlen(src)+1];
	strcpy(t, src);
	return  t;
}

<xsl:for-each select="type|tag|action|filter|style|stackitem|namespaceconstant|multinameconstant|trait|opcode">
void <xsl:value-of select="@name"/>::parseXML( xmlNodePtr node, Context *ctx ) {
	xmlNodePtr node2;
	xmlChar *tmp;
	
//	printf("<xsl:value-of select="@name"/>::parseXML\n");

	<xsl:for-each select="*[@context]">
		ctx-&gt;<xsl:value-of select="@name"/> = <xsl:apply-templates select="." mode="default"/>;
	</xsl:for-each>

	<xsl:apply-templates select="*[@prop]|flagged|if|context" mode="parsexml"/>
	
	<xsl:if test="@name='UnknownTag' or @name='UnknownAction' or @name='UnknownOpCode'">
		tmp = xmlGetProp( node, (const xmlChar *)"id" );
		if( tmp ) { 
			sscanf( (char *)tmp, "%X", &amp;type ); 
			xmlFree( (xmlChar *)tmp ); 
		}
	</xsl:if>

	<xsl:for-each select="*[@context]">
		<xsl:choose>
			<xsl:when test="@context='inverse'">
				ctx-><xsl:value-of select="@name"/> = <xsl:value-of select="@name"/>;
			</xsl:when>
			<xsl:when test="@set-from-bits-needed">
				<xsl:value-of select="@name"/> = SWFBitsNeeded( <xsl:value-of select="@set-from-bits-needed"/> );
				if( <xsl:value-of select="@name"/> > ctx-><xsl:value-of select="@name"/> ) ctx-><xsl:value-of select="@name"/> = <xsl:value-of select="@name"/>;
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="@name"/> = ctx-><xsl:value-of select="@name"/>;
			</xsl:otherwise>
		</xsl:choose>
	</xsl:for-each>
}
</xsl:for-each>

}

</xsl:document>
	</xsl:template>


<xsl:template match="byte|word|byteOrWord|fixedpoint|fixedpoint2|bit|integer|string|uint32|float|double|double2|half|u30|s24|encodedu32" mode="has">
	if( <xsl:if test="../@negative">!</xsl:if>xmlHasProp( node, (const xmlChar *)"<xsl:value-of select="@name"/>" ) ) has = true;
</xsl:template>

<xsl:template match="object|list|data|xml" mode="has">
	{
		xmlNodePtr child = node->children;
		while( child &amp;&amp; !has ) {
			if( !strcmp( (const char *)child->name, "<xsl:value-of select="@name"/>" ) ) has = true;
			child = child->next;
		}
	}
</xsl:template>


<xsl:template match="flagged" mode="parsexml">
	{
		bool has = false;
		<xsl:for-each select="*[@prop]|flagged|if">
			<xsl:apply-templates select="." mode="has"/>
			<xsl:choose>
				<xsl:when test="../@signifier">
					if( has ) <xsl:value-of select="../@flag"/> |= <xsl:value-of select="$signifier"/>;
					else <xsl:value-of select="../@flag"/> ^= <xsl:value-of select="$signifier"/>;
				</xsl:when>
				<xsl:otherwise>
					if( has ) <xsl:value-of select="../@flag"/> = <xsl:if test="../@negative">!</xsl:if>true;
				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
		
		if( has ) {
			<xsl:apply-templates select="*[@prop]|flagged|if" mode="parsexml"/>
		}
	
		<xsl:for-each select="*[@context]">
			<xsl:value-of select="@name"/> = ctx-><xsl:value-of select="@name"/>;
		</xsl:for-each>
	}
</xsl:template>

<xsl:template match="if" mode="parsexml">
	if( <xsl:value-of select="@expression"/> ) {
		<xsl:apply-templates select="*[@prop]|flagged|if" mode="parsexml"/>
	}
</xsl:template>


<xsl:template match="byte|word|byteOrWord|bit|uint32|u30|s24" mode="parsexml">
	tmp = xmlGetProp( node, (const xmlChar *)"<xsl:value-of select="@name"/>" );
	if( tmp ) {
		int tmp_int;
		sscanf( (char *)tmp, "<xsl:apply-templates select="." mode="printf"/>", &amp;tmp_int );
		<xsl:value-of select="@name"/> = tmp_int;
		xmlFree( tmp );
	}
</xsl:template>

<xsl:template match="float|double|double2|half" mode="parsexml">
	tmp = xmlGetProp( node, (const xmlChar *)"<xsl:value-of select="@name"/>" );
	if( tmp ) {
		double tmp_float;
		sscanf( (char *)tmp, "%lg", &amp;tmp_float );
		<xsl:value-of select="@name"/> = tmp_float;
		xmlFree( tmp );
	}
</xsl:template>

<xsl:template match="fixedpoint|fixedpoint2" mode="parsexml">
	tmp = xmlGetProp( node, (const xmlChar *)"<xsl:value-of select="@name"/>" );
	if( tmp ) {
		double t;
		sscanf( (char *)tmp, "%lg", &amp;t);
		<xsl:value-of select="@name"/> = t;
		xmlFree( tmp );
		<xsl:choose>
		<!-- should this be done in writer.xsl? -->
			<xsl:when test="@constant-size"/>
			<xsl:otherwise>
				int b = SWFBitsNeeded( <xsl:value-of select="@name"/>, <xsl:value-of select="@exp"/><xsl:if test="@signed">, true</xsl:if> );
				<xsl:if test="@size-add">b -= <xsl:value-of select="@size-add"/>;</xsl:if>
				if( b > <xsl:value-of select="@size"/> )
				<xsl:choose>
					<xsl:when test="@context-size">
						<xsl:value-of select="@size"/> = b;
					</xsl:when>
					<xsl:otherwise>
						set<xsl:value-of select="@size"/> (b);
					</xsl:otherwise>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
	} else {
		fprintf(stderr,"WARNING: no %s property in %s element\n", "<xsl:value-of select="@name"/>", (const char *)node->name );
	}
</xsl:template>

<xsl:template match="integer" mode="parsexml">
	tmp = xmlGetProp( node, (const xmlChar *)"<xsl:value-of select="@name"/>" );
	if( tmp ) {
		sscanf( (char *)tmp, "<xsl:apply-templates select="." mode="printf"/>", &amp;<xsl:value-of select="@name"/>);
		xmlFree( tmp );
		<xsl:choose>
		<!-- should this be done in writer.xsl? -->
			<xsl:when test="@constant-size"/>
			<xsl:otherwise>
				int b = SWFBitsNeeded( <xsl:value-of select="@name"/><xsl:if test="@signed">, true</xsl:if> );
				<xsl:if test="@size-add">b -= <xsl:value-of select="@size-add"/>;</xsl:if>
				if( b > <xsl:value-of select="@size"/> )
				<xsl:choose>
					<xsl:when test="@context-size">
						<xsl:value-of select="@size"/> = b;
					</xsl:when>
					<xsl:otherwise>
						set<xsl:value-of select="@size"/> (b);
					</xsl:otherwise>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
	} else {
		fprintf(stderr,"WARNING: no %s property in %s element\n", "<xsl:value-of select="@name"/>", (const char *)node->name );
	}
</xsl:template>

<xsl:template match="string" mode="parsexml">
	tmp = xmlGetProp( node, (const xmlChar *)"<xsl:value-of select="@name"/>" );
	if( tmp ) {
		<xsl:value-of select="@name"/> = fromXmlChar(ctx, (const xmlChar*)tmp);
		xmlFree(tmp);
	} else {
		fprintf(stderr,"WARNING: no %s property in %s element\n", "<xsl:value-of select="@name"/>", (const char *)node->name );
		<xsl:value-of select="@name"/> = strdupx("[undefined]");
	}
</xsl:template>

<xsl:template match="object" mode="parsexml">
	node2 = node->children;
	while( node2 ) {
		if( !strcmp( (const char *)node2->name, "<xsl:value-of select="@name"/>" ) ) {
<!--
			<xsl:value-of select="@name"/>.parseXML( node2, ctx );
			node=NULL;
-->
		xmlNodePtr child = node2->children;
			while( child ) {
				if( !xmlNodeIsText( child ) ) {
					<xsl:value-of select="@name"/>.parseXML( child, ctx );
					node2 = child = NULL;
					node2 = NULL;
				} else {
					child = child->next;
				}
			}
		}
		if( node2 ) node2 = node2->next;
	}
</xsl:template>

<xsl:template match="list" mode="parsexml">
	node2 = node->children;
	while( node2 ) {
		if( !strcmp( (const char *)node2->name, "<xsl:value-of select="@name"/>" ) ) {
			<xsl:if test="@length and not(@constant-length)">
				<xsl:value-of select="@length"/>=0;
			</xsl:if>
			<xsl:if test="@end">
				size_t l = 0;
			</xsl:if>

			xmlNodePtr child = node2->children;
			while( child ) {
				if( !xmlNodeIsText( child ) ) {
					<xsl:value-of select="@type"/>* item = <xsl:value-of select="@type"/>::getByName( (const char *)child->name );
					if( item ) {
						item->parseXML( child, ctx );
						<xsl:value-of select="@name"/>.append( item );
						<xsl:if test="@length and not(@constant-length)">
							<xsl:value-of select="@length"/>++;
						</xsl:if>
						<xsl:if test="@end">
							l += item->getSize(ctx, l);
						</xsl:if>
					}
				}
				child = child->next;
			}
			
			<xsl:if test="@length and @length-add">
				<xsl:value-of select="@length"/> -= <xsl:value-of select="@length-add"/>;
			</xsl:if>
			<xsl:if test="@length and @length-sub">
				if( <xsl:value-of select="@length"/> > 0 ) {
					<xsl:value-of select="@length"/> += <xsl:value-of select="@length-sub"/>;
				}
			</xsl:if>
			<xsl:if test="@end">
				<xsl:value-of select="@end"/> = (l/8)<xsl:if test="@end-offset"> - (<xsl:value-of select="@end-offset"/>)</xsl:if>;
			</xsl:if>

			node2=NULL;
		} else {
			node2 = node2->next;
		}
	}
</xsl:template>

<xsl:template match="data" mode="parsexml">
	{
		<xsl:value-of select="@name"/> = NULL;
		<xsl:value-of select="@size"/> = 0;

		xmlChar *xmld = xmlNodeGetContent( node );
		char *d = (char *)xmld;
		if( d ) {
			// unsure if this is neccessary
			for( int i=strlen(d)-1; i>0 &amp;&amp; isspace(d[i]); i-- ) d[i]=0;
			while( isspace(d[0]) ) d++;
			int l = strlen(d); //BASE64_GET_MAX_DECODED_LEN(strlen( d ));
			char *dst = new char[ l ];
			int lout = base64_decode( dst, (char*)d, l );
			if( lout > 0 ) {
				<xsl:value-of select="@size"/> = lout;
				<xsl:value-of select="@name"/> = new unsigned char[ lout ];
				memcpy( <xsl:value-of select="@name"/>, dst, lout );
			}
			delete dst;
			xmlFree( xmld );
		}
	}
</xsl:template>

<xsl:template match="xml" mode="parsexml">
	{
		xmlNodePtr child = NULL;
		xmlNodePtr currentChild = node->children;
		while( currentChild &amp;&amp; child == NULL) {
			if( ! strcmp( (const char *)currentChild->name, (const char *)"<xsl:value-of select="@name"/>" ) ) {
				child = currentChild;
			}
			
			currentChild = currentChild->next;
		}

		<!-- FIXME: standardize string handling on xmlString. this should be deleted somewhere, and checked... -->
		if (child == NULL) {
			fprintf(stderr,"WARNING: no %s child element in %s element\n", "<xsl:value-of select="@name"/>", (const char *)node->name );
			<xsl:value-of select="@name"/> = strdupx("[undefined]");
		} else {
			xmlDocPtr out = xmlNewDoc((const xmlChar*)"1.0");
			out->xmlRootNode = xmlCopyNode( child, 1 );
			
			char *data; int size;
			xmlDocDumpFormatMemory( out, (xmlChar**)&amp;data, &amp;size, 1 );
			<xsl:value-of select="@name"/> = data;
		}
	}
</xsl:template>

<xsl:template match="context" mode="parsexml">
	ctx-><xsl:value-of select="@param"/> = <xsl:value-of select="@value"/>;
</xsl:template>

</xsl:stylesheet>
