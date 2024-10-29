#include <iostream>
#include <chrono>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include <vector>
#include "include/OsKiplist.h"
using namespace std;
#define NUM_THREADS 4
#define TEST_COUNT 100000
#define TEMP TEST_COUNT/NUM_THREADS
//10w/4=2.5w个？。。
//全局变量，共享！
OsKiplist<string> skipList(18);
//docker valgrind检查内存泄漏
void *insertElement(void* threadid) {
    long tid;
    srand(time(NULL));
    tid = (long)threadid;
    int start = TEMP*(tid);
    int end = start+TEMP;
    std::cout << "start "<<start <<" end "<<end<< std::endl;
    for (int i=start; i<start+TEMP; i++) {
        //s不要重复，因为相同元素会更新而非插入，跳表不会变长，无法起到压力测试的作用
        string s = "key_" + to_string(i);
        //score 在start end之间
        skipList.insert(start+rand()%(end-start), s);
    }
}
//跳表便捷在于范围搜索，单点搜索访问哈希表即可！
//res长度加起来是TEST_COUNT
void *searchElement(void* threadid) {
    long tid;
    tid = (long)threadid;
    int minS =TEMP*(tid);
    int maxS = minS+TEMP;
    std::cout << "minS "<<minS <<" maxS "<<maxS<< std::endl;
    vector<string> res = skipList.searchBetween(minS,maxS);
    cout<<"tid "<<tid<<" search num "<<res.size()<<" for example, "<<res[0]<<", "<<res[1]<<endl;
}

int main() {

    //insertion stress testing
    pthread_t threads[NUM_THREADS];
    int code;
    auto start = std::chrono::high_resolution_clock::now();

    for( int i = 0; i < NUM_THREADS; i++ ) {
        std::cout << "main() : creating thread, " << i << std::endl;
        code = pthread_create(&threads[i], NULL, insertElement, (void *)i);
        //code = 0, success
        if (code) {
            std::cout << "Error:unable to create thread," << code << std::endl;
            exit(-1);
        }
    }
    void *ret;
    for(int i = 0; i < NUM_THREADS; i++ ) {
        if (pthread_join(threads[i], &ret) !=0 )  {
            perror("pthread_create() error");
            exit(3);
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "insert elapsed:" << elapsed.count() << std::endl;
    cout<<"================================="<<endl;
    cout<<skipList.record.size()<<endl;//val永不重复，应该等于TEST_COUNT
    int res = 0;
    Node<string>* pt = skipList.header->forwards[0];
    while(pt!= nullptr){
        pt = pt->forwards[0];
        res+=1;
    }
    cout<<res<<endl;
    cout<<"================================="<<endl;
    cout<<"begin searching"<<endl;


    pthread_t newThreads[NUM_THREADS];
    int code2;
    auto start2 = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < NUM_THREADS; i++ ) {
        std::cout << "main() : creating thread, " << i << std::endl;
        code2 = pthread_create(&newThreads[i], NULL,searchElement, (void *)i);
        //code = 0, success
        if (code2) {
            std::cout << "Error:unable to create thread," << code2 << std::endl;
            exit(-1);
        }
    }
    void *re;
    for( int i = 0; i < NUM_THREADS; i++ ) {
        if (pthread_join(newThreads[i], &re) !=0 )  {
            perror("pthread_create() error");
            exit(3);
        }
    }
    auto finish2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed2 = finish2 - start2;
    std::cout << "searching elapsed:" << elapsed2.count() << std::endl;
    return 0;
}