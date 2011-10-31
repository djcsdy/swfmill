<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>
	<xsl:template name="writer">
<xsl:document href="g{/format/@format}Writer.cpp" method="text">

#include "<xsl:value-of select="/format/@format"/>.h"
#include "<xsl:value-of select="/format/@format"/>Item.h"
#include "<xsl:value-of select="/format/@format"/>List.h"

namespace <xsl:value-of select="/format/@format"/> {

<xsl:for-each select="type|tag|action|filter|style|stackitem|namespaceconstant|multinameconstant|trait|opcode">
//////////////////////////////////// <xsl:value-of select="@name"/>

void <xsl:value-of select="@name"/>::write( Writer *w, Context *ctx ) {
	int start=w->getBitPosition();
	if( ctx->debugTrace ) {
		printf("WRITE %s @%i\n", "<xsl:value-of select="@name"/>", start/8 );
		dump(1,ctx);
	}
<!--
	<xsl:for-each select="*[@set-from-bits-needed]">
		<xsl:value-of select="@name"/> = SWFBitsNeeded( <xsl:value-of select="@set-from-bits-needed"/> );
		if( <xsl:value-of select="@name"/> > ctx-><xsl:value-of select="@name"/> ) ctx-><xsl:value-of select="@name"/> = <xsl:value-of select="@name"/>;
	</xsl:for-each>
-->
	<!-- calculate "next" offsets for some other lists-->
	<xsl:for-each select="*[@next]">
		if( !ctx->isLast ) {
			int r = 0;
			<xsl:apply-templates select="following-sibling::*" mode="size"/>
			<xsl:value-of select="@name"/> = (r/8)<xsl:if test="@next-offset"> - (<xsl:value-of select="@next-offset"/>)</xsl:if>;
		} else {
			<xsl:value-of select="@name"/> = 0;
		}
	</xsl:for-each>

	<xsl:choose>
		<xsl:when test="@parent">
			<xsl:apply-templates select="." mode="baseclass"/>::writeHeader( w, ctx, getSize(ctx,w->getBitPosition())/8 );
		</xsl:when>
		<xsl:when test="name()='type'"/>
		<xsl:otherwise>
			<xsl:apply-templates select="." mode="baseclass"/>::writeHeader( w, ctx, (getSize(ctx,w->getBitPosition())/8) <xsl:if test="@swallow"> - <xsl:value-of select="@swallow"/></xsl:if> );
		</xsl:otherwise>
	</xsl:choose>
	
	<xsl:apply-templates mode="write"/>

	int l = w->getBitPosition()-start;
	if( l != getSize(ctx,start)) {
		printf("WARNING: %s has different size than expected: %i bits instead of %i\n",
				"<xsl:value-of select="@name"/>",
				l,
				getSize(ctx,start) );
	}

}

</xsl:for-each>

}

</xsl:document>
	</xsl:template>


<xsl:template match="flagged" mode="write">
	if( <xsl:if test="@negative">!</xsl:if><xsl:value-of select="@flag"/>
		<xsl:if test="@signifier"> &amp; <xsl:value-of select="@signifier"/></xsl:if> ) {
	
		<xsl:apply-templates mode="write"/>
	}
</xsl:template>

<xsl:template match="if" mode="write">
	if( <xsl:value-of select="@expression"/> ) {
		<xsl:apply-templates mode="write"/>
	}
</xsl:template>

<xsl:template match="byte|word|fixedpoint|fixedpoint2|bit|integer|string|uint32|float|double|double2|half|xml|u30|s24|encodedu32" mode="write">
	<xsl:apply-templates select="." mode="put"/>;
	<xsl:if test="@context">
		ctx-&gt;<xsl:value-of select="@name"/> = <xsl:value-of select="@name"/>;
	</xsl:if>
