<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
				xmlns:swft="http://subsignal.org/swfml/swft"
				xmlns:str="http://exslt.org/strings"
				extension-element-prefixes="swft"
				version='1.0'>

<xsl:output method="xml" indent="yes"/>

<!-- basic SWF setup -->
<xsl:template match="movie">
	<!-- set defaults for movie -->
	<xsl:variable name="version">
		<xsl:choose>
			<xsl:when test="@version"><xsl:value-of select="@version"/></xsl:when>
			<xsl:otherwise>7</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="compressed">
		<xsl:choose>
			<xsl:when test="@compressed='true'">1</xsl:when>
			<xsl:when test="@compressed='false'">0</xsl:when>
			<xsl:otherwise>1</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="framerate">
		<xsl:choose>
			<xsl:when test="@framerate"><xsl:value-of select="@framerate"/></xsl:when>
			<xsl:otherwise>12</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="frames">
		<xsl:choose>
			<xsl:when test="@frames"><xsl:value-of select="@frames"/></xsl:when>
			<xsl:otherwise>1</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="left">
		<xsl:choose>
			<xsl:when test="@left"><xsl:value-of select="@left * 20"/></xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="top">
		<xsl:choose>
			<xsl:when test="@top"><xsl:value-of select="@top * 20"/></xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="right">
		<xsl:choose>
			<xsl:when test="@width"><xsl:value-of select="$left + (@width * 20)"/></xsl:when>
			<xsl:otherwise>6400</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="bottom">
		<xsl:choose>
			<xsl:when test="@height"><xsl:value-of select="$top + (@height * 20)"/></xsl:when>
			<xsl:otherwise>4800</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<swf version="{$version}" compressed="{$compressed}">
		<Header framerate="{$framerate}" frames="{$frames}">
			<size>
				<Rectangle left="{$left}" right="{$right}" top="{$top}" bottom="{$bottom}"/>
			</size>
			<tags>
				<xsl:apply-templates/>
				<End/>
			</tags>
		</Header>
	</swf>
</xsl:template>

<!-- Color from @color="#rrggbb" or @red, @green, @blue-->
<xsl:template name="color">
	<xsl:variable name="red">
		<xsl:choose>
			<xsl:when test="@red"><xsl:value-of select="@red"/></xsl:when>
			<xsl:when test="@color">0x<xsl:value-of select="substring(@color,2,2)"/></xsl:when>
			<xsl:otherwise>200</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="green">
		<xsl:choose>
			<xsl:when test="@green"><xsl:value-of select="@green"/></xsl:when>
			<xsl:when test="@color">0x<xsl:value-of select="substring(@color,4,2)"/></xsl:when>
			<xsl:otherwise>200</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="blue">
		<xsl:choose>
			<xsl:when test="@blue"><xsl:value-of select="@blue"/></xsl:when>
			<xsl:when test="@color">0x<xsl:value-of select="substring(@color,6)"/></xsl:when>
			<xsl:otherwise>200</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<Color red="{$red}" green="{$green}" blue="{$blue}"/>
</xsl:template>

<!-- ColorRGBA from @color="#rrggbbaa" or @red, @green, @blue, @alpha -->
<xsl:template name="color-rgba">
	<xsl:variable name="red">
		<xsl:choose>
			<xsl:when test="@red"><xsl:value-of select="@red"/></xsl:when>
			<xsl:when test="@color">0x<xsl:value-of select="substring(@color,2,2)"/></xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="green">
		<xsl:choose>
			<xsl:when test="@green"><xsl:value-of select="@green"/></xsl:when>
			<xsl:when test="@color">0x<xsl:value-of select="substring(@color,4,2)"/></xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="blue">
		<xsl:choose>
			<xsl:when test="@blue"><xsl:value-of select="@blue"/></xsl:when>
			<xsl:when test="@color">0x<xsl:value-of select="substring(@color,6)"/></xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="alpha">
		<xsl:choose>
			<xsl:when test="@alpha"><xsl:value-of select="@alpha"/></xsl:when>
			<xsl:otherwise>255</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<ColorRGBA red="{$red}" green="{$green}" blue="{$blue}" alpha="{$alpha}"/>
