<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>

<xsl:template mode="ctype" match="byte">unsigned char</xsl:template>
<xsl:template mode="ctype" match="word">unsigned short</xsl:template>
<xsl:template mode="ctype" match="byteOrWord">unsigned short</xsl:template>
<xsl:template mode="ctype" match="string">char *</xsl:template>
<xsl:template mode="ctype" match="fixedpoint">float</xsl:template>
<xsl:template mode="ctype" match="fixedpoint2">float</xsl:template>
<xsl:template mode="ctype" match="bit">bool</xsl:template>
<xsl:template mode="ctype" match="integer">int</xsl:template>
<xsl:template mode="ctype" match="object"><xsl:value-of select="@type"/></xsl:template>
<xsl:template mode="ctype" match="list">List&lt;<xsl:value-of select="@type"/>&gt;</xsl:template>
<xsl:template mode="ctype" match="data">unsigned char *</xsl:template>
<xsl:template mode="ctype" match="uint32">unsigned int</xsl:template>
<xsl:template mode="ctype" match="float">float</xsl:template>
<xsl:template mode="ctype" match="double">double</xsl:template>
<xsl:template mode="ctype" match="double2">double</xsl:template>
<xsl:template mode="ctype" match="half">float</xsl:template>
<xsl:template mode="ctype" match="xml">char *</xsl:template>
<xsl:template mode="ctype" match="u30" >unsigned int</xsl:template>
<xsl:template mode="ctype" match="s24" >int</xsl:template>
<xsl:template mode="ctype" match="encodedu32">unsigned int</xsl:template>

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
<xsl:template mode="printf" match="float|double|double2|half">%g</xsl:template>
<xsl:template mode="printf" match="xml">%s</xsl:template>
<xsl:template mode="printf" match="u30">%i</xsl:template>
<xsl:template mode="printf" match="s24">%i</xsl:template>
<xsl:template mode="printf" match="encodedu32">%i</xsl:template>

<xsl:template mode="default" match="byte|word|byteOrWord|fixedpoint|fixedpoint2|bit|integer">0</xsl:template>
<xsl:template mode="default" match="uint32|float|double|double2|half|u30|s24|encodedu32">0</xsl:template>
<xsl:template mode="default" match="string|data|xml">NULL</xsl:template>

</xsl:stylesheet>
