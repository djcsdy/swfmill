<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
				version='1.0'>

<xsl:output method="xml" indent="yes"/>

<xsl:template match="compilation">
	<xsl:element name="xsl:stylesheet">
		<xsl:attribute name="namespaces">hack</xsl:attribute>
		<xsl:attribute name="extension-element-prefixes">swft</xsl:attribute>
		<xsl:attribute name="version">1.0</xsl:attribute>
		<xsl:text>
</xsl:text>
	
		<xsl:element name="xsl:output">
			<xsl:attribute name="method">xml</xsl:attribute>
			<xsl:attribute name="indent">yes</xsl:attribute>
		</xsl:element>

		<xsl:element name="xsl:param">
			<xsl:attribute name="name">quiet</xsl:attribute>
			<xsl:attribute name="select">'false'</xsl:attribute>
		</xsl:element>

		<xsl:apply-templates/>
		
	</xsl:element>
</xsl:template>

<xsl:template match="include">
	<xsl:for-each select="document(@src)/xsl:stylesheet/*">
		<xsl:text>
</xsl:text>
		<xsl:copy-of select="."/> 
	</xsl:for-each>
</xsl:template>

</xsl:stylesheet>
