#!/bin/sh

echo Generate Documentation...
if [ ! -d "output" ]; then
  mkdir "output"
fi
rm -f output/*.html

asciidoctor "FIXRulesOfEngagement.adoc" -a imagesDir="../images" -D "output"
echo HTML Done

asciidoctor-pdf -a allow-uri-read -a pdf-theme="../common/pdf/PDFTheme.yml" "FIXRulesOfEngagement.adoc" -D "output"
echo PDF Done
