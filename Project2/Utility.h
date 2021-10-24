#ifndef UTILITY_H
#define UTILITY_H
#include"vector"
using namespace std;
const int DEFAULT_ID = -1;
const char DEFAULT_WORD = ' ';
const char EPSILION = '#';

/**
    定义各个结点的状态：开始，结束，普通
*/
enum State{
    START,END,NORMAL
};

/**
    NFA的边，先声明，在NFANode中要用到，稍后定义
*/
struct NFAEdge;

/**
    NFA的结点
*/
struct NFANode{
    //唯一标记结点
    int id;
    //标实该结点是否为开始结点或结束结点
    State state;
    //入边
    vector<NFAEdge> inEdges;
    //出边
    vector<NFAEdge> outEdges;
    //构造函数
    NFANode(int i, State s) : id(i), state(s){}
    //空参构造函数
    NFANode(): id(DEFAULT_ID), state(NORMAL){}
};

/**
    NFA的边
*/
struct NFAEdge{
    //该边由startNode指向endNode
    NFANode * startNode;
    NFANode * endNode;
    //该边处理字符word
    char word;
    //构造函数
    NFAEdge(NFANode * s, NFANode * e, char c): startNode(s), endNode(e), word(c){}
    //空参构造函数
    NFAEdge(): startNode(NULL), endNode(NULL), word('\0'){}
};

/**
    NFA类，集成构建NFA图的操作
*/
class NFA{
public:
    //NFA图的开始结点
    NFANode * startNode;
    //NFA图的结束结点
    NFANode * endNode;
    //结点个数
    int nodeNumber;
    //空参构造函数
    NFA():startNode(NULL), endNode(NULL), nodeNumber(2){}
    //初始化一个处理字符c的NFA图
    //这里原作者id的类型是char ？？？ 暂时没看懂，，先改成 int
    NFA(char c, int id1, int id2);
    //初始化一个识别c的标号未定的NFA图
    NFA(char c);
    //已知开始结点和结束结点
    NFA(NFANode * s, NFANode * e): startNode(s), endNode(e){}
    // a | b 选择
    void Or(const NFA &nfa);
    // ab 连接
    void And(const NFA &nfa);
    // a* 闭包
    void Star();
    //当前NFA图是否为空
    bool isEmpty() const;
    //id对应的结点是不是endNode
    bool isEnd(int id) const;
    //获取endNode的id
    int getEndId() const;
    //获取startNode的id
    int getStartId() const;
};

#endif // UTILITY_H
