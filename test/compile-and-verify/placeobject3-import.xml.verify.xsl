<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  
  <xsl:template match="/">
    <xsl:variable name="objectId"
        select="/swf/Header/tags/Export/symbols/Symbol[@name='placeobject3_swf']
            /@objectID"/>
    
    <xsl:if test="not($objectId)">
      <xsl:message terminate="yes">
        Could not determine object ID of imported SWF.
      </xsl:message>
    </xsl:if>
    
    <xsl:variable name="defineSprite"
        select="/swf/Header/tags/DefineSprite[@objectID=$objectId]"/>
    
    <xsl:if test="not($defineSprite)">
      <xsl:message terminate="yes">
        Could not find DefineSprite defining imported SWF.
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="count($defineSprite/tags/PlaceObject2) != 2">
      <xsl:message terminate="yes">
        DefineSprite defining imported SWF should contain two PlaceObject2
        tags.
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="count($defineSprite/tags/PlaceObject3) != 1">
      <xsl:message terminate="yes">
        DefineSprite defining imported SWF should contain one PlaceObject3
        tag.
      </xsl:message>
    </xsl:if>
    
    <xsl:if test="not(/swf/Header/tags/PlaceObject2[@objectID=$objectId])">
      <xsl:message terminate="yes">
        Could not find PlaceObject2 tag placing imported SWF on stage.
      </xsl:message>
    </xsl:if>
  </xsl:template>
  
</xsl:stylesheet>
