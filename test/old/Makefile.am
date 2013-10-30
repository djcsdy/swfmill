TESTS_ENVIRONMENT = \
	MTASC="$(MTASC)" \
	SWFMILL="$(top_builddir)/src/swfmill$(EXEEXT)" \
	$(NULL)

TESTS = test.sh


EXTRA_DIST = \
	test.sh \
	input.xml \
	expected-output.xml \
	src/Main.as \
	src/org/swfmill/Foo.as \
	library/star.swf \
	library/testgradient.png \
	library/testjpg.jpg \
	library/testpng.png \
	library/testpng8.png \
	library/testpng24.png \
	library/testpng-alpha.png \
	library/vera.ttf \
	$(NULL)

CLEANFILES = \
	output.html \
	build/output.swf \
	build/output.xml \
	build/output.diff \
	build/classes.swf \
	build/input.xml \
	build/library/star.swf \
	build/library/testgradient.png \
	build/library/testjpg.jpg \
	build/library/testpng.png \
	build/library/testpng8.png \
	build/library/testpng24.png \
	build/library/testpng-alpha.png \
	build/library/vera.ttf \
	$(NULL)
