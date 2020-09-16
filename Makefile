APPIMAGETOOL = appimagetool-x86_64.AppImage

default: libdesktopentry.so
	mkdir -p build
	cd build ; qmake ../laniakea-dock.pro
	cd build ; make

libdesktopentry.so:
	cd libs/desktopentry ; cargo build --release

appimage: libdesktopentry.so $(APPIMAGETOOL)
	mkdir -p AppDir/usr/bin
	mkdir -p AppDir/usr/lib
	cp build/laniakea-dock AppDir/usr/bin/
	cp libs/desktopentry/target/release/libdesktopentry.so AppDir/usr/lib/
	./$(APPIMAGETOOL) AppDir

$(APPIMAGETOOL):
	wget https://github.com/AppImage/AppImageKit/releases/download/12/appimagetool-x86_64.AppImage
	chmod +x $(APPIMAGETOOL)

clean:
	rm -rf build
	rm Dock-x86_64.AppImage
