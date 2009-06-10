<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
				xmlns:swft="http://subsignal.org/swfml/swft"
				xmlns:str="http://exslt.org/strings"
				xmlns:math="http://exslt.org/math"
				extension-element-prefixes="swft"
				version='1.0'>


<xsl:template match="clip[@import]|sound[@import]">
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
	<xsl:variable name="mask">
		<xsl:value-of select="@mask"/>
	</xsl:variable>
	<xsl:variable name="ext">
		<xsl:value-of select="translate(substring(@import,string-length(@import)-2),'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')"/>
	</xsl:variable>
	<xsl:choose>
		<xsl:when test="@mask">
			<xsl:apply-templates select="swft:import-jpega($file,$mask)" mode="makeswf">
				<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:when test="$ext = 'jpg' or $ext = 'peg'">
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
		<!-- this is stupid, shouldnt be used.
		<xsl:when test="$ext = 'ttf'">
			<xsl:apply-templates select="swft:import-ttf($file,@glyphs,'hello')" mode="makeswf">
				<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
			</xsl:apply-templates>
		</xsl:when>
		-->
		<xsl:when test="$ext = 'svg'">
			<swft:push-map/>
			<xsl:apply-templates select="document($file)" mode="svg">
				<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
				<xsl:with-param name="export">
					<xsl:choose>
						<xsl:when test="@export"><xsl:value-of select="@export"/></xsl:when>
						<xsl:otherwise>all</xsl:otherwise>
					</xsl:choose>
				</xsl:with-param>
			</xsl:apply-templates>
			<swft:pop-map/>
		</xsl:when>
		<xsl:when test="$ext = 'mp3'">
			<xsl:apply-templates select="swft:import-mp3($file)" mode="makeswf">
				<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:when test="$ext = 'wav'">
			<xsl:apply-templates select="swft:import-wav($file)" mode="makeswf">
				<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:otherwise>
			<xsl:message>WARNING: Cannot import <xsl:value-of select="$file"/> (unknown extension), skipping.</xsl:message>
		</xsl:otherwise>
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

<xsl:template match="bitmap[@import]">
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
	<xsl:variable name="mask">
		<xsl:value-of select="@mask"/>
	</xsl:variable>
	<xsl:variable name="ext">
		<xsl:value-of select="translate(substring(@import,string-length(@import)-2),'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')"/>
	</xsl:variable>
	<xsl:choose>
		<xsl:when test="@mask">
			<xsl:apply-templates select="swft:import-jpega($file,$mask)" mode="bitmap">
				<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:when test="$ext = 'jpg' or $ext = 'peg'">
			<xsl:apply-templates select="swft:import-jpeg($file)" mode="bitmap">
				<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:when test="$ext = 'png'">
			<xsl:apply-templates select="swft:import-png($file)" mode="bitmap">
				<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:otherwise>
			<xsl:message>WARNING: Cannot import <xsl:value-of select="$file"/> (unknown extension), skipping.</xsl:message>
		</xsl:otherwise>
	</xsl:choose>
	<xsl:if test="ancestor::library">
		<xsl:apply-templates select="*|@*" mode="export">
			<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
		</xsl:apply-templates>
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
	<xsl:variable name="offset">
		<xsl:choose>
			<xsl:when test="@symbol='true'">
				<xsl:value-of select="-61440"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="0"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:apply-templates select="swft:import-ttf($file,$movie-version,@glyphs,@name,$offset)" mode="makeswf">
		<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
	</xsl:apply-templates>

	<xsl:if test="ancestor::library">
		<xsl:apply-templates select="*|@*" mode="export">
			<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
		</xsl:apply-templates>
	</xsl:if>
</xsl:template>

<xsl:template match="binary[@import]">
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
	
	<xsl:apply-templates select="swft:import-binary($file)" mode="makeswf">
		<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
	</xsl:apply-templates>

	<xsl:if test="ancestor::library">
		<xsl:apply-templates select="*|@*" mode="export">
			<xsl:with-param name="id"><xsl:value-of select="$id"/></xsl:with-param>
		</xsl:apply-templates>
	</xsl:if>
</xsl:template>

