TESTS_ENVIRONMENT = \
	SWFMILL="$(top_builddir)/src/swfmill$(EXEEXT)" \
	XSLTPROC="$(XSLTPROC)"

LOG_COMPILER = $(SH) "$(srcdir)/run-test.sh"

TESTS = placeobject3-import.xml import-happy-ttf.xml

CLEANFILES = placeobject3-import.xml.swf placeobject3-import.xml.swf.xml \
	import-happy-ttf.xml.swf import-happy-ttf.xml.swf.xml

EXTRA_DIST = run-test.sh $(TESTS) \
	placeobject3.swf \
	placeobject3-import.xml.verify.xsl \
	happy.ttf \
	import-happy-ttf.xml.verify.xsl \
	$(NULL)
