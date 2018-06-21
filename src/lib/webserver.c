//
// Created by Max on 16-5-18.
//

#include "webserver.h"
#include "../include/session.h"
#include "../include/database.h"
#include "../include/pages.h"
#include "../include/staticloader.h"

onion *o = NULL;

char *get_post_data(void *_, onion_request *req, onion_response *res, char *postName) {
    if (onion_request_get_flags(req) & OR_HEAD) {
        onion_response_write_headers(res);
        return NULL;
    }
    char *data = (char *) onion_request_get_post(req, postName);
    return data;
}

static void shutdown_server(int _) {
    if (o)
        onion_listen_stop(o);
}

int webserver() {
    signal(SIGINT, shutdown_server);
    signal(SIGTERM, shutdown_server);

    o = onion_new(O_POOL);
    onion_set_certificate(o, O_SSL_CERTIFICATE_KEY, "../cert/localhost.crt", "../cert/localhost.key", O_SSL_NONE);
    onion_set_timeout(o, 5000);
    onion_set_hostname(o, "0.0.0.0");
    onion_url *urls = onion_root_url(o);

    onion_url_add(urls, "^login", login_page);
    onion_url_add(urls, "^index", index_page);
    onion_url_add(urls, "^orders", orders_page);
    onion_url_add(urls, "^profile", profile_page);
    onion_url_add(urls, "^logout", logout);
    onion_url_add(urls, "^register", register_page);
    onion_url_add(urls, "^admin", admin_page);
    onion_url_add_static(urls, "^css/bootstrap", getStaticFile("../assets/css/bootstrap.min.css"), 200);
    onion_url_add_static(urls, "^js/jquery", getStaticFile("../assets/js/jquery.min.js"), 200);
    onion_url_add_static(urls, "^js/bootstrap", getStaticFile("../assets/js/bootstrap.bundle.min.js"), 200);
    onion_url_add(urls, "^(.*)$", login_page);

    onion_listen(o);
    onion_free(o);
}