</xsl:template>

<!-- background color -->
<xsl:template match="background">
	<SetBackgroundColor>
		<color>
			<xsl:call-template name="color"/>
		</color>
	</SetBackgroundColor>
</xsl:template>

<!-- library just passes thru, children decide wether to export by themselves -->
<xsl:template match="library">
	<xsl:apply-templates/>
</xsl:template>

<!-- linkage export -->
<xsl:template match="@id" mode="export">
	<xsl:param name="id"/>
	<Export count="1">
		<symbols>
			<Symbol objectID="{$id}">
				<xsl:attribute name="name"><xsl:value-of select="."/></xsl:attribute>
			</Symbol>
		</symbols>
	</Export>
</xsl:template>
<xsl:template match="@*" mode="export" priority="-1"/>

<!-- place -->
<xsl:template match="place">
	<xsl:variable name="id">
		<xsl:value-of select="swft:map-id(@id)"/>
	</xsl:variable>
	<xsl:variable name="x">
		<xsl:choose>
			<xsl:when test="@x"><xsl:value-of select="@x * 20"/></xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="y">
		<xsl:choose>
			<xsl:when test="@y"><xsl:value-of select="@y * 20"/></xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="scale">
		<xsl:choose>
			<xsl:when test="@scale"><xsl:value-of select="@scale"/></xsl:when>
			<xsl:otherwise>1</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="depth"><xsl:value-of select="@depth"/></xsl:variable>
	
	<!-- if we have a former place with the same depth, use morph="1" replace="0"
		 using morph="0" and replace="1" only works for the same objectID
		 that is already placed in layer (depth) -->
	<xsl:variable name="replace">
		<xsl:choose>
			<xsl:when test="preceding-sibling::place[@depth=$depth] or ../preceding-sibling::frame/place[@depth=$depth]">1</xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:if test="$replace = '1'">
		<RemoveObject2 depth="{@depth}"/>
	</xsl:if>
	<PlaceObject2 replace="0" morph="{$replace}" depth="{$depth}" objectID="{$id}" name="{@name}">
		<transform>
			<Transform transX="{$x}" transY="{$y}" scaleX="{$scale}" scaleY="{$scale}"/>
		</transform>
	</PlaceObject2>
</xsl:template>

<!-- transform -->
<xsl:template match="transform">
	<xsl:variable name="id">
		<xsl:value-of select="swft:map-id(@id)"/>
	</xsl:variable>
	<xsl:variable name="myid">
		<xsl:value-of select="@id"/>
	</xsl:variable>
	<xsl:variable name="x">
		<xsl:choose>
			<xsl:when test="@x"><xsl:value-of select="@x * 20"/></xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="y">
		<xsl:choose>
			<xsl:when test="@y"><xsl:value-of select="@y * 20"/></xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="scale">
		<xsl:choose>
			<xsl:when test="@scale"><xsl:value-of select="@scale"/></xsl:when>
			<xsl:otherwise>1</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="depth">
		<xsl:value-of select="preceding-sibling::place[@id=$myid]/@depth"/>
	</xsl:variable>
	
	<PlaceObject2 replace="1" depth="{$depth}" objectID="{$id}">
		<transform>
			<Transform transX="{$x}" transY="{$y}" scaleX="{$scale}" scaleY="{$scale}"/>
		</transform>
	</PlaceObject2>
</xsl:template>

