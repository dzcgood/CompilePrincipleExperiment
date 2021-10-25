#ifndef UTILITY_H
#define UTILITY_H
#include<vector>
#include<set>
#include<string>
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
    //该结点包含的结点编号
    set<int> nodes;
    //由该结点出发的边
    vector<DFAEdge> edges;
    //不知道这东西是干嘛的、、、、、、、、、、、、、、、、、、
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


#endif // UTILITY_H
