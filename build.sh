#!/bin/bash
cp DEBIAN/mrai.control DEBIAN/control
VERSION=$(cat DEBIAN/control | grep 'Version: ' | sed 's/Version: //g')
PAK=$(cat DEBIAN/control | grep 'Package: ' | sed 's/Package: //g')
ARCH=$(cat DEBIAN/control | grep 'Architecture: '| sed 's/Architecture: //g')
FOLDER="$PAK\_$VERSION\_$ARCH"
FOLDER=$(echo "$FOLDER" | sed 's/\\//g')
if [ "$ARCH" == "amd64" ]; then
	COMPILER="g++ -m64"
	ARGS="-Wall"
elif [ "$ARCH" == "arm64" ]; then
	COMPILER="aarch64-linux-gnu-g++"
	ARGS=""
fi
mkdir ../"$FOLDER"
##############################################################
#							     #
#							     #
#  COMPILE ANYTHING NECSSARY HERE			     #
#							     #
#							     #
##############################################################
cd usr/share/mrai
$COMPILER $ARGS -o "log-out" "log-out.cxx" && echo "log-out compiled successfully"
$COMPILER $ARGS -o "edit-apt-sources" "edit-apt-sources.cxx" && echo "edit-apt-sources compiled successfully"
$COMPILER $ARGS -o "base-spinner" "base-spinner.cxx" && echo "base-spinner compiled successfully"
$COMPILER $ARGS -o "clean" "clean.cxx" && echo "clean compiled successfully"
$COMPILER $ARGS -o "gitautoinst" "gitautoinst.cxx" && echo "gitautoinst compiled successfully"
$COMPILER $ARGS -o "aptremove" "aptremove.cxx" && echo "aptremove compiled successfully"
cd ../../lib
$COMPILER $ARGS -Werror -fpic -o "mrai_lib.o" "mrai_lib.cxx"
$COMPILER -shared -o "libmrai.so.1" "mrai_lib.o"
cd ../..
cd sbin
$COMPILER $ARGS -o "snapupdate" "snapupdate.cxx" && echo "snapupdate compiled successfully"
$COMPILER $ARGS -o "aptupdate" "aptupdate.cxx" && echo "aptupdate compiled successfully"
$COMPILER $ARGS -o "mrai" "mrai.cxx" && echo "mrai compiled successfully"
cd ..
##############################################################
#							     #
#							     #
#  REMEMBER TO DELETE SOURCE FILES FROM TMP		     #
#  FOLDER BEFORE BUILD					     #
#							     #
#							     #
##############################################################
if [ -d bin ]; then
	cp -R bin ../"$FOLDER"/bin
fi
if [ -d etc ]; then
	cp -R etc ../"$FOLDER"/etc
fi
if [ -d usr ]; then
	cp -R usr ../"$FOLDER"/usr
fi
if [ -d lib ]; then
	cp -R lib ../"$FOLDER"/lib
fi
if [ -d lib32 ]; then
	cp -R lib32 ../"$FOLDER"/lib32
fi
if [ -d lib64 ]; then
	cp -R lib64 ../"$FOLDER"/lib64
fi
if [ -d libx32 ]; then
	cp -R libx32 ../"$FOLDER"/libx32
fi
if [ -d dev ]; then
	cp -R dev ../"$FOLDER"/dev
fi
if [ -d home ]; then
	cp -R home ../"$FOLDER"/home
fi
if [ -d proc ]; then
	cp -R proc ../"$FOLDER"/proc
fi
if [ -d root ]; then
	cp -R root ../"$FOLDER"/root
fi
if [ -d run ]; then
	cp -R run ../"$FOLDER"/run
fi
if [ -d sbin ]; then
	cp -R sbin ../"$FOLDER"/sbin
fi
if [ -d sys ]; then
	cp -R sys ../"$FOLDER"/sys
fi
if [ -d tmp ]; then
	cp -R tmp ../"$FOLDER"/tmp
fi
if [ -d var ]; then
	cp -R var ../"$FOLDER"/var
fi
if [ -d opt ]; then
	cp -R opt ../"$FOLDER"/opt
fi
if [ -d srv ]; then
	cp -R srv ../"$FOLDER"/srv
fi
cp -R DEBIAN ../"$FOLDER"/DEBIAN
cd ..
#DELETE STUFF HERE
rm "$FOLDER"/usr/share/mrai/log-out.cxx
rm "$FOLDER"/usr/share/mrai/edit-apt-sources.cxx
rm "$FOLDER"/usr/share/mrai/base-spinner.cxx
rm "$FOLDER"/usr/share/mrai/clean.cxx
rm "$FOLDER"/usr/share/mrai/gitautoinst.cxx
rm "$FOLDER"/usr/share/mrai/aptremove.cxx
rm "$FOLDER"/usr/share/mrai/mrai_lib.hpp
rm mrai/usr/share/mrai/aptremove
rm mrai/usr/share/mrai/gitautoinst
rm mrai/usr/share/mrai/clean
rm mrai/usr/share/mrai/base-spinner
rm mrai/usr/share/mrai/log-out
rm mrai/usr/share/mrai/edit-apt-sources
rm "$FOLDER"/sbin/snapupdate.cxx
rm "$FOLDER"/sbin/aptupdate.cxx
rm "$FOLDER"/sbin/mrai.cxx
rm mrai/sbin/mrai
rm mrai/sbin/aptupdate
rm mrai/sbin/snapupdate
#build the shit
rm "$FOLDER"/DEBIAN/mrai-common.control "$FOLDER"/DEBIAN/mrai.control "$FOLDER"/DEBIAN/mrai-common.install 
dpkg-deb --build "$FOLDER"
cd mrai
cp DEBIAN/mrai-common.control DEBIAN/control
VERSION=$(cat DEBIAN/control | grep 'Version: ' | sed 's/Version: //g')
PAK=$(cat DEBIAN/control | grep 'Package: ' | sed 's/Package: //g')
ARCH=$(cat DEBIAN/control | grep 'Architecture: '| sed 's/Architecture: //g')
NEW_FOLDER="$PAK\_$VERSION\_$ARCH"
NEW_FOLDER=$(echo "$NEW_FOLDER" | sed 's/\\//g')
mv "$FOLDER" "$NEW_FOLDER"
mv DEBIAN/control "$NEW_FOLDER"/DEBIAN/control
cp DEBIAN/mrai-common.install "$NEW_FOLDER"/DEBIAN/mrai-common.install
rm "$NEW_FOLDER"/DEBIAN/mrai.install
cd ..
dpkg-deb --build "$NEW_FOLDER"
rm -rf "$FOLDER"
