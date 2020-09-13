use std::os::raw::c_char;
use std::ffi::c_void;
use std::ptr;
use std::ffi::CStr;
use std::ffi::CString;

use std::collections::HashMap;
use std::fs;
use std::fs::File;
use std::path::Path;
use std::io::Read;

#[repr(C)]
pub struct desktopentry_entry {
    name: *mut c_char,
    n_locale_names: u64,
    locale_names: *mut c_void,
    exec: *mut c_char,
    icon_name: *mut c_char,
    icons: *mut c_void,
}

#[repr(C)]
pub struct desktopentry_action {
}

#[repr(C)]
pub struct desktopentry_desktop {
    path: *mut c_char,
    entry: desktopentry_entry,
}

#[derive(PartialEq)]
enum ParsingPart {
    Start,
    Entry,
    Action,
}

#[no_mangle]
pub extern "C" fn desktopentry_string_free(c_str: *mut c_char) {
    if !c_str.is_null() {
        unsafe {
            Box::from_raw(c_str);
        }
    }
}

#[no_mangle]
pub extern "C" fn desktopentry_desktop_new() -> *mut desktopentry_desktop {
    let box_de = Box::new(desktopentry_desktop {
        path: ptr::null_mut(),
        entry: desktopentry_entry {
            name: ptr::null_mut(),
            n_locale_names: 0,
            locale_names: ptr::null_mut(),
            exec: ptr::null_mut(),
            icon_name: ptr::null_mut(),
            icons: ptr::null_mut(),
        },
    });

    Box::into_raw(box_de)
}

#[no_mangle]
pub extern "C" fn desktopentry_desktop_parse(path: *const c_char) -> *mut desktopentry_desktop {
    let mut box_de = Box::new(desktopentry_desktop {
        path: ptr::null_mut(),
        entry: desktopentry_entry {
            name: ptr::null_mut(),
            n_locale_names: 0,
            locale_names: ptr::null_mut(),
            exec: ptr::null_mut(),
            icon_name: ptr::null_mut(),
            icons: ptr::null_mut(),
        },
    });

    // Set filename.
    let file_path = unsafe {
        CStr::from_ptr(path)
    };
    let file_path = CString::from(file_path);
    (*box_de).path = file_path.into_raw();

    // Path as Rust str.
    let file_path = unsafe {
        CStr::from_ptr(path)
    };
    let file_path = file_path.to_str().unwrap();
    let f = File::open(file_path);
    let mut contents = String::new();
    match f {
        Ok(mut f) => {
            f.read_to_string(&mut contents).unwrap();
        }
        Err(_) => {
            return ptr::null_mut();
        }
    }
    // Parse contents.
    let mut part = ParsingPart::Start;
    for line in contents.lines() {
        // Discard empty line.
        if line.trim() == "" {
            continue;
        }
        // Discard comment.
        if line.trim().starts_with("#") {
            continue;
        }
        // Desktop entry.
        if line.trim() == "[Desktop Entry]" {
            part = ParsingPart::Entry;
        }
        // Desktop action.
        if line.trim().starts_with("[Desktop Action") {
            part = ParsingPart::Action;
        }
        // Desktop entry - Name.
        if part == ParsingPart::Entry && line.trim().starts_with("Name=") {
            let spl: Vec<&str> = line.trim().splitn(2, '=').collect();
            let name = spl[1];
            let name = CString::new(name.as_bytes()).unwrap();
            (*box_de).entry.name = name.into_raw();
        }
        // Desktop entry - Icon.
        if part == ParsingPart::Entry && line.trim().starts_with("Icon=") {
            let spl: Vec<&str> = line.trim().splitn(2, '=').collect();
            let icon = spl[1];
            let icon = CString::new(icon.as_bytes()).unwrap();
            (*box_de).entry.icon_name = icon.into_raw();
        }
        // Desktop entry - Exec.
        if part == ParsingPart::Entry && line.trim().starts_with("Exec=") {
            let spl: Vec<&str> = line.trim().splitn(2, '=').collect();
            let exec = spl[1];
            let exec = CString::new(exec.as_bytes()).unwrap();
            (*box_de).entry.exec = exec.into_raw();
        }
    }

    // Icons.
    if !(*box_de).entry.icon_name.is_null() {
        let icon_name = unsafe {
            CStr::from_ptr((*box_de).entry.icon_name)
        };

        let icons = Box::new(get_icons(icon_name.to_str().unwrap()));
        (*box_de).entry.icons = Box::into_raw(icons) as *mut c_void;
    }

    Box::into_raw(box_de)
}

