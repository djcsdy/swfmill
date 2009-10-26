<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<xsl:template match="dialect">
	<book id="swfml-simple-ref">
		<bookinfo>
			<title>swfml simple profile</title>
			<author>
				<surname>Fischer</surname>
				<firstname>Daniel</firstname>
				<affiliation>
					<address>
						<email>dan[at]subsignal.org</email>
					</address>
				</affiliation>
			</author>
			<copyright>
				<year>2005</year>
				<holder>Daniel Fischer</holder>
			</copyright>
			<legalnotice>
				Permission is granted to copy, distribute and/or modify this document
				under the terms of the GNU Free Documentation License, Version 1.2
				or any later version published by the Free Software Foundation;
				with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
				A copy of the license is included in the distribution in doc/licenses/fdl.txt,
				or can be obtained from <a href="http://www.fsf.org/licenses/fdl.html">http://www.fsf.org/licenses/fdl.html</a>.<br />
				The author shall not be held responsible for content of other websites linked from here.
			</legalnotice>
		</bookinfo>

		<script language="JavaScript">
			function mouseOver( id ) {
				document.getElementById(id).style.background="#eeeeee";
			}
			function mouseOut( id ) {
				document.getElementById(id).style.background="";
			}
			function toggleCollapse( id ) {
				var node = document.getElementById(id+"-attributes");
				if( node.style.display == "inline" ) {
					node.style.display = "none";
				} else {
					node.style.display = "inline";
				}
			}
		</script>

		<chapter id="intro">
			<title>Introduction</title>
			<p>This is the reference to the <b>swfml simple profile</b> XML dialect.
			<a href="http://iterative.org/swfmill/">swfmill</a> can convert from this dialect to binary SWF or to
			swfml basic profile XML.
			</p>
		</chapter>
		<chapter id="tags">
			<title>Tags</title>
			<table cellspacing="0" cellpadding="2" border="0">
				<xsl:apply-templates select="tag">
					<xsl:sort select="@name" data-type="text"/>
				</xsl:apply-templates>
			</table>
		</chapter>
	</book>
</xsl:template>

<xsl:template match="tag">
	<tr class="tag" id="{@name}" 
		onmouseover="mouseOver('{@name}')"
		onmouseout="mouseOut('{@name}')"
		onclick="toggleCollapse('{@name}')"
		>
		<td valign="top">
			<a name="{@name}" />
			<p>
				<xsl:value-of select="@name"/>
			</p>
		</td>
		<td valign="top">
			<p>
				<xsl:apply-templates select="*|text()" mode="desc"/>
			</p>
			<table id="{@name}-attributes" class="attributes" cellspacing="0" cellpadding="2" border="0">
				<xsl:apply-templates select="attribute">
					<xsl:sort select="@name" data-type="text"/>
				</xsl:apply-templates>
				<xsl:if test="count(attribute) = 0">
					<tr class="attr">
						<td>(no Attributes)</td>
					</tr>
				</xsl:if>
			</table>
		</td>
	</tr>
</xsl:template>

<xsl:template match="attribute">
	<tr class="attr">
		<td valign="top">
			<a name="{ancestor::tag/@name}_{@name}" />
			<xsl:value-of select="@name"/>
		</td>
		<td valign="top">
			<xsl:apply-templates select="*|text()" mode="desc"/>
		</td>
	</tr>
</xsl:template>


<xsl:template match="ref[@attr]" mode="desc">
	<xsl:variable name="tag">
		<xsl:choose>
			<xsl:when test="@tag">
				<xsl:value-of select="@tag"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="ancestor::tag/@name"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<a href="#{$tag}_{@attr}"><xsl:value-of select="@attr"/></a>
</xsl:template>

<xsl:template match="ref" mode="desc" priority="-1">
	<xsl:variable name="tag">
		<xsl:choose>
			<xsl:when test="@tag">
				<xsl:value-of select="@tag"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="ancestor::tag/@name"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<a href="#{$tag}"><xsl:value-of select="$tag"/></a>
</xsl:template>

<xsl:template match="text()" mode="desc">
	<xsl:copy-of select="."/>
</xsl:template>
<xsl:template match="*|@*|text()" mode="desc" priority="-1"/>

</xsl:stylesheet>
