<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>

<xsl:template match="list|object|fill-byte|context" mode="ctor"/>
<xsl:template match="flagged|if" mode="ctor">
	<xsl:apply-templates mode="ctor"/>
</xsl:template>

<xsl:template match="*" mode="ctor" priority="-1">
		<xsl:value-of select="@name"/> = <xsl:choose>
			<xsl:when test="@default"><xsl:value-of select="@default"/></xsl:when>
			<xsl:otherwise><xsl:apply-templates select="." mode="default"/></xsl:otherwise>
		</xsl:choose>;
</xsl:template>

<xsl:template match="string|data" mode="dtor">
	delete[] <xsl:value-of select="@name"/>;
</xsl:template>

<xsl:template name="objectList">
	<xsl:param name="class"/>
	<xsl:param name="itemName"/>

	<xsl:variable name="tag">
		<xsl:value-of select="translate($class,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')"/>
	</xsl:variable>

	<xsl:for-each select="*[name() = $tag]">
		<xsl:value-of select="$class"/> *<xsl:value-of select="@name"/>Factory() {
			return (<xsl:value-of select="$class"/>*)new <xsl:value-of select="@name"/>;
		}
	</xsl:for-each>

	template&lt;&gt; IdItem&lt;<xsl:value-of select="$class"/>&gt;::Description IdItem&lt;<xsl:value-of select="$class"/>&gt;::Registry[] = {
	<xsl:for-each select="*[name() = $tag]">
			{ <xsl:value-of select="@id"/>, "<xsl:value-of select="@name"/>", <xsl:value-of select="@name"/>Factory },
	</xsl:for-each>
		{ 0, NULL }
	};
	template&lt;&gt; int IdItem&lt;<xsl:value-of select="$class"/>&gt;::nRegistered = <xsl:value-of select="count(*[name() = $tag])"/>;

	template&lt;&gt; const char* IdItem&lt;<xsl:value-of select="$class"/>&gt;::itemName = "<xsl:value-of select="$itemName"/>";

</xsl:template>

<!-- *************** -->

	<xsl:template name="basics">
<xsl:document href="g{/format/@format}Basics.cpp" method="text">

#include "<xsl:value-of select="/format/@format"/>.h"
#include "<xsl:value-of select="/format/@format"/>Tag.h"
#include &lt;cstring&gt;

namespace <xsl:value-of select="/format/@format"/> {


// ------------ context structure

Context::Context() : swf_encoding(0) {
	swfVersion = 0;
	transientPropsToXML = false;
	debugTrace = false;
	exportAssets = false;
	isLast = false;
	tagVersion = 0;
	quiet = false;
	convertEncoding = false;
	<xsl:apply-templates select="//*[@context]" mode="ctor"/>
}

// ------------ basic object functions

<xsl:for-each select="type|tag|action|filter|style|stackitem|namespaceconstant|multinameconstant|trait|opcode">

<xsl:value-of select="@name"/>::<xsl:value-of select="@name"/>() {
	<xsl:apply-templates mode="ctor"/>
}

<xsl:value-of select="@name"/>::~<xsl:value-of select="@name"/>() {
	<xsl:apply-templates mode="dtor"/>
}

<xsl:value-of select="@name"/>* <xsl:value-of select="@name"/>::get( Reader *r, int end, Context *ctx ) {
	<xsl:value-of select="@name"/>* ret = new <xsl:value-of select="@name"/>;
	ret->parse( r, end, ctx );
	return ret;
}

<xsl:value-of select="@name"/>* <xsl:value-of select="@name"/>::getByName( const char *name ) {
	if( strcmp( name, "<xsl:value-of select="@name"/>" ) ) return NULL;
	<xsl:value-of select="@name"/>* ret = new <xsl:value-of select="@name"/>;
	return ret;
}

<xsl:apply-templates mode="defineAccessors"/>

</xsl:for-each>

// ------------ object lists

<xsl:call-template name="objectList">
	<xsl:with-param name="class">Tag</xsl:with-param>
	<xsl:with-param name="itemName">tag</xsl:with-param>
</xsl:call-template>

<xsl:call-template name="objectList">
	<xsl:with-param name="class">Action</xsl:with-param>
	<xsl:with-param name="itemName">action</xsl:with-param>
</xsl:call-template>

<xsl:call-template name="objectList">
	<xsl:with-param name="class">Filter</xsl:with-param>
	<xsl:with-param name="itemName">filter</xsl:with-param>
</xsl:call-template>

<xsl:call-template name="objectList">
	<xsl:with-param name="class">Style</xsl:with-param>
	<xsl:with-param name="itemName">fill style</xsl:with-param>
</xsl:call-template>

<xsl:call-template name="objectList">
	<xsl:with-param name="class">StackItem</xsl:with-param>
	<xsl:with-param name="itemName">stack item</xsl:with-param>
</xsl:call-template>

<xsl:for-each select="stackitem">
<xsl:if test="@id">
int <xsl:value-of select="@name"/>::id = <xsl:value-of select="@id"/>;
</xsl:if>
</xsl:for-each>

<xsl:call-template name="objectList">
	<xsl:with-param name="class">NamespaceConstant</xsl:with-param>
	<xsl:with-param name="itemName">namespace constant</xsl:with-param>
</xsl:call-template>

<xsl:call-template name="objectList">
	<xsl:with-param name="class">MultinameConstant</xsl:with-param>
	<xsl:with-param name="itemName">multiname constant</xsl:with-param>
</xsl:call-template>

<xsl:call-template name="objectList">
	<xsl:with-param name="class">Trait</xsl:with-param>
	<xsl:with-param name="itemName">trait</xsl:with-param>
</xsl:call-template>

<xsl:call-template name="objectList">
	<xsl:with-param name="class">OpCode</xsl:with-param>
	<xsl:with-param name="itemName">opcode</xsl:with-param>
</xsl:call-template>
}
</xsl:document>
	</xsl:template>


