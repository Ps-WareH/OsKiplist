//
// Created by 张博坤 on 2024/10/1.
//

#include "RaftNode.h"

RaftNode::RaftNode(int id, const std::vector<int>& peers) : nodeId(id), peerIds(peers) {}


bool RaftNode::appendLog(const std::string& entry) {
    // Simulate log replication
    std::lock_guard<std::mutex> lock(mtx);

    // 1. 将日志条目添加到领导者的日志
    log.push_back(entry);
    std::cout << "Node " << nodeId << " appended log: " << entry << std::endl;
    int successCount = 1;  // 包括自己
    for (int peer : peerIds) {
        if (sendLogToFollower(peer, entry)) {
            successCount++;
        }
    }
    if(successCount==peerIds.size()){
        std::cout << "Node " << nodeId << " successfully replicated log to majority." << std::endl;
        commitLog(entry);  // 提交日志条目
        return true;
    }else {
        std::cout << "Node " << nodeId << " failed to replicate log to majority." << std::endl;
        return false;
    }
}
#include "httplib.h"  // 使用 httplib 库进行 HTTP 通信

bool RaftNode::sendLogToFollower(int followerId, const std::string& entry) {
    // 假设每个跟随者节点都监听在不同的端口上，端口号和 followerId 有映射关系
    std::string followerAddress = "localhost";  // 假设所有节点都在本地
    int followerPort = 8080 + followerId;  // 比如节点1在8081, 节点2在8082

    httplib::Client cli(followerAddress.c_str(), followerPort);

    // 准备发送的数据，POST 请求包含日志条目
    httplib::Params params;
    params.emplace("logEntry", entry);

    // 发送日志条目到跟随者节点的 /appendLog 路由
    auto res = cli.Post("/appendLog", params);

    // 检查跟随者的响应状态
    if (res && res->status == 200) {
        std::cout << "Successfully sent log entry to follower " << followerId << std::endl;
        return true;
    } else {
        std::cerr << "Failed to send log entry to follower " << followerId << std::endl;
        return false;
    }
}
