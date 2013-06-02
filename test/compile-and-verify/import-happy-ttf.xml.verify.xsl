<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  
  <xsl:template match="/">
    <xsl:variable name="defineFont2" select="/swf/Header/tags/DefineFont2"/>
    
    <xsl:if test="count($defineFont2) != 1">
      <xsl:message terminate="yes">
        Generated SWF should contain one DefineFont2 tag.
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="number($defineFont2/@isShiftJIS) != 0">
      <xsl:message terminate="yes">
        DefineFont2/@isShiftJIS attribute should not be set.
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="number($defineFont2/@isUnicode) != 0">
      <xsl:message terminate="yes">
        DefineFont2/@isUnicode attribute should not be set.
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="number($defineFont2/@isANSII) != 0">
      <xsl:message terminate="yes">
        DefineFont2/@isANSII attribute should not be set.
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="$defineFont2/@wideGlyphOffsets != '1'">
      <xsl:message terminate="yes">
        DefineFont2/@wideGlyphOffsets attribute should be set to "1".
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="number($defineFont2/@italic) != 0">
      <xsl:message terminate="yes">
        DefineFont2/@italic attribute should not be set.
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="number($defineFont2/@bold) != 0">
      <xsl:message terminate="yes">
        DefineFont2/@bold attribute should not be set.
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="$defineFont2/@language != '0'">
      <xsl:message terminate="yes">
        DefineFont2/@language attribute should be set to "0".
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="$defineFont2/@name != 'Happy'">
      <xsl:message terminate="yes">
        DefineFont2/@name attribute should be set to "Happy".
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="$defineFont2/@ascent != '1024'">
      <xsl:message terminate="yes">
        DefineFont2/@ascent attribute should be set to "1024".
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="$defineFont2/@descent != '512'">
      <xsl:message terminate="yes">
        DefineFont2/@descent attribute should be set to "512".
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="$defineFont2/@leading != '0'">
      <xsl:message terminate="yes">
        DefineFont2/@leading attribute should be set to "0".
      </xsl:message>
    </xsl:if>
    
    <xsl:variable name="glyph" select="$defineFont2/glyphs/Glyph"/>
    
    <xsl:if test="count($glyph) != 1">
      <xsl:message terminate="yes">
        DefineFont2 should contain one Glyph.
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="$glyph/@map != '65'">
      <xsl:message terminate="yes">
        Glyph should be mapped to U+0065 ("A").
      </xsl:message>
    </xsl:if>
    
    <xsl:variable name="advances" select="$defineFont2/advance/*"/>
    
    <xsl:if test="count($advances) != 1">
      <xsl:message terminate="yes">
        There should be one advance value.
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="$advances[0][self::Short]/@value != '1537'">
      <xsl:message terminate="yes">
        The first advance value should be &lt;Short value="1537"/>.
      </xsl:message>
    </xsl:if>
    
    <xsl:variable name="bounds" select="$defineFont2/bounds/*"/>
    
    <xsl:if test="count($bounds) != 1
        or count($bounds) != count($bounds[self::Rectangle])">
      <xsl:message terminate="yes">
        There should be one bounds rectangle.
      </xsl:message>
    </xsl:if>
    
    <xsl:variable name="boundsRectangle0" select="$bounds[0]"/>
    
    <xsl:if test="$boundsRectangle0/@left != '0'">
      <xsl:message terminate="yes">
        The first bounds Rectangle/@left attribute should be set to "0".
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="$boundsRectangle0/@right != '1024'">
      <xsl:message terminate="yes">
        The first bounds Rectangle/@right attribute should be set to "1024".
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="$boundsRectangle0/@top != '-972'">
      <xsl:message terminate="yes">
        The first bounds Rectangle/@top attribute should be set to "-972".
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="$boundsRectangle0/@bottom != '0'">
      <xsl:message terminate="yes">
        The first bounds Rectangle/@bottom attribute should be set to "0".
      </xsl:message>
    </xsl:if>
  </xsl:template>
</xsl:stylesheet>
