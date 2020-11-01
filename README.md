laniakea-dock
================
Laniakea desktop environment dock.

This is a part of Laniakea desktop environment, but currently can be working standalone. This feature is not garranted for future version of it.

Build
------

### Dependencies
- Cargo (for libdesktopentry.so)

### Compile
```sh
$ make
```

### Make AppImage
```sh
$ make appimage
```

AppImage tool is automatically downloaded at first time.

### Install
```sh
$ mkdir -p /usr/appimages
$ cp Dock-x86_64.AppImage /usr/appimages/
```
