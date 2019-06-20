#!/bin/bash
VERSION=$(cat DEBIAN/control | grep 'Version: ' | sed 's/Version: //g')
PAK=$(cat DEBIAN/control | grep 'Package: ' | sed 's/Package: //g')
mkdir ../"$PAK"_"$VERSION"_all
if [ -d bin ]; then
	cp -R bin ../"$PAK"_"$VERSION"_all/bin
fi
if [ -d etc ]; then
	cp -R etc ../"$PAK"_"$VERSION"_all/etc
fi
if [ -d usr ]; then
	cp -R usr ../"$PAK"_"$VERSION"_all/usr
fi
if [ -d lib ]; then
	cp -R lib ../"$PAK"_"$VERSION"_all/lib
fi
if [ -d lib32 ]; then
	cp -R lib32 ../"$PAK"_"$VERSION"_all/lib32
fi
if [ -d lib64 ]; then
	cp -R lib64 ../"$PAK"_"$VERSION"_all/lib64
fi
if [ -d libx32 ]; then
	cp -R libx32 ../"$PAK"_"$VERSION"_all/libx32
fi
if [ -d dev ]; then
	cp -R dev ../"$PAK"_"$VERSION"_all/dev
fi
if [ -d home ]; then
	cp -R home ../"$PAK"_"$VERSION"_all/home
fi
if [ -d proc ]; then
	cp -R proc ../"$PAK"_"$VERSION"_all/proc
fi
if [ -d root ]; then
	cp -R root ../"$PAK"_"$VERSION"_all/root
fi
if [ -d run ]; then
	cp -R run ../"$PAK"_"$VERSION"_all/run
fi
if [ -d sbin ]; then
	cp -R sbin ../"$PAK"_"$VERSION"_all/sbin
fi
if [ -d sys ]; then
	cp -R sys ../"$PAK"_"$VERSION"_all/sys
fi
if [ -d tmp ]; then
	cp -R tmp ../"$PAK"_"$VERSION"_all/tmp
fi
if [ -d var ]; then
	cp -R var ../"$PAK"_"$VERSION"_all/var
fi
if [ -d opt ]; then
	cp -R opt ../"$PAK"_"$VERSION"_all/opt
fi
if [ -d srv ]; then
	cp -R srv ../"$PAK"_"$VERSION"_all/srv
fi
cp -R DEBIAN ../"$PAK"_"$VERSION"_all/DEBIAN
cd ..
dpkg-deb --build "$PAK"_"$VERSION"_all
rm -rf "$PAK"_"$VERSION"_all
