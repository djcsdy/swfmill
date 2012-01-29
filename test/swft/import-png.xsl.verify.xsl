<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="/">
    <xsl:if test="not(/out/png)">
      <xsl:message terminate="yes">
        /out/png element is missing.
      </xsl:message>
    </xsl:if>

    <xsl:if test="not(/out/png/data)">
      <xsl:message terminate="yes">
        /out/png/data element is missing.
      </xsl:message>
    </xsl:if>

    <xsl:if test="string(/out/png/data) != 'eNr7f09C4j8DA8N/ZHr7PQgbmZa4l46BQXIS6ffgGMTfKwGhYRjIBwB+FS9n'">
      <xsl:message terminate="yes">
        PNG data was not imported correctly.
      </xsl:message>
    </xsl:if>
  </xsl:template>
</xsl:stylesheet>
