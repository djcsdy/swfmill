<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
				xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape"
				xmlns:sodipodi="http://inkscape.sourceforge.net/DTD/sodipodi-0.dtd"
				xmlns:svg="http://www.w3.org/2000/svg"
				xmlns:swft="http://subsignal.org/swfml/swft"
				xmlns:str="http://exslt.org/strings"
				xmlns:xlink="http://www.w3.org/1999/xlink"
				extension-element-prefixes="swft"
				version='1.0'>

<xsl:template match="svg:svg" mode="svg">
	<xsl:param name="id"/>
	<DefineSprite objectID="{$id}" frames="1">
		<tags>
			<xsl:apply-templates mode="svg"/>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>
</xsl:template>

<xsl:template match="svg:g|svg:path|svg:rect|svg:use|svg:text" mode="svg">
	<xsl:variable name="id"><xsl:value-of select="swft:map-id(@id)"/></xsl:variable> 
	<xsl:variable name="name" select="@id"/>
	<xsl:variable name="class" select="@class"/>

	<xsl:apply-templates select="." mode="svg-inner">
		<xsl:with-param name="id" select="$id"/>
		<xsl:with-param name="name" select="$name"/>
	</xsl:apply-templates>
	<Export>
		<symbols>
		  <Symbol objectID="{$id}" name="{$name}"/>
		</symbols>
	</Export>
	
	<xsl:if test="string-length($class) > 0">
		<xsl:call-template name="register-class">
			<xsl:with-param name="class" select="$class"/>
			<xsl:with-param name="linkage-id" select="$name"/>
		</xsl:call-template>
	</xsl:if>
</xsl:template>

<xsl:template match="svg:g[@regard-pivot]" mode="svg-inner">
	<xsl:param name="id"/>
	<xsl:param name="name"/>
	<xsl:variable name="subid"><xsl:value-of select="swft:next-id()"/></xsl:variable> 
		
	<xsl:message>PY: <xsl:value-of select="@inkscape:py"/></xsl:message>
		
	<xsl:variable name="xoffset">
		<xsl:value-of select="@inkscape:px"/>
	</xsl:variable>
	<xsl:variable name="yoffset">
		<xsl:value-of select="@inkscape:py"/>
	</xsl:variable>

	<DefineSprite objectID="{$subid}" frames="1">
		<tags>
			<xsl:apply-templates mode="svg"/>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>

	<DefineSprite objectID="{$id}" frames="1">
		<tags>
			<PlaceObject2 replace="0" depth="{swft:next-depth()}" objectID="{$subid}">
				<transform>
					<xsl:choose>
						<xsl:when test="@transform">
							<xsl:copy-of select="swft:transform(@transform,-$xoffset,-$yoffset)"/>
						</xsl:when>
						<xsl:otherwise>
							<Transform transX="{$xoffset * -20}" transY="{$yoffset * -20}"/>
						</xsl:otherwise>
					</xsl:choose>
				</transform>
			</PlaceObject2>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>
	<PlaceObject2 replace="0" depth="{swft:next-depth()}" objectID="{$id}">
		<transform>
			<Transform transX="{$xoffset * 20}" transY="{$yoffset * 20}"/>
		</transform>
	</PlaceObject2>
</xsl:template>

<xsl:template match="svg:g" mode="svg-inner" priority="-1">
	<xsl:param name="id"/>
	<xsl:param name="name"/>
	<xsl:variable name="subid"><xsl:value-of select="swft:next-id()"/></xsl:variable> 

	<DefineSprite objectID="{$subid}" frames="1">
		<tags>
			<xsl:apply-templates mode="svg"/>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>

	<DefineSprite objectID="{$id}" frames="1">
		<tags>
			<PlaceObject2 replace="0" depth="{swft:next-depth()}" objectID="{$subid}">
				<transform>
					<xsl:choose>
						<xsl:when test="@transform">
							<xsl:copy-of select="swft:transform(@transform)"/>
						</xsl:when>
						<xsl:otherwise>
							<Transform transX="0" transY="0"/>
						</xsl:otherwise>
					</xsl:choose>
				</transform>
			</PlaceObject2>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>
	<PlaceObject2 replace="0" depth="{swft:next-depth()}" name="{$name}" objectID="{$id}">
		<transform>
			<Transform transX="0" transY="0"/>
		</transform>
	</PlaceObject2>