#[no_mangle]
pub extern "C" fn desktopentry_desktop_path(desktop: *const desktopentry_desktop) -> *const c_char {
    let box_de = unsafe {
        Box::from_raw(desktop as *mut desktopentry_desktop)
    };

    let path = (*box_de).path;

    // Put back desktop pointer.
    Box::into_raw(box_de);

    path
}

#[no_mangle]
pub extern "C" fn desktopentry_desktop_entry_name(desktop: *const desktopentry_desktop) -> *const c_char {
    let box_desktop = unsafe {
        Box::from_raw(desktop as *mut desktopentry_desktop)
    };

    let entry_name = (*box_desktop).entry.name;

    Box::into_raw(box_desktop);

    entry_name
}

#[no_mangle]
pub extern "C" fn desktopentry_desktop_entry_exec(desktop: *const desktopentry_desktop) -> *const c_char {
    let box_de = unsafe {
        Box::from_raw(desktop as *mut desktopentry_desktop)
    };

    let entry_exec = (*box_de).entry.exec;

    // Put back the pointer.
    Box::into_raw(box_de);

    entry_exec
}

#[no_mangle]
pub extern "C" fn desktopentry_desktop_entry_icon(desktop: *const desktopentry_desktop) -> *const c_char {
    let box_de = unsafe {
        Box::from_raw(desktop as *mut desktopentry_desktop)
    };

    let entry_icon = (*box_de).entry.icon_name;

    // Put back the pointer.
    Box::into_raw(box_de);

    entry_icon
}

#[no_mangle]
pub extern "C" fn desktopentry_desktop_get_proper_icon(desktop: *const desktopentry_desktop,
        size: usize) -> *mut c_char {
    const SCALABLE_SIZE: usize = 9999;

    let box_desktop = unsafe {
        Box::from_raw(desktop as *mut desktopentry_desktop)
    };

    // If icon is absolute path, return that.
    let icon_name = unsafe {
        CStr::from_ptr((*box_desktop).entry.icon_name)
    };
    let icon_name = icon_name.to_str().unwrap();
    if icon_name.starts_with('/') {
        let ret = CString::new(icon_name).unwrap();
        // Put back boxes.
        Box::into_raw(box_desktop);

        return ret.into_raw();
    }

    // Cast icons.
    let icons = unsafe {
        Box::from_raw((*box_desktop).entry.icons as *mut Vec::<String>)
    };
    // Find proper icon size.
    let found_size: usize;
    let mut sizes: Vec<usize> = Vec::new();
    let mut icons_map: HashMap<usize, String> = HashMap::new();
    for icon_path in *icons.clone() {
        let size_str = extract_size(&icon_path);
        let width = match size_str {
            "scalable" => SCALABLE_SIZE,
            _ => match size_str.split('x').next() {
                Some(num) => num.parse::<usize>().unwrap(),
                None => SCALABLE_SIZE,
            }
        };
        sizes.push(width);
        icons_map.insert(width, icon_path);
    }

    // Put back boxes.
    Box::into_raw(icons) as *mut c_void;
    Box::into_raw(box_desktop);

    // Sort sizes to ge and lt.
    let list_ge: Vec<usize> = sizes.iter().cloned().filter(|&x| x >= size).collect();
    let list_lt: Vec<usize> = sizes.into_iter().filter(|&x| x < size).collect();
    match list_ge.iter().min() {
        Some(min) => {
            found_size = *min;
            let path = &icons_map[&(found_size as usize)];
            return CString::new(path.as_bytes()).unwrap().into_raw();
        }
        None => {
            match list_lt.iter().max() {
                Some(max) => {
                    found_size = *max;
                    let path = &icons_map[&(found_size as usize)];
                    return CString::new(path.as_bytes()).unwrap().into_raw();
                }
                None => {}
            }
        }
    }

    ptr::null_mut()
}

