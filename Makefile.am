SUBDIRS = \
	arithmetic_folding \
	base64 \
	bit_pattern \
	bsearch \
	c_wrapper \
	event_dispatcher \
	lambda \
	packet_reader \
	popcount \
	time

.PHONY: docs
docs:
	cd '@srcdir@' && doxygen

clean-local:
	cd '@srcdir@' && rm -rf docs/html docs/latex