<!-- device fonts -->
<xsl:template match="font" priority="-1">
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
	<DefineFont2 objectID="{$id}" isShiftJIS="0" isUnicode="0" isANSII="0" wideGlyphOffsets="0" italic="0" bold="0" language="1" name="{@name}">
	</DefineFont2>
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

<!-- JPEG-A import -->
<xsl:template match="jpega" mode="makeswf">
	<xsl:param name="id"/>
	<xsl:variable name="bitmapID"><xsl:value-of select="swft:next-id()"/></xsl:variable> 
	<xsl:variable name="shapeID"><xsl:value-of select="swft:next-id()"/></xsl:variable> 
	<DefineBitsJPEG3 objectID="{$bitmapID}" offset_to_alpha="{@offset_to_alpha}">
		<data>
			<xsl:copy-of select="data"/>
		</data>
	</DefineBitsJPEG3>
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
	<DefineBitsLossless2 format="{@format}" width="{@width}" height="{@height}" n_colormap="{@n_colormap}" objectID="{$bitmapID}">
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


<!-- direct bitmap import (not placing them in a clip) -->
<xsl:template match="jpeg" mode="bitmap">
	<xsl:param name="id"/>
	<DefineBitsJPEG2 objectID="{$id}">
		<data>
			<xsl:copy-of select="data"/>
		</data>
	</DefineBitsJPEG2>
</xsl:template>

<xsl:template match="jpega" mode="bitmap">
	<xsl:param name="id"/>
	<DefineBitsJPEG3 objectID="{$id}" offset_to_alpha="{@offset_to_alpha}">
		<data>
			<xsl:copy-of select="data"/>
		</data>
	</DefineBitsJPEG3>
</xsl:template>

<xsl:template match="png" mode="bitmap">
	<xsl:param name="id"/>
	<DefineBitsLossless2 format="{@format}" width="{@width}" height="{@height}" n_colormap="{@n_colormap}" objectID="{$id}">
		<data>
			<xsl:copy-of select="data"/>
		</data>
	</DefineBitsLossless2>
</xsl:template>


<!-- SWF import -->
<xsl:template match="swf" mode="makeswf">
	<xsl:param name="id"/>
	<swft:push-map/>

	<xsl:apply-templates select="Header/tags/*" mode="sprite-global"/>
	
	<DefineSprite objectID="{$id}" frames="{count(Header/tags/ShowFrame)}">
		<tags>
			<xsl:apply-templates select="Header/tags/*" mode="sprite-local"/>
		</tags>
	</DefineSprite>
	<swft:pop-map/>
</xsl:template>

<!-- binary import -->
<xsl:template match="binary" mode="makeswf">
	<xsl:param name="id"/>
	<DefineBinaryData objectID="{$id}">
		<data>
			<xsl:copy-of select="data"/>
		</data>
	</DefineBinaryData>
</xsl:template>

<!-- global id remapping -->
<xsl:template match="@objectID|@fontRef|@sprite" mode="idmap">
	<xsl:attribute name="{name()}"><xsl:value-of select="swft:map-id(.)"/></xsl:attribute>
</xsl:template>
<xsl:template match="text()" mode="idmap" priority="-1">
	<xsl:copy select="."/>
</xsl:template>
<xsl:template match="@*" mode="idmap" priority="-1">
	<xsl:copy select="."/>
</xsl:template>
<xsl:template match="*" mode="idmap" priority="-1">
	<xsl:copy select=".">
		<xsl:apply-templates select="@*" mode="idmap"/>
		<xsl:apply-templates mode="idmap"/>
	</xsl:copy>
</xsl:template>

<!-- for tags that are "globalized" -->
<xsl:template match="SetBackgroundColor|FileAttributes" mode="sprite-global"/>
<xsl:template match="DoAction|End|FrameLabel|PlaceObject|PlaceObject2|RemoveObject|RemoveObject2|ShowFrame|SoundStreamBlock|SoundStreamHead|StartSound" mode="sprite-global"/>
<xsl:template match="*|@*|text()" mode="sprite-global" priority="-1">
	<xsl:apply-templates select="." mode="idmap"/>
</xsl:template>

