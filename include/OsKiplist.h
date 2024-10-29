//
// Created by 张博坤 on 2024/9/20.
//
#ifndef OSKIPLIST_OSKIPLIST_H
#include "Node.h"
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <mutex>
#include <unordered_map>
#include <limits>
#define SKIPLIST_P 0.5
#define OSKIPLIST_OSKIPLIST_H

using namespace std;
template<typename T>
//如果多个线程持有同一个skiplist实例，那么mtx是共享的，即同一时刻，只有一个线程，可以访问mtx
class OsKiplist {

    int maxLevel=3;

    std::mutex mtx;
public:
    Node<T>* header;
    unordered_map<T,double> record;
    OsKiplist(int maxLevel){
        assert(maxLevel>0);
        this->maxLevel=maxLevel;
        header= new Node(0-numeric_limits<double>::infinity(),static_cast<T>("dummy"),maxLevel);

    }
    //score from low->high
    //value from low->high
    //如果一个元素已经存在，score会覆盖
    void insertNew(Node<T>* newNode);
    void insert(double score, T val);
    bool deleteMember(T memeber);
    void dumpFile();
    void loadFile();
//    ZRANGEBYSCORE myzset 1.0 3.0
    vector<T> searchBetween(double minS, double maxS);
    int getRandomLevel();
    double getScore(T value){
        if(record.find(value)==record.end())return NULL;
        return record[value];
    }
};

template<typename T>
vector<T> OsKiplist<T>::searchBetween(double minS, double maxS){
    mtx.lock();
    vector<T> res;
    Node<T> * ptr = header;
    for(int i = this->maxLevel-1;i>=0;i--){
        while(ptr->forwards[i]!= nullptr){
            if(ptr->forwards[i]->score<minS){
                ptr= ptr->forwards[i];
            }else if (ptr->forwards[i]->score > minS){
                break;//go to next level
            }else if(ptr->forwards[i]->score==minS){
                break;
            }
        }
    }
    while(ptr!= nullptr){
        if(ptr->score>=minS && ptr->score<=maxS){
            res.push_back(ptr->value);
            ptr=ptr->forwards[0];
        }else if(ptr->score<minS){
            ptr=ptr->forwards[0];
        }else{
            break;
        }
    }
    mtx.unlock();
    return res;
}
template<typename T>
void OsKiplist<T>::insertNew(Node<T>* newNode){
    Node<T>* ptr = this->header;
    double score = newNode->score;
    T val = newNode->value;

    for(int i = this->maxLevel-1;i>=0;i--) {
        while (ptr->forwards[i] != nullptr) {
            if (ptr->forwards[i]->score < score) {
                ptr = ptr->forwards[i];
            } else if (ptr->forwards[i]->score == score) {
                if (ptr->forwards[i]->value < val) {
                    ptr = ptr->forwards[i];
                } else if (ptr->forwards[i]->value == val) {
                    ptr->forwards[i]->score = score;//update
                    break;//to next level
                } else {//ptr->forwards[i]->value>val,consider insert
                    if (i <= newNode->level - 1) {
                        newNode->backward = ptr;
                        newNode->forwards[i] = ptr->forwards[i];
                        ptr->forwards[i] = newNode;
                        newNode->forwards[i]->backward = newNode;
                    }
                    break;
                    //go next level,i--
                }
            } else {//ptr->forwards[i]->score>score
                if (i <= newNode->level - 1 && i>=0) {
                    newNode->backward = ptr;
                    newNode->forwards[i] = ptr->forwards[i];
                    ptr->forwards[i] = newNode;
                    newNode->forwards[i]->backward = newNode;
//                    i-=1;
                }
                break;
                //go next level,i--
            }
        }
        if (ptr->forwards[i] == nullptr) {
            if (i <= newNode->level - 1 && i>=0) {
                ptr->forwards[i] = newNode;
                newNode->backward = ptr;
//                i-=1;
            }
        }
    }
}
template<typename T>
void OsKiplist<T>::insert(double score, T val) {
    mtx.lock();
    assert(!val.empty());
    //score & value from low->high in list
    Node<T>* newNode = nullptr;
    double originScore = 0;
    if(this->record.find(val)==this->record.end()){
        newNode = new Node(score, val,getRandomLevel());//it's insert not update
//        cout<<newNode->level<<endl;//0,1,2
    }else originScore=this->record[val];
    this->record[val]=score;

    if(newNode!= nullptr){
        insertNew(newNode);
    }else {
        Node<T>* ptr = this->header;
        for(int i = this->maxLevel-1;i>=0;i--){
            while(ptr->forwards[i]!= nullptr){
                if(ptr->forwards[i]->score<originScore){
                    ptr= ptr->forwards[i];
                }else if (ptr->forwards[i]->score > originScore){
                    break;//go to next level
                }else if(ptr->forwards[i]->score==originScore){
                    ptr->forwards[i]->score = score;
                    break;
                }
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
    for(int i = this->maxLevel-1;i>=0;i--){
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
    int level = 1;
    // 使用标准的随机数生成方法
    while ((static_cast<double>(rand()) / RAND_MAX) < SKIPLIST_P && level <=this->maxLevel) {
        level++;
    }
    return level;
}
void dumpFile(){

}


#endif //OSKIPLIST_OSKIPLIST_H
