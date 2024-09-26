//
// Created by 张博坤 on 2024/9/20.
//
#ifndef OSKIPLIST_NODE_H
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#define OSKIPLIST_NODE_H

template<typename T>
class Node {
    double score;
    const char* value;
    int level;//如果在level 1， 那么有2个forward指针
    Node* backward;// node before
    Node* forwards[];// forwards[0]: node after this in level 0
    const char* valueToString(T val) {
        if constexpr (std::is_arithmetic<T>::value) {
            // 如果是数值类型，使用 std::to_string 转换
            return std::to_string(val).c_str();
        } else if constexpr (std::is_same<T, std::string>::value) {
            // 如果是 std::string，返回 c_str()
            return val.c_str();
        } else if constexpr (std::is_same<T, const char*>::value) {
            // 如果是 const char*，直接返回
            return val;
        } else {
            throw std::invalid_argument("Unsupported type");
        }
    }
public:
    Node(double score, T value, int level){
        this->level=level;
        // 把value从栈存到堆
        this->score = score;
        const char* strVal = valueToString(value);
        //(strlen(strVal)+1)/4???? for one byte
        value = (char*)malloc(strlen(strVal) + 1);
        // 分配内存 +1 是为了包含 '\0'
        //模仿raw编码
        strcpy((char*)value, strVal);
        this->forwards = malloc((this->level + 1) * sizeof(Node<T>*));//a vec of nullptr
    };
    ~Node(){
        free(this->value);
        for(auto& p : this->forwards){
            if(p!= nullptr)free(p);
            free(backward);
        }
    }
};
#endif //OSKIPLIST_NODE_H
