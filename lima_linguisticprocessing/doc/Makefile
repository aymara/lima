#
# Makefile for LinguisticProcessing documentation
#  cree par besancon (besanconr@zoe.cea.fr) le 28/06/05 
#

HTML_FILES=spec-namedEntities.html spec-idiomatic.html compile-idiomatic.html docTL.html linguisticProcessing.html specifs-fonctionnelles-analyse-syntaxique-lima1.2.html specif-techniques.html squelette-composant.html specif_regles.html lima-user-manual.html

all: index.html 
	@echo All done.

$(HTML_FILES): %.html: %.xml
	xmlto html-nochunks $<

html:
	@cd ..; doxygen Doxyfile

index.html: $(HTML_FILES) html create-index-html.pl
	./create-index-html.pl

clean:
	@/bin/rm -rf html *.html *~
