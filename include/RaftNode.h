//
// Created by 张博坤 on 2024/10/1.
//

#ifndef RAFT_NODE_H
#define RAFT_NODE_H

#include <string>
#include <functional>
#include <vector>
#include <iostream>

class RaftNode {
public:
    RaftNode(int id, const std::vector<int>& peers);
    bool appendLog(const std::string& value);

private:
    int nodeId;
    std::vector<int> peerIds;
    std::vector<std::string> log;
    std::mutex mtx;//啊？？
    bool sendLogToFollower(int followerId, const std::string& entry);
//    std::function<void(const std::string&)> commitCallback;
};

#endif // RAFT_NODE_H
