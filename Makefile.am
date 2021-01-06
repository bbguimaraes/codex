SUBDIRS = \
	base64 \
	bit_pattern \
	lambda

.PHONY: docs
docs:
	cd '@srcdir@' && doxygen

clean-local:
	cd '@srcdir@' && rm -rf docs/html docs/latex
