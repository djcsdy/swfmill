<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>
	<xsl:template name="dumper">
<xsl:document href="g{/format/@format}Dumper.cpp" method="text">

#include "<xsl:value-of select="/format/@format"/>.h"

namespace <xsl:value-of select="/format/@format"/> {

void print_indent( int n ) {
	for( int i=0; i&lt;n; i++ ) printf("  ");
}

<xsl:for-each select="type|tag|action|filter|style|stackitem|namespaceconstant|multinameconstant|trait|opcode">
void <xsl:value-of select="@name"/>::dump( int indent, Context *ctx ) {
	print_indent(indent);
	<xsl:if test="@id">
		printf("[%02X] ", type ); //<xsl:value-of select="@id"/>);
	</xsl:if>
	
	printf("<xsl:value-of select="@name"/>"); // (sz %i)", getSize( ctx ) );
	
	printf("\n");
	indent++;
	<xsl:apply-templates select="*" mode="dump-wrap"/>
}
</xsl:for-each>

}

</xsl:document>
	</xsl:template>


<xsl:template match="flagged" mode="dump-wrap">
	if( <xsl:if test="@negative">!</xsl:if><xsl:value-of select="@flag"/>
		<xsl:if test="@signifier"> &amp; <xsl:value-of select="@signifier"/></xsl:if> ) {
		
		<xsl:apply-templates select="*" mode="dump-wrap"/>
	}
</xsl:template>

<xsl:template match="if" mode="dump-wrap">
	if( <xsl:value-of select="@expression"/> ) {
		<xsl:apply-templates select="*" mode="dump-wrap"/>
	}
</xsl:template>

<xsl:template match="fill-byte|context" mode="dump-wrap"/>

<xsl:template match="*" mode="dump-wrap" priority="-1">
	print_indent(indent);
	printf("%s: ", "<xsl:value-of select="@name"/>"); <xsl:apply-templates select="." mode="dump"/>
</xsl:template>



<xsl:template match="byte|word|byteOrWord|integer|fixedpoint|fixedpoint2|bit|u30|s24" mode="dump">
	printf("<xsl:apply-templates select="." mode="printf"/>\n", <xsl:value-of select="@name"/>);
</xsl:template>

<xsl:template match="string" mode="dump">
	printf("<xsl:apply-templates select="." mode="printf"/>\n", <xsl:value-of select="@name"/>?<xsl:value-of select="@name"/>:"(nil)");
</xsl:template>

<xsl:template match="object" mode="dump">
	printf("[%s]\n", "<xsl:value-of select="@type"/>");
	<xsl:value-of select="@name"/>.dump( indent+1, ctx );
</xsl:template>

<xsl:template match="list" mode="dump">
	{
		printf("[list of %ss]\n", "<xsl:value-of select="@type"/>");
		<xsl:value-of select="@type"/> *item;
		ListItem&lt;<xsl:value-of select="@type"/>&gt;* i;
		i = <xsl:value-of select="@name"/>.first();
		while( i ) {
			item = i->data();
			if( item ) {
				item->dump(indent+1,ctx);
			}
			i = i->next();
		}
	}
</xsl:template>

<xsl:template match="data" mode="dump">
	printf("(length %i)\n", <xsl:value-of select="@size"/> );
	if( <xsl:value-of select="@size"/> &amp;&amp; <xsl:value-of select="@name"/> != NULL ) {
		int i=0;
		while( i&lt;<xsl:value-of select="@size"/> ) {
			print_indent( indent+1 );
			for( int n=0; n&lt;8 &amp;&amp; i&lt;<xsl:value-of select="@size"/>; n++ ) {
				printf(" %02X",  <xsl:value-of select="@name"/>[i] );
				i++;
			}
			printf("\n");
		}
	}
</xsl:template>


<xsl:template match="bytealign|context" mode="dump"/>

<xsl:template match="context" mode="dump-wrap">
	ctx-><xsl:value-of select="@param"/> = <xsl:value-of select="@value"/>;
</xsl:template>

</xsl:stylesheet>
