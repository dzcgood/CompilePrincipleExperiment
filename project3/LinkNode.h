#ifndef LINKNODE_H
#define LINKNODE_H
#include<vector>
#include<string>
#include<iostream>
using namespace std;
//使用'@'标识EPSILON
#define EPSILON "@"
/**
    文法结点的定义
*/
class LinkNode
{
public:
    //规则左部
    string left;
    //规则右部分 abc | def | hji，一个vector<string>元素代表一种可能
    vector<vector<string>> right;
    //构造函数，str是左边部分
    LinkNode(string str)
    {
        left = str;
        right.clear();
    }
    //右边添加规则
    void insert(vector<string> &nodes)
    {
        right.push_back(nodes);
    }
    //判断是否有epsilon存在
    bool includesEpsilon()
    {
        for(size_t i = 0; i < right.size(); i++)
        {
            if(right[i].size() == 1 && right[i][0] == EPSILON)
            {
                return true;
            }
        }
        return false;
    }
    //获取当前语法规则
    string toString()
    {
        string message = "";
        for(string s : this -> right[0])
        {
            message += s;
            cout<< s;
        }
        for(size_t i = 1; i < right.size(); i++)
        {
            message += " | ";
            cout<< " | ";
            for(string s : this -> right[i])
            {
                message += s;
                cout << s;
            }
            cout << endl;
        }
        return message;
    }
};

#endif // LINKNODE_H