<!-- texfield -->
<xsl:template match="textfield">
	<xsl:variable name="id">
		<xsl:value-of select="swft:map-id(@id)"/>
	</xsl:variable>
	<xsl:variable name="x">
		<xsl:choose>
			<xsl:when test="@x"><xsl:value-of select="@x * 20"/></xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="y">
		<xsl:choose>
			<xsl:when test="@y"><xsl:value-of select="@y * 20"/></xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="width">
		<xsl:choose>
			<xsl:when test="@width"><xsl:value-of select="@width * 20"/></xsl:when>
			<xsl:otherwise>100</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="height">
		<xsl:choose>
			<xsl:when test="@height"><xsl:value-of select="@height * 20"/></xsl:when>
			<xsl:otherwise>100</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="size">
		<xsl:choose>
			<xsl:when test="@size"><xsl:value-of select="@size * 20"/></xsl:when>
			<xsl:otherwise>240</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<DefineEditText objectID="{$id}" wordWrap="1" multiLine="1" password="0" 
		readOnly="0" autoSize="0" hasLayout="1"
		notSelectable="0" hasBorder="0" isHTML="0" useOutlines="1" 
		fontRef="{swft:map-id(@font)}" fontHeight="{$size}"
		align="0" leftMargin="0" rightMargin="0" indent="0" leading="40" 
		variableName="{@name}" initialText="{@text}">
		<size>
			<Rectangle left="{$x}" right="{$x + $width}" top="{$y}" bottom="{$y + $height}"/>
		</size>
		<color>
			<xsl:call-template name="color-rgba"/>
		</color>
	</DefineEditText>

</xsl:template>

<!-- video object -->
<xsl:template match="video">
	<xsl:variable name="id">
		<xsl:choose>
			<xsl:when test="@id">
				<xsl:value-of select="swft:map-id(@id)"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="swft:next-id()"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="frames">
		<xsl:choose>
			<xsl:when test="@frames"><xsl:value-of select="@frames"/></xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="width">
		<xsl:choose>
			<xsl:when test="@width"><xsl:value-of select="@width"/></xsl:when>
			<xsl:otherwise>160</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="height">
		<xsl:choose>
			<xsl:when test="@height"><xsl:value-of select="@height"/></xsl:when>
			<xsl:otherwise>120</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="deblocking">
		<xsl:choose>
			<xsl:when test="@deblocking"><xsl:value-of select="@deblocking"/></xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="smoothing">
		<xsl:choose>
			<xsl:when test="@smoothing">1</xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="codec">
		<xsl:choose>
			<xsl:when test="@codec"><xsl:value-of select="@codec"/></xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<DefineVideoStream objectID="{$id}" frames="{$frames}" width="{$width}" height="{$height}" deblocking="{$deblocking}" smoothing="{$smoothing}" codec="{$codec}"/>
</xsl:template>

<!-- frame -->
<xsl:template match="frame">
	<xsl:apply-templates/>
	<xsl:if test="@name">
		<FrameLabel label="{@name}"/>
	</xsl:if>
	<ShowFrame/>
</xsl:template>

<!-- clips -->
<xsl:template match="clip" priority="-1">
	<xsl:variable name="id">
		<xsl:choose>
			<xsl:when test="@id">
				<xsl:value-of select="swft:map-id(@id)"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="swft:next-id()"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<DefineSprite objectID="{$id}" frames="1">
		<tags>
			<xsl:apply-templates/>
			<End/>
		</tags>
	</DefineSprite>	
	<xsl:if test="ancestor::library">
		<xsl:apply-templates select="*|@*" mode="export">
			<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
		</xsl:apply-templates>
	</xsl:if>
	<xsl:if test="@class">
		<xsl:call-template name="register-class">
			<xsl:with-param name="class" select="@class"/>
			<xsl:with-param name="linkage-id" select="@id"/>
		</xsl:call-template>
	</xsl:if>
</xsl:template>