</xsl:template>

<xsl:template match="svg:path" mode="svg-inner">
	<xsl:param name="id"/>
	<xsl:variable name="shapeid"><xsl:value-of select="swft:next-id()"/></xsl:variable> 

	<xsl:copy-of select="swft:path( @d, $shapeid, @style )"/>

	<DefineSprite objectID="{$id}" frames="1">
		<tags>
			<PlaceObject2 replace="0" depth="{swft:next-depth()}" objectID="{$shapeid}">
				<transform>
					<xsl:choose>
						<xsl:when test="@transform">
							<xsl:copy-of select="swft:transform(@transform)"/>
						</xsl:when>
						<xsl:otherwise>
							<Transform transX="0" transY="0"/>
						</xsl:otherwise>
					</xsl:choose>
				</transform>
			</PlaceObject2>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>
	<PlaceObject2 replace="0" depth="{swft:next-depth()}" name="{$name}" objectID="{$id}">
		<transform>
			<Transform transX="0" transY="0"/>
		</transform>
	</PlaceObject2>
</xsl:template>

<xsl:template match="svg:rect" mode="svg-inner">
	<xsl:param name="id"/>
	<xsl:param name="name"/>
	<xsl:variable name="shapeid"><xsl:value-of select="swft:next-id()"/></xsl:variable> 

	<DefineShape3 objectID="{$shapeid}">
		<bounds>
			<Rectangle left="{@x}" right="{(@x+@width)*20}" top="{@y}" bottom="{(@y+@height)*20}"/>
		</bounds>
		<styles>
			<StyleList>
				<xsl:copy-of select="swft:css(@style)/tmp/*"/>
			</StyleList>
		</styles>
		<shapes>
			<Shape>
				<edges>
					<ShapeSetup x="{(@x+@width)*20}" y="{(@y+@height)*20}" fillStyle0="1" lineStyle="1"/>
					<LineTo x="-{(@width)*20}" y="0"/>
					<LineTo x="0" y="-{(@height)*20}"/>
					<LineTo x="{(@width)*20}" y="0"/>
					<LineTo x="0" y="{(@height)*20}"/>
					<ShapeSetup/>
				</edges>
			</Shape>
		</shapes>
	</DefineShape3>
	<DefineSprite objectID="{$id}" frames="1">
		<tags>
			<PlaceObject2 replace="0" depth="{swft:next-depth()}" objectID="{$shapeid}">
				<transform>
					<xsl:choose>
						<xsl:when test="@transform">
							<xsl:copy-of select="swft:transform(@transform)"/>
						</xsl:when>
						<xsl:otherwise>
							<Transform transX="0" transY="0"/>
						</xsl:otherwise>
					</xsl:choose>
				</transform>
			</PlaceObject2>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>
	<PlaceObject2 replace="0" depth="{swft:next-depth()}" name="{$name}" objectID="{$id}">
		<transform>
			<Transform transX="0" transY="0"/>
		</transform>
	</PlaceObject2>
</xsl:template>

<xsl:template match="svg:use" mode="svg-inner">
	<xsl:param name="id"/>
	<xsl:param name="name"/>
	<xsl:variable name="subid"><xsl:value-of select="swft:map-id(substring(@xlink:href,2))"/></xsl:variable> 

	<DefineSprite objectID="{$id}" frames="1">
		<tags>
			<PlaceObject2 replace="0" depth="{swft:next-depth()}" objectID="{$subid}">
				<transform>
					<xsl:choose>
						<xsl:when test="@transform">
							<xsl:copy-of select="swft:transform(@transform)"/>
						</xsl:when>
						<xsl:otherwise>
							<Transform transX="0" transY="0"/>
						</xsl:otherwise>
					</xsl:choose>
				</transform>
			</PlaceObject2>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>
	<PlaceObject2 replace="0" depth="{swft:next-depth()}" name="{$name}" objectID="{$id}">
		<transform>
			<Transform transX="0" transY="0"/>
		</transform>
	</PlaceObject2>
