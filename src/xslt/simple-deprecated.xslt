<?xml version="1.0"?>
<xsl:stylesheet	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
				xmlns:swft="http://subsignal.org/swfml/swft"
				xmlns:str="http://exslt.org/strings"
				xmlns:math="http://exslt.org/math"
				extension-element-prefixes="swft"
				version='1.0'>

<!-- set -->
<xsl:template match="set">
	<xsl:if test="$quiet != 'true'">
		<xsl:message>"set" is deprecated, please use assignments in place</xsl:message>
	</xsl:if>
	
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

</xsl:stylesheet>
