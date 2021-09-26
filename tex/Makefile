ENV = LANG=C max_print_line=1000 error_line=254 half_error_line=238
PDFLATEX := texfot pdflatex
MAKE_PDF := \
	$(ENV) latexmk \
		--pdflatex='$(PDFLATEX)' \
		--latexoption=--interaction=nonstopmode \
		--latexoption=--halt-on-error \
		--latexoption=--file-line-error \
		--pdf
WARNINGS := -Wall -Wextra -Wpedantic -Wconversion
CFLAGS := -std=c18 $(WARNINGS)
CXXFLAGS := -std=c++20 $(WARNINGS)
TEX_INPUT := \
	codex.bib *.tex */*.tex
LISTINGS :=

.PHONY: all clean clean-listings link-full link-fast listings
all: codex.pdf listings
codex.pdf: codex.tex $(TEX_INPUT)
codex_fast.pdf: codex_fast.tex $(TEX_INPUT)
codex.pdf codex_fast.pdf:
	$(MAKE_PDF) $<
clean: clean-listings
	latexmk -c codex.tex
	latexmk -c codex_fast.tex
	rm -f codex.{bbl,lol,run.xml} codex_fast.{bbl,lol,run.xml}
clean-listings:
	rm -f $(LISTINGS)
listings: $(LISTINGS)
