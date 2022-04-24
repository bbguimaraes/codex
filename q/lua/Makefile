CXXFLAGS = -std=c++20
LDLIBS = -llua

test: \
	test.cpp \
	gen.hpp \
	lexer.hpp \
	lua.hpp \
	parser.hpp \
	print.hpp \
	reader.hpp \
	utils.hpp
test: test.cpp
	$(LINK.cpp) $< $(LDLIBS) -o $@
