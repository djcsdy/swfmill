TESTS_ENVIRONMENT = \
	SWFMILL="$(top_builddir)/src/swfmill$(EXEEXT)" \
	XSLTPROC="$(XSLTPROC)" \
	SED="$(SED)" \
	$(SH) "$(srcdir)/run-test.sh"

TESTS = import-jpeg.xsl import-png.xsl

CLEANFILES = import-jpeg.xsl.out.xml import-png.xsl.out.xml

EXTRA_DIST = run-test.sh $(TESTS) \
	import-jpeg.xml \
	import-jpeg.xsl.verify.xsl \
	import-png.xml \
	import-png.xsl.verify.xsl \
	$(NULL)
