<?xml version="1.0"?>
<axsl:stylesheet xmlns:axsl="http://www.w3.org/1999/XSL/Transform"
				xmlns:xsl="http://swfmill.org/ns/xsl-alias"
				exclude-result-prefixes="axsl"
				version='1.0'>

	<axsl:output method="xml" indent="yes"/>

	<axsl:namespace-alias stylesheet-prefix="xsl" result-prefix="axsl"/>

	<axsl:template match="compilation">
		<xsl:stylesheet xmlns:swft="http://subsignal.org/swfml/swft"
				version="1.0" extension-element-prefixes="swft">
			<xsl:output method="xml" indent="yes"/>
			<xsl:param name="quiet" select="'false'"/>
			<axsl:apply-templates/>
		</xsl:stylesheet>
	</axsl:template>

	<axsl:template match="include">
		<axsl:for-each select="document(@src)/axsl:stylesheet/*">
			<axsl:text>&#xa;</axsl:text>
			<axsl:copy-of select="."/>
		</axsl:for-each>
	</axsl:template>

</axsl:stylesheet>
