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

<!-- named template for redundant transforms -->
<xsl:template name="transform">
	<transform>
		<xsl:choose>
			<!-- catch empty transform attributes. -->
			<xsl:when test="not(@transform) or @transform=''">
				<Transform transX="0" transY="0"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy-of select="swft:transform(@transform)"/>
			</xsl:otherwise>
		</xsl:choose>
	</transform>
</xsl:template>

<!-- named template for redundant placing -->
<xsl:template name="placeObject">
	<!-- place the element, or the referenced element (if it's a reference). 
-->
	<xsl:variable name="id">
		<xsl:choose>
			<xsl:when test="name()='use'">
				<xsl:value-of select="swft:map-id(substring(@xlink:href,2))"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="swft:map-id(@id)"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<!-- use inkscape label as an instance name instead of id, allowing 
multiple instances with the same name. -->
	<xsl:variable name="name">
		<xsl:choose>
			<xsl:when test="@inkscape:label">
				<xsl:choose>
					<xsl:when test="substring(@inkscape:label,1,1)='#'">
						<xsl:value-of select="substring(@inkscape:label,2)"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="@inkscape:label"/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="@id"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<!-- place the object. -->
	<PlaceObject2 replace="0" depth="{swft:next-depth()}" name="{$name}" 
objectID="{$id}">
		<!-- svg:use elements add an instance transform, all others have it 
included in their definition. -->
		<xsl:choose>
			<xsl:when test="name()='use'">
				<xsl:call-template name="transform" />
			</xsl:when>
			<xsl:otherwise>
				<transform>
					<Transform transX="0" transY="0"/>
				</transform>
			</xsl:otherwise>
		</xsl:choose>
	</PlaceObject2>
</xsl:template>

<!-- named template for redundant wrappers -->
<xsl:template name="wrapElement">
	<xsl:param name="innerid" />
	<xsl:variable name="id" select="swft:map-id(@id)" />
	<DefineSprite objectID="{$id}" frames="1">
		<tags>
			<PlaceObject2 replace="0" depth="{swft:next-depth()}" 
objectID="{$innerid}">
				<xsl:call-template name="transform" />
			</PlaceObject2>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>
</xsl:template>

<!-- named template for redundant exports -->
<xsl:template name="exportElement">
	<xsl:variable name="id" select="swft:map-id(@id)" />
	<xsl:variable name="name" select="@id" />
	<!-- export the element. -->
	<xsl:if test="@id">
		<Export>
			<symbols>
				<Symbol objectID="{$id}" name="{$name}"/>
			</symbols>
		</Export>
	</xsl:if>
	<!-- define a class, if applicable. -->
	<xsl:variable name="class" select="@class"/>
	<xsl:if test="string-length($class) > 0">
		<xsl:call-template name="register-class">
			<xsl:with-param name="class" select="$class"/>
			<xsl:with-param name="linkage-id" select="$name"/>
		</xsl:call-template>
	</xsl:if>

</xsl:template>


<!-- entry point: starts 2 passes, one for queuing up the definitions, one 
for placing the elements. -->
<xsl:template match="svg:svg" mode="svg">
	<xsl:param name="id"/>
	<!-- initiate the definition pass. -->
	<xsl:apply-templates mode="queue" />
	<!-- define svg root as sprite. -->
	<DefineSprite objectID="{$id}" frames="1">
		<tags>
			<!-- initiate the placement pass. -->
			<xsl:apply-templates mode="placement" />
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>
</xsl:template>

<xsl:template match="svg:g|svg:path|svg:rect|svg:use|svg:text|svg:flowRoot" 
mode="queue">
	<xsl:variable name="id"><xsl:value-of 
select="swft:map-id(@id)"/></xsl:variable>
	<xsl:variable name="name" select="@id"/>

	<!-- first define the subparts, so that we get the innermost ones queued 
first. -->
	<xsl:apply-templates mode="queue" />
	<!-- now define this element, which is based on the subparts. -->
	<xsl:apply-templates select="." mode="definition">
		<xsl:with-param name="id" select="$id"/>
		<xsl:with-param name="name" select="$name"/>
	</xsl:apply-templates>

</xsl:template>

<xsl:template match="svg:g|svg:path|svg:rect|svg:use|svg:text|svg:flowRoot" 
mode="placement">
	<!-- no definition. just place this element. -->
	<xsl:call-template name="placeObject" />
</xsl:template>

<xsl:template match="svg:g" mode="definition" priority="-1">
	<xsl:param name="id"/>
	<xsl:param name="name"/>

	<!-- test if a wrapper is needed for a group transform -->
	<xsl:choose>
		<xsl:when test="not(@transform) or @transform='' or 
transform='translate(0,0)'">
			<!-- no transform, define the group and place the subparts -->
			<DefineSprite objectID="{$id}" frames="1">
				<tags>
					<xsl:apply-templates mode="placement" />
					<ShowFrame/>
					<End/>
				</tags>
			</DefineSprite>
		</xsl:when>
		<xsl:otherwise>
			<!-- define an inner group and wrap it with the group transform -->
			<xsl:variable name="innerid"><xsl:value-of 
select="swft:next-id()"/></xsl:variable>
			<DefineSprite objectID="{$innerid}" frames="1">
				<tags>
					<xsl:apply-templates mode="placement" />
					<ShowFrame/>
					<End/>
				</tags>
			</DefineSprite>
			<xsl:call-template name="wrapElement">
				<xsl:with-param name="innerid" select="$innerid" />
			</xsl:call-template>
		</xsl:otherwise>
	</xsl:choose>
	<!-- export -->
	<xsl:call-template name="exportElement" />

</xsl:template>

<xsl:template match="svg:path" mode="definition">
	<xsl:param name="id"/>
	<xsl:variable name="shapeid"><xsl:value-of 
select="swft:next-id()"/></xsl:variable>

	<!-- define the path -->
	<xsl:copy-of select="swft:path( @d, $shapeid, @style )"/>
	<!-- wrap in sprite -->
	<xsl:call-template name="wrapElement">
		<xsl:with-param name="innerid" select="$shapeid" />
	</xsl:call-template>
	<!-- export -->
	<xsl:call-template name="exportElement" />
</xsl:template>

<xsl:template match="svg:rect" mode="definition">
	<xsl:param name="id"/>
	<xsl:param name="name"/>
	<xsl:variable name="shapeid"><xsl:value-of 
select="swft:next-id()"/></xsl:variable>

	<!-- define the element -->
	<DefineShape3 objectID="{$shapeid}">
		<bounds>
			<Rectangle left="{@x}" right="{(@x+@width)*20}" top="{@y}" 
bottom="{(@y+@height)*20}"/>
		</bounds>
		<styles>
			<StyleList>
				<xsl:copy-of select="swft:css(@style)/tmp/*"/>
			</StyleList>
		</styles>
		<shapes>
			<Shape>
				<edges>
					<ShapeSetup x="{(@x+@width)*20}" y="{(@y+@height)*20}" fillStyle0="1" 
lineStyle="1"/>
					<LineTo x="-{(@width)*20}" y="0"/>
					<LineTo x="0" y="-{(@height)*20}"/>
					<LineTo x="{(@width)*20}" y="0"/>
					<LineTo x="0" y="{(@height)*20}"/>
					<ShapeSetup/>
				</edges>
			</Shape>
		</shapes>
	</DefineShape3>
	<!-- wrap in sprite -->
	<xsl:call-template name="wrapElement">
		<xsl:with-param name="innerid" select="$shapeid" />
	</xsl:call-template>
	<!-- export -->
	<xsl:call-template name="exportElement" />
</xsl:template>

<xsl:template match="svg:flowRoot" mode="definition">
	<xsl:param name="id"/>
	<xsl:param name="name"/>
	<xsl:variable name="shapeid"><xsl:value-of 
select="swft:next-id()"/></xsl:variable>

	<!-- define the element -->
	<DefineEditText objectID="{$shapeid}" wordWrap="1" multiLine="1" 
password="0" readOnly="0" autoSize="0" hasLayout="1" notSelectable="0" 
hasBorder="0" isHTML="0" useOutlines="0" fontRef="{swft:map-id('vera')}" 
fontHeight="240" align="0" leftMargin="0" rightMargin="0" indent="0" 
leading="40" variableName="{@name}">
		<xsl:attribute name="initialText">
			<xsl:apply-templates mode="svg-text"/>
		</xsl:attribute>
		<size>
			<Rectangle left="{svg:flowRegion/svg:rect/@x * 20}" 
right="{(svg:flowRegion/svg:rect/@x + svg:flowRegion/svg:rect/@width)* 20}" 
top="{svg:flowRegion/svg:rect/@y * 20}" bottom="{(svg:flowRegion/svg:rect/@y 
+ svg:flowRegion/svg:rect/@height)* 20}"/>
		</size>
		<color>
			<Color red="0" green="0" blue="0" alpha="255"/>
		</color>
	</DefineEditText>
	<!-- wrap in sprite -->
	<xsl:call-template name="wrapElement">
		<xsl:with-param name="innerid" select="$shapeid" />
	</xsl:call-template>
	<!-- export -->
	<xsl:call-template name="exportElement" />
