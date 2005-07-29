<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
				xmlns:swft="http://subsignal.org/swfml/swft"
				xmlns:str="http://exslt.org/strings"
				xmlns:math="http://exslt.org/math"
				extension-element-prefixes="swft"
				version='1.0'>

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
			<xsl:when test="string-length(@color) = 9">0x<xsl:value-of select="substring(@color,8)"/></xsl:when>
			<xsl:otherwise>255</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<Color red="{$red}" green="{$green}" blue="{$blue}" alpha="{$alpha}"/>
</xsl:template>

<!-- set primitive types with SetVariable, for PlaceObject2 events -->
<xsl:template match="string" mode="set">
	<PushData>
		<items>
			<StackString value="{@name}"/>
			<StackString value="{@value}"/>
		</items>
	</PushData>
	<SetVariable/>
</xsl:template>
<xsl:template match="number" mode="set">
	<PushData>
		<items>
			<StackString value="{@name}"/>
			<StackDouble value="{@value}"/>
		</items>
	</PushData>
	<SetVariable/>
</xsl:template>
<xsl:template match="boolean" mode="set">
	<PushData>
		<items>
			<StackString value="{@name}"/>
			<StackBoolean value="{@value}"/>
		</items>
	</PushData>
	<SetVariable/>
</xsl:template>


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