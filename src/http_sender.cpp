//
// Created by user on 22.09.2026.
//

#include "httplib.h"

#include "agent/http_sender.h"
#include "agent/config.h"

SendResult HttpSender::post(const std::string &body) {
    httplib::Client client(host, port);
    client.set_connection_timeout(1);
    client.set_read_timeout(3);

    httplib::Result result = client.Post(endpoint_path, body, "application/json");

    if (!result) return {false, 0, httplib::to_string(result.error())};

    int status = result->status;

    if (status >= 200 && status < 300) return {true, status, ""};

    return {false, status, ""};
}
