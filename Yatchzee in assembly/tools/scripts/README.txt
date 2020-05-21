How to package debian:

From this directory invoke ./package_deb. It will place a debian package in out/packages/libminiat-dev-ARCH.deb

To make a source zip run this command (Tested on Ubunutu):
zip -r miniat.zip . -x \*out\* \*.git\* \*.repo\* .sconsign.dblite