</xsl:template>

<xsl:template match="svg:flowRoot" mode="svg-inner">
	<xsl:param name="id"/>
	<xsl:param name="name"/>
	<DefineEditText objectID="{$id}" wordWrap="1" multiLine="1" password="0" 
		readOnly="0" autoSize="0" hasLayout="1"
		notSelectable="0" hasBorder="1" isHTML="0" useOutlines="1" 
		fontRef="{swft:map-id('vera')}" fontHeight="240"
		align="0" leftMargin="0" rightMargin="0" indent="0" leading="41" 
		variableName="{@name}">
		<xsl:attribute name="initialText">
			<xsl:value-of select="normalize-space(.)"/>
		</xsl:attribute>
		<size>
			<Rectangle left="{svg:flowRegion/svg:rect/@x * 20}" 
						right="{(svg:flowRegion/svg:rect/@x + svg:flowRegion/svg:rect/@width)* 20}"
						top="{svg:flowRegion/svg:rect/@y * 20}" 
						bottom="{(svg:flowRegion/svg:rect/@y + svg:flowRegion/svg:rect/@height)* 20}"/>
		</size>
		<color>
			<ColorRGBA red="100" green="150" blue="200" alpha="127"/>
		</color>
	</DefineEditText>
	<PlaceObject2 replace="0" depth="{swft:next-depth()}" name="{$name}" objectID="{$id}">
		<transform>
			<xsl:choose>
				<xsl:when test="@transform">
					<xsl:copy-of select="swft:transform(@transform)"/>
				</xsl:when>
				<xsl:otherwise>
					<Transform transX="0" transY="0"/>
				</xsl:otherwise>
			</xsl:choose>
		</transform>
	</PlaceObject2>
</xsl:template>

<xsl:template match="svg:text" mode="svg-inner">
	<xsl:param name="id"/>
	<xsl:param name="name"/>
	<DefineEditText objectID="{$id}" wordWrap="0" multiLine="1" password="0" 
		readOnly="1" autoSize="1" hasLayout="1"
		notSelectable="1" hasBorder="0" isHTML="0" useOutlines="1" 
		fontRef="{swft:map-id('vera')}" fontHeight="240"
		align="0" leftMargin="0" rightMargin="0" indent="0" leading="1"
		variableName="{@name}">
		<xsl:attribute name="initialText">
			<xsl:apply-templates mode="svg-text"/>
		</xsl:attribute>
		<size>
			<Rectangle left="{@x * 20}" 
						right="{@x * 30}"
						top="{@y * 20}" 
						bottom="{@y * 30}"/>
		</size>
		<color>
			<ColorRGBA red="255" green="255" blue="255" alpha="255"/>
		</color>
	</DefineEditText>
	<PlaceObject2 replace="0" depth="{swft:next-depth()}" name="{$name}" objectID="{$id}">
		<transform>
			<xsl:choose>
				<xsl:when test="@transform">
					<xsl:copy-of select="swft:transform(@transform,-2.05,-11.55)"/>
				</xsl:when>
				<xsl:otherwise>
					<Transform transX="0" transY="-240"/>
				</xsl:otherwise>
			</xsl:choose>
		</transform>
	</PlaceObject2>
</xsl:template>

<xsl:template match="svg:tspan[position()=1]" mode="svg-text">
	<xsl:apply-templates mode="svg-text"/>
</xsl:template>

<xsl:template match="svg:tspan" mode="svg-text" priority="-1">
	<xsl:text>
</xsl:text>
	<xsl:apply-templates mode="svg-text"/>
</xsl:template>

<xsl:template match="text()" mode="svg-text">
	<xsl:copy-of select="."/>
</xsl:template>

<xsl:template match="*|@*|text()" mode="svg" priority="-1"/>

<xsl:template match="ShapeSetup" mode="shape">
	<ShapeSetup fillStyle0="1" fillStyle1="2" lineStyle="1">
		<xsl:apply-templates select="*|@*" mode="shape"/>
	</ShapeSetup>
</xsl:template>
<xsl:template match="*|@*|text()" mode="shape" priority="-1">
	<xsl:copy-of select="."/>
</xsl:template>


</xsl:stylesheet>
