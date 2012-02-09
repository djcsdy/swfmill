<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="/">
    <xsl:if test="not(/out/ttf)">
      <xsl:message terminate="yes">
        /out/ttf element is missing.
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="not(/out/ttf/DefineFont2)">
      <xsl:message terminate="yes">
        /out/ttf/DefineFont2 element is missing.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/ttf/DefineFont2/@objectID != 0">
      <xsl:message terminate="yes">
        /out/ttf/DefineFont2/@objectID attribute is missing or has the wrong
        value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/ttf/DefineFont2/@isShiftJIS != 0">
      <xsl:message terminate="yes">
        /out/ttf/DefineFont2/@isShiftJIS attribute is missing or has the
        wrong value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/ttf/DefineFont2/@isUnicode != 0">
      <xsl:message terminate="yes">
        /out/ttf/DefineFont2/@isUnicode attribute is missing or has the
        wrong value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/ttf/DefineFont2/@isANSII != 0">
      <xsl:message terminate="yes">
        /out/ttf/DefineFont2/@isANSII attribute is missing or has the wrong
        value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/ttf/DefineFont2/@wideGlyphOffsets != 1">
      <xsl:message terminate="yes">
        /out/ttf/DefineFont2/@wideGlyphOffsets attribute is missing or has
        the wrong value
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/ttf/DefineFont2/@italic != 0">
      <xsl:message terminate="yes">
        /out/ttf/DefineFont2/@italic attribute is missing or has the wrong
        value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/ttf/DefineFont2/@bold != 0">
      <xsl:message terminate="yes">
        /out/ttf/DefineFont2/@bold attribute is missing or has the wrong
        value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/ttf/DefineFont2/@language != 0">
      <xsl:message terminate="yes">
        /out/ttf/DefineFont2/@language attribute is missing or has the wrong
        value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/ttf/DefineFont2/@name != 'Bitstream Vera Sans'">
      <xsl:message terminate="yes">
        /out/ttf/DefineFont2/@name attribute is missing or has the wrong
        value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/ttf/DefineFont2/@ascent != 0">
      <xsl:message terminate="yes">
        /out/ttf/DefineFont2/@ascent attribute is missing or has the wrong
        value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/ttf/DefineFont2/@descent != 0">
      <xsl:message terminate="yes">
        /out/ttf/DefineFont2/@descent attribute is missing or has the wrong
        value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="/out/ttf/DefineFont2/@leading != 0">
      <xsl:message terminate="yes">
        /out/ttf/DefineFont2/@leading attribute is missing or has the wrong
        value.
      </xsl:message>
    </xsl:if>

    <xsl:if test="not(/out/ttf/DefineFont2/glyphs)">
      <xsl:message terminate="yes">
        /out/ttf/DefineFont2/glyphs element is missing.
      </xsl:message>
    </xsl:if>

    <xsl:if test="count(/out/ttf/DefineFont2/glyphs/Glyph) != 256">
      <xsl:message terminate="yes">
        Incorrect number of /out/ttf/DefineFont2/glyphs/Glyph elements.
      </xsl:message>
    </xsl:if>

    <xsl:for-each select="/out/ttf/DefineFont2/glyphs/Glyph">
      <xsl:variable name="map" select="@map"/>

      <xsl:if test="not(GlyphShape)">
        <xsl:message terminate="yes">
          Glyph[@map=<xsl:value-of select="$map"/>] does not contain a
          GlyphShape element.
        </xsl:message>
      </xsl:if>

      <xsl:if test="not(GlyphShape/edges)">
        <xsl:message terminate="yes">
          Glyph[@map=<xsl:value-of select="$map"/>] does not contain a
          GlyphShape/edges element.
        </xsl:message>
      </xsl:if>

      <xsl:if test="not(GlyphShape/edges/*[1][self::ShapeSetup])">
        <xsl:message terminate="yes">
          Glyph[@map=<xsl:value-of select="$map"/>] does not contain a
          ShapeSetup element as the first element child of GlyphShape/edges.
        </xsl:message>
      </xsl:if>

      <xsl:for-each select="GlyphShape/edges/*[
          not(self::ShapeSetup | self::LineTo | self::CurveTo)]">
        <xsl:message terminate="yes">
          Glyph[@map=<xsl:value-of select="$map"/>] contains an unexpected
          element '<xsl:value-of select="name()"/>'.
        </xsl:message>
      </xsl:for-each>
    </xsl:for-each>

    <xsl:variable name="space"
        select="/out/ttf/DefineFont2/glyphs/Glyph[@map=32]"/>

    <xsl:if test="not($space)">
      <xsl:message terminate="yes">
        'Space' glyph (Glyph[@map=32]) is missing.
      </xsl:message>
    </xsl:if>

    <xsl:if test="$space/GlyphShape/edges/*[not(self::ShapeSetup)]">
      <xsl:message terminate="yes">
        'Space' glyph (Glyph[@map=32]) contains unexpected edges.
      </xsl:message>
    </xsl:if>

    <xsl:variable name="exclamation-mark"
        select="/out/ttf/DefineFont2/glyphs/Glyph[@map=33]"/>

    <xsl:if test="not($exclamation-mark)">
      <xsl:message terminate="yes">
        'Exclamation mark' glyph (Glyph[@map=33]) is missing.
      </xsl:message>
    </xsl:if>

    <xsl:if test="not($exclamation-mark/GlyphShape/edges/*[1]
        [self::ShapeSetup][@x=155][@y=-127][@fillStyle0=1])">
      <xsl:message terminate="yes">
        'Exclamation mark' ShapeSetup is missing or has unexpected values.
      </xsl:message>
    </xsl:if>

    <xsl:if test="not($exclamation-mark/GlyphShape/edges/*[2]
        [self::LineTo][@x=102][@y=0])">
      <xsl:message terminate="yes">
        'Exclamation mark' edge 2 is missing or has unexpected values.
      </xsl:message>
    </xsl:if>

    <xsl:if test="not($exclamation-mark/GlyphShape/edges/*[6]
        [self::ShapeSetup][@x=155][@y=-747][@fillStyle0=1]">
      <xsl:message terminate="yes">
        'Exclamation mark' edge 6 is missing or has unexpected values.
      </xsl:message>
    </xsl:if>
  </xsl:template>
</xsl:stylesheet>
