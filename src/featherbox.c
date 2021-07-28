#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include "featherbox.h"

xcb_connection_t *conn;
const xcb_setup_t *setup;
xcb_screen_t *screen;

void quit(int status) {
    xcb_disconnect(conn);
    exit(status);
}

bool existing_wm(void) {
    xcb_generic_error_t *error;
    unsigned int values[] = {EVENT_MASK};

    // error and quit if found
    error = xcb_request_check(
        conn, xcb_change_window_attributes_checked(conn, screen->root,
                                                   XCB_CW_EVENT_MASK, values));
    return error != NULL;
}

void initialize() {
    int screen_num;
    conn = xcb_connect(NULL, &screen_num);
    if (xcb_connection_has_error(conn)) {
        FLOG("Error opening display");
        exit(1);
    }
    setup = xcb_get_setup(conn);
    screen = xcb_setup_roots_iterator(setup).data;
    if (existing_wm()) {
        FLOG("A window manager is already running");
        quit(1);
    }
    xcb_grab_button(conn, 0, screen->root,
                    XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
                    XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, screen->root,
                    XCB_NONE, XCB_BUTTON_INDEX_1, XCB_MOD_MASK_4);
    xcb_grab_button(conn, 0, screen->root,
                    XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
                    XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, screen->root,
                    XCB_NONE, XCB_BUTTON_INDEX_3, XCB_MOD_MASK_4);
    xcb_flush(conn);
}

void close_window(struct client *c) { xcb_destroy_window(conn, c->win); }

int main(int argc, char *argv[]) {
    initialize();
    xcb_generic_event_t *e;
    while (1) {
        e = xcb_wait_for_event(conn);
        switch (e->response_type & ~0x80) {
        case XCB_CREATE_NOTIFY: {
            break;
        }
        case XCB_DESTROY_NOTIFY: {
            break;
        }
        case XCB_MAP_NOTIFY: {
            break;
        }
        case XCB_MAP_REQUEST: {
            xcb_map_request_event_t *ev = (xcb_map_request_event_t *)e;
            xcb_map_window(conn, ev->window);
            break;
        }
        case XCB_UNMAP_NOTIFY: {
            break;
        }
        case XCB_CONFIGURE_NOTIFY: {
            break;
        }
        case XCB_CONFIGURE_REQUEST: {
            break;
        }
        case XCB_KEY_PRESS: {
            break;
        }
        case XCB_KEY_RELEASE: {
            break;
        }
        case XCB_BUTTON_PRESS: {
            break;
        }
        case XCB_BUTTON_RELEASE: {
            break;
        }
        default: {
            TLOG("Ignoring event")
            break;
        }
        }
        free(e);
        if (xcb_connection_has_error(conn)) {
            FLOG("An unexpected error has occured");
            quit(1);
            break;
        }
        xcb_flush(conn);
    }
}
