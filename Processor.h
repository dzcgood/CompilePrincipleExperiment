#ifndef GRAMMER_H
#define GRAMMER_H
#include<LinkNode.h>
#include<map>
#include<set>
#include<fstream>
//定义最大非终结符号数
#define MAX_NODE_NUMBER 1024
using namespace std;

class Processor
{
public:
    //存用户输入的语句
    vector<string>input;
    //开始节点索引
    int startWordIndex;
    //开始结点值
    string startNode;
    //存文法
    vector<LinkNode> grammers;
    //终结符号
    vector<string> finalWord;
    //非终结符号
    vector<string> nonFinalWord;
    //First集合
    map<string, set<string>> first;
    //Follow集合
    map<string, set<string>> follow;
    //文件流，用于写出日志
    ofstream log;
    //构造函数，参数是文件路径
    Processor(string filePath);
    //初始化终结符号和非终结符号
    void init();
    //处理左递归
    void dealLeftRecursion();
    //处理左公因子
    void dealLeftCommonFacotr();
    //化简
    void simplify();
    //生成First集合
    void getFirst();
    //生成Follow集合
    void getFollow();
    //判断是不是终结符号
    bool isFinalWord(string word);
    //判断是不是该文法能处理的符号
    bool isWord(string word);
    //按照指定符号拆分字符串
    vector<string> splitString(string str, string spliter);
    //输出当前文法信息到log文件
    void printGrammers();
    //最左推导
    vector<vector<string>> leftMostDerivation();
private:
    //用于存储是否访问过
    bool visited[MAX_NODE_NUMBER];
    //找到非终结符号word对应的是第几条规则
    int getIndex(string word);
    //配合getFirst()函数使用，递归实现求某条规则的first集合
    void findFirst(int i);
    //深度优先遍历
    void dfs(int x);
    //处理单条文法的左公因子
    vector<LinkNode> dealCommonFactor(LinkNode&node);
    //生成一个没有使用过的非终结符号名
    string generateNewName(string source);
    //将str1的follow添加到str2的follow集合
    void append(const string &str1, const string &str2);
};

#endif // GRAMMER_H
