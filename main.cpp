#include <iostream>
#include "OsKiplist.h"
#include <vector>
#include <string>

using namespace std;
typedef struct{
    string h;
} hello;
//这个是普通的正确性测试
int main() {
    OsKiplist<string> skiplist(3);
    for (int i=0; i<10; i++) {
        string s = "key_" + to_string(i);
        skiplist.insert(i,s);
    }
    int res = 1;
    Node<string>* pt = skiplist.header->forwards[0];
    while(pt!= nullptr){
        pt = pt->forwards[0];
        res+=1;
    }
    cout<<skiplist.record.size()<<endl;
    cout<<res<<endl;


//    return 0;

}
