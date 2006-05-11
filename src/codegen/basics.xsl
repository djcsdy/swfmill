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
	if( <xsl:value-of select="@name"/> ) delete <xsl:value-of select="@name"/>;
</xsl:template>


	<xsl:template name="basics">
<xsl:document href="g{/format/@format}Basics.cpp" method="text">

#include "<xsl:value-of select="/format/@format"/>.h"
#include "<xsl:value-of select="/format/@format"/>Tag.h"
#include &lt;string.h&gt;

namespace <xsl:value-of select="/format/@format"/> {


// ------------ context structure

Context::Context() {
	swfVersion = 0;
	transientPropsToXML = false;
	debugTrace = false;
	exportAssets = false;
	isLast = false;
	tagVersion = 0;
	quiet = false;
	<xsl:apply-templates select="//*[@context]" mode="ctor"/>
}

// ------------ basic object functions

<xsl:for-each select="type|tag|action|style|stackitem">

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

<xsl:for-each select="tag">
Tag *<xsl:value-of select="@name"/>Factory() {
	return (Tag*)new <xsl:value-of select="@name"/>;
}
</xsl:for-each>

Tag::Description Tag::Registry[] = {
<xsl:for-each select="tag">
	{ <xsl:value-of select="@id"/>, "<xsl:value-of select="@name"/>", <xsl:value-of select="@name"/>Factory },
</xsl:for-each>
	{ 0, NULL }
};
int Tag::nRegistered = <xsl:value-of select="count(tag)"/>;



<xsl:for-each select="action">
Action *<xsl:value-of select="@name"/>Factory() {
	return (Action*)new <xsl:value-of select="@name"/>;
}
</xsl:for-each>

Action::Description Action::Registry[] = {
<xsl:for-each select="action">
	{ <xsl:value-of select="@id"/>, "<xsl:value-of select="@name"/>", <xsl:value-of select="@name"/>Factory },
</xsl:for-each>
	{ 0, NULL }
};
int Action::nRegistered = <xsl:value-of select="count(action)"/>;



<xsl:for-each select="style">
Style *<xsl:value-of select="@name"/>Factory() {
	return (Style*)new <xsl:value-of select="@name"/>;
}
</xsl:for-each>

Style::Description Style::Registry[] = {
<xsl:for-each select="style">
	{ <xsl:value-of select="@id"/>, "<xsl:value-of select="@name"/>", <xsl:value-of select="@name"/>Factory },
</xsl:for-each>
	{ 0, NULL }
};
int Style::nRegistered = <xsl:value-of select="count(style)"/>;


<xsl:for-each select="stackitem">
StackItem *<xsl:value-of select="@name"/>Factory() {
	return (StackItem*)new <xsl:value-of select="@name"/>;
}

<xsl:if test="@id">
int <xsl:value-of select="@name"/>::id = <xsl:value-of select="@id"/>;
</xsl:if>
</xsl:for-each>

StackItem::Description StackItem::Registry[] = {
<xsl:for-each select="stackitem">
	{ <xsl:value-of select="@id"/>, "<xsl:value-of select="@name"/>", <xsl:value-of select="@name"/>Factory },
</xsl:for-each>
	{ 0, NULL }
};
int StackItem::nRegistered = <xsl:value-of select="count(stackitem)"/>;

}
</xsl:document>
	</xsl:template>


<!-- accessors -->
<xsl:template match="flagged|if" mode="defineAccessors">
	<xsl:apply-templates mode="defineAccessors"/>
</xsl:template>
<xsl:template match="fill-byte|context" mode="defineAccessors"/>
<xsl:template match="byte|word|byteOrWord|fixedpoint|bit|integer[@constant-size]|uint32" mode="defineAccessors">
	// Constant Size Primitive
	<xsl:apply-templates mode="ctype" select="."/><xsl:text> </xsl:text>
	<xsl:value-of select="ancestor::*[@name]/@name"/>::get<xsl:value-of select="@name"/>() {
		return <xsl:value-of select="@name"/>;
	}
	
	void <xsl:value-of select="ancestor::*[@name]/@name"/>::set<xsl:value-of select="@name"/>( <xsl:apply-templates mode="ctype" select="."/> v ) {
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
		<xsl:value-of select="@name"/> = v;
		<!-- FIXME: this must be done in write, as we have no context here
		int b = SWFBitsNeeded( <xsl:value-of select="@name"/><xsl:if test="@signed">, true</xsl:if> );
		<xsl:if test="@size-add">b -= <xsl:value-of select="@size-add"/>;</xsl:if>
		if( b > <xsl:value-of select="@size"/> ) <xsl:value-of select="@size"/> = b;
		-->
	}
</xsl:template>
<xsl:template match="string" mode="defineAccessors">
	<xsl:apply-templates mode="ctype" select="."/><xsl:text> </xsl:text>
	<xsl:value-of select="ancestor::*[@name]/@name"/>::get<xsl:value-of select="@name"/>() {
		return <xsl:value-of select="@name"/>;
	}
	
	void <xsl:value-of select="ancestor::*[@name]/@name"/>::set<xsl:value-of select="@name"/>( <xsl:apply-templates mode="ctype" select="."/> v ) {
		if( <xsl:value-of select="@name"/> ) delete <xsl:value-of select="@name"/>;
		<xsl:value-of select="@name"/> = v;
	}
</xsl:template>
<xsl:template match="data" mode="defineAccessors">
		void <xsl:value-of select="ancestor::*[@name]/@name"/>::get<xsl:value-of select="@name"/>( unsigned char **dst, int *size ) {
			*dst = <xsl:value-of select="@name"/>;
			*size = <xsl:value-of select="@size"/>;
		}
		void <xsl:value-of select="ancestor::*[@name]/@name"/>::set<xsl:value-of select="@name"/>( const unsigned char *src, int size ) {
			if( <xsl:value-of select="@name"/> ) delete <xsl:value-of select="@name"/>;
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
		if( <xsl:value-of select="@name"/> ) delete <xsl:value-of select="@name"/>;
		<xsl:value-of select="@name"/> = v;
	}
</xsl:template>

</xsl:stylesheet>
