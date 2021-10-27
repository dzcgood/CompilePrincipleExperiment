#ifndef UTILITY_H
#define UTILITY_H
#include<vector>
#include<set>
#include<string>
using namespace std;
const int DEFAULT_ID = -1;
const char DEFAULT_WORD = ' ';
const char EPSILION = '#';
const int MAX_NODE_NUMBER = 256;

/**
    #####################################################
                      该头文件涉及到相关类的定义
                         包括NFA,DFA,Proxy
    #####################################################
*/


/**
    定义各个结点的状态：开始，结束，普通
*/
enum State{
    START,END,NORMAL
};


/**
    #####################################################
                            NFA的相关定义
    #####################################################
*/
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
    //浅复制
    void operator=(const NFA &nfa);
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



/**
    #####################################################
                            DFA的相关定义
    #####################################################
*/
//DFA结点，先做声明，稍后定义
struct DFANode;

//DFA边
struct DFAEdge
{
    //边指向的结点
    DFANode * next;
    //处理的字符
    char word;
    //构造函数
    DFAEdge(DFANode *n, char c): next(n), word(c){}
};

//DFA结点
struct DFANode
{
    //该结点的EPSILION闭包
    set<int> nodes;
    //由该结点出发的边
    vector<DFAEdge> edges;
    //不知道这东西是干嘛的、、、、、、、、、、、、、、、、、、
    //貌似就是给这个结点取个名字，A, B, C, D这样子。。
    string minName;
    //该结点在DFA图中的状态
    State state = NORMAL;
    //判断结点id是否在该结点中
    bool contains(int id);
    //在该结点插入id
    void insert(int id);
    //在该结点插入边
    void insert(DFAEdge edge);
    //将两个结点合成一个结点
    void unionNode(DFANode * node);
    //该结点处理字符c后转变为的结点
    DFANode * process(char c);
    //输出该结点信息 如： {1，2，4，6}
    string toString();
};

//DFA类

class DFA
{
public:
    //DFA图的结点
    vector<DFANode*> graph;
    //通过id返回图的结点
    vector<DFANode*> getNodes(int id);
    //DFA图最小开始结点
    DFANode *minStartNode;
    //DFA图最小结束结点
    vector<DFANode*> minEndNodes;
    //能处理的字符
    set<char> wordList;
    //建一个新结点
    DFANode* crateNewNode(int id);
    //判断两个结点的转化是否等价，若转化后是同一个结点，则等价
    static bool equals(DFANode *node1, DFANode *node2, set<char> words);
    //判断该结点是否为结束结点
    bool isEndNode(DFANode *node) const;
    //从当前DFA中删除某个结点
    void delNode(DFANode *node);
    //获取该结点的c语言代码，n是结点指针，lines是生成的代码，tabNumber是缩进数
    void getCode(DFANode *n, vector<string> &lines, int tabNumber);
private:
    //获取缩进的空格
    string getTabs(int tabNumber);
};


/**
    #####################################################
                        代理类Proxy的相关定义
                        涉及对NFA和DFA的处理
    #####################################################
*/
class Proxy
{
public:
    //正则表达式
    string regularExpression;
    //NFA图
    NFA nfa;
    //最初始的DFA图
    DFA dfa;
    /*
        经过处理后的DFA图
        因为有好几个结点最后会合成一个结点，所以minDFA其实是有好多个vector组成的，
        每个vector元素又包含了好几个结点，这写结点最后会被合成一个结点
        例子： minDFA = {{1}， {3，5，7}, {2,4}, {6,8}}
     */
    vector<vector<DFANode *>> minDFA;
    //最终的DFA
    DFA finalDFA;
    //DFA最小化生成的表格
    char chart[MAX_NODE_NUMBER][MAX_NODE_NUMBER];
    //最后生成的代码
    string code;
    //能处理的字符集
    set<char> wordList;
    //构造函数，用正则表达式来初始化Proxy代理类
    Proxy(const string regExp);
    //给NFA的结点编号并建立初始DFA结点
    void serializeNFA();
    //初始DFA图后处理
    void processDFA();
    //最小化DFA
    void minimizeDFA();
    //最小DFA图后处理
    void processMinDFA();
    //生成c语言代码
    void generateCode();
private:
    //获取NFA图，由输入的正则表达式产生NFA
    NFA getNFA(const string regExp);
    //把正则表达式看成 a | b | c的形式，以'|'为分隔符号，所以要先获取'|' 的索引
    vector<int> getOrOperatorIndex(const string regExp);
    //判断ch是不是字符（字母或数字）
    bool isLetter(char ch);
    //获取索引为index的左括号对应的右括号的索引，在初始化NFA的时候会用到
    int getRightBracketIndex(const string regExp, const int leftIndex);
    //用于得知id号结点可以有哪些转化
    vector<int> getConnections(int id);
};

#endif // UTILITY_H
