<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
				xmlns:keep="http://iterative.org/misc/keep-0.1"
                version='1.0'>

<xsl:strip-space elements="*"/>

<xsl:variable name="series">swfmill</xsl:variable>
<xsl:variable name="url">http://iterative.org/swfmill/</xsl:variable>

<xsl:template match="book">
	<html>
		<head>
			<title><xsl:value-of select="$series"/>: <xsl:value-of select="bookinfo/title"/></title>
			<link rel="stylesheet" href="html.css"/>
			<xsl:apply-templates select="script"/>
		</head>
		<body>
			<xsl:apply-templates select="bookinfo"/>

			<h2>Table of Contents</h2>
			<div id="toc">
				<xsl:apply-templates select="chapter" mode="toc"/>
			</div>
			
			<xsl:apply-templates select="chapter"/>
			
			<hr />
			<xsl:apply-templates select="bookinfo" mode="footer"/>
		</body>
	</html>
</xsl:template>


<!-- Table of Contents -->
<xsl:template match="chapter|section" mode="toc">
	<div class="tocentry">
		<xsl:attribute name="style">padding-left:<xsl:value-of select="15 * (1 + count(ancestor::chapter|ancestor::section))"/>px</xsl:attribute>
		<a href="#{@id}"><xsl:value-of select="title"/></a>
	</div>
	<xsl:apply-templates select="section" mode="toc"/>
</xsl:template>

<!-- contents -->
<xsl:template match="bookinfo">
	<h1><xsl:value-of select="title"/></h1>
	by <xsl:value-of select="author/firstname"/><xsl:text> </xsl:text><xsl:value-of select="author/surname"/>
	&lt;<xsl:value-of select="author/affiliation/address/email"/>&gt;<br />
</xsl:template>

<xsl:template match="bookinfo" mode="footer">
	<small>
		Copyright <xsl:value-of select="copyright/year"/><xsl:text> </xsl:text><xsl:value-of select="copyright/holder"/><br />
		<xsl:apply-templates select="legalnotice"/>
	</small>
</xsl:template>

<xsl:template match="chapter">
	<a name="{@id}" />
	<h2><xsl:value-of select="title"/></h2>
	
	<xsl:apply-templates/>
</xsl:template>

<xsl:template match="section">
	<a name="{@id}" />
	<h3><xsl:value-of select="title"/></h3>
	
	<xsl:apply-templates/>
</xsl:template>

<!-- strip some completely -->
<xsl:template match="title"/>
<!-- strip some partially-->
<xsl:template match="legalnotice">
	<xsl:apply-templates/>
</xsl:template>
<!-- copy all others -->
<xsl:template match="*|@*|text()" priority="-1">
	<xsl:copy select=".">
		<xsl:apply-templates select="*|@*|text()"/>
	</xsl:copy>
</xsl:template>

<!-- code -->
<xsl:template match="code">
	<div class="code">
		<xsl:apply-templates/>
	</div>
</xsl:template>

<!-- xml prettyprint -->
<xsl:template match="xml">
	<div class="xml"><pre>
		<xsl:apply-templates mode="prettyprint"/>
	</pre></div>
</xsl:template>
<xsl:template match="*" mode="prettyprint" priority="-1">
	<!-- indent -->
	<xsl:for-each select="ancestor::*[ancestor::xml]">
		<xsl:text>  </xsl:text>
	</xsl:for-each>
	
	<!-- opening tag -->
	<xsl:text>&lt;</xsl:text>
	<xsl:value-of select="name()"/>
	<xsl:apply-templates select="@*" mode="prettyprint"/>
	<xsl:choose>
		<xsl:when test="*|text()">
			<!-- tag has children -->
			<xsl:text>&gt;</xsl:text>
		</xsl:when>
		<xsl:otherwise>
			<!-- empty tag -->
			<xsl:text>/&gt;</xsl:text>
		</xsl:otherwise>
	</xsl:choose>
	<xsl:text>
