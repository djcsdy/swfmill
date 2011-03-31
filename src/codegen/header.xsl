<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common" version='1.0'>
	<xsl:template name="header">
<xsl:document href="{/format/@format}.h" method="text">
#ifndef <xsl:value-of select="/format/@format"/>_H
#define <xsl:value-of select="/format/@format"/>_H

#include &lt;<xsl:value-of select="/format/@format"/>Reader.h&gt;
#include &lt;<xsl:value-of select="/format/@format"/>Tag.h&gt;
#include &lt;<xsl:value-of select="/format/@format"/>Action.h&gt;
#include &lt;<xsl:value-of select="/format/@format"/>Filter.h&gt;
#include &lt;<xsl:value-of select="/format/@format"/>Trait.h&gt;
#include &lt;<xsl:value-of select="/format/@format"/>List.h&gt;
#include &lt;<xsl:value-of select="/format/@format"/>ShapeItem.h&gt;
#include &lt;<xsl:value-of select="/format/@format"/>GlyphList.h&gt;
#include &lt;<xsl:value-of select="/format/@format"/>IdItems.h&gt;
#include &lt;<xsl:value-of select="/format/@format"/>OpCode.h&gt;

namespace <xsl:value-of select="/format/@format"/> {

struct Context {
	public:
		Context();
	
		int swfVersion;
		bool transientPropsToXML;
		bool debugTrace;
		bool exportAssets;
		bool quiet;
		
		bool isLast;
		char tagVersion;
		bool alpha;
		bool many_shapes;

		bool convertEncoding;
		const char *swf_encoding;
		
		<xsl:for-each select="contextvariable">
			<xsl:variable name="tmp">
				<xsl:element name="{@type}"/>
			</xsl:variable>
			<xsl:apply-templates select="exslt:node-set($tmp)" mode="ctype"/>
			<xsl:text> </xsl:text>
			<xsl:value-of select="@name"/>;
		</xsl:for-each>
};

<xsl:for-each select="type|tag|action|filter|style|stackitem|namespaceconstant|multinameconstant|trait|opcode">
class <xsl:value-of select="@name"/> : public <xsl:apply-templates select="." mode="baseclass"/> {
	public:
		<xsl:value-of select="@name"/>();
		virtual ~<xsl:value-of select="@name"/>();
		
		virtual bool parse( Reader *r, int end, Context *ctx );
		virtual void dump( int indent, Context *ctx );
		virtual size_t calcSize( Context *ctx, int start_at );
		virtual void write( Writer *w, Context *ctx );
		virtual void writeXML( xmlNodePtr xml, Context *ctx );
		virtual void parseXML( xmlNodePtr xml, Context *ctx );
		
		static <xsl:value-of select="@name"/>* get( Reader *r, int end, Context *ctx );
		static <xsl:value-of select="@name"/>* getByName( const char *name );

		<xsl:apply-templates mode="declareAccessors"/>

	protected:
		<xsl:apply-templates mode="declare"/>
		
		<xsl:if test="@id">
			static int id;
		</xsl:if>
};
</xsl:for-each>

}
#endif
</xsl:document>
	</xsl:template>

<!-- definitions -->
	<xsl:template match="flagged|if" mode="declare">
		<xsl:apply-templates mode="declare"/>
	</xsl:template>
	<xsl:template match="fill-byte|context" mode="declare"/>

	<xsl:template match="*" mode="declare" priority="-1">
			<xsl:apply-templates mode="ctype" select="."/><xsl:text> </xsl:text><xsl:value-of select="@name"/>; // <xsl:value-of select="name()"/><xsl:text>
</xsl:text>
	</xsl:template>

<!-- accessor definitions -->
	<xsl:template match="flagged|if" mode="declareAccessors">
		<xsl:apply-templates mode="declareAccessors"/>
	</xsl:template>
	<xsl:template match="fill-byte|context" mode="declareAccessors"/>
	<xsl:template match="byte|word|byteOrWord|string|fixedpoint|fixedpoint2|bit|integer|uint32|float|double|double2|half|u30|s24|encodedu32" mode="declareAccessors" priority="-1">
			<xsl:apply-templates mode="ctype" select="."/><xsl:text> </xsl:text>get<xsl:value-of select="@name"/>();
			void set<xsl:value-of select="@name"/>( <xsl:apply-templates mode="ctype" select="."/> );
	</xsl:template>
	<xsl:template match="xml" mode="declareAccessors" priority="-1">
			<xsl:apply-templates mode="ctype" select="."/><xsl:text> </xsl:text>get<xsl:value-of select="@name"/>();
			void set<xsl:value-of select="@name"/>( <xsl:apply-templates mode="ctype" select="."/> );
	</xsl:template>
	<xsl:template match="data" mode="declareAccessors" priority="-1">
			void get<xsl:value-of select="@name"/>( unsigned char **dst, int *size );
			void set<xsl:value-of select="@name"/>( const unsigned char *src, int size );
	</xsl:template>
	<xsl:template match="object" mode="declareAccessors" priority="-1">
			<xsl:apply-templates mode="ctype" select="."/> *get<xsl:value-of select="@name"/>();
	</xsl:template>
	<xsl:template match="list" mode="declareAccessors" priority="-1">
			<xsl:apply-templates mode="ctype" select="."/> *get<xsl:value-of select="@name"/>();
	</xsl:template>

<!-- base classes -->
	<xsl:template match="type" mode="baseclass">
		<xsl:choose>
			<xsl:when test="@parent">
				<xsl:value-of select="@parent"/>
			</xsl:when>
			<xsl:otherwise>Item</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<xsl:template match="tag" mode="baseclass">Tag</xsl:template>
	<xsl:template match="action" mode="baseclass">Action</xsl:template>
	<xsl:template match="filter" mode="baseclass">Filter</xsl:template>
	<xsl:template match="style" mode="baseclass">Style</xsl:template>
	<xsl:template match="stackitem" mode="baseclass">StackItem</xsl:template>
	<xsl:template match="namespaceconstant" mode="baseclass">NamespaceConstant</xsl:template>
	<xsl:template match="multinameconstant" mode="baseclass">MultinameConstant</xsl:template>
	<xsl:template match="trait" mode="baseclass">Trait</xsl:template>
	<xsl:template match="opcode" mode="baseclass">OpCode</xsl:template>

</xsl:stylesheet>
