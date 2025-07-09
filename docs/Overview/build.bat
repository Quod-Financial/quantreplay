@ECHO OFF

echo Generate Documentation...
if not exist "output" mkdir "output"
del "output\*.html" >nul 2>&1

call asciidoctor "Overview.adoc" -D "output"
echo HTML Done

call asciidoctor-pdf -a allow-uri-read -a pdf-theme="../common/pdf/PDFTheme.yml" "Overview.adoc" -D "output"
echo PDF Done

IF %0 == "%~0" pause
