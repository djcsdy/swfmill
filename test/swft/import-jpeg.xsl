<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:swft="http://subsignal.org/swfml/swft"
    extension-element-prefixes="swft">
  <xsl:template match="/">
    <out>
      <xsl:apply-templates mode="copy"
          select="swft:import-jpeg('test.jpg')"/>
    </out>
  </xsl:template>

  <xsl:template mode="copy" match="node()">
    <xsl:copy>
      <xsl:apply-templates mode="copy" select="node()"/>
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>
