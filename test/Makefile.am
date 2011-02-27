SUBDIRS = xml

classes.swf: Main.as org/swfmill/Foo.as
	mtasc -cp /usr/local/share/mtasc/std -swf $@ -header 320:240:25 Main.as org/swfmill/Foo.as

output.swf: test.swfml classes.swf xml/* library/*
	../src/swfmill$(EXEEXT) -v simple test.swfml $@

check-local: output.swf


CLEANFILES = \
	classes.swf \
	output.swf
