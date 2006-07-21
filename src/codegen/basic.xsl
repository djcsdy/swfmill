<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>

<xsl:template match="byte" mode="ctype">unsigned char</xsl:template>
<xsl:template match="word" mode="ctype">unsigned short</xsl:template>
<xsl:template match="byteOrWord" mode="ctype">unsigned short</xsl:template>
<xsl:template match="string" mode="ctype">char *</xsl:template>
<xsl:template match="fixedpoint" mode="ctype">float</xsl:template>
<xsl:template match="fixedpoint2" mode="ctype">float</xsl:template>
<xsl:template match="bit" mode="ctype">bool</xsl:template>
<xsl:template match="integer" mode="ctype">int</xsl:template>
<xsl:template match="object" mode="ctype"><xsl:value-of select="@type"/></xsl:template>
<xsl:template match="list" mode="ctype">List&lt;<xsl:value-of select="@type"/>&gt;</xsl:template>
<xsl:template match="data" mode="ctype">unsigned char *</xsl:template>
<xsl:template match="uint32" mode="ctype">unsigned int</xsl:template>
<xsl:template match="float" mode="ctype">float</xsl:template>
<xsl:template match="double" mode="ctype">double</xsl:template>
<xsl:template match="xml" mode="ctype">char *</xsl:template>

<xsl:template mode="printf" match="byte">%i</xsl:template>
<xsl:template mode="printf" match="word">%i</xsl:template>
<xsl:template mode="printf" match="byteOrWord">%i</xsl:template>
<xsl:template mode="printf" match="string">%s</xsl:template>
<xsl:template mode="printf" match="fixedpoint">%G</xsl:template>
<xsl:template mode="printf" match="fixedpoint2">%G</xsl:template>
<xsl:template mode="printf" match="bit">%i</xsl:template>
<xsl:template mode="printf" match="integer">%i</xsl:template>
<xsl:template mode="printf" match="object|list|data">%p</xsl:template>
<xsl:template mode="printf" match="uint32">%i</xsl:template>
<xsl:template mode="printf" match="float|double">%g</xsl:template>
<xsl:template mode="printf" match="xml">%s</xsl:template>

<xsl:template mode="default" match="byte|word|byteOrWord|fixedpoint|fixedpoint2|bit|integer">0</xsl:template>
<xsl:template mode="default" match="uint32|float|double">0</xsl:template>
<xsl:template mode="default" match="string|data|xml">NULL</xsl:template>

</xsl:stylesheet>
