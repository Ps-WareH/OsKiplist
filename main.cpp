#include <iostream>
#include "OsKiplist.h"

#include "HttpServer.h"
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./skiplist_server <node_id> <port>" << std::endl;
        return 1;
    }
    int port = std::stoi(argv[1]);
    std::vector<std::string> peers;
    if (port == 8080) {
        peers = {"http://localhost:8081", "http://localhost:8082"};
    } else if (port == 8081) {
        peers = {"http://localhost:8080", "http://localhost:8082"};
    } else if (port == 8082) {
        peers = {"http://localhost:8080", "http://localhost:8081"};
    }

    OsKiplist<std::string> skiplist(3);
    HttpServer server(skiplist, peers);

    server.start(port);
    return 0;
//    OsKiplist<string> temp(3);
//    temp.insert(10,"owoooo");
//    cout<<temp.getScore("owoooo");
}