</xsl:template>

<xsl:template match="svg:text" mode="definition">
	<xsl:param name="id"/>
	<xsl:param name="name"/>
	<xsl:variable name="shapeid"><xsl:value-of 
select="swft:next-id()"/></xsl:variable>

	<!-- define the element -->
	<DefineEditText objectID="{$shapeid}" wordWrap="0" multiLine="1" 
password="0" readOnly="1" autoSize="1" hasLayout="1" notSelectable="1" 
hasBorder="0" isHTML="0" useOutlines="0" fontRef="{swft:map-id('vera')}" 
fontHeight="240" align="0" leftMargin="0" rightMargin="0" indent="0" 
leading="40" variableName="{@name}">
	<xsl:attribute name="initialText">
		<xsl:apply-templates mode="svg-text"/>
	</xsl:attribute>
	<size>
		<Rectangle left="{@x * 20}" right="{@x * 30}" top="{@y * 20}" bottom="{@y 
* 30}"/>
	</size>
	<color>
		<Color red="0" green="0" blue="0" alpha="255"/>
	</color>
	</DefineEditText>
	<!-- wrap in sprite -->
	<xsl:call-template name="wrapElement">
		<xsl:with-param name="innerid" select="$shapeid" />
	</xsl:call-template>
	<!-- export -->
	<xsl:call-template name="exportElement" />
</xsl:template>

<xsl:template match="svg:flowRegion" mode="svg-text">
	<xsl:apply-templates mode="svg-text"/>
</xsl:template>

<xsl:template match="svg:flowPara" mode="svg-text">
	<xsl:apply-templates mode="svg-text"/>
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
