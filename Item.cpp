#include "Item.h"

#include <stdint.h>

#include <QUuid>

#include <xcb/xcb.h>

Item::Item(ItemType type, const QString& path, QObject *parent)
    : QObject(parent)
{
    this->setId(QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces));

    // Initialize members.
    this->m_pinned = false;
    this->setType(type);
    this->setPath(path);

    QObject::connect(this, &Item::iconGeometryChanged,
                     this, &Item::changeNetWmIconGeometry);

    // Initialize to null the objects.
    this->_appimage = nullptr;
    this->_exec = nullptr;

    if (type == Item::ItemType::DesktopEntry) {
    } else if (type == Item::ItemType::AppImage) {
    } else if (type == Item::ItemType::Exec) {
    }
}

Item::~Item()
{
    // Free heap memory allocation.
    switch (this->type()) {
    case Item::ItemType::DesktopEntry:
        break;
    case Item::ItemType::AppImage:
        if (this->_appimage != nullptr) {
        }
        break;
    case Item::ItemType::Exec:
        if (this->_exec != nullptr) {
        }
        break;
    default:
        break;
    }
}

//=============================
// Property getters/setters
//=============================

QString Item::id() const
{
    return this->m_id;
}

void Item::setId(const QString &id)
{
    if (this->m_id != id) {
        this->m_id = id;
        emit this->idChanged(id);
    }
}

bool Item::pinned() const
{
    return this->m_pinned;
}

void Item::setPinned(bool val)
{
    if (this->m_pinned != val) {
        this->m_pinned = val;

        emit this->pinnedChanged(val);
    }
}

Item::ItemType Item::type() const
{
    return this->m_type;
}

void Item::setType(ItemType type)
{
    this->m_type = type;
}

std::optional<QString> Item::cls() const
{
    return this->m_class;
}

void Item::setCls(const QString &cls)
{
    this->m_class = cls;
}

void Item::clearCls()
{
    this->m_class.reset();
}

QString Item::path() const
{
    return this->m_path;
}

void Item::setPath(const QString &path)
{
    if (this->m_path != path) {
        this->m_path = path;

        emit this->pathChanged(path);
    }
}

QList<int> Item::windows() const
{
    return this->m_wIds;
}

void Item::appendWindow(int wId)
{
    this->m_wIds.append(wId);

    emit this->windowsChanged();
}

void Item::removeWindow(int wId)
{
    this->m_wIds.removeOne(wId);

    emit this->windowsChanged();
}

QRect Item::iconGeometry() const
{
    return this->m_iconGeometry;
}

void Item::setIconGeometry(const QRect &rect)
{
    if (this->m_iconGeometry != rect) {
        this->m_iconGeometry = rect;

        emit this->iconGeometryChanged(rect);
    }
}

//=========================
// Slots
//=========================
void Item::changeNetWmIconGeometry(const QRect& rect)
{
    uint32_t x = rect.x();
    uint32_t y = rect.y();
    uint32_t width = rect.width();
    uint32_t height = rect.height();

    // Connect to the X server.
    xcb_connection_t *conn;

    conn = xcb_connect(NULL, NULL);

    // Get atom.
    xcb_atom_t net_wm_icon_geometry;
    xcb_intern_atom_cookie_t intern_atom_cookie;
    xcb_intern_atom_reply_t *intern_atom_reply;
    intern_atom_cookie = xcb_intern_atom(
        conn, 1, strlen("_NET_WM_ICON_GEOMETRY"), "_NET_WM_ICON_GEOMETRY");
    intern_atom_reply = xcb_intern_atom_reply(conn, intern_atom_cookie, NULL);
    net_wm_icon_geometry = intern_atom_reply->atom;
    free(intern_atom_reply);

    uint32_t data[4];
    data[0] = x;
    data[1] = y;
    data[2] = width;
    data[3] = height;

    for (int i = 0; i < this->windows().length(); ++i) {
        xcb_void_cookie_t cookie = xcb_change_property_checked(
            conn,
            XCB_PROP_MODE_REPLACE,
            this->windows()[i],
            net_wm_icon_geometry,
            XCB_ATOM_CARDINAL,
            32,
            4,
            (void*)data
        );
        xcb_flush(conn);

        xcb_generic_error_t *err = xcb_request_check(conn, cookie);
        if (err) {}
    }

    // Close connection from the X server.
    xcb_disconnect(conn);
}