</xsl:text>

	<!-- children (if any)-->
	<xsl:apply-templates select="*|text()" mode="prettyprint"/>

	<!-- close opening tag -->
	<xsl:if test="*|text()">
		<xsl:for-each select="ancestor::*[ancestor::xml]">
			<xsl:text>  </xsl:text>
		</xsl:for-each>
		
		<xsl:text>&lt;/</xsl:text>
		<xsl:value-of select="name()"/>
		<xsl:text>&gt;</xsl:text>
		<xsl:text>
</xsl:text>
	</xsl:if>
	
</xsl:template>

<xsl:template match="@*" mode="prettyprint">
	<!-- wrap if attribute list gets too long -->
<!--
	<xsl:variable name="position">
		<xsl:value-of select="position()"/>
	</xsl:variable>
	<xsl:variable name="parent-name">
		<xsl:for-each select="..">
			<xsl:value-of select="name()"/>
		</xsl:for-each>
	</xsl:variable>
	<xsl:variable name="preceding-and-self">
		<xsl:for-each select="ancestor::*[ancestor::xml]">
			<xsl:text>  </xsl:text>
		</xsl:for-each>
		<xsl:text>&lt;</xsl:text>
		<xsl:value-of select="$parent-name"/>
		<xsl:for-each select="../@*[position()&lt;=$position]">
			<xsl:value-of select="name()"/>=""<xsl:value-of select="."/>
		</xsl:for-each>
	</xsl:variable>
	<xsl:if test="string-length($preceding-and-self) &gt; 80">
		<xsl:text> 
    </xsl:text>
		<xsl:for-each select="ancestor::*[ancestor::xml]">
			<xsl:text>  </xsl:text>
		</xsl:for-each>
	</xsl:if>
-->

<!-- wrap every after 1st
	<xsl:if test="name() != 'encoding' and count(../@*)&gt;1 and position()&gt;1">
		<xsl:variable name="parent-name">
			<xsl:for-each select="..">
				<xsl:value-of select="name()"/>
			</xsl:for-each>
		</xsl:variable>
		<xsl:variable name="indent">
			<xsl:for-each select="../ancestor::*[ancestor::xml]">
				<xsl:text>  </xsl:text>
			</xsl:for-each>
			<xsl:text>&lt;</xsl:text>
			<xsl:value-of select="$parent-name"/>
		</xsl:variable>
	<xsl:text> 
</xsl:text>
		<xsl:call-template name="indent">
			<xsl:with-param name="n" select="string-length($indent)"/>
		</xsl:call-template>
	</xsl:if>
-->
	
	<xsl:text> </xsl:text>
	<xsl:value-of select="name()"/>
	<xsl:text>="</xsl:text>
	<xsl:value-of select="."/>
	<xsl:text>"</xsl:text>
</xsl:template>
<xsl:template name="indent">
	<xsl:param name="n"/>
	<xsl:text> </xsl:text>
	<xsl:if test="$n &gt; 1">
		<xsl:call-template name="indent">
			<xsl:with-param name="n" select="$n - 1"/>
		</xsl:call-template>
	</xsl:if>
</xsl:template>

<xsl:template match="text()" mode="prettyprint">
	<xsl:value-of select="."/>
</xsl:template>

<xsl:template match="pi" mode="prettyprint">
	<xsl:text>&lt;?</xsl:text>
	<xsl:value-of select="@name"/>

	<xsl:apply-templates select="@*[name()!='name']" mode="prettyprint"/>
	<xsl:text> ?&gt;</xsl:text>
	<xsl:text>
	
</xsl:text>
</xsl:template>

<xsl:template match="comment" mode="prettyprint">
	<xsl:text>
</xsl:text>
	<!-- indent -->
	<xsl:for-each select="ancestor::*[ancestor::xml]">
		<xsl:text>  </xsl:text>
	</xsl:for-each>
	<xsl:text>&lt;!-- </xsl:text>
	<xsl:value-of select="@name"/>
		<xsl:value-of select="." mode="prettyprint"/> 
	<xsl:text> --&gt;
</xsl:text>
</xsl:template>
</xsl:stylesheet>
