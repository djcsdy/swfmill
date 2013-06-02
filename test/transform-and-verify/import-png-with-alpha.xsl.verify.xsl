<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="/">
    <xsl:if test="not(/out/png)">
      <xsl:message terminate="yes">
        /out/png element is missing.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/png/@name != 'test-alpha'">
      <xsl:message terminate="yes">
        /out/png/@name attribute is missing or has the wrong value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/png/@width != 5">
      <xsl:message terminate="yes">
        /out/png/@width attribute is missing or has the wrong value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/png/@height != 5">
      <xsl:message terminate="yes">
        /out/png/@height attribute is missing or has the wrong value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/png/@format != 5">
      <xsl:message terminate="yes">
        /out/png/@format attribute is missing or has the wrong value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="not(/out/png/data)">
      <xsl:message terminate="yes">
        /out/png/data element is missing.
      </xsl:message>
    </xsl:if>

    <xsl:if test="string(/out/png/data/text()) != 'eNpTVWBm3s/AwPDjurg4iFYF8TuXCf4Hsv9vvycBokH8H+LXU/5L3EuHYRAfpP6/RPo9GAbrl2ZWAJsH0gflAwBaZyQw'">
      <xsl:message terminate="yes">
        PNG data was not imported correctly.
      </xsl:message>
    </xsl:if>
  </xsl:template>
</xsl:stylesheet>