<xsl:template match="clip[@import]">
	<xsl:variable name="id">
		<xsl:choose>
			<xsl:when test="@id">
				<xsl:value-of select="swft:map-id(@id)"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="swft:next-id()"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="file">
		<xsl:value-of select="@import"/>
	</xsl:variable>
	<xsl:variable name="ext">
		<xsl:value-of select="translate(substring-after(@import,'.'),'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')"/>
	</xsl:variable>
	<xsl:choose>
		<xsl:when test="$ext = 'jpg' or $ext = 'jpeg'">
			<xsl:apply-templates select="swft:import-jpeg($file)" mode="makeswf">
				<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:when test="$ext = 'png'">
			<xsl:apply-templates select="swft:import-png($file)" mode="makeswf">
				<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:when test="$ext = 'swf'">
			<xsl:apply-templates select="swft:document($file)" mode="makeswf">
				<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:when test="$ext = 'ttf'">
			<xsl:apply-templates select="swft:import-ttf($file,@glyphs)" mode="makeswf">
				<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
			</xsl:apply-templates>
		</xsl:when>
	</xsl:choose>
	<xsl:if test="ancestor::library">
		<xsl:apply-templates select="*|@*" mode="export">
			<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
		</xsl:apply-templates>
	</xsl:if>
	<xsl:if test="@class">
		<xsl:call-template name="register-class">
			<xsl:with-param name="class" select="@class"/>
			<xsl:with-param name="linkage-id" select="@id"/>
		</xsl:call-template>
	</xsl:if>
</xsl:template>

<xsl:template match="font[@import]">
	<xsl:variable name="id">
		<xsl:choose>
			<xsl:when test="@id">
				<xsl:value-of select="swft:map-id(@id)"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="swft:next-id()"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="file">
		<xsl:value-of select="@import"/>
	</xsl:variable>
	
	<xsl:apply-templates select="swft:import-ttf($file,@glyphs)" mode="makeswf">
		<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
	</xsl:apply-templates>
</xsl:template>

<!-- JPEG import -->
<xsl:template match="jpeg" mode="makeswf">
	<xsl:param name="id"/>
	<xsl:variable name="bitmapID"><xsl:value-of select="swft:next-id()"/></xsl:variable> 
	<xsl:variable name="shapeID"><xsl:value-of select="swft:next-id()"/></xsl:variable> 
	<DefineBitsJPEG2 objectID="{$bitmapID}">
		<data>
			<xsl:copy-of select="data"/>
		</data>
	</DefineBitsJPEG2>
	<DefineShape objectID="{$shapeID}">
		<bounds>
			<Rectangle left="0" right="{@width}" top="0" bottom="{@height}"/>
		</bounds>
		<styles>
			<StyleList>
				<fillStyles>
					<ClippedBitmap objectID="{$bitmapID}">
						<matrix>
							<Transform transX="0" transY="0"/>
						</matrix>
					</ClippedBitmap>
				</fillStyles>
				<lineStyles/>
			</StyleList>
		</styles>
		<shapes>
			<Shape>
				<edges>
					<ShapeSetup x="{@width}" y="{@height}" fillStyle1="1"/>
					<LineTo x="-{@width}" y="0"/>
					<LineTo x="0" y="-{@height}"/>
					<LineTo x="{@width}" y="0"/>
					<LineTo x="0" y="{@height}"/>
					<ShapeSetup/>
				</edges>
			</Shape>
		</shapes>
	</DefineShape>
	<DefineSprite objectID="{$id}" frames="1">
		<tags>
			<PlaceObject2 replace="0" depth="1" objectID="{$shapeID}">
				<transform>
					<Transform transX="0" transY="0" scaleX="20" scaleY="20"/>
				</transform>
			</PlaceObject2>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>
</xsl:template>

