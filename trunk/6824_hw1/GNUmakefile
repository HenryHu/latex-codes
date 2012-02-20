PAPER = p
EVERYTHING = $(PAPER).pdf abstract.txt

OURENV = TEXINPUTS="sty:" 

LATEX = $(OURENV) pdflatex
BIBTEX = bibtex --min-crossrefs=100

all: 
	pdflatex article.tex

TEX  := $(wildcard *.tex)
BIBS := $(wildcard *.bib)
FIGS := $(wildcard *.fig)
DEPS := $(TEX) $(BIBS) $(patsubst %.fig,%.pdf,$(FIGS))

RERUN = egrep -q '(^LaTeX Warning:|\(natbib\)).* Rerun'
UNDEFINED = egrep -q '^(LaTeX|Package natbib) Warning:.* undefined'

$(PAPER).pdf: $(DEPS)
	test ! -s $(PAPER).aux || $(BIBTEX) $(PAPER) || rm -f $(PAPER).aux $(PAPER).bbl
	$(LATEX) $(PAPER)
	@if test ! -f $(PAPER).bbl || $(RERUN) $(PAPER).log \
			|| $(UNDEFINED) $(PAPER).log; then \
		set -x; \
		$(BIBTEX) $(PAPER); \
		$(LATEX) $(PAPER); \
		$(LATEX) $(PAPER); \
		$(LATEX) $(PAPER); \
	fi
	test ! -f .xpdf-running || xpdf -remote $(PAPER)-server -reload
	touch $(PAPER).dvi

spell: clean
	for i in $(TEX); do aspell -p ./aspell.words -c $$i; done
	for i in $(TEX); do perl double.pl $$i; done
	( head -1 aspell.words ; tail -n +2 aspell.words | sort ) > aspell.words~
	mv aspell.words~ aspell.words

clean:
	-rm -f *.aux *.dvi *.log *.blg *.bbl *.bak *.lof *.lot *.toc *.brf *.out
	-rm -f $(EVERYTHING)

#abstract.txt: abstract.tex
#	cat abstract.tex | GREP_OPTIONS="" grep -v ^% | GREP_OPTIONS="" grep -v '{abstract}' | sed -e 's,{\\name},iTarget,g'| sed -e 's,\\[a-zA-Z0-9]*{,,g' | sed -e 's,},,g' | sed -e 's,\\%,%,g' | fmt -w72 > abstract.txt

.PHONY: clean spell
