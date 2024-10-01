//
// Created by 张博坤 on 2024/9/20.
//
#ifndef OSKIPLIST_OSKIPLIST_H
#include "Node.h"
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#define SKIPLIST_P 0.5
#define OSKIPLIST_OSKIPLIST_H

using namespace std;
template<typename T>
//can this T be of different type?...
class OsKiplist {
    Node<T>* header;
    Node<T>* tail;
    int maxLevel=3;
    unordered_map<T,double> record;
    //？？？
    std::mutex mtx;//全局变量还是类的成员变量？？
public:
    OsKiplist(int maxLevel){
        assert(maxLevel>0);
        this->maxLevel=maxLevel;
        header= new Node(maxLevel,static_cast<T>("dummy"),0);
        tail = nullptr;
    }
    //score from low->high
    //value from low->high
    //如果一个元素已经存在，score会覆盖
    void insert(double score, T val);
    bool deleteMember(T memeber);

//    randomLevel() 方法返回 0 表示当前插入的该元素不需要建索引，只需要存储数据到原始链表即可（概率 1/2）
//    randomLevel() 方法返回 1 表示当前插入的该元素需要建一级索引（概率 1/4）
//    randomLevel() 方法返回 2 表示当前插入的该元素需要建二级索引（概率 1/8）
//    randomLevel() 方法返回 3 表示当前插入的该元素需要建三级索引（概率 1/16）
//    。。。以此类推
    int getRandomLevel();

};


#endif //OSKIPLIST_OSKIPLIST_H