<!-- accessors -->
<xsl:template match="flagged|if" mode="defineAccessors">
	<xsl:apply-templates mode="defineAccessors"/>
</xsl:template>
<xsl:template match="fill-byte|context" mode="defineAccessors"/>
<xsl:template match="byte|word|byteOrWord|bit|integer[@constant-size]|uint32|u30|s24|encodedu32" mode="defineAccessors">
	// Constant Size Primitive
	<xsl:apply-templates mode="ctype" select="."/><xsl:text> </xsl:text>
	<xsl:value-of select="ancestor::*[@name]/@name"/>::get<xsl:value-of select="@name"/>() {
		return <xsl:value-of select="@name"/>;
	}
	
	void <xsl:value-of select="ancestor::*[@name]/@name"/>::set<xsl:value-of select="@name"/>( <xsl:apply-templates mode="ctype" select="."/> v ) {
		<xsl:if test="@size">
			int b = SWFBitsNeeded( v<xsl:if test="@signed">, true</xsl:if> );
			if (b > <xsl:value-of select="@size"/>) {
				printf ("WARNING: '%s::%s' (%i) is too large to fit in %i bits\n", "<xsl:value-of select="ancestor::*[@name]/@name"/>", "<xsl:value-of select="@name"/>", v, <xsl:value-of select="@size"/>);
			}
		</xsl:if>
		<xsl:value-of select="@name"/> = v;
	}
</xsl:template>
<xsl:template match="fixedpoint|fixedpoint2" mode="defineAccessors">
	// Constant Size Primitive
	<xsl:apply-templates mode="ctype" select="."/><xsl:text> </xsl:text>
	<xsl:value-of select="ancestor::*[@name]/@name"/>::get<xsl:value-of select="@name"/>() {
		return <xsl:value-of select="@name"/>;
	}
	
	void <xsl:value-of select="ancestor::*[@name]/@name"/>::set<xsl:value-of select="@name"/>( <xsl:apply-templates mode="ctype" select="."/> v ) {
		<xsl:if test="@size">
			int b = SWFBitsNeeded( v, <xsl:value-of select="@exp"/><xsl:if test="@signed">, true</xsl:if> );
			if (b > <xsl:value-of select="@size"/>) {
				fprintf (stderr, "WARNING: '%s::%s' ("
						"<xsl:apply-templates mode="printf" select="."/>"
						") is too large to fit in %i bits\n",
						"<xsl:value-of select="ancestor::*[@name]/@name"/>",
						"<xsl:value-of select="@name"/>",
						v, <xsl:value-of select="@size"/>);
			}
		</xsl:if>
		<xsl:value-of select="@name"/> = v;
	}
