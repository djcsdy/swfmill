<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="/">
    <xsl:if test="string(.) != 'eNpTVWBm3s/AwPDjurg4iFYF8TuXCf4Hsv9vvycBokH8H+LXU/5L3EuHYRAfpP6/RPo9GAbrl2ZWAJsH0gflAwBaZyQw'">
      <xsl:message terminate="yes">
        PNG data was not imported correctly.
      </xsl:message>
    </xsl:if>
  </xsl:template>
</xsl:stylesheet>
