CTEX=pdflatex
LTEX=./songs-2.12/src/songidx/songidx

TEXFILE=chant_inge
PRINT=print_chant_inge


.phony: all

all: $(PRINT).pdf


$(PRINT).pdf: $(TEXFILE).pdf
	pdftk $(TEXFILE).pdf cat 3 2 2S 3S 3 2 2S 3S output tmp.pdf
	pdfnup --nup 2x2 --no-landscape --frame true --outfile $(PRINT).pdf tmp.pdf
	#pdfnup --nup 2x2 --outfile $(PRINT).pdf tmp.pdf
	#pdf2ps tmp.pdf tmp.ps
	# psnup -pa4 -Pa4 -d1 -4 tmp.ps print.ps
	# ps2pdf print.ps $(PRINT).pdf
	rm tmp.pdf



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
