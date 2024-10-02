//
// Created by 张博坤 on 2024/10/1.
//

#ifndef OSKIPLIST_HTTPSERVER_H
#define OSKIPLIST_HTTPSERVER_H

#include "httplib.h"
#include "OsKiplist.h"
#include "RaftNode.h"
template <typename T>
class HttpServer {
private:
    OsKiplist<T>&  Oskiplist;
    RaftNode& raftNode;
public:
    HttpServer(OsKiplist<T>& list, RaftNode& raft)
            : Oskiplist(list), raftNode(raft) {  // 初始化列表
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

        raftNode.appendLog(value);  // Append to Raft log
        res.set_content("Inserted", "text/plain");
    });
    svr.listen("localhost", port);
}
#endif // HTTP_SERVER_H