<!-- PNG import -->
<xsl:template match="png" mode="makeswf">
	<xsl:param name="id"/>
	<xsl:variable name="bitmapID"><xsl:value-of select="swft:next-id()"/></xsl:variable> 
	<xsl:variable name="shapeID"><xsl:value-of select="swft:next-id()"/></xsl:variable> 
	<DefineBitsLossless2 format="{@format}" width="{@width}" height="{@height}" objectID="{$bitmapID}">
		<data>
			<xsl:copy-of select="data"/>
		</data>
	</DefineBitsLossless2>
	<DefineShape objectID="{$shapeID}">
		<bounds>
			<Rectangle left="0" right="{@width}" top="0" bottom="{@height}"/>
		</bounds>
		<styles>
			<StyleList>
				<fillStyles>
					<ClippedBitmap objectID="{$bitmapID}">
						<matrix>
							<Transform transX="0" transY="0"/>
						</matrix>
					</ClippedBitmap>
				</fillStyles>
				<lineStyles/>
			</StyleList>
		</styles>
		<shapes>
			<Shape>
				<edges>
					<ShapeSetup x="{@width}" y="{@height}" fillStyle1="1"/>
					<LineTo x="-{@width}" y="0"/>
					<LineTo x="0" y="-{@height}"/>
					<LineTo x="{@width}" y="0"/>
					<LineTo x="0" y="{@height}"/>
					<ShapeSetup/>
				</edges>
			</Shape>
		</shapes>
	</DefineShape>
	<DefineSprite objectID="{$id}" frames="1">
		<tags>
			<PlaceObject2 replace="0" depth="1" objectID="{$shapeID}">
				<transform>
					<Transform transX="0" transY="0" scaleX="20" scaleY="20"/>
				</transform>
			</PlaceObject2>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>
</xsl:template>


<!-- SWF import -->
<xsl:template match="swf" mode="makeswf">
	<swft:push-map/>
	<xsl:param name="id"/>
	<xsl:variable name="swfID"><xsl:value-of select="swft:next-id()"/></xsl:variable> 
	
	<xsl:apply-templates select="Header/tags/*" mode="sprite-global"/>
		
	<DefineSprite objectID="{$swfID}" frames="{count(Header/tags/ShowFrame)}">
		<tags>
			<xsl:apply-templates select="Header/tags/*" mode="sprite-local"/>
		</tags>
	</DefineSprite>
	<DefineSprite objectID="{$id}" frames="1">
		<tags>
			<PlaceObject2 replace="0" depth="1" objectID="{$swfID}">
				<transform>
					<Transform transX="0" transY="0" />
				</transform>
			</PlaceObject2>
			<ShowFrame/>
			<End/>
		</tags>
	</DefineSprite>
	<swft:pop-map/>
</xsl:template>

<!-- shared library import -->
<xsl:template match="import">
	<swft:push-map/>
	<Import url="{@url}">
		<symbols>
			<xsl:if test="@symbol">
				<Symbol objectID="{swft:next-id()}" name="{@symbol}"/>
			</xsl:if>	
			<xsl:if test="file">
				<xsl:apply-templates select="swft:document(@file)/swf/Header/tags/Export/symbols/*" mode="import"/>
			</xsl:if>
		</symbols>
	</Import>
	<swft:pop-map/>
</xsl:template>

<xsl:template match="Symbol" mode="import">
	<Symbol objectID="{swft:map-id(@objectID)}" name="{@name}"/>
</xsl:template>

<!-- global id remapping -->
<xsl:template match="@objectID|@fontRef|@sprite" mode="idmap">
	<xsl:attribute name="{name()}"><xsl:value-of select="swft:map-id(.)"/></xsl:attribute>
</xsl:template>
<xsl:template match="*|@*|text()" mode="idmap" priority="-1">
	<xsl:copy select=".">
		<xsl:apply-templates select="*|@*|text()" mode="idmap"/>
	</xsl:copy>
</xsl:template>

<!-- for tags that are "globalized" -->
<xsl:template match="SetBackgroundColor" mode="sprite-global"/>
<xsl:template match="DoAction|End|FrameLabel|PlaceObject|PlaceObject2|RemoveObject|RemoveObject2|ShowFrame|SoundStreamBlock|SoundStreamHead|StartSound" mode="sprite-global"/>
<xsl:template match="*|@*|text()" mode="sprite-global" priority="-1">
	<xsl:apply-templates select="." mode="idmap"/>
</xsl:template>

<!-- for tags that move into the DefineSprite -->
<xsl:template match="DoAction|End|FrameLabel|PlaceObject|PlaceObject2|RemoveObject|RemoveObject2|ShowFrame|SoundStreamBlock|SoundStreamHead|StartSound" mode="sprite-local">
	<xsl:apply-templates select="." mode="idmap"/>
</xsl:template>
<xsl:template match="*|@*|text()" mode="sprite-local" priority="-1"/>


