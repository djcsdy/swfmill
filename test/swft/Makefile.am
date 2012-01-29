TESTS_ENVIRONMENT = \
	SWFMILL="$(top_builddir)/src/swfmill$(EXEEXT)" \
	XSLTPROC="$(XSLTPROC)" \
	SED="$(SED)" \
	$(SH) "$(srcdir)/run-test.sh"

TESTS = import-jpeg.xsl \
	import-jpega.xsl \
	import-png.xsl \
	import-png-with-alpha.xsl \
	$(NULL)

CLEANFILES = import-jpeg.xsl.out.xml \
	import-jpega.xsl.out.xml \
	import-png.xsl.out.xml \
	import-png-with-alpha.xsl.out.xml \
	$(NULL)

EXTRA_DIST = run-test.sh $(TESTS) \
	import-jpeg.xml \
	import-jpeg.xsl.verify.xsl \
	import-jpega.xml \
	import-jpega.xsl.verify.xsl \
	import-png.xml \
	import-png.xsl.verify.xsl \
	import-png-with-alpha.xml \
	import-png-with-alpha.xsl.verify.xsl \
	$(NULL)
