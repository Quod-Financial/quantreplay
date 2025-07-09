#!/bin/sh

echo Generate Documentation...
if [ ! -d "output" ]; then
  mkdir "output"
fi
rm -f output/*.html

asciidoctor "Overview.adoc" -D "output"
echo HTML Done

asciidoctor-pdf -a allow-uri-read -a pdf-theme="../common/pdf/PDFTheme.yml" "Overview.adoc" -D "output"
echo PDF Done
