<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<xsl:template match="format">
	<book id="sample">
		<bookinfo>
			<title>swfml basic profile</title>
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

		<chapter id="intro">
			<title>Introduction</title>
			<p>This is the reference to the <b>swfml basic profile</b> XML dialect.
			<a href="http://iterative.org/swfmill/">swfmill</a> can convert from binary SWF to and from this dialect.
			It is very closely modeled after the SWF file format, which means that you can produce all kinds of illegal SWFs with it,
			so be careful.
			</p>
			
			<section id="document">
				<title>About this Document</title>
				<p>This reference lists all types, tags and actions available in swfml basic.
				It is organized into chapters, first listing <a href="#types">basic types</a>, 
                then <a href="#tags">Tags</a> and <a href="#actions">Actions</a>.
				All tables are in alphabetical order.
				The difference of internal and basic types should be irrelevant to the swfml user.
				</p>
				
				<section id="sample-entry">
					<title>Example entry</title>
					<p>Let's look at the <a href="#tag_Import">Import</a> tag as an example.
					It's entry looks like this:
						<table cellspacing="1" border="0">
							<tr>
								<th align="left">Tag Name</th>
								<th align="left">ID</th>
								<th align="left">Attributes</th>
							</tr>
							<tr>
								<td valign="top">
									Import
								</td>
								<td valign="top">0x39</td>
								<td valign="top">
									url (string)<br />
									symbols (List of <a href="#type_Symbol">Symbol</a>s)<br />
								</td>
							</tr>
						</table>
					</p>
					<p>
					This tells us that there is a swfml tag called "Import" which has two
					parameters: url and symbols. You can safely ignore the ID, it is given
					for reference purposes only. 
					Note, though, that "Symbol" is a link to a type definition, which looks like this:
						<table cellspacing="1" border="0">
							<tr>
								<th align="left">Type Name</th>
								<th align="left">Attributes</th>
							</tr>
							<tr>
								<td valign="top">
									Symbol
								</td>
								<td valign="top">
									objectID (word)<br />
									name (string)<br />
								</td>
							</tr>
						</table>
					</p>
					<p>
					We now have enough information to construct an Import tag, it could look like this:
					<xml>
						<Import url="http://foo.com/mylibrary.swf">
							<symbols>
								<Symbol objectID="1" name="myFoo"/>
								<Symbol objectID="2" name="myBar"/>
							</symbols>
						</Import>
					</xml>
					</p>
					<p>
					Note that, while primitive types like string must be given
					as <i>Attributes</i>, Lists and Object parameters become <i>Child Elements</i>
					of our tag instance.
					</p>
				</section>
			</section>
		</chapter>
		<chapter id="types">
			<title>Basic Types</title>
			
			<table cellspacing="1" border="0">
				<tr>
					<th align="left">Type Name</th>
					<th align="left">Attributes</th>
				</tr>
				<xsl:apply-templates select="type">
					<xsl:sort select="@name" data-type="text"/>
				</xsl:apply-templates>
			</table>
		</chapter>
		<chapter id="tags">
			<title>Tags</title>
			<a name="type_Tag"/>
			<table cellspacing="1" border="0">
				<tr>
					<th align="left">Tag Name</th>
					<th align="left">ID</th>
					<th align="left">Attributes</th>
				</tr>
				<xsl:apply-templates select="tag">
					<xsl:sort select="@name" data-type="text"/>
				</xsl:apply-templates>
			</table>
		</chapter>
		<chapter id="actions">
			<title>Actions</title>
			<a name="type_Action"/>
			<table cellspacing="1" border="0">
				<tr>
					<th align="left">Action Name</th>
					<th align="left">ID</th>
					<th align="left">Attributes</th>
				</tr>
				<xsl:apply-templates select="action">
					<xsl:sort select="@name" data-type="text"/>
				</xsl:apply-templates>
			</table>
		</chapter>
	</book>
</xsl:template>

<xsl:template match="type">
	<tr>
		<td valign="top">
			<a name="{name()}_{@name}" />
			<xsl:value-of select="@name"/>
		</td>
		<td valign="top">
			<xsl:apply-templates select="*" mode="attributes"/>
		</td>
	</tr>
</xsl:template>

<xsl:template match="tag|action">
	<tr>
		<td valign="top">
			<a name="{name()}_{@name}" />
			<xsl:value-of select="@name"/>
		</td>
		<td valign="top"><xsl:value-of select="@id"/></td>
		<td valign="top">
			<xsl:apply-templates select="*" mode="attributes"/>
		</td>
	</tr>
</xsl:template>


<!-- definition contents propre -->
<xsl:template match="*[@prop]" mode="attributes">
	<xsl:value-of select="@name"/>
			(<xsl:apply-templates select="." mode="type"/>)
	<br />
</xsl:template>


<!-- definition contents that vanish, but retain children -->
<xsl:template match="if|flagged" mode="attributes">
	<xsl:apply-templates/>
</xsl:template>

<!-- all other definition contents vanish-->
<xsl:template match="*" mode="attributes" priority="-1"/>


<!-- type specifiers -->
<xsl:template match="object" mode="type">Object of type
	<a href="#type_{@type}"><xsl:value-of select="@type"/></a>
</xsl:template>
<xsl:template match="list" mode="type">List of
	<a href="#type_{@type}"><xsl:value-of select="@type"/></a>s</xsl:template>
<xsl:template match="*" mode="type" priority="-1">
	<xsl:value-of select="name()"/>
</xsl:template>


</xsl:stylesheet>
