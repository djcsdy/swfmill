<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
				xmlns:swft="http://subsignal.org/swfml/swft"
				xmlns:str="http://exslt.org/strings"
				xmlns:math="http://exslt.org/math"
				xmlns:dc="http://purl.org/dc/elements/1.1/"
				xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
				extension-element-prefixes="swft"
				version='1.0'>

<!-- 
	contains templates for most of the swfml-simple elements 
-->

<xsl:variable name="movie-version">
	<xsl:choose>
		<xsl:when test="movie/@version"><xsl:value-of select="movie/@version"/></xsl:when>
		<xsl:otherwise>7</xsl:otherwise>
	</xsl:choose>
</xsl:variable>

<!-- basic SWF setup -->
<xsl:template match="movie">
	<!-- set defaults for movie -->
	<xsl:variable name="version" select="$movie-version"/>
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
			<xsl:when test="frame"><xsl:value-of select="count(frame)"/></xsl:when>
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
	<xsl:variable name="hasMetaData">
		<xsl:choose>
			<xsl:when test="count(meta) &gt; 0">1</xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="useNetwork">
		<xsl:choose>
			<xsl:when test="@local-access = 'network'">1</xsl:when>
			<xsl:when test="@local-access = 'filesystem'">0</xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="as3">
		<xsl:choose>
			<xsl:when test="@as3">1</xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<swf version="{$version}" compressed="{$compressed}">
		<Header framerate="{$framerate}" frames="{$frames}">
			<size>
				<Rectangle left="{$left}" right="{$right}" top="{$top}" bottom="{$bottom}"/>
			</size>
			<tags>
				<xsl:if test="$version >= 8">
					<FileAttributes hasMetaData="{$hasMetaData}" useNetwork="{$useNetwork}" allowABC="{$as3}"/>
				</xsl:if>
				<xsl:apply-templates/>
				<End/>
			</tags>
		</Header>
	</swf>
</xsl:template>

<!-- background color -->
<xsl:template match="background">
	<SetBackgroundColor>
		<color>
			<xsl:call-template name="color"/>
		</color>
	</SetBackgroundColor>
</xsl:template>

<xsl:template match="meta">
	<Metadata>
		<rdf:RDF>
			<rdf:Description>
				<xsl:apply-templates select="@*" mode="meta"/>
		    </rdf:Description>
			<xsl:copy-of select="*"/>
		</rdf:RDF>
	</Metadata>
</xsl:template>

<xsl:template match="@title" mode="meta">
	<dc:title>
		<xsl:apply-templates/>
	</dc:title>
</xsl:template>

<xsl:template match="@description" mode="meta">
	<dc:description>
		<xsl:apply-templates/>
	</dc:description>
</xsl:template>

