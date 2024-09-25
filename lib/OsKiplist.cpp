//
// Created by 张博坤 on 2024/9/20.
//

#include "OsKiplist.h"
template<typename T>

bool OsKiplist<T>::insert(double score, T val) {
    assert(val!= NULL);
    // find position of this score first
    //if this elem alr exits,
    //score from low->high in list
    Node<T>* ptr = header;
    for(int i = this->maxLevel;i>=0;i--){
        while(ptr->forwards!= nullptr){//not the tail of the list
            if(ptr->forwards[i]->score<score){
                ptr=ptr->forwards[i];
            }else if (ptr->forwards[i]->score==score){
                if(ptr->forwards[i]->value<val){
                    ptr=ptr->forwards[i];
                }else if(ptr->forwards->value==val){
                    return false;
                }else{//go next level,i--
                    break;
                }
            }else break;//go next level, i--
        }
    }
    Node<T>* newNode = new Node (score, val,getRandomLevel());
    ptr = header;
    for(int i = this->maxLevel;i>=0;i--){
        while(ptr->forwards[i]!= nullptr){
            if(ptr->forwards[i]->score<score){
                ptr=ptr->forwards[i];
            }else if (ptr->forwards[i]->score==score){
                if(ptr->forwards[i]->value<val) {
                    ptr = ptr->forwards[i];
                }else{//ptr->forwards[i]->value>val
                    newNode->backward=ptr;
                    newNode->forwards[i]=ptr->forwards[i];
                    ptr->forwards[i]=newNode;
                    newNode->forwards[i]->backward=newNode;
                    break;
                    //go next level,i--
                }
            }else {//ptr->forwards[i]->score>score
                newNode->backward=ptr;
                newNode->forwards[i]=ptr->forwards[i];
                ptr->forwards[i]=newNode;
                newNode->forwards[i]->backward=newNode;
                break;
                //go next level,i--
            }
        }
        if(ptr->forwards[i]== nullptr){
            ptr->forwards[i]=newNode;
            newNode->backward=ptr;
            if(i==0)this->tail = newNode;
        }
    }
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