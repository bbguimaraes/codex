SUBDIRS = \
	algo \
	arithmetic_folding \
	base64 \
	bit_pattern \
	bsearch \
	c_wrapper \
	event_dispatcher \
	lambda \
	list \
	math \
	packet_reader \
	popcount \
	reflection \
	sort \
	time \
	yaml

.PHONY: docs
docs:
	cd '@srcdir@' && doxygen

clean-local:
	cd '@srcdir@' && rm -rf docs/html docs/latex
