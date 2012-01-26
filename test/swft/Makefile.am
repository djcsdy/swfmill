TESTS_ENVIRONMENT = \
	SWFMILL="$(top_builddir)/src/swfmill$(EXEEXT)" \
	XSLTPROC="$(XSLTPROC)" \
	SED="$(SED)" \
	$(SH) "$(srcdir)/run-test.sh"

TESTS = import-jpeg.xsl

CLEANFILES = import-jpeg.xsl.out.xml

EXTRA_DIST = run-test.sh $(TESTS) \
	import-jpeg.xml \
	import-jpeg.xsl.verify.xsl \
	$(NULL)
