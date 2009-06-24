<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>
	<xsl:template name="size">
<xsl:document href="g{/format/@format}Size.cpp" method="text">

#include "<xsl:value-of select="/format/@format"/>.h"
#include &lt;cstring&gt;

namespace <xsl:value-of select="/format/@format"/> {

<xsl:for-each select="tag|action|filter|style|stackitem|namespaceconstant|multinameconstant|trait|opcode">
size_t <xsl:value-of select="@name"/>::calcSize( Context *ctx, int start_at ) {
	size_t r = start_at;
	<xsl:apply-templates select="*[@context]" mode="size-context"/>
	<xsl:apply-templates select="*" mode="size"/>

	r += <xsl:apply-templates select="." mode="baseclass"/>::getHeaderSize( r-start_at );
//	printf("<xsl:value-of select="@name"/> sz %i bits (%.2f bytes, start %i)\n", r-start_at, ((float)r-start_at)/8, start_at );

	return r-start_at;
}
</xsl:for-each>

<xsl:for-each select="type">
size_t <xsl:value-of select="@name"/>::calcSize( Context *ctx, int start_at ) {
	size_t r = start_at;

	r += <xsl:apply-templates select="." mode="baseclass"/>::getHeaderSize( r-start_at );

	<xsl:apply-templates select="*[@context]" mode="size-context"/>
	<xsl:apply-templates select="*" mode="size"/>

	<xsl:for-each select="*[@set-from-bits-needed]">
		<xsl:value-of select="@name"/> = SWFBitsNeeded( <xsl:value-of select="@set-from-bits-needed"/> );
		ctx-><xsl:value-of select="@name"/> = <xsl:value-of select="@name"/>;
	</xsl:for-each>

	<xsl:for-each select="*[@set-from]">
		<xsl:value-of select="@name"/> =<xsl:value-of select="@set-from"/>;
	</xsl:for-each>

//	printf("<xsl:value-of select="@name"/> sz %i bits (%.2f bytes, start %i)\n", r-start_at, ((float)r-start_at)/8, start_at );

	return r-start_at;
}
</xsl:for-each>

}

</xsl:document>
	</xsl:template>

<xsl:template match="*[@context]" mode="size-context">
	ctx-><xsl:value-of select="@name"/> = <xsl:value-of select="@name"/>;
</xsl:template>

<xsl:template match="flagged" mode="size" priority="1">
	if( <xsl:if test="@negative">!</xsl:if><xsl:value-of select="@flag"/>
		<xsl:if test="@signifier"> &amp; <xsl:value-of select="@signifier"/></xsl:if> ) {
		
		<xsl:apply-templates select="*" mode="size"/>
	}
</xsl:template>

<xsl:template match="if" mode="size" priority="1">
	if( <xsl:value-of select="@expression"/> ) {
		<xsl:apply-templates select="*" mode="size"/>
	}
</xsl:template>

<xsl:template match="byte" mode="size">
	r += 8;
</xsl:template>

<xsl:template match="encodedu32" mode="size">
    {
         unsigned int i = <xsl:value-of select="@name"/>;
	 do {
	     r += 8;
	 } while ((i>>=7) > 0);
    }
</xsl:template>

<xsl:template match="byteOrWord" mode="size">
    if( <xsl:value-of select="@name"/> &gt;= 255 ) {
        r += 24;
    } else {
    	r += 8;
    }
</xsl:template>

<xsl:template match="word" mode="size">
	r += 16;
</xsl:template>
<xsl:template match="uint32" mode="size">
	r += 32;
</xsl:template>
<xsl:template match="float" mode="size">
	r += 32;
</xsl:template>
<xsl:template match="double|double2" mode="size">
	r += 64;
</xsl:template>
<xsl:template match="half" mode="size">
	r += 16;
</xsl:template>

<xsl:template match="integer" mode="size">
	r += <xsl:value-of select="@size"/><xsl:if test="@size-add">+<xsl:value-of select="@size-add"/></xsl:if>;
</xsl:template>

<xsl:template match="fixedpoint|fixedpoint2" mode="size">
	r += <xsl:value-of select="@size"/>;
</xsl:template>

<xsl:template match="bit" mode="size">
	r++;
</xsl:template>

<xsl:template match="string" mode="size">
	{
		int bytes = 1;
		<xsl:if test="@mode='pascalU30'">
			if( <xsl:value-of select="@name"/> ) {
				int len = strlen( <xsl:value-of select="@name"/> );
				uint32_t limit = 0x80;
				for(; len > limit - 1; limit *= 0x80) {
					bytes++;
				}
			}
		</xsl:if>
		r += ((<xsl:value-of select="@name"/> ? strlen( <xsl:value-of select="@name"/> ) : 0)+bytes)*8;
	}
</xsl:template>

<xsl:template match="xml" mode="size">
	r += ((<xsl:value-of select="@name"/> ? strlen( <xsl:value-of select="@name"/> ) : 0)+1)*8;
</xsl:template>

<xsl:template match="object" mode="size">
	r += <xsl:value-of select="@name"/>.getSize(ctx,r);
</xsl:template>

<xsl:template match="list" mode="size">
	{
		<xsl:value-of select="@type"/>* item;
		ListItem&lt;<xsl:value-of select="@type"/>&gt;* i;
		i = <xsl:value-of select="@name"/>.first();
		while( i ) {
			item = i->data();
			if( item ) {
				r += item->getSize(ctx,r);
			}
			i = i->next();
		}
	}
</xsl:template>

<xsl:template match="data" mode="size">
	r += <xsl:value-of select="@size"/> * 8;
</xsl:template>


<xsl:template match="fill-byte" mode="size">
	if( r%8 != 0 ) r += 8-(r%8);
</xsl:template>

<xsl:template match="u30" mode="size">
	{
		int bytes = 1;
	
		uint32_t limit = 0x80;
		for(; <xsl:value-of select="@name"/> > limit - 1; limit *= 0x80) {
			bytes++;
		}

		r += bytes * 8;
	}
</xsl:template>

<xsl:template match="s24" mode="size">
	r += 24;
</xsl:template>

<xsl:template match="context" mode="size">
	ctx-><xsl:value-of select="@param"/> = <xsl:value-of select="@value"/>;
</xsl:template>

</xsl:stylesheet>