#[no_mangle]
pub extern "C" fn desktopentry_desktop_free(desktop: *mut desktopentry_desktop) {
    let box_desktop = unsafe {
        Box::from_raw(desktop)
    };
    // Free path.
    desktopentry_string_free((*box_desktop).path);

    // Free entry name.
    desktopentry_string_free((*box_desktop).entry.name);

    if !(*box_desktop).entry.icon_name.is_null() {
        unsafe {
            CString::from_raw((*box_desktop).entry.icon_name);  // Free.
        }
    }
    // Free icons.
    if !(*box_desktop).entry.icons.is_null() {
        unsafe {
            Box::from_raw((*box_desktop).entry.icons as *mut Vec<String>);
        }
    }
}

//==========================
// Rust helper functions
//==========================

/// Get available icons path as `Vec<String>`.
fn get_icons(icon_name: &str) -> Vec<String> {
    // let local_icons_path = "~/.local/share/icons";
    let share_icons_path = "/usr/share/icons/hicolor";

    let mut icons = Vec::<String>::new();

    let share_icons = match fs::read_dir(share_icons_path) {
        Ok(dir_entry) => dir_entry,
        Err(e) => {
            eprintln!("{:?}", e);
            return icons;
        }
    };
    for size in share_icons {
        let size = size.unwrap();
        let file_type = size.file_type().unwrap();
        if file_type.is_dir() {
            let size_path = Path::new(share_icons_path);
            // e.g. /usr/share/icons/48x48
            let size_path = size_path.join(size.file_name());
            // e.g. /usr/share/icons/48x48/apps
            let apps_path = size_path.join("apps");
            let apps_icons = fs::read_dir(apps_path);
            match apps_icons {
                Ok(size_entry) => {
                    for icon in size_entry {
                        let icon = icon.unwrap();
                        let icon_name_search = String::from(icon_name) + ".";
                        if icon.file_name().to_str().unwrap().starts_with(&icon_name_search) {
                            icons.push(icon.path().to_str().unwrap().to_string());
                        }
                    }
                }
                Err(_) => {}
            }
        }
    }

    icons
}

/// Extract size slice from given path.
fn extract_size(path: &str) -> &str {
    // e.g. /usr/share/icons/hicolor/128x128/apps/firefox.png
    //      /usr/share/icons/hicolor/scalable/apps/filezilla.svg
    match path.find("hicolor/scalable") {
        Some(_) => {
            let scalable_idx = path.find("scalable").unwrap();
            return &path[scalable_idx..scalable_idx + "scalable".len()];
        }
        None => {}
    }
    let parts = path.split('/');
    for part in parts {
        if part.contains('x') {
            let x_index = part.find('x').unwrap();
            if x_index == 0 {
                continue;
            }
            let mut before_x = true;
            let mut starts_with_numeric = false;
            let mut ends_with_numeric = false;
            let part_chars = part.chars();
            for ch in part_chars {
                if ch == 'x' {
                    before_x = false;
                    continue;
                }
                if before_x {
                    starts_with_numeric = ch.is_numeric();
                } else {
                    ends_with_numeric = ch.is_numeric();
                }
            }
            if starts_with_numeric && ends_with_numeric {
                let part_idx = path.find(part).unwrap();
                return &path[part_idx..part_idx + part.len()];
            }
        }
    }

    ""
}

#[cfg(test)]
mod tests {
    #[test]
    fn test_extract_size() {
        let x128 = super::extract_size("/usr/share/icons/hicolor/128x128/apps/firefox.png");
        assert!(x128 == "128x128");

        let scalable = super::extract_size("/usr/share/icons/hicolor/scalable/apps/filezilla.svg");
        assert!(scalable == "scalable");
    }

    #[test]
    fn test_get_icons() {
        super::get_icons("firefox");
    }

    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
