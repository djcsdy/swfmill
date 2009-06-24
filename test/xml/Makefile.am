

.PHONY: test run-tests

check-local: test

test: run-tests

EXTRA_DIST = \
	test-xml \
	$(NULL)

check_SCRIPTS = \
	run-xml-tests \
	$(NULL)

run-tests:
	sh $(srcdir)/run-xml-tests $(srcdir)

CLEANFILES = *.swf *.xml.swf
