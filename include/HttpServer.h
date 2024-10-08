//
// Created by 张博坤 on 2024/10/1.
//

#ifndef OSKIPLIST_HTTPSERVER_H
#define OSKIPLIST_HTTPSERVER_H

#include "httplib.h"
#include "OsKiplist.h"
using namespace std;
template <typename T>
class HttpServer {
private:
    OsKiplist<T>&  Oskiplist;
    vector<std::string> peers;
public:
    HttpServer(OsKiplist<T>& list,const std::vector<std::string>& peer_addresses)
            : Oskiplist(list),peers(peer_addresses){  // 初始化列表
    }
    void start(int port);

};
//只需要实现服务器的逻辑
template<typename T>
void HttpServer<T>::start(int port) {
    httplib::Server svr;
    svr.Get("/getScore", [&](const httplib::Request& req, httplib::Response& res) {
        T value = static_cast<T>(req.get_param_value("value"));
        double score = Oskiplist.getScore(value);
        res.set_content(to_string(score), "text/plain");
    });
    svr.Post("/insert", [&](const httplib::Request& req, httplib::Response& res) {
        double score = stod(req.get_param_value("score"));
        T value = static_cast<T> (req.get_param_value("value"));

        Oskiplist.insert(score,value);
        res.set_content("Inserted locally", "text/plain");
        for (const auto& peer : peers) {
            httplib::Client client(peer.c_str());
            client.Post("/update", req.body, "application/x-www-form-urlencoded");
        }
    });
    svr.Post("/update", [&](const httplib::Request& req, httplib::Response& res) {
        double score = std::stod(req.get_param_value("score"));
        T value = static_cast<T>(req.get_param_value("value"));
        // 更新本地数据
        Oskiplist.insert(score, value);
        res.set_content("Updated from peer", "text/plain");
    });
    svr.listen("localhost", port);
}
#endif // HTTP_SERVER_H