/**
    Utility.h的实现，包括NFA, DFA, Proxy三个类函数的实现
*/

#include"Utility.h"
#include<vector>


/**
    #####################################################
                            NFA的实现
    #####################################################
*/

//初始化一个处理字符c的NFA图
//这里原作者id的类型是char ？？？ 暂时没看懂，，先改成 int
NFA::NFA(char c, int id1, int id2)
{
    //给开始结点和结束结点分配空间，各结点的状态设置为NORMAL
    startNode = new NFANode(id1, NORMAL);
    endNode = new NFANode(id2, NORMAL);
    //新建一条从startNode到endNode的，处理字符c的边
    NFAEdge edge(startNode, endNode, c);
    //将edge加入startNode的出边中
    //为什么不用加入endNode的入边呢？？？？？？？暂时没看懂
    startNode -> outEdges.push_back(edge);
}


//初始化一个识别符号未知的NFA图
NFA::NFA(char c)
{
    startNode = new NFANode();
    endNode = new NFANode();
    NFAEdge edge(startNode, endNode, c);
    startNode -> outEdges.push_back(edge);
}


/**
    this | nfa
    实现思路：新建startNode和endNode，将二者连接
    这里只考虑了二分支的情况，多分支的情况具体交给读取程序。。。。还没看到这部分。。。。先欠着
*/
void NFA::Or(const NFA &nfa)
{
    //编号工作最后进行，所以使用DEFAULT_ID
    NFANode * newStartNode = new NFANode();
    NFANode * newEndNode = new NFANode();
    //新建四条边
    NFAEdge edge11(newStartNode, nfa.startNode, EPSILION);
    NFAEdge edge12(newStartNode, this -> startNode, EPSILION);
    NFAEdge edge21(newEndNode, nfa.endNode, EPSILION);
    NFAEdge edge22(newStartNode, this -> endNode, EPSILION);
    //把边和结点连起来
    newStartNode -> outEdges.push_back(edge11);
    newStartNode -> outEdges.push_back(edge12);
    newEndNode -> inEdges.push_back(edge21);
    newEndNode -> inEdges.push_back(edge22);
    //修改新NFA图的startNode和endNode
    this -> startNode = newStartNode;
    this -> endNode = newEndNode;
}


/**
    ab
    实现思路：直接把this->endNode 和nfa -> startNode之间加一条NFAEdge连起来
*/
void NFA::And(const NFA &nfa)
{
    //新建一条边
    NFAEdge newEdge(this -> endNode, nfa.startNode, EPSILION);
    //前后连起来
    this -> endNode->outEdges.push_back(newEdge);
    nfa.startNode->inEdges.push_back(newEdge);
    //修改endNode
    this -> endNode = nfa.endNode;
}
/**
    a* 闭包
    实现思路：比较复杂，
            需要新建startNode和endNode，然后和原先的连起来，然后把原先的startNode和endNode连起来
            总结一下，就是新建两个结点和四条边，具体的看浴帘老师的ppt好吧。。。
*/
void NFA::Star()
{
    //新建开始结点和结束结点
    NFANode * newStartNode = new NFANode();
    NFANode * newEndNode = new NFANode();
    //新建四条边
    NFAEdge edge11(newStartNode, this -> startNode, EPSILION);
    NFAEdge edge12(newStartNode, this -> endNode, EPSILION);
    NFAEdge circleEdge(this -> endNode, this -> startNode, EPSILION);
    NFAEdge endEdge(this -> endNode, newEndNode, EPSILION);
    //连起来
    newStartNode -> outEdges.push_back(edge11);
    newStartNode -> outEdges.push_back(edge12);
    this -> startNode -> inEdges.push_back(edge11);
    this -> startNode -> inEdges.push_back(circleEdge);
    this -> endNode -> outEdges.push_back(circleEdge);
    this -> endNode -> outEdges.push_back(endEdge);
    newEndNode -> inEdges.push_back(endEdge);
    newEndNode -> inEdges.push_back(edge12);
    //修改startNode和endNode
    this -> startNode = newStartNode;
    this -> endNode = newEndNode;
}

//当前NFA图是否为空
bool NFA::isEmpty() const
{
    return this -> startNode == NULL;
}

//id对应的结点是不是endNode
bool NFA::isEnd(int id) const
{
    return this -> endNode -> id == id;
}
//获取endNode的id
int NFA::getEndId() const
{
    return this -> endNode -> id;
}
//获取startNode的id
int NFA::getStartId() const
{
    return this -> startNode -> id;
}


/**
    #####################################################
                            DFA的实现
    #####################################################
*/


