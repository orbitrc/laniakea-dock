use std::os::raw::c_char;
use std::ffi::c_void;
use std::ptr;
use std::ffi::CStr;
use std::ffi::CString;

use std::fs::File;
use std::io::Read;

#[repr(C)]
pub struct desktopentry_entry {
    name: *mut c_char,
    n_locale_names: u64,
    locale_names: *mut c_void,
    exec: *mut c_char,
}

#[repr(C)]
pub struct desktopentry_action {
}

#[repr(C)]
pub struct desktopentry_desktop {
    entry: desktopentry_entry,
}

#[derive(PartialEq)]
enum ParsingPart {
    Start,
    Entry,
    Action,
}

#[no_mangle]
pub extern "C" fn desktopentry_desktop_new() -> *mut desktopentry_desktop {
    let box_de = Box::new(desktopentry_desktop {
        entry: desktopentry_entry {
            name: ptr::null_mut(),
            n_locale_names: 0,
            locale_names: ptr::null_mut(),
            exec: ptr::null_mut(),
        },
    });

    Box::into_raw(box_de)
}

#[no_mangle]
pub extern "C" fn desktopentry_desktop_parse(path: *const c_char) -> *mut desktopentry_desktop {
    let mut box_de = Box::new(desktopentry_desktop {
        entry: desktopentry_entry {
            name: ptr::null_mut(),
            n_locale_names: 0,
            locale_names: ptr::null_mut(),
            exec: ptr::null_mut(),
        },
    });

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
    }

    Box::into_raw(box_de)
}

#[no_mangle]
pub extern "C" fn desktopentry_desktop_free(desktop: *mut desktopentry_desktop) {
    let box_desktop = unsafe {
        Box::from_raw(desktop)
    };
    if !(*box_desktop).entry.name.is_null() {
        unsafe {
            CString::from_raw((*box_desktop).entry.name);   // Free.
        }
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
