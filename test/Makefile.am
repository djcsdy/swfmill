SUBDIRS = xml

classes.swf: Main.as org/swfmill/Foo.as
	mtasc -swf $@ -header 320:240:25 \
		$(srcdir)/Main.as $(srcdir)/org/swfmill/Foo.as

output.swf: test.swfml classes.swf xml/* $(srcdir)/library/*
	../src/swfmill$(EXEEXT) -v simple $(srcdir)/test.swfml $@

check-local: output.swf


CLEANFILES = \
	classes.swf \
	output.swf \
	$(NULL)

EXTRA_DIST = \
	Main.as \
	test.swfml \
	org/swfmill/Foo.as \
	library/star.swf \
	library/testgradient.png \
	library/testjpg.jpg \
	library/testpng.png \
	library/testpng8.png \
	library/testpng24.png \
	library/testpng-alpha.png \
	library/vera.ttf \
	$(NULL)
