<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
				xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape"
				xmlns:svg="http://www.w3.org/2000/svg"
				xmlns:swft="http://subsignal.org/swfml/swft"
				extension-element-prefixes="swft"
				version='1.0'>

<xsl:output method="xml" indent="yes"/>

<xsl:template match="svg:svg">
	<swf version="7" compressed="1">
		<Header framerate="1" frames="1">
			<size>
				<Rectangle left="0" right="2000" top="0" bottom="2000"/>
			</size>
			<tags>
				<SetBackgroundColor>
					<color>
						<Color red="100" green="150" blue="200"/>
					</color>
				</SetBackgroundColor>
				<xsl:apply-templates mode="svg">
					<xsl:with-param name="parentx">100</xsl:with-param>
					<xsl:with-param name="parenty">100</xsl:with-param>
				</xsl:apply-templates>
				<ShowFrame/>
				<End/>
			</tags>
		</Header>
	</swf>
</xsl:template>

<xsl:template match="svg:g|svg:path|svg:rect" mode="svg">
	<xsl:param name="parentx">0</xsl:param>
	<xsl:param name="parenty">0</xsl:param>
	
	<xsl:variable name="id"><xsl:value-of select="swft:map-id(@id)"/></xsl:variable> 
	<xsl:variable name="xofs">
		<xsl:choose>
			<xsl:when test="@inkscape:px">
				<xsl:value-of select="@inkscape:px"/>
			</xsl:when>
			<xsl:otherwise>0</xsl:otherwise> <!-- "none" for special treatment? -->
		</xsl:choose>
	</xsl:variable> 
	<xsl:variable name="yofs">
		<xsl:choose>
			<xsl:when test="@inkscape:py">
				<xsl:value-of select="@inkscape:py"/>
			</xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable> 
	<xsl:variable name="name" select="@id"/>
	<xsl:variable name="class" select="@class"/>

	<xsl:apply-templates select="." mode="svg-inner">
		<xsl:with-param name="id" select="$id"/>
		<xsl:with-param name="name" select="$name"/>
		<xsl:with-param name="xofs" select="$xofs"/>
		<xsl:with-param name="yofs" select="$yofs"/>
		<xsl:with-param name="parentx" select="$parentx"/>
		<xsl:with-param name="parenty" select="$parenty"/>
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

<xsl:template match="svg:g" mode="svg-inner">
	<xsl:param name="id"/>
	<xsl:param name="name"/>
	<xsl:param name="xofs"/>
	<xsl:param name="yofs"/>
	<xsl:param name="parentx"/>
	<xsl:param name="parenty"/>
	<xsl:variable name="subid"><xsl:value-of select="swft:next-id()"/></xsl:variable> 

	<DefineSprite objectID="{$id}" frames="1">
		<tags>
			<xsl:apply-templates mode="svg">
				<xsl:with-param name="parentx" select="-$xofs"/>
				<xsl:with-param name="parenty" select="-$yofs"/>
			</xsl:apply-templates>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>
	<PlaceObject2 replace="0" depth="{swft:next-depth()}" name="{$name}" objectID="{$id}">
		<transform>
			<xsl:choose>
				<xsl:when test="@transform">
					<xsl:copy-of select="swft:transform(@transform,($parentx+$xofs),($parenty+$yofs))"/>
				</xsl:when>
				<xsl:otherwise>
					<Transform transX="{($parentx+$xofs)*20}" transY="{($parenty+$yofs)*20}"/>
				</xsl:otherwise>
			</xsl:choose>
		</transform>
	</PlaceObject2>
</xsl:template>

<xsl:template match="svg:path" mode="svg-inner">
	<xsl:param name="id"/>
	<xsl:param name="parentx">0</xsl:param>
	<xsl:param name="parenty">0</xsl:param>
	<xsl:param name="xofs"/>
	<xsl:param name="yofs"/>
	<xsl:variable name="shapeid"><xsl:value-of select="swft:next-id()"/></xsl:variable> 

	<xsl:variable name="bounds" select="swft:bounds(@d)"/>
	<xsl:variable name="xo">
		<xsl:choose>
			<xsl:when test="$xofs = 'none'">
				<xsl:value-of select="number($bounds/Rectangle/@left) div 20"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$xofs"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="yo">
		<xsl:choose>
			<xsl:when test="$yofs = 'none'">
				<xsl:value-of select="number($bounds/Rectangle/@top) div 20"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$yofs"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<DefineShape3 objectID="{$shapeid}">
		<bounds>
			<xsl:copy-of select="swft:bounds(@d,-1 * $xo,-1 * $yo)"/>
		</bounds>
		<styles>
			<StyleList>
				<xsl:copy-of select="swft:css(@style)/tmp/*"/>
			</StyleList>
		</styles>
		<shapes>
			<Shape>
				<xsl:apply-templates mode="shape" select="swft:path(@d,(-$xo),(-$yo))/tmp/Shape/*"/>
			</Shape>
		</shapes>
	</DefineShape3>
	<DefineSprite objectID="{$id}" frames="1">
		<tags>
			<PlaceObject2 replace="0" depth="{swft:next-depth()}" objectID="{$shapeid}">
				<transform>
					<Transform transX="0" transY="0"/>
				</transform>
			</PlaceObject2>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>
	<PlaceObject2 replace="0" depth="{swft:next-depth()}" name="{$name}" objectID="{$id}">
		<transform>
			<xsl:choose>
				<xsl:when test="@transform">
					<xsl:copy-of select="swft:transform(@transform,($xo + $parentx),($yo + $parenty))"/>
				</xsl:when>
				<xsl:otherwise>
					<Transform transX="{($parentx + $xo)*20}" transY="{($parenty + $yo)*20}"/>
				</xsl:otherwise>
			</xsl:choose>
		</transform>
	</PlaceObject2>
