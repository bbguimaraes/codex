AUTOMAKE_OPTIONS = subdir-objects

AM_CFLAGS = \
	-std=c11 -O2 \
	-Wall -Wextra -Wno-switch -pedantic
AM_LDFLAGS = -lcjson -lyaml

bin_PROGRAMS = yaml

noinst_HEADERS = yaml.h

yaml_SOURCES = \
	text.c \
	json.c \
	yaml.c \
	main.c

%.moc.cpp: %.hpp
	moc -o $@ $^
