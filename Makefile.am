SUBDIRS = \
	base64 \
	bit_pattern \
	c_wrapper \
	event_dispatcher \
	lambda \
	packet_reader

.PHONY: docs
docs:
	cd '@srcdir@' && doxygen

clean-local:
	cd '@srcdir@' && rm -rf docs/html docs/latex