</xsl:template>

<xsl:template match="svg:rect" mode="svg-inner">
	<xsl:param name="id"/>
	<xsl:param name="name"/>
	<xsl:param name="xofs"/>
	<xsl:param name="yofs"/>
	<xsl:param name="parentx">0</xsl:param>
	<xsl:param name="parenty">0</xsl:param>
	<xsl:variable name="shapeid"><xsl:value-of select="swft:next-id()"/></xsl:variable> 

	<xsl:variable name="xo">
		<xsl:choose>
			<xsl:when test="$xofs = 'none'">
				<xsl:value-of select="@x"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$xofs"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="yo">
		<xsl:choose>
			<xsl:when test="$yofs = 'none'">
				<xsl:value-of select="@y"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$yofs"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

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
					<Transform transX="{-$xo*20}" transY="{-$yo*20}"/>
				</transform>
			</PlaceObject2>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>
	<PlaceObject2 replace="0" depth="{swft:next-depth()}" name="{$name}" objectID="{$id}">
		<transform>
			<xsl:choose>
				<xsl:when test="@transform">
					<xsl:copy-of select="swft:transform(@transform,($xo + $parentx),($yo + $parenty))"/>
				</xsl:when>
				<xsl:otherwise>
					<Transform transX="{($parentx + $xo)*20}" transY="{($parenty + $yo)*20}"/>
				</xsl:otherwise>
			</xsl:choose>
		</transform>
	</PlaceObject2>
</xsl:template>

<xsl:template match="*|@*|text()" mode="svg" priority="-1"/>

<!-- works only if the id is exported! -->
<xsl:template name="register-class">
	<xsl:param name="class"/>
	<xsl:param name="linkage-id"/>
	<xsl:variable name="packages" select="str:tokenize($class,'.')"/>
	<xsl:variable name="strofs">3</xsl:variable>
	<xsl:variable name="id" select="swft:next-id()"/>

	<DefineSprite objectID="{$id}" frames="1">
		<tags>
		  <End/>
		</tags>
	</DefineSprite>
	<Export>
		<symbols>
		  <Symbol objectID="{$id}" name="__Packages.swfmill.registerClass.{$linkage-id}"/>
		</symbols>
	</Export>
	<DoInitAction sprite="{$id}">
		<actions>
			<Dictionary>
				<strings>
					<String value="Object"/>
					<String value="registerClass"/>
					<String value="{$linkage-id}"/>
					<xsl:for-each select="$packages">
						<String value="{.}"/>
					</xsl:for-each>
				</strings>
			</Dictionary>
			
			<!-- package root -->
			<PushData>
				<items>
				  <StackDictionaryLookup index="{$strofs}"/> 
				</items>
			</PushData>
			<GetVariable/>
			
			<!-- packages -->
			<xsl:for-each select="$packages[position()>1]">
				<PushData>
					<items>
					  <StackDictionaryLookup index="{$strofs + position()}"/>
					</items>
				</PushData>
				<GetMember/>
			</xsl:for-each>
			
			<!-- linkage id, 2 (arguments), Object-->
			<PushData>
				<items>
				  <StackDictionaryLookup index="2"/>
				  <StackInteger value="2"/>
				  <StackDictionaryLookup index="0"/>
				</items>
			</PushData>
			<GetVariable/>
			<!-- registerClass -->
			<PushData>
				<items>
				  <StackDictionaryLookup index="1"/>
				</items>
			</PushData>
			<CallMethod/>
			<Pop/>
			<EndAction/>
		</actions>
	</DoInitAction>
	
</xsl:template>

<xsl:template match="ShapeSetup" mode="shape">
	<ShapeSetup fillStyle0="1" fillStyle1="2" lineStyle="1">
		<xsl:apply-templates select="*|@*" mode="shape"/>
	</ShapeSetup>
</xsl:template>
<xsl:template match="*|@*|text()" mode="shape" priority="-1">
	<xsl:copy-of select="."/>
</xsl:template>


</xsl:stylesheet>
