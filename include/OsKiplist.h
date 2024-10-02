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
        header= new Node(maxLevel,static_cast<T>("dummy"),maxLevel);
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
    double getScore(T value){
        if(record.find(value)==record.end())return NULL;
        return record[value];
    }

};
template<typename T>
void OsKiplist<T>::insert(double score, T val) {
    mtx.lock();
    assert(!val.empty());
    //score & value from low->high in list
    Node<T>* newNode = nullptr;
    if(this->record.find(val)==this->record.end())newNode = new Node (score, val,getRandomLevel());//it's insert not update

    this->record[val]=score;
    Node<T>* ptr = header;
    ptr = header;
    for(int i = this->maxLevel;i>=0;i--){
        while(ptr->forwards[i]!= nullptr){
            if(ptr->forwards[i]->score<score){
                ptr=ptr->forwards[i];
            }else if (ptr->forwards[i]->score==score){
                if(ptr->forwards[i]->value<val) {
                    ptr = ptr->forwards[i];
                }else if(ptr->forwards[i]->value==val){
                    ptr->forwards[i]->score = score;//update
                    break;//to next level
                }
                else{//ptr->forwards[i]->value>val,consider insert
                    if(i<=newNode->level) {
                        newNode->backward = ptr;
                        newNode->forwards[i] = ptr->forwards[i];
                        ptr->forwards[i] = newNode;
                        newNode->forwards[i]->backward = newNode;
                    }
                    break;
                    //go next level,i--
                }
            }else {//ptr->forwards[i]->score>score
                if(i<=newNode->level){
                    newNode->backward=ptr;
                    newNode->forwards[i]=ptr->forwards[i];
                    ptr->forwards[i]=newNode;
                    newNode->forwards[i]->backward=newNode;
                }
                break;
                //go next level,i--
            }
        }
        if(ptr->forwards[i]== nullptr){
            if(i<=newNode->level){
                ptr->forwards[i]=newNode;
                newNode->backward=ptr;
                if(i==0)this->tail = newNode;
            }
        }
    }
    mtx.unlock();
    return;
}
template <typename T>
bool OsKiplist<T>::deleteMember(T member){
    mtx.lock();
    if(this->record.find(member)==record.end())return false;
    double score = this->record[member];
    remove(this->record.begin(), this->record.end(),member);
    Node<T> * ptr = header;
    for(int i = this->maxLevel;i>=0;i--){
        while(ptr->forwards[i]!= nullptr){
            if(ptr->forwards[i]->score<score){
                ptr= ptr->forwards[i];
            }else if (ptr->forwards[i]->score > score){
                break;//go to next level
            }else if(ptr->forwards[i]->score==score){
                if(ptr->forwards[i]->value<member){
                    ptr=ptr->forwards[i];
                }else if(ptr->forwards[i]->value>member){
                    break;//go to next level
                }else{//ptr->forwards[i]->value==member
                    ptr->forwards[i]=ptr->forwards[i]->forwards[i];
                    if(i==0)delete ptr->forwards[i]->backward;
                    if(ptr->forwards[i]!= nullptr)ptr->forwards[i]->backward=ptr;
                    break;//go to next level to delete more
                }
            }
        }
    }
    mtx.unlock();
    return true;
}

template <typename T>
int OsKiplist<T>::getRandomLevel() {
    int level = 0;
    // 使用标准的随机数生成方法
    while ((static_cast<double>(rand()) / RAND_MAX) < SKIPLIST_P && level < this->maxLevel) {
        level++;
    }
    return level;
}


#endif //OSKIPLIST_OSKIPLIST_H
