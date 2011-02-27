SWFMILL = ../../src/swfmill$(EXEEXT)

SUFFIXES = .swf .xml

.PRECIOUS: %.test %.swf %.swf.xml

%.swf: %.xml $(SWFMILL)
	@$(SWFMILL) xml2swf $< $@

%.swf.xml : %.swf 
	@$(SWFMILL) swf2xml $< $@

.%.test : %.swf.xml 
	diff -q $*.xml $*.swf.xml > $@ || echo FAILED: $*

check-local: $(patsubst %.xml,.%.test,$(filter-out %.swf.xml, $(wildcard *.xml)))

CLEANFILES = *.swf *.swf.xml .*.test