</xsl:template>
<xsl:template match="byte" mode="put">w->putByte(<xsl:value-of select="@name"/>)</xsl:template>
<xsl:template match="word" mode="put">w->putWord(<xsl:value-of select="@name"/>)</xsl:template>
<xsl:template match="uint32" mode="put">w->putInt(<xsl:value-of select="@name"/>)</xsl:template>
<xsl:template match="float" mode="put">w->putFloat(<xsl:value-of select="@name"/>)</xsl:template>
<xsl:template match="double" mode="put">w->putDouble(<xsl:value-of select="@name"/>)</xsl:template>
<xsl:template match="double2" mode="put">w->putDouble2(<xsl:value-of select="@name"/>)</xsl:template>
<xsl:template match="half" mode="put">w->putHalf(<xsl:value-of select="@name"/>)</xsl:template>
<xsl:template match="fixedpoint" mode="put">w->putNBitFixed(<xsl:value-of select="@name"/>,<xsl:value-of select="@size"/>,<xsl:value-of select="@exp"/><xsl:if test="@signed">,true</xsl:if>)</xsl:template>
<xsl:template match="fixedpoint2" mode="put">w->putNBitFixed2(<xsl:value-of select="@name"/>,<xsl:value-of select="@size"/>,<xsl:value-of select="@exp"/><xsl:if test="@signed">,true</xsl:if>)</xsl:template>
<xsl:template match="bit" mode="put">w->putNBitInt(<xsl:value-of select="@name"/>,1)</xsl:template>
<xsl:template match="integer" mode="put">w->putNBitInt(<xsl:value-of select="@name"/>,<xsl:value-of select="@size"/><xsl:if test="@size-add">+<xsl:value-of select="@size-add"/></xsl:if><xsl:if test="@signed">,true</xsl:if>)</xsl:template>
<xsl:template match="string[@mode='pascal']" mode="put">w->putPString(<xsl:value-of select="@name"/>)</xsl:template>
<xsl:template match="string[@mode='pascalU30']" mode="put">w->putPStringU30(<xsl:value-of select="@name"/>)</xsl:template>
<xsl:template match="string" mode="put" priority="-1">w->putString(<xsl:value-of select="@name"/>)</xsl:template>
<xsl:template match="data" mode="put">w->putData(<xsl:value-of select="@name"/>,<xsl:value-of select="@size"/>)</xsl:template>
<xsl:template match="xml" mode="put" priority="-1">w->putString(<xsl:value-of select="@name"/>)</xsl:template>
<xsl:template match="u30" mode="put">w->putU30(<xsl:value-of select="@name"/>)</xsl:template>
<xsl:template match="s24" mode="put">w->putS24(<xsl:value-of select="@name"/>)</xsl:template>
<xsl:template match="encodedu32" mode="put">w->putEncodedU32(<xsl:value-of select="@name"/>)</xsl:template>

<xsl:template match="byteOrWord" mode="write">
    {
        int v = <xsl:value-of select="@name"/>;
        if( v >= 255  &amp;&amp; ctx->tagVersion >= 2 ) {
            w->putByte( 255 );
            w->putWord( v );
        } else {
            w->putByte( v );
        }
    }

	<xsl:if test="@context">
		ctx-&gt;<xsl:value-of select="@name"/> = <xsl:value-of select="@name"/>;
	</xsl:if>
</xsl:template>

<xsl:template match="object" mode="write">
	<xsl:value-of select="@name"/>.write(w,ctx);
</xsl:template>

<xsl:template match="list[@length]" mode="write" priority="1">
	<xsl:variable name="length">
		<xsl:choose>
			<xsl:when test="@length-add">
				( <xsl:value-of select="@length"/> + <xsl:value-of select="@length-add"/> )
			</xsl:when>
			<xsl:when test="@length-sub">
				( <xsl:value-of select="@length"/> &lt; <xsl:value-of select="@length-sub"/>
				  ? 0
				  : <xsl:value-of select="@length"/> - <xsl:value-of select="@length-sub"/> )
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="@length"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	{
		<xsl:value-of select="@type"/> *item;
		ListItem&lt;<xsl:value-of select="@type"/>&gt;* i;
		i = <xsl:value-of select="@name"/>.first();
		for( int j=0; j&lt;<xsl:value-of select="$length"/>; j++ ) {
			if( !i || !i->data() ) {
				printf("WARNING: list %s has %i items (%i expected)\n",
						"<xsl:value-of select="@name"/>",
						j,
						<xsl:value-of select="$length"/> );
			} else {
				i->data()->write( w, ctx );
			}
			i = i->next();
		}
	}
</xsl:template>

<xsl:template match="list" mode="write" priority="-1">
	{
		int start = w->getPosition();
		<xsl:value-of select="@type"/> *item;
		ListItem&lt;<xsl:value-of select="@type"/>&gt;* i;
		i = <xsl:value-of select="@name"/>.first();
		while( i ) {
			if( !i->data() ) {
				printf("WARNING: list %s has an invalid item\n",
						"<xsl:value-of select="@name"/>" );
			} else {
				ctx->isLast = i->isLast();
				i->data()->write( w, ctx );
			}
			i = i->next();
		}
	}
</xsl:template>

<xsl:template match="data" mode="write">
	if( <xsl:value-of select="@size"/> &amp;&amp; <xsl:value-of select="@name"/> ) {
		w->putData( <xsl:value-of select="@name"/>, <xsl:value-of select="@size"/> );
	}
	w->byteAlign();
</xsl:template>

<xsl:template match="fill-byte" mode="write">
	w->byteAlign();
</xsl:template>

<xsl:template match="context" mode="write">
	ctx-><xsl:value-of select="@param"/> = <xsl:value-of select="@value"/>;
</xsl:template>

</xsl:stylesheet>