<xsl:template match="*|text()" mode="meta"/>

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
    <xsl:variable name="skewx">
            <xsl:choose>
                    <xsl:when test="@skewx"><xsl:value-of select="@skewx"/></xsl:when>
                    <xsl:otherwise>0</xsl:otherwise>
            </xsl:choose>
    </xsl:variable>
    <xsl:variable name="skewy">
            <xsl:choose>
                    <xsl:when test="@skewy"><xsl:value-of select="@skewy"/></xsl:when>
                    <xsl:otherwise>0</xsl:otherwise>
            </xsl:choose>
    </xsl:variable>
    <xsl:variable name="scalex">
            <xsl:choose>
                    <xsl:when test="@scalex"><xsl:value-of select="@scalex"/></xsl:when>
                    <xsl:when test="@scale"><xsl:value-of select="@scale"/></xsl:when>
                    <xsl:otherwise>1</xsl:otherwise>
            </xsl:choose>
    </xsl:variable>
    <xsl:variable name="scaley">
            <xsl:choose>
                    <xsl:when test="@scaley"><xsl:value-of select="@scaley"/></xsl:when>
                    <xsl:when test="@scale"><xsl:value-of select="@scale"/></xsl:when>
                    <xsl:otherwise>1</xsl:otherwise>
            </xsl:choose>
    </xsl:variable>

	<xsl:variable name="depth"><xsl:value-of select="@depth"/></xsl:variable>
	
	<!-- Setting replace="1" doesn't work if we are replacing a sprite.
		 Instead, we remove the previous sprite with RemoveObject2,
		 and increment "morph". This strange requirement isn't
		 documented anywhere, but this is what Flash does, and it
		 works. See bug #409165 on Launchpad. -->
	<xsl:variable name="replace">
		<xsl:choose>
			<xsl:when test="preceding-sibling::place[@depth=$depth] or ../preceding-sibling::frame/place[@depth=$depth]">1</xsl:when>
			<xsl:otherwise>0</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="morph">
		<xsl:value-of select="count(preceding-sibling::place[@depth=$depth] |
				../preceding-sibling::frame/place[@depth=$depth])"/>
	</xsl:variable>
	<xsl:if test="$replace = '1'">
		<RemoveObject2 depth="{@depth}"/>
	</xsl:if>
	<PlaceObject2 replace="0" morph="{$morph}" depth="{$depth}" objectID="{$id}">
		<xsl:if test="@name">
			<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
		</xsl:if>
		<xsl:if test="*">
			<!-- 0x200: initialize -->
			<xsl:attribute name="allflags1">0x200</xsl:attribute>
			<events>
				<Event flags1="0x200">
					<actions>
						<xsl:apply-templates mode="set"/>
						<EndAction/>
					</actions>
				</Event>
				<Event/>
			</events>
		</xsl:if>
		<transform>
			<Transform transX="{$x}" transY="{$y}" scaleX="{$scalex}" scaleY="{$scaley}" skewX="{$skewx}" skewY="{$skewy}"/>
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

    <xsl:variable name="scalex">
            <xsl:choose>
                    <xsl:when test="@scalex"><xsl:value-of select="@scalex"/></xsl:when>
                    <xsl:when test="@scale"><xsl:value-of select="@scale"/></xsl:when>
                    <xsl:otherwise>1</xsl:otherwise>
            </xsl:choose>
    </xsl:variable>
    <xsl:variable name="scaley">
            <xsl:choose>
                    <xsl:when test="@scaley"><xsl:value-of select="@scaley"/></xsl:when>
                    <xsl:when test="@scale"><xsl:value-of select="@scale"/></xsl:when>
                    <xsl:otherwise>1</xsl:otherwise>
            </xsl:choose>
    </xsl:variable>
    <xsl:variable name="skewx">
            <xsl:choose>
                    <xsl:when test="@skewx"><xsl:value-of select="@skewx"/></xsl:when>
                    <xsl:otherwise>0</xsl:otherwise>
            </xsl:choose>
    </xsl:variable>
    <xsl:variable name="skewy">
            <xsl:choose>
                    <xsl:when test="@skewy"><xsl:value-of select="@skewy"/></xsl:when>
                    <xsl:otherwise>0</xsl:otherwise>
            </xsl:choose>
    </xsl:variable>
 
	<xsl:variable name="depth">
		<xsl:value-of select="preceding-sibling::place[@id=$myid]/@depth"/>
	</xsl:variable>
	
	<PlaceObject2 replace="1" depth="{$depth}" objectID="{$id}">
		<transform>
			<Transform transX="{$x}" transY="{$y}" scaleX="{$scalex}" scaleY="{$scaley}" skewX="{$skewx}" skewY="{$skewy}"/>
		</transform>
	</PlaceObject2>
</xsl:template>

