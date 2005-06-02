<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>
				
	<xsl:include href="basic.xsl"/>
	<xsl:include href="basics.xsl"/>
	<xsl:include href="header.xsl"/>
	<xsl:include href="parser.xsl"/>
	<xsl:include href="dumper.xsl"/>
	<xsl:include href="size.xsl"/>
	<xsl:include href="writer.xsl"/>
	<xsl:include href="writexml.xsl"/>
	<xsl:include href="parsexml.xsl"/>
	<xsl:strip-space elements="*"/>

	<xsl:template match="format">
		<xsl:call-template name="basics"/>
		<xsl:call-template name="header"/>
		<xsl:call-template name="parser"/>
		<xsl:call-template name="dumper"/>
		<xsl:call-template name="size"/>
		<xsl:call-template name="writer"/>
		<xsl:call-template name="writexml"/>
		<xsl:call-template name="parsexml"/>
	</xsl:template>

</xsl:stylesheet>
