//
// Created by user on 22.09.2026.
//

#ifndef INTEK_TESTOVOE_HTTP_SENDER_H
#define INTEK_TESTOVOE_HTTP_SENDER_H
#include <string>


struct SendResult {
    bool ok;
    int status;
    std::string error;
};

class HttpSender {
public:
    SendResult post(const std::string& body);
};


#endif //INTEK_TESTOVOE_HTTP_SENDER_H
