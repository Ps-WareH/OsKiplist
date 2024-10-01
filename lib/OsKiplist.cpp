//
// Created by 张博坤 on 2024/9/20.
//

#include "OsKiplist.h"
template<typename T>

void OsKiplist<T>::insert(double score, T val) {
    mtx.lock();
    assert(val!= NULL);
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
    // 当 level < maxLevel，且随机数小于设定的晋升概率时，level + 1
    srand(static_cast<unsigned int>(std::time(nullptr)));
    // 生成一个 0 到 1 之间的随机浮点数
    double randomNum = static_cast<double>(rand());
    cout<<"randomNum "<<randomNum<<endl;
    while (randomNum < SKIPLIST_P && level < this->maxLevel)
        level += 1;
    cout<<"level "<<level<<endl;
    return level;
}