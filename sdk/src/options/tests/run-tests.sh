#!/usr/bin/env bash

#
# run-tests
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

FAILURES=0

# Create the rstudio-server user, if it does not already exist
grep rstudio-server < /etc/passwd >/dev/null
ADD_USER=$?
if [[ $ADD_USER -ne 0 ]]; then
  sudo adduser --system rstudio-server
fi

# Add test users for UserProfileTests

sudo groupadd "rlpstestgrpone" >/dev/null
sudo groupadd "rlpstestgrptwo" >/dev/null
sudo groupadd "rlpstestgrpthree" >/dev/null
sudo useradd -p "" -g "rlpstestgrpone" "rlpstestusrone" >/dev/null
sudo useradd -p "" -g "rlpstestgrpone" -G "rlpstestgrptwo,rlpstestgrpthree" "rlpstestusrtwo" >/dev/null
sudo useradd -p "" -g "rlpstestgrptwo" "rlpstestusrthree" >/dev/null
sudo useradd -p "" -g "rlpstestgrptwo" -G "rlpstestgrpthree" "rlpstestusrfour" >/dev/null
sudo useradd -p "" -g "rlpstestgrpone" -G "rlpstestgrpthree" "rlpstestusrfive" >/dev/null

for test in ./*-tests;
do
  echo "Running ${test}..."
  ${test}
  FAILURES=$((FAILURES + $?))
done

# Remove the user if it was added by this script
if [[ $ADD_USER -ne 0 ]]; then
  sudo userdel rstudio-server
fi

sudo userdel --remove "rlpstestusrone" >/dev/null 2>&1
sudo userdel --remove "rlpstestusrtwo" >/dev/null 2>&1
sudo userdel --remove "rlpstestusrthree" >/dev/null 2>&1
sudo userdel --remove "rlpstestusrfour" >/dev/null 2>&1
sudo userdel --remove "rlpstestusrfive" >/dev/null 2>&1
sudo delgroup "rlpstestgrpone" >/dev/null
sudo delgroup "rlpstestgrptwo" >/dev/null
sudo delgroup "rlpstestgrpthree" >/dev/null

exit $FAILURES
