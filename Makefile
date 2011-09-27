CTEX=pdflatex
LTEX=./songs-2.12/src/songidx/songidx

TEXFILE=chant_inge


.phony: all

all: $(TEXFILE).pdf



$(TEXFILE).pdf: titleidx.sbx songs.sty
	$(CTEX) $(TEXFILE).tex
	rm -f $(TEXFILE).aux $(TEXFILE).log titleidx.sxd titleidx.sbx

titleidx.sbx: titleidx.sxd
	$(LTEX) titleidx.sxd titleidx.sbx

titleidx.sxd: $(TEXFILE).tex
	$(CTEX) $(TEXFILE).tex


clean:
	rm -f $(TEXFILE).aux $(TEXFILE).log titleidx.sxd titleidx.sbx
	rm -f $(TEXFILE).pdf