<!-- for tags that move into the DefineSprite -->
<xsl:template match="DoAction|End|FrameLabel|PlaceObject|PlaceObject2|RemoveObject|RemoveObject2|ShowFrame|SoundStreamBlock|SoundStreamHead|StartSound" mode="sprite-local">
	<xsl:apply-templates select="." mode="idmap"/>
</xsl:template>
<xsl:template match="*|@*|text()" mode="sprite-local" priority="-1"/>



<!-- component import (experimental) -->
<xsl:template match="component">
	
	<xsl:variable name="name" select="@id"/>
	<xsl:variable name="component" select="swft:document(@file)"/>

	<swft:push-map/>
		<xsl:apply-templates select="$component/swf/Header/tags/*" mode="component"/>
		<xsl:variable name="id" select="swft:map-id($component/swf/Header/tags/Export/symbols/Symbol[@name = $name]/@objectID)"/>
	<swft:pop-map/>

	<xsl:value-of select="swft:set-map($name,$id)"/>
	<xsl:message>New ID of "<xsl:value-of select="$name"/>" component is: <xsl:value-of select="$id"/></xsl:message>

</xsl:template>

<xsl:template match="PlaceObject2|ShowFrame|End" mode="component"/>
<xsl:template match="*" mode="component" priority="-1">
	<xsl:apply-templates select="." mode="idmap"/>
</xsl:template>


<!-- shared library import -->
<xsl:template match="import">
	<xsl:choose>
		<xsl:when test="number(/movie/@version) >= 8">
			<ImportAssets2 url="{@url}" reserved1="1" reserved2="0">
				<symbols>
					<xsl:if test="@symbol">
						<Symbol objectID="{swft:map-id(@symbol)}" name="{@symbol}"/>
					</xsl:if>	
					<xsl:if test="@file">
						<xsl:apply-templates select="swft:document(@file)/swf/Header/tags/Export/symbols/*" mode="import"/>
					</xsl:if>
				</symbols>
			</ImportAssets2>
		</xsl:when>
		<xsl:otherwise>
			<Import url="{@url}">
				<symbols>
					<xsl:if test="@symbol">
						<Symbol objectID="{swft:map-id(@symbol)}" name="{@symbol}"/>
					</xsl:if>	
					<xsl:if test="@file">
						<xsl:apply-templates select="swft:document(@file)/swf/Header/tags/Export/symbols/*" mode="import"/>
					</xsl:if>
				</symbols>
			</Import>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="Symbol" mode="import">
	<Symbol objectID="{swft:map-id(@name)}" name="{@name}"/>
</xsl:template>



<!-- TTF import -->
<xsl:template match="ttf" mode="makeswf">
	<xsl:param name="id"/>
	<xsl:choose>
		<xsl:when test="$movie-version >= 8">
			<DefineFont3 objectID="{$id}">
				<xsl:copy-of select="DefineFont3/*|DefineFont3/@*[name() != 'objectID']"/>
			</DefineFont3>
		</xsl:when>
		<xsl:otherwise>
			<DefineFont2 objectID="{$id}">
				<xsl:copy-of select="DefineFont2/*|DefineFont2/@*[name() != 'objectID']"/>
			</DefineFont2>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- MP3 import -->
<xsl:template match="mp3" mode="makeswf">
	<xsl:param name="id"/>
	<DefineSound objectID="{$id}" format="{@format}" rate="{@rate}" is16bit="{@is16bit}" stereo="{@stereo}" samples="{@samples}">
		<data>
			<xsl:copy-of select="data"/>
		</data>
	</DefineSound>
</xsl:template>

<!-- wav import -->
<xsl:template match="wav" mode="makeswf">
	<xsl:param name="id"/>
	<DefineSound objectID="{$id}" format="{@format}" rate="{@rate}" is16bit="{@is16bit}" stereo="{@stereo}" samples="{@samples}">
		<data>
			<xsl:copy-of select="data"/>
		</data>
	</DefineSound>
</xsl:template>

<xsl:template match="include[@file]">
	<xsl:apply-templates select="document(@file)/movie/*"/>	
</xsl:template>

</xsl:stylesheet>