<!-- textfield -->
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
	<xsl:variable name="leading">
		<xsl:choose>
			<xsl:when test="@leading"><xsl:value-of select="@leading * 20"/></xsl:when>
			<xsl:otherwise>20</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<DefineEditText objectID="{$id}"
		fontRef="{swft:map-id(@font)}" fontHeight="{$size}"
		readOnly="0" autoSize="0" hasLayout="0"
		notSelectable="0" hasBorder="0" isHTML="0" useOutlines="{@useOutlines}" 
		align="0" leftMargin="0" rightMargin="0" indent="0" leading="{$leading}" 
		wordWrap="1" multiLine="1" password="0" 
		variableName="{@name}" 
		>
		<xsl:for-each select="@wordWrap|@multiLine|@password|@readOnly|@autoSize|@notSelectable|@hasBorder|@isHTML|@useOutlines|@align|@leftMargin|@rightMargin|@indent">
			<xsl:copy-of select="."/>
		</xsl:for-each>
		<xsl:choose>
			<xsl:when test="*|text()">
				<xsl:attribute name="initialText">
					<xsl:apply-templates select="*|text()" mode="htmltext"/>
				</xsl:attribute>
			</xsl:when>
			<xsl:when test="@text">
				<xsl:attribute name="initialText">
					<xsl:value-of select="@text"/>
				</xsl:attribute>
			</xsl:when>
		</xsl:choose>
		<size>
			<Rectangle left="{$x}" right="{$x + $width}" top="{$y}" bottom="{$y + $height}"/>
		</size>
		<color>
			<xsl:call-template name="color-rgba"/>
		</color>
	</DefineEditText>
	
	<xsl:if test="$movie-version &gt; 7">
		<xsl:apply-templates select="text-settings">
			<xsl:with-param name="id" select="$id"/>
		</xsl:apply-templates>
	</xsl:if>

</xsl:template>

<xsl:template match="text-settings">
	<xsl:param name="id" select="0"/>
	
	<CSMTextSettings objectId="{$id}">
		<xsl:attribute name="useFlashType">
			<xsl:choose>
				<xsl:when test="@render = 'advanced'">1</xsl:when>
				<xsl:otherwise>0</xsl:otherwise>
			</xsl:choose>
		</xsl:attribute>
		<xsl:attribute name="gridFit">
			<xsl:choose>
				<xsl:when test="@grid = 'sub-pixel'">2</xsl:when>
				<xsl:when test="@grid = 'pixel'">1</xsl:when>
				<xsl:otherwise>0</xsl:otherwise>
			</xsl:choose>
		</xsl:attribute>
		<xsl:attribute name="thickness">
			<xsl:choose>
				<xsl:when test="@thickness">
					<xsl:choose>
						<xsl:when test="@thickness &lt; -200">
							<xsl:value-of select="-200"/>
						</xsl:when>
						<xsl:when test="@thickness &gt; 200">
							<xsl:value-of select="200"/>
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="@thickness"/>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:when>
				<xsl:otherwise>0</xsl:otherwise>
			</xsl:choose>
		</xsl:attribute>
		<xsl:attribute name="sharpness">
			<xsl:choose>
				<xsl:when test="@sharpness">
					<xsl:choose>
						<xsl:when test="@sharpness &lt; -400">
							<xsl:value-of select="-400"/>
						</xsl:when>
						<xsl:when test="@sharpness &gt; 400">
							<xsl:value-of select="400"/>
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="@sharpness"/>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:when>
				<xsl:otherwise>0</xsl:otherwise>
			</xsl:choose>
		</xsl:attribute>
	</CSMTextSettings>
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


<!-- generic clip (w/o @import, these are handled in simple-import.xslt) -->
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
	<xsl:variable name="frames">
		<xsl:choose>
			<xsl:when test="count(frame)">
				<xsl:value-of select="count(frame)"/>
			</xsl:when>
			<xsl:otherwise>1</xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
	<DefineSprite objectID="{$id}" frames="{$frames}">
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

<!-- scale grid --> 
<xsl:template match="scale-grid">
	<xsl:variable name="id">
		<xsl:value-of select="swft:map-id(@id)"/>
	</xsl:variable>
    <xsl:if test="number(/movie/@version) >= 8"> 
		<DefineScalingGrid objectID="{$id}"> <!-- FIXME ID not Id? does it matter? -->
			<splitter> 
				<Rectangle left="{@left * 20}" right="{@right * 20}" top="{@top * 20}" bottom="{@bottom * 20}"/> 
			</splitter> 
		</DefineScalingGrid>     
	</xsl:if> 
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


</xsl:stylesheet>