</xsl:template>
<xsl:template match="integer" mode="defineAccessors" priority="-1">
	// NBitInt
	<xsl:apply-templates mode="ctype" select="."/><xsl:text> </xsl:text>
	<xsl:value-of select="ancestor::*[@name]/@name"/>::get<xsl:value-of select="@name"/>() {
		return <xsl:value-of select="@name"/>;
	}
	
	void <xsl:value-of select="ancestor::*[@name]/@name"/>::set<xsl:value-of select="@name"/>( <xsl:apply-templates mode="ctype" select="."/> v ) {
		<xsl:if test="not(@context-size) and not(@constant-size)">
			int b = SWFBitsNeeded( v<xsl:if test="@signed">, true</xsl:if> );
			<xsl:if test="@size-add">b -= <xsl:value-of select="@size-add"/>;</xsl:if>
			if( b > <xsl:value-of select="@size"/> )
				set<xsl:value-of select="@size"/> (b);
		</xsl:if>
		<xsl:value-of select="@name"/> = v;
	}
</xsl:template>
<xsl:template match="string" mode="defineAccessors">
	<xsl:apply-templates mode="ctype" select="."/><xsl:text> </xsl:text>
	<xsl:value-of select="ancestor::*[@name]/@name"/>::get<xsl:value-of select="@name"/>() {
		return <xsl:value-of select="@name"/>;
	}
	
	void <xsl:value-of select="ancestor::*[@name]/@name"/>::set<xsl:value-of select="@name"/>( <xsl:apply-templates mode="ctype" select="."/> v ) {
		delete <xsl:value-of select="@name"/>;
		<xsl:value-of select="@name"/> = v;
	}
</xsl:template>
<xsl:template match="data" mode="defineAccessors">
		void <xsl:value-of select="ancestor::*[@name]/@name"/>::get<xsl:value-of select="@name"/>( unsigned char **dst, int *size ) {
			*dst = <xsl:value-of select="@name"/>;
			*size = <xsl:value-of select="@size"/>;
		}
		void <xsl:value-of select="ancestor::*[@name]/@name"/>::set<xsl:value-of select="@name"/>( const unsigned char *src, int size ) {
			delete <xsl:value-of select="@name"/>;
			<xsl:value-of select="@size"/> = size;
			<xsl:value-of select="@name"/> = new unsigned char[ size ];
			memcpy( <xsl:value-of select="@name"/>, src, size ); 
		}
</xsl:template>
<xsl:template match="object" mode="defineAccessors">
	<xsl:apply-templates mode="ctype" select="."/><xsl:text> *</xsl:text>
	<xsl:value-of select="ancestor::*[@name]/@name"/>::get<xsl:value-of select="@name"/>() {
		return &amp;<xsl:value-of select="@name"/>;
	}
</xsl:template>
<xsl:template match="list" mode="defineAccessors">
	<xsl:apply-templates mode="ctype" select="."/><xsl:text> *</xsl:text>
	<xsl:value-of select="ancestor::*[@name]/@name"/>::get<xsl:value-of select="@name"/>() {
		return &amp;<xsl:value-of select="@name"/>;
	}
</xsl:template>
<xsl:template match="xml" mode="defineAccessors">
	<xsl:apply-templates mode="ctype" select="."/><xsl:text> </xsl:text>
	<xsl:value-of select="ancestor::*[@name]/@name"/>::get<xsl:value-of select="@name"/>() {
		return <xsl:value-of select="@name"/>;
	}
	
	void <xsl:value-of select="ancestor::*[@name]/@name"/>::set<xsl:value-of select="@name"/>( <xsl:apply-templates mode="ctype" select="."/> v ) {
		delete <xsl:value-of select="@name"/>;
		<xsl:value-of select="@name"/> = v;
	}
</xsl:template>

</xsl:stylesheet>
