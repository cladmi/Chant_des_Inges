CTEX=pdflatex
LTEX=./songs-2.12/src/songidx/songidx

TEXFILE=chant_inge
PRINT=print_chant_inge


.phony: all

all: $(PRINT).pdf


$(PRINT).pdf: $(TEXFILE).pdf
	pdftk $(TEXFILE).pdf cat 2 1 1S 2S 2 1 1S 2S output tmp.pdf
	pdfnup --nup 2x2 --no-landscape --frame true --outfile $(PRINT).pdf tmp.pdf
	rm tmp.pdf



$(TEXFILE).pdf: titleidx.sbx songs.sty
	$(CTEX) $(TEXFILE).tex
	pdftk $(TEXFILE).pdf cat 2 3 output tmp.pdf
	cp ${TEXFILE}.pdf aux.pdf
	mv tmp.pdf $(TEXFILE).pdf
	rm -f $(TEXFILE).aux $(TEXFILE).log titleidx.sxd titleidx.sbx

titleidx.sbx: titleidx.sxd
	$(LTEX) titleidx.sxd titleidx.sbx

titleidx.sxd: $(TEXFILE).tex
	$(CTEX) $(TEXFILE).tex


clean:
	rm -f $(TEXFILE).aux $(TEXFILE).log titleidx.sxd titleidx.sbx
	rm -f $(TEXFILE).pdf
