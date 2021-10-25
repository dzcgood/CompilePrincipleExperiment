/**
    Utility.h的实现，包括NFA, DFA, Proxy三个类函数的实现
*/

#include"Utility.h"
#include<vector>
#include<iterator>


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
                          DFANode的实现
    #####################################################
*/

//判断结点id是否在该结点中
bool DFANode::contains(int id)
{
    if(this -> nodes.find(id) != nodes.end())return true;
    else return false;
}

//在该结点插入id
void DFANode::insert(int id)
{
    nodes.insert(id);
}

//在该结点插入边
void DFANode::insert(DFAEdge edge)
{
    edges.push_back(edge);
}

/**
    将两个结点合成一个结点
    id合在一起，edge合在一起就可以了
*/
void DFANode::unionNode(DFANode * node)
{
    nodes.insert(node -> nodes.begin(), node  -> nodes.end());
    edges.insert(edges.end(), node -> edges.begin(), node -> edges.end());
}

//该结点处理字符c后转变为的结点
DFANode * DFANode::process(char c)
{
    //如果该结点的某条边是处理字符c的，就返回这条边的next
    for(DFAEdge edge : edges)
    {
        if(edge.word == c)
        {
            return edge.next;
        }
    }
    //找不到，说明这个节点不能处理字符c
    return NULL;
}


//输出该结点信息 如： {1，2，4，6}
string DFANode::toString()
{
    string str = "{";
    set<int>::iterator it = nodes.begin();
    //int型转string型，加在{后面
    str += std::to_string(*it);
    it++;
    while(it != nodes.end())
    {
        str += ", ";
        str += std::to_string(*it);
        it++;
    }
    str += "}";
    return str;
}

/**
    #####################################################
                            DFA的实现
    #####################################################
*/

//返回该id可以在的结点的指针向量，例如id为8的可以在1，3，5号结点
vector<DFANode*> DFA::getNodes(int id)
{
    vector<DFANode*> v;
    for(DFANode * node : graph)
    {
        if(node->contains(id))
            v.push_back(node);
    }
    return v;
}

//建一个新结点
DFANode* DFA::crateNewNode(int id)
{
    DFANode * node = new DFANode();
    //把id加入新建的结点
    node -> nodes.insert(id);
    //把新结点加入graph
    graph.push_back(node);
    return node;
}

//判断两个结点的转化是否等价，若转化后是同一个结点，则等价
bool DFA::equals(DFANode *node1, DFANode *node2, set<char> words)
{
    if(node1 == node2)return true;
    for(char c : words)
    {
        if(node1 -> process(c) != node2->process(c))
            return false;
    }
    return true;
}

//判断该结点是否为结束结点
bool DFA::isEndNode(DFANode *node) const
{
    for(DFANode * n : this -> minEndNodes)
    {
        if(node == n)
            return true;
    }
    return false;
}

/**
    从当前DFA中删除某个结点
    先从graph中删除点，然后在剩下的点中，移除指向被删除点的边
*/
void DFA::delNode(DFANode *node)
{
    vector<DFANode *>::iterator it1;
    vector<DFAEdge>::iterator it2;
    //先删除点
    it1 = this -> graph.begin();
    while(it1 != this -> graph.end())
    {
        if(node == *it1)
            it1 = this -> graph.erase(it1);
        else
            it1++;
    }
    //在剩下的点中，移除指向被删除点的边
    it1 = this -> graph.begin();
    for(it1 = this -> graph.begin(); it1 != this -> graph.end(); it1++)
        for(it2 = (*it1)->edges.begin();it2 != (*it1)->edges.end();)
        {
            //如果这条边指向被删除的点
            if((*it2).next == node)
                it2 = (*it1) -> edges.erase(it2);
            else
                it2++;
        }
}

/**
    获取该结点的c语言代码
    对于从某结点出发的点，分为两类，指向自己的，以及指向下一个结点的
*/
void DFA::getCode(DFANode *n, vector<string> &lines, int tabNumber)
{
    int edgeNumber = n -> edges.size();
    if(edgeNumber > 0)
    {
        //指向下一个结点的边
        vector<DFAEdge> nextEdges;
        //指向自己的边
        vector<DFAEdge> circleEdges;
        //用于标记是否为第一行代码，如果不是第一行代码，在前面就需要加上if
        bool flag = true;
        //先写入获取字符的代码
        lines.push_back(getTabs(tabNumber) + "char ch = getChar();");
        for(DFAEdge e : n -> edges)
        {
            if(e.next == n)//指向自己
                circleEdges.push_back(e);
            else//指向别的结点
                nextEdges.push_back(e);
        }
        //先处理指向自己的边
        if(circleEdges.size() > 0)
        {
            string value = " ch == ";
            // ch == a
            value += std::to_string(circleEdges[0].word);
            for(size_t i = 1;i < circleEdges.size(); i++)
            {
                // ch == a || ch == b
                value += " || char == ";
                value += std::to_string(circleEdges[i].word);
            }
            /*
                while( ch == a || ch == b || .....)
                {
                    input(ch);
                    getChar(ch);
                }
            **/
            //开一个数组来存这五行代码
            string line[5];
            line[0] = getTabs(tabNumber) +"while(" + value + " )";
            line[1] = getTabs(tabNumber) + "{";
            line[2] = getTabs(tabNumber + 1) + "input(ch);";
            line[3] = getTabs(tabNumber + 1) + "ch = getChar();";
            line[4] = getTabs(tabNumber) + "}";
            //把这五行代码加到lines中
            for(string str : line)
            {
                lines.push_back(str);
            }
        }
        //处理指向下一个结点的边
        if(nextEdges.size() > 0)
        {
            /**
                写成这种形式，中间需要递归
                if(ch == a)
                {
                    input();
                    if(ch == b)
                    {
                        input();
                    }
                    else if(....)
                }
                else if(.....)
                {

                }
                else {cout << "error"; exit(1);}
            */
            for(DFAEdge e : nextEdges)
            {
                string line[4];
                /*
                    if(ch == e.word)
                或者
                    else if(ch == e.word)
                主要取决于是不是第一个if语句
                */
                line[0] = getTabs(tabNumber) + (flag == false ? "else " : "")
                        + "if(ch == " + e.word + ")";
                line[1] = getTabs(tabNumber) + "{";
                line[2] = getTabs(tabNumber + 1) + "input(ch)";
                line[3] = getTabs(tabNumber) + "}";
                /*
                    把这几行加入lines，需要注意的是，这里用到递归，因为下一个结点还会指向下一个结点，
                    这样才能构成完整的一个流程，所以递归到该结点的下一个结点,
                    递归回来后，在把line[3]加入到lines
                */
                for(int i = 0; i< 3; i++)
                {
                    lines.push_back(line[i]);
                }
                //递归
                getCode(e.next, lines, tabNumber + 1);
                lines.push_back(line[3]);
                //标记flag为flase，下一个if前面就需要加上else
                flag = false;
            }
            //最后补上错误信息
            lines.push_back("else {cout << \"error\"; exit(1);");
        }
    }
    //如果这个结点是终止结点，补上完成的信息
    if(isEndNode(n))
        lines.push_back(getTabs(tabNumber) + "Done();");
}

//用于获取缩进
string DFA::getTabs(int tabNumber)
{
    string str = "";
    for(int i = 0; i < tabNumber; i++)
    {
        str += "    ";
    }
    return str;
}





