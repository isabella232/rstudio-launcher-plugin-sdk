#!/usr/bin/env bash

#
# generate-developers-guide
#
# Copyright (C) 2019-20 by RStudio, PBC
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

set -e # exit on failed commands.

VERSION=99.9.9
if [[ -n "$1" ]]; then
  VERSION="$1"
fi

cd "$(dirname "${BASH_SOURCE[0]}")"

# update the version number
sed -e "s/\\\${RLPS_VERSION}/${VERSION}/g" _bookdown.yml.in > _bookdown.yml

# Make the PDF version
RSTUDIO_PANDOC="/usr/lib/rstudio/bin/pandoc" Rscript -e "bookdown::render_book(\"index.Rmd\", \"bookdown::pdf_book\", params = list(version = '${VERSION}'))"
cp "_book/rlps-${VERSION}-release-notes.pdf" "../../RStudio Launcher Plugin SDK Release Notes.pdf"

# Clean up the output for the PDF version
rm -r _book

# Make the HTML version
RSTUDIO_PANDOC="/usr/lib/rstudio/bin/pandoc" Rscript -e "bookdown::render_book(\"index.Rmd\", \"bookdown::gitbook\", params = list(version = '${VERSION}'))"
mv "_book" "../../QuickStartHtml"