<!-- TTF import -->
<xsl:template match="ttf" mode="makeswf">
	<xsl:param name="id"/>
	<DefineFont2 objectID="{$id}" name="{@name}">
		<xsl:apply-templates select="DefineFont2/*|DefineFont2/@*[name() != 'objectID']"/>
	</DefineFont2>
</xsl:template>

<!-- call -->
<xsl:template match="call">
	<xsl:variable name="object">
		<xsl:choose>
			<xsl:when test="@object"><xsl:value-of select="@object"/></xsl:when>
			<xsl:otherwise>Main</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="method">
		<xsl:choose>
			<xsl:when test="@method"><xsl:value-of select="@method"/></xsl:when>
			<xsl:otherwise>main</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="spriteid">
		<xsl:value-of select="swft:next-id()"/>
	</xsl:variable>
	<DefineSprite objectID="{$spriteid}" frames="1">
		<tags>
			<End/>
		</tags>
	</DefineSprite>
	<Export>
		<symbols>
			<Symbol objectID="{$spriteid}" name="__Packages.swfmill.call.{$object}.{$method}"/>
		</symbols>
	</Export>
	<DoInitAction sprite="{$spriteid}">
		<actions>
			<PushData>
				<items>
					<StackString value="{@object}"/>
				</items>
			</PushData>
			<GetVariable/>
			<PushData>
				<items>
					<StackString value="{@method}"/>
				</items>
			</PushData>
			<CallMethod/>
			<Pop/>
			<EndAction/>
		</actions>
	</DoInitAction>
</xsl:template>

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

<xsl:template match="set">
	<xsl:variable name="spriteid">
		<xsl:value-of select="swft:next-id()"/>
	</xsl:variable>
	<DefineSprite objectID="{$spriteid}" frames="1">
		<tags>
			<End/>
		</tags>
	</DefineSprite>
	<Export>
		<symbols>
			<Symbol objectID="{$spriteid}" name="__Packages.swfmill.set.{@name}.{@member}"/>
		</symbols>
	</Export>
	<DoInitAction sprite="{$spriteid}">
		<actions>
			<Dictionary>
				<strings>
					<String value="_root"/>
					<String value="{@name}"/>
					<String value="{@member}"/>
					<String value="{@value}"/>
				</strings>
			</Dictionary>
			<PushData>
				<items>
					<StackDictionaryLookup index="0"/>
				</items>
			</PushData>
			<GetVariable/>
			<PushData>
				<items>
					<StackDictionaryLookup index="1"/>
				</items>
			</PushData>
			<GetMember/>
			<PushData>
				<items>
					<StackDictionaryLookup index="2"/>
					<StackDictionaryLookup index="3"/>
				</items>
			</PushData>
			<SetMember/>
			<EndAction/>
		</actions>
	</DoInitAction>
</xsl:template>

<!-- stop -->
<xsl:template match="stop">
	<DoAction>
		<actions>
			<Stop/>
			<EndAction/>
		</actions>
	</DoAction>
</xsl:template>


<!-- HTML wrapper -->
<xsl:template match="html-wrapper">
	<xsl:document href="{@name}">
<html>
	<body>
	<object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" 
			codebase="http://fpdownload.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=7,0,0,0" 
			width="{/movie/@width}" height="{/movie/@width}" id="video" align="middle">
		<param name="allowScriptAccess" value="sameDomain" />
		<param name="movie" value="{@swf}" />
		<param name="quality" value="high" />
		<embed src="{@swf}" quality="high" 
			width="{/movie/@width}" height="{/movie/@height}" name="video" align="middle" allowScriptAccess="sameDomain" 
			type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer" />
	</object>
	</body>
</html>
	</xsl:document>
</xsl:template>

<!-- copy anything else -->
<xsl:template match="*|@*|text()" priority="-1">
	<xsl:copy select=".">
		<xsl:apply-templates select="*|@*|text()"/>
	</xsl:copy>
</xsl:template>

</xsl:stylesheet>
