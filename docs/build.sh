#!/usr/bin/env bash
# Builds the HTML and PDF documentation in place, next to each document's sources.
#
# Usage: build.sh [Overview|FIXRulesOfEngagement|RESTAPI ...]   (no argument builds all)
#
# DOCS_FAILURE_LEVEL (default WARN) is passed to --failure-level, because asciidoctor
# otherwise exits 0 even when it drops content from the output.

set -u -o pipefail

cd "$(dirname -- "${BASH_SOURCE[0]}")" || exit 1

FAILURE_LEVEL="${DOCS_FAILURE_LEVEL:-WARN}"

docs=("$@")
[ "${#docs[@]}" -gt 0 ] || docs=(Overview FIXRulesOfEngagement RESTAPI)
for doc in "${docs[@]}"; do
    case "${doc}" in
        Overview|FIXRulesOfEngagement|RESTAPI) ;;
        *)
            echo "Unknown document '${doc}' (expected Overview, FIXRulesOfEngagement or RESTAPI)" >&2
            exit 2
            ;;
    esac
done

cmd_out=$(mktemp)
failures=$(mktemp)
trap 'rm -f "${cmd_out}" "${failures}"' EXIT

run() {
    printf '$ %s\n' "$*"
    if ! "$@" 2>&1 | tee "${cmd_out}"; then
        { printf '$ %s\n' "$*"; cat "${cmd_out}"; } >> "${failures}"
    fi
}

build_html() {
    run asciidoctor -v --failure-level "${FAILURE_LEVEL}" "$1/$1.adoc" -o "$1/$1.html" \
        -a favicon=../common/favicon.ico
}

build_pdf() {
    run asciidoctor-pdf -v --failure-level "${FAILURE_LEVEL}" -r asciidoctor-mathematical \
        -a allow-uri-read -a pdf-theme=common/pdf/PDFTheme.yml "$1/$1.adoc" -D "$1"
}

for doc in "${docs[@]}"; do
    build_html "${doc}"
    [ "${doc}" = Overview ] || build_pdf "${doc}"
done

if [ -s "${failures}" ]; then
    echo "Documentation build failed:"
    cat "${failures}"
    exit 1
fi
