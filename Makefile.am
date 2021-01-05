SUBDIRS = \
	base64 \
	lambda

.PHONY: docs
docs:
	cd '@srcdir@' && doxygen

clean-local:
	cd '@srcdir@' && rm -rf docs/html docs/latex
