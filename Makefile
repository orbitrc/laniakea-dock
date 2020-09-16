default: libdesktopentry.so
	mkdir -p build
	cd build ; qmake ../laniakea-dock.pro
	cd build ; make

libdesktopentry.so:
	cd libs/desktopentry ; cargo build --release

clean:
	rm -rf build
