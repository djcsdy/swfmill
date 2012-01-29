<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:swft="http://subsignal.org/swfml/swft"
    extension-element-prefixes="swft">
  <xsl:template match="/">
    <out>
      <xsl:value-of select="swft:import-png('test.png')"/>
    </out>
  </xsl:template>
</xsl:stylesheet>
