<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="/">
    <xsl:if test="not(/out/jpega)">
      <xsl:message terminate="yes">
        /out/jpega element is missing.
      </xsl:message>
    </xsl:if>

    <xsl:if test="not(/out/jpega/data)">
      <xsl:message terminate="yes">
        /out/jpega/data element is missing.
      </xsl:message>
    </xsl:if>

    <xsl:if test="string(/out/jpega/data/text()) != '/9n/2P/Y/+AAEEpGSUYAAQEBAEgASAAA/9sAQwABAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEB/9sAQwEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEB/8AAEQgABQAFAwERAAIRAQMRAf/EABQAAQAAAAAAAAAAAAAAAAAAAAn/xAAbEAADAQEBAQEAAAAAAAAAAAAFBgcDBAgCFv/EABUBAQEAAAAAAAAAAAAAAAAAAAcI/8QAHhEAAgMBAAMBAQAAAAAAAAAABAUCAwYHAQgJERX/2gAMAwEAAhEDEQA/ABy8+eoT7ZSr6Cm8E8fIDROWTVQoLI2wszWQ1C7+ypW501MKqaWpq9ySvj/cttIMfIfkKNm2YBlV0XEzkqTJPH8yrm/m9yTtTfYYMXe9izF/FnVWcYtyugPG9OrMiAPiYMwAUJeJYIBogczCvktdP9vbYSfedJn5dE6FzpKC4twP2o0+87anq+i3snnZYzT/AMODPPq8AAyfD26voLCy56RBJ+l3TckO30vzxGEnem0TOUPJjYy67//ZeNpT3f9jv+r+////7/8BJH6AWaogMQD8IBNk'">
      <xsl:message terminate="yes">
        JPEG+alpha data was not imported correctly.
      </xsl:message>
    </xsl:if>
  </xsl:template>
</xsl:stylesheet>
