<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" omit-xml-declaration="yes" indent="no"/>

<xsl:template match="properties">
	<xsl:for-each select="property">
		<xsl:value-of select="concat('SET(ant_',@name,' &quot;',@value,'&quot;)', '&#xa;')" />
	</xsl:for-each>
</xsl:template>

</xsl:stylesheet>