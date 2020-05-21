#!/usr/bin/python
# -*- coding: utf-8 -*-
from xml.sax import saxutils, handler, make_parser
import xml.sax


class Package(object):

    roottag = 'package'
    libTag = 'lib'
    exeTag = 'exe'
    docTag = 'doc'
    incTag = 'inc'
    srcTag = 'src'
    extensionTag = 'extensions'

    def __init__(self, name):
        self.name = name
        self.tags = dict()

    def addTag(
        self,
        tag,
        attrs,
        lineno,
        ):
        if tag not in self.tags:
            self.tags[tag] = list()

        self.tags[tag].append(attrs._attrs)

    def getTag(self, tag):
        return self.tags[tag]

    def getPkg(self):
        return self.tags

    def getName(self):
        return self.name


class PackageParser(handler.ContentHandler):

    def __init__(self, pkglist):
        handler.ContentHandler()
        self._locator = xml.sax.xmlreader.Locator()
        handler.ContentHandler.setDocumentLocator(self, self._locator)
        self.pkg = None
        self.pkglist = pkglist

    def startElement(self, name, attrs):
        lineno = self._locator.getLineNumber()

        if name == Package.roottag:
            self.pkg = Package(attrs._attrs['name'])
        else:
            self.pkg.addTag(name, attrs, lineno)

    def endElement(self, name):
        if name == Package.roottag:
            self.pkglist.append(self.pkg)
            self.pkg = None

    def characters(self, content):
        pass

    def ignorableWhitespace(self, content):
        pass

    def processingInstruction(self, target, data):
        pass


