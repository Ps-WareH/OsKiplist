#include <iostream>
#include "OsKiplist.h"
#include "RaftNode.h"
#include "HttpServer.h"
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: ./skiplist_server <node_id> <port>" << std::endl;
        return 1;
    }
    int nodeId=std::stoi(argv[1]);
    int port = std::stoi(argv[2]);
    vector<int> peers = {1, 2, 3};
    OsKiplist<std::string> slOfThisServer(3);
    RaftNode raftNode(nodeId, peers);
    HttpServer server(slOfThisServer, raftNode);
    server.start(port);
    return 0;
//    OsKiplist<string> temp(3);
//    temp.insert(10,"owoooo");
//    cout<<temp.getScore("owoooo");
}
