#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
sys.path.insert(0, '../build')
from packageparser import PackageParser, Package
from xml.sax import saxutils, handler, make_parser
import xml.sax
import subprocess
import os
import hashlib
import shutil

ROOT_PATH = '../../'
DEB_PATH = 'out/packages/'
PKG_PATH = '../build/package.xml'


class DebPackage:

    deb_vers = '2.0'
    deb_root = 'DEBIAN'
    deb_control = \
        '''Package: libminiat-dev
Source: miniat
Version: 0.001
Architecture: $ARCH
Priority: optional
Recommends: gcc
Maintainer: William Roberts <bill.c.roberts@gmail.com>
Description: The MiniAT Virtual Embedded Architecture library, assembler and development headers.
'''

    def __init__(
        self,
        root,
        outdir,
        name,
        ):

        ps = subprocess.Popen(['dpkg-architecture', '-qDEB_BUILD_ARCH'
                              ], close_fds=True, stdin=subprocess.PIPE,
                              stdout=subprocess.PIPE,
                              stderr=subprocess.STDOUT)
        ps.wait()
        arch = ps.stdout.read().strip()

        self.map = {
            Package.libTag: '/usr/lib',
            Package.exeTag: '/usr/bin',
            Package.incTag: '/usr/include',
            Package.docTag: os.path.join('/usr/share/', name, 'doc'),
            Package.srcTag: os.path.join('/usr/share/', name, 'examples'
                    ),
            }

        self.arch = arch
        self.name = name + '-' + arch
        self.root = root
        self.outdir = os.path.join(root, outdir)
        self.pkgdir = os.path.join(root, outdir, 'debian-' + arch)
        self.debdir = os.path.join(self.pkgdir, 'DEBIAN')

        try:
            os.makedirs(self.pkgdir)
        except OSError:
            shutil.rmtree(self.pkgdir)
            os.makedirs(self.pkgdir)

        os.mkdir(self.debdir)

        self.md5sumFile = open(os.path.join(self.debdir, 'md5sums'),
                               'w+')

        controlFile = open(os.path.join(self.debdir, 'control'), 'w+')
        controlText = DebPackage.deb_control.replace('$ARCH', arch)

        controlFile.write(controlText)
        controlFile.close()

    def package(self, items):

        out = self.pkgdir

        extensions = dict()

        s = items[Package.extensionTag]

        for x in s:
            plat = x['os']
            if plat == sys.platform:

                sfx = ''
                prx = ''
                if 'suffix' in x:
                    sfx = x['suffix']
                if 'prefix' in x:
                    prx = x['prefix']

                extensions[x['type']] = {'suffix': sfx, 'prefix': prx}

        # Start copy file loop

        for name in items:

            if name == Package.extensionTag:
                continue

            for kvp in items[name]:
                path = kvp['path']
                fname = kvp['name']

                sfx = ''
                pfx = ''
                if name in extensions:
                    sfx = extensions[name]['suffix']
                    pfx = extensions[name]['prefix']

                path = os.path.join(os.path.split(path)[0], pfx
                                    + os.path.split(path)[1])
                src = os.path.normpath(self.root + os.path.sep + path)

                destname = self.map[name]
                dest = os.path.normpath(self.pkgdir + os.path.sep
                        + destname)
                fname = os.path.join(dest, kvp['name'])
                destname = os.path.join(destname, kvp['name'])

                src = src + sfx
                destname = destname + sfx

                # make the "file system" path in pkgdir

                try:
                    p = os.path.dirname(fname)
                    os.makedirs(p)
                except:
                    pass

                # Copy the src file to dest file name

                shutil.copyfile(src, fname)
                shutil.copymode(src, fname)

                # Generate m5sum

                m = hashlib.md5()
                f = open(fname, 'r')
                while True:
                    data = f.read(128000)
                    if len(data) == 0:
                        break

                    m.update(data)

                sum = m.hexdigest()
                self.md5sumFile.write(sum + ' ' + destname + '\n')

        # End copy file loop

        self.md5sumFile.close()

        # Build the package

        target = os.path.join(self.outdir, self.name + '.deb')
        ps = subprocess.Popen(['dpkg', '-b', self.pkgdir, target],
                              close_fds=True, stdin=subprocess.PIPE,
                              stdout=subprocess.PIPE,
                              stderr=subprocess.STDOUT)
        ps.wait()


def main(argv):

    pkglist = list()

    packageFile = open(PKG_PATH, 'r')
    prsr = xml.sax.make_parser()
    x = PackageParser(pkglist)
    prsr.setContentHandler(x)
    prsr.parse(packageFile)

    for pkg in pkglist:
        items = pkg.getPkg()

        # XXX Get rid of these hardcodes through some xml attrs

        dpkg = DebPackage(ROOT_PATH, DEB_PATH, pkg.getName())
        dpkg.package(items)


if __name__ == '__main__':
    main(sys.argv)
