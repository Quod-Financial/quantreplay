#!/bin/sh

echo Generate Documentation...
if [ ! -d "output" ]; then
  mkdir "output"
fi
rm -f output/*.html

asciidoctor "RESTAPI.adoc" -a imagesDir="../images" -D "output"
echo HTML Done

asciidoctor-pdf -a allow-uri-read -a pdf-theme="../common/pdf/PDFTheme.yml" "RESTAPI.adoc" -D "output"
echo PDF Done
