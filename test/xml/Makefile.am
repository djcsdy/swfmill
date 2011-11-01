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

EXTRA_DIST = \
	as3.xml \
	car_smash.xml \
	clip_as_button2.xml \
	clip_as_button.xml \
	counter.xml \
	display_list_test.xml \
	dlist_test1.xml \
	event-test1.xml \
	extended_clipping_test_1.xml \
	GotoAndPlayTest.xml \
	gradient-tests.xml \
	input-fields.xml \
	money3.xml \
	movieclip_test.xml \
	player.xml \
	shared-fonts.xml \
	slider.xml \
	sound1.xml \
	sr2_title.xml \
	subshapes.xml \
	test_action_order2.xml \
	test_action_order.xml \
	test_basic_types.xml \
	test_button_functions.xml \
	test_cascaded_clipbuttons.xml \
	test_clipping_layer.xml \
	test_colour_tweening.xml \
	test_forin_array.xml \
	test_frame1.xml \
	test_frame2.xml \
	test_function2.xml \
	TestFunction2.xml \
	test_goto_frame.xml \
	test_goto_play.xml \
	test_goto_stop.xml \
	test_gradients_alpha.xml \
	test_gradients_no_alpha.xml \
	test_gradient_tweening.xml \
	test_long_static_text.xml \
	test_rotation2.xml \
	test_rotation_shear.xml \
	test_rotation.xml \
	test_shape_tweening-2.xml \
	test_shape_tweening.xml \
	test_string.xml \
	test_undefined_v6.xml \
	test_undefined_v7.xml \
	test.xml \
	text_formatting.xml \
	text_sizes.xml \
	text-test2.xml \
	text-test.xml \
	visible_and_transparency.xml \
	$(NULL)
