/**
    Utility.h的实现，包括NFA, DFA, Proxy三个类函数的实现
*/

#include"Utility.h"
#include<vector>
#include<iterator>
#include<iostream>
#include<queue>
#include<string.h>

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
    这里只考虑了二分支的情况
    多分支的情况具体交给读取程序,程序读出n个'|'，就会有n+1个分支，就相当于执行n次Or
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

//浅复制
void NFA::operator=(const NFA &nfa)
{
    this -> startNode = nfa.startNode;
    this -> endNode = nfa.endNode;
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


/**
    #####################################################
                            Proxy的实现
    #####################################################
*/
//构造函数，用正则表达式来初始化Proxy代理类
Proxy::Proxy(const string regExp)
{
    //设置正则表达式
    this -> regularExpression = regExp;
    //产生NFA
    this -> nfa = getNFA(this -> regularExpression);
    //给NFA编号并初始化DFA
    this -> serializeNFA();
}

/**
 * 把正则表达式看成 a | b | c的形式，以'|'为分隔符号，所以要先获取'|' 的索引
 * 这里会忽略‘()’内的'|'，因为‘()’内可以看成是另外一个正则表达式，一次递归只处理一个正则表达式
 */
vector<int> Proxy::getOrOperatorIndex(const string regExp)
{
    vector<int> index;
    int len = regExp.length();
    for(int i = 0; i < len; i++)
    {
        if(regExp[i] == '|')
            index.push_back(i);
        else if(regExp[i] == '(')
        {
            //忽略掉()内的'|'
            int temp = getRightBracketIndex(regExp, i);
            if(temp = -1)//说明不存在与之对应的')'
            {
                std::cout << "illeagal regular expression!";
                exit(1);
            }
            i = temp;
        }
    }
    return index;
}

//判断ch是不是字符（字母或数字）
bool Proxy::isLetter(char ch)
{
    return isdigit(ch) || isalpha(ch);
}


//获取索引为index的左括号对应的右括号的索引，在初始化NFA的时候会用到
int Proxy::getRightBracketIndex(const string regExp, const int leftIndex)
{
    //可能会出现((()))层层嵌套的情况，所以要记录在leftIndex之后，出现了几个'('
    int newLeftBrackets = 0;
    int len = regExp.length();
    for(int i = leftIndex + 1; i < len; i++)
    {
        switch (regExp[i]){
        case ')':
            //新的'('个数为0的情况下，才是我们要找的')'索引
            if(newLeftBrackets == 0)
                return i;
            else newLeftBrackets--;
            break;
        case '(':
            //遇到新的未匹配'('，个数 + 1
            newLeftBrackets++;
            break;
        default:
            break;
        }
    }
    return -1;
}

/**
    获取NFA图，由输入的正则表达式产生NFA
    正则表达式转NFA递归方法思路：
    首先把(..) 看成一个单元素NFA,和a等价
    把单个NFA看成一个或数个元素的Union，即 NFA = a[|b|c...]
    扫描正则表达式，首先扫描 | 进行拆分递归
    逐项建立NFA并连接对于括号进行递归处理
*/
NFA Proxy::getNFA(const string regExp)
{
    //先获取'|'的索引
    vector<int> orIndex = getOrOperatorIndex(regExp);
    int len = regExp.length();
    //先按照'|'拆分正则表达式，然后以拆分好每份表达式为单位
    if(orIndex.size() != 0)
    {
        //用于存放每个单位生成的NFA，最后把每个单位用类似于or的操作连起来就行了
        vector<NFA> NFAs;
        //正则表达式字串
        string subReg;
        int curIndex = 0;
        for(size_t i = 0; i < orIndex.size(); i++)
        {
            subReg = regExp.substr(curIndex, orIndex[i] - curIndex);
            curIndex = orIndex[i];
            //将字串进行递归，生成的NFA放入NFAs
            NFAs.push_back(getNFA(subReg));
        }
        //最后还有一个字串
        subReg = regExp.substr(curIndex, regExp.size() - curIndex);
        NFAs.push_back(getNFA(subReg));

        //接下来执行类似于or的操作，NFAs里面有几个NFA，就会有几个分支
        /*
            大概长这样
                  ／ o ＼
                o  — o — o
                  ＼ o ／
            左右两个是新增结点，中间的叉是NFAs的元素，有几个元素，就有几个叉
        */
        //新增的两个结点，不管有几个叉，只需要两个新结点
        NFANode * node1 = new NFANode();
        NFANode * node2 = new NFANode();
        for(NFA n : NFAs)
        {
            //对于每个叉，新增两条边
            //新节点到NFA起点的边
            NFAEdge edge1(node1, n.startNode, EPSILION);
            //NFA结束点到新结点的边
            NFAEdge edge2(n.endNode, node2, EPSILION);
            //把前后结点和边建立联系
            node1 -> outEdges.push_back(edge1);
            n.startNode -> inEdges.push_back(edge1);
            node2 -> inEdges.push_back(edge2);
            n.endNode -> outEdges.push_back(edge2);
        }
        //各个NFA都已经连好了，接下来直接返回以node1为起点，以node2为终点的NFA图即可
        return NFA(node1, node2);
    }

    //程序能走到这里，说明regExp已经不包含'|'了，接下来需要对括号进行处理

    NFA nfa;
    //记录当前字母和下一个字母
    char cur, next;
    //因为这里设计到next，为了不下标越界，将最后一个字符分开处理
    for(int i = 0; i< len - 1; i++)
    {
        cur = regExp[i];
        next = regExp[i + 1];
        if(isLetter(cur))//如果当前字符是待匹配字符
        {
            //初始化一个处理字符cur的NFA
            NFA n(cur);
            //如果下一个元素是'*'，则需要先闭包再连接
            if(next == '*')
            {
                n.Star();
                //越过'*'
                i++;
            }
            //判空
            if(nfa.isEmpty())
            {
                nfa = n;
            }
            else//执行连接操作
            {
                nfa.And(n);
            }
        }
        //遇见()需要递归
        else if(cur == '(')
        {
            int rightBacketIndex = getRightBracketIndex(regExp, i);
            //没有对应匹配的')' 程序终止
            if(rightBacketIndex == -1)
            {
                exit(1);
            }
            //取出括号内的内容
            string subReg = regExp.substr(i + 1, rightBacketIndex - i - i);
            next = regExp[rightBacketIndex + 1];
            if(next == '*')//需要先闭包后连接
            {
                if(nfa.isEmpty())
                {
                    nfa = getNFA(subReg);
                    nfa.Star();
                    i = rightBacketIndex + 1;
                }
                else
                {
                    NFA subNfa = getNFA(subReg);
                    subNfa.Star();
                    nfa.And(subNfa);
                    i = rightBacketIndex + 1;
                }
            }
            else//直接连接操作
            {
                if(nfa.isEmpty())
                {
                    nfa = getNFA(subReg);
                }
                else
                {
                    nfa.And(getNFA(subReg));
                }
            }
        }
    }
    //处理最后一个字符
    if(isLetter(regExp[len -1]))
    {
        if(nfa.isEmpty())
        {
            nfa = NFA(regExp[len - 1]);
        }
        else
        {
            nfa.And(NFA(regExp[len - 1]));
        }
    }
    return nfa;
}



//给NFA的结点编号并建立初始DFA结点
void Proxy::serializeNFA()
{
    //空NFA图，直接结束
    if(nfa.isEmpty())
        return;
    //先设置好NFA图的开始结点和结束结点
    nfa.startNode -> state = START;
    nfa.endNode -> state = END;
    //指针
    NFANode * beginNode = nfa.startNode;
    //用于编号，第一个结点的编号为1
    int id = 1;
    beginNode -> id = id++;
    //
    NFANode *nfaNode;
    DFANode *dfaNode;
    /*
        接下来开始寻找可建立的DFANode，最后找到的是n个EPSILION闭包，每个闭包构成一个DFANdoe
        que1用于寻找DFANode，que2用于寻找某个DFANode的EPSILION闭包
        简单地说，每一个DFANode就是一个状态集合
    */
    //采用广度优先方法
    queue<NFANode*> que1,que2;
    que1.push(beginNode);
    //设置bool数组，以防止反复地以同一结点建立EPSILION闭包
    bool visited[MAX_NODE_NUMBER];
    memset(visited, false, sizeof(visited));
    while(!que1.empty())
    {
        //取出队首元素
        nfaNode = que1.front();
        que1.pop();
        //建一个新DFANode
        dfaNode = dfa.crateNewNode(nfaNode -> id);
        //设置该结点已被访问
        visited[nfaNode -> id] = true;
        //接下来开始寻找这个DFANode的EPSILION闭包
        //防止重复访问
        bool visited1[MAX_NODE_NUMBER];
        memset(visited1, false, sizeof(visited1));
        que2.push(nfaNode);
        while(!que2.empty())
        {
            nfaNode = que2.front();
            que2.pop();
            visited1[nfaNode -> id] = true;
            //开始遍历从nfaNode出发的边
            for(NFAEdge edge : nfaNode -> outEdges)
            {
                //如果这条边指向的结点还未被编号，就给它编号
                if(edge.endNode -> id == DEFAULT_ID)
                {
                    edge.endNode -> id = id++;
                }
                //在图表中将这条边记录下来
                chart[edge.startNode -> id][edge.endNode -> id] = edge.word;
                //如果这条边是处理字符EPSILION的
                if(edge.word == EPSILION)
                {
                    //就加入此DFANode的EPSILION闭包
                    dfaNode -> insert(edge.endNode -> id);
                    //如果edge.endNode还被没访问过，就加入que2
                    if(!visited1[edge.endNode -> id])
                    {
                        que2.push(edge.endNode);
                    }
                }
                else//说明不是处理EPSILION的
                {
                    //将edge -> word加入到此DFA图能处理的字符中
                    wordList.insert(edge.word);
                    //如果这个点还么被访问过，就需要新建一个DFANode了
                    if(!visited[edge.endNode -> id])
                    {
                        que1.push(edge.endNode);
                        //这样可以防止队列中出现重复元素
                        visited[edge.endNode -> id] = true;
                    }
                }
            }
        }
        //如果此DFANode是终止点，修改其状态
        if(dfaNode -> contains(nfa.getEndId()))
        {
            nfaNode -> state = END;
        }
    }
    //进一步处理这些建立好的DFANode
    processDFA();
}

/**
    初始化DFA后，只是建立了DFA的结点，但是结点与结点之间的关系并没有确定，所以需要进一步处理
*/
void Proxy::processDFA()
{

}

//最小化DFA
void Proxy::minimizeDFA()
{

}


//最小DFA图后处理
void Proxy::processMinDFA()
{

}


/*
    生成c语言代码
    从finalDFA的开始结点获取code即可
*/
void Proxy::generateCode()
{
    this -> code = "";
    vector<string> lines;
    DFANode * node = this -> finalDFA.minStartNode;
    finalDFA.getCode(node, lines, 0);
    for(string line : lines)
    {
        code += line;
        code += std::to_string('\n');
    }
}

/**
    获取与编号为id的结点有关联的DFANode的id构成的向量
*/
vector<int> Proxy::getConnections(int id)
{
    vector<int> con;
    for(int i = 1; i <= nfa.nodeNumber; i++)
    {
        //说明可以执行 id -> i的非EPSILION转换
        if(chart[id][i] != DEFAULT_WORD && chart[id][i] != EPSILION)
        {
            con.push_back(i);
        }
    }
    return con;
}






