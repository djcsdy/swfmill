TESTS_ENVIRONMENT = \
	SWFMILL="$(top_builddir)/src/swfmill$(EXEEXT)" \
	XSLTPROC="$(XSLTPROC)" \
	$(SH) "$(srcdir)/run-test.sh"

TESTS = placeobject3-import.xml

CLEANFILES = placeobject3-import.xml.swf placeobject3-import.xml.swf.xml

EXTRA_DIST = run-test.sh $(TESTS) \
	placeobject3.swf \
	placeobject3-import.xml.verify.xsl \
	$(NULL)
