/**
    Processor类的实现
*/
#include"Processor.h"
#include<string.h>
#include<iterator>
#include<algorithm>
#include<vector>
using namespace std;

/**
    按照分隔符spliter分割字符串str，并将分割结果组成vector返回
*/
vector<string> Processor::splitString(string str, string spliter)
{
    string::size_type pos1,pos2;
    pos1 = str.find(spliter);
    pos2 = 0;
    vector<string> results;
    //只要还有'|'就继续执行
    while(pos1 != string::npos)
    {
        string res = str.substr(pos2, pos1-pos2);
        results.push_back(res);
        pos2 = pos1 + 1;
        pos1 = str.find(spliter, pos2);
    }
    //最后一段子串
    results.push_back(str.substr(pos2));
    return results;
}

/**
    生成终结符号和非终结符号
*/
void Processor::init()
{
    //用于记录，如果wordMap[word]为1，则表示已经添加过
    map<string,int> wordMap;
    //清空原先的终结符号和非终结符号
    this->nonFinalWord.clear();
    this->finalWord.clear();
    for(LinkNode node : grammers)
    {
        //左边的一定是非终结符号
        wordMap[node.left] = 1;
        this->nonFinalWord.push_back(node.left);
    }
    //对于每一条规则
    for(LinkNode node : grammers)
    {
        //每一条规则的右边部分，abc | def | ghi算三个部分
        for(vector<string> words : node.right)
        {
            for(string word : words)
            {
                //如果没有在左边出现过且没有被记录过
                if(wordMap.count(word) == 0)
                {
                    this->finalWord.push_back(word);
                    wordMap[word] = 1;
                }
            }
        }
    }
}

/**
    构造函数
        创建日志文件，将处理过程写入到日志文件中，方便后续显示；
        从filePath中读取文法规则，建立每条文法规则结点，并连接成grammers
*/
Processor::Processor(string filePath)
{
    //先创建日志文件
    log.open("log.txt");
    if(!log.is_open())
    {
        cout << "创建日志文件失败！" << endl;
        exit(1);
    }
    ifstream input(filePath);
    if(!input.is_open())
    {
        cout << "文件打开失败！" << endl;
        exit(1);
    }
    string line;
    bool isFirstLine = true;
    while(getline(input, line))
    {
        // E->a，长度至少是4
        if(line.size() < 4) continue;
        log << "Scanning...>>>>\t" << line << '\n';
        string str = "";
        //去除line中所有的空格，方便处理
        for(size_t i = 0; i < line.length(); i++)
        {
            if(line[i] != ' ')
                str += line[i];
        }
        //如果找不到"->"，就报错
        if(str[1] != '-' || str[2] != '>')
        {
            cout << "输入文法格式错误！" << endl;
            exit(1);
        }
        // A->A，这样的是有害规则
        if(str.length() == 4 && str[0] == str[3])
        {
            log << "有害规则，已经删除\n";
            continue;
        }
        //左边部分
        string left = "";
        left += str[0];
        if(isFirstLine)
        {
            //设置开始结点的值
            startNode = str[0];
            isFirstLine = false;
        }

        //E->abc|def从下标3开始，都是右边部分
        string rights = str.substr(3);
        vector<string> toNodes = splitString(rights, "|");
        //新建一个文法规则的结点
        LinkNode node(left);
        //将分割好的字串添加到文法规则中
        for(string str:toNodes)
        {
            vector<string> temp;
            for(size_t i = 0; i < str.length(); i++)
            {
                string s = "";
                temp.push_back(s + str[i]);
            }
            node.insert(temp);
        }
        grammers.push_back(node);
    }
    //关闭输入流
    input.close();
    startWordIndex = -1;
    //生成终结符号和非终结符号
    init();
}

/**
    打印当前文法信息到log文件
*/
void Processor::printGrammers()
{
    for(size_t i = 0; i < grammers.size(); i++)
    {
        const string &left = grammers[i].left;
        const vector<vector<string>> &right = grammers[i].right;

        log << left << "==>";
        for(size_t i = 0; i < right[0].size(); i++)
        {
            log << ' ' << right[0][i];
        }

        for(size_t i = 1; i < right.size(); i++)
        {
            log << " |";
            for(size_t j = 0; j < right[i].size(); j++)
            {
                log << ' ' << right[i][j];
            }
        }
        log << '\n';
    }
    log << "非终结符号如下:" << endl;
    for(string word : nonFinalWord)
    {
        log << word << ' ';
    }
    log << '\n';
    log << "终结符号如下:" << '\n';
    for(string word: finalWord)
    {
        log << word << ' ';
    }
    log << '\n';
    log << "非终结符号个数:" << nonFinalWord.size() << '\n';
    log << "终结符号个数:" << finalWord.size() << '\n';
}

void Processor::dealLeftRecursion()
{
    int sizeBefore = grammers.size();
    for(size_t i = 0; i < grammers.size(); i++)
    {
        for(size_t j = 0; j < i; j++)
        {
            /*
             * 例子：S -> Qc|c
             *      R -> Sa|a
             *      Q -> Rb|b
             *  设 i : Q -> Rb|b
             *     j : R -> Sa|a
             */

            //临时存储新的右边部分
            vector< vector<string> > cont;
            vector< vector<string> > &right1 = grammers[i].right;
            vector< vector<string> > &right2 = grammers[j].right;
            string check = grammers[j].left;
            vector<vector<string>>::iterator it1 = right1.begin();
            vector<vector<string>>::iterator it2 = right2.begin();
            for(; it1 != right1.end(); it1++)
            {
                // i的右边第一个元素与j的非终结符号相同，代入
                if(it1->at(0) == check)
                {
                    // 将 j 代入 i
                    // 例如 此时 *it1 = {R,b}
                    //       而  right2 = {{S,a},{a}}
                    //   代入后  为 {S,a,b}、{a,b}

                    //先取出i的右边除了R外的所有元素
                    vector<string> other;
                    for(size_t k = 1; k < (*it1).size(); k++)   // 将i中R之后的符号代入
                    {
                        other.push_back((*it1)[k]);
                    }
                    // 此时 other = {b}

                    for(; it2 != right2.end(); it2++)
                    {
                        //用于临时存储带入后产生的式子
                        vector<string> after = *it2;
                        //将other的内容分别插入{Sa}和{b}中
                        after.insert(after.end(),other.begin(),other.end());
                        // {S, a, b} 、 {a, b}
                        cont.push_back(after);
                    }
                }
            }

            //接下来需要修改i：Q->Rb|b的右边部分
            //先把右边部分删掉
            size_t nn = right1.size();
            while(nn--)
            {
                //如果是Rb，就删掉，因为cont中已经有替换好的部分{{S，a，b}{a, b}}了
                //如果是b，就先删掉，并添加到cont中，因为cont中没有b
                if(right1.begin()->at(0) != check)
                    cont.push_back(*right1.begin());
                right1.erase(right1.begin());
            }
            //执行完后Q->{}
            //然后把cont中的内容添加到Q->的右边
            //Q->Sab|ab|b
            for( size_t i = 0; i < cont.size(); i++)
            {
                right1.push_back(cont[i]);
            }
        }
    }
    int sizeAfter = grammers.size();
    if(sizeAfter > sizeBefore)
        startWordIndex = grammers.size()-1;
    else
        startWordIndex = 0;

    // 下面消除直接左递归
    for(size_t i = 0; i < grammers.size(); i++)
    {
        //左边部分
        string check = grammers[i].left;
        //右边部分
        vector<vector<string>> temp = grammers[i].right;
        vector<vector<string>>::iterator it = temp.begin();
        //新的结点
        //如： A -> Aa|b
        //那么新建一个结点A'
        string tt = check + "'";
        bool flag = true;
        //判断这条规则是否包含左递归
        for(; it != temp.end(); it++)
        {
            // 左 = 右边第一个，发现直接左递归
            if(it->at(0) == check)
            {
                //新建一个文法结点A'
                grammers.push_back(LinkNode(tt));
                flag = false;
                break;
            }
        }
        //不包含左递归
        if(flag)
            continue;
        //包含左递归的情况：
        //临时存储A'的右边
        vector<vector<string>> cont;
        //指向最后一条文法结点
        vector<vector<string>> &ss = grammers[grammers.size()-1].right;
        vector<string> eplision;
        eplision.push_back("@");
        //ss: {{"@"}}
        ss.push_back(eplision);
        vector<vector<string>>&temp1 = grammers[i].right;
        while(!temp1.empty())
        {
            //对于A->Aa|b，如果现在处理的是Aa
            if(temp1.begin()->at(0) == check)  // 发现左递归元素
            {
                vector<string> vt;
                //就把a复制到vt中
                //vt:{a}
                vt.assign(temp1.begin()->begin()+1,
                          temp1.begin()->end());
                //在末尾增加A'
                //vt:{aA'}
                vt.push_back(tt);
                //ss:{{aA'},{"@"}}
                ss.push_back(vt);
            }
            else  // 否则，应该修改原语法,修改成：A->bA'
            {
                vector<string> vt;
                //vt:{b}
                vt.assign(temp1.begin()->begin(),
                          temp1.begin()->end());
                //vt:{bA'}
                vt.push_back(tt);
                //cont:{{bA'}}
                cont.push_back(vt);
            }
            temp1.erase(temp1.begin());
        }
        //这时A->{}
        //cont:{{bA'}}
        //用cont里面的内容取代A的右边
        for(size_t i = 0; i < cont.size(); i++)
        {
            temp1.push_back(cont[i]);
        }
    }
    //加入了新的结点，要重新init
    init();
}

/**
    判断word是不是终结符号
*/
bool Processor::isFinalWord(string word)
{
    return find(finalWord.begin(), finalWord.end(), word) != finalWord.end();
}

/**
    获取非终结符号word是出现在第几条文法规则的左边
*/
int Processor::getIndex(string word)
{
    for(size_t i = 0; i < grammers.size(); i++)
    {
        if(grammers[i].left == word)
            return i;
    }
    return -1;
}

/**
    求第i条规则左边的非终结符号的first集合
*/
void Processor::findFirst(int i)
{
    //已经求过了就不用求了
    if(visited[i])
        return;
    //标记为求过
    visited[i] = true;
    //规则左边
    string left = grammers[i].left;
    //规则右边
    vector<vector<string>> right = grammers[i].right;
    vector<vector<string>>::iterator it = right.begin();
    for(; it != right.end(); it++)
    {
        for(size_t j = 0; j < it -> size(); j++)
        {
            string word = it -> at(j);
            if(isFinalWord(it -> at(j)) == false)//说明是非终结符号或者EPSILON
            {
                //接下来就要求 it -> at(j)的first集合
                int index = getIndex(word);
                //递归求word的first集合
                findFirst(index);
                //flag为true，则word的first集合没有EPSILON，为false，则有
                bool flag = true;
                //开始遍历word的first集合
                set<string> first_y = first[word];
                set<string>::iterator it1 = first_y.begin();
                for(; it1 != first_y.end(); it1++)
                {
                    //说明word的first集合包含EPSILON
                    if(*it1 == EPSILON)
                    {
                        flag = false;
                    }
                    else
                        first[left].insert(*it1);
                }
                if(flag)
                    break;
                else if(j == it -> size() - 1)
                    first[left].insert(EPSILON);
            }
            else//说明是终结符号
            {
                first[left].insert(word);
                break;
            }
        }
    }

}

/**
    求所有非终结符号的first集合
*/
void Processor::getFirst()
{
    memset(visited, false, sizeof(visited));
    //对所有规则左边的非终结符号求first集合
    for(size_t i = 0; i < grammers.size(); i++)
        findFirst(i);
    //记录日志信息
    map<string, set<string>>::iterator it = first.begin();
    for(; it!=first.end(); it++)
    {
        log << "FIRST(" << it->first << ")={";
        set<string> &temp = it->second;
        set<string>::iterator it1 = temp.begin();
        bool flag = false;
        while(it1!= temp.end())
        {
            if(flag)    log << ",";
            log << *it1;
            flag = true;
            it1++;
        }
        log << "}" << '\n';
    }
}

/**
    深度优先遍历每一条文法规则
*/
void Processor::dfs(int i)
{
    if(visited[i])
        return;
    visited[i] = true;
    vector< vector<string> >  &right = grammers[i].right;
    vector<vector<string>>::iterator it = right.begin();
    for(; it!= right.end(); it++)
    {
        for(size_t j = 0; j < it->size(); j++)
        {
            string word = it->at(j);
            //如果不是终结符号，就遍历这个非终结符号对应的文法规则
            if(isFinalWord(word) == false)
                dfs(getIndex(word));
        }
    }
}

/**
    删除无用的文法规则
    有害的文法规则（形如A->A）已经在读取文件的时候去除
*/
void Processor::simplify()
{
    // 设置开始结点，然后进行遍历，把遍历不到的结点统统删除
    if(startWordIndex == -1)
    {
        log << "尚未进行正常去除左递归" << '\n';
        exit(-1);
    }
    memset(visited, 0, sizeof(visited));
    int x = startWordIndex;
    dfs(x);
    vector<LinkNode> res;
    for(size_t i = 0; i < grammers.size(); i++)
    {
        if(visited[i])
            res.push_back(grammers[i]);
    }
    grammers.clear();
    grammers = res;
    //结点发生了变化，所以需要重新生成终结符号和非终结符号
    init();
}

/**
    处理左公因子
*/
void Processor::dealLeftCommonFacotr()
{
    for(size_t i = 0; i < grammers.size();)
    {
        //执行一次就能提取出一个公因子，执行多次，就能提取出所有公因子
        vector<LinkNode> afterDeal = dealCommonFactor(grammers[i]);
        if(afterDeal.empty())
        {
            //如果afterDeal为空，则处理下一条文法
            i++;
            continue;
        }
        //erase之后，grammers.begin() + i自动指向下一文法，无需i++
        grammers.erase(grammers.begin() + i);
        grammers.insert(grammers.end(), afterDeal.begin(), afterDeal.end());
    }
    //找到开始符号
    for(size_t i = 0;i<grammers.size();i++)
    {
        if(grammers[i].left == startNode)
        {
            startWordIndex = i;
            break;
        }
    }
    //重新生成
    init();
}

/**
    处理单条文法的左公因子
    每次只提取一位的公因子，执行多次即可提取多位
*/
vector<LinkNode> Processor::dealCommonFactor(LinkNode &node)
{
    //返回的结果
    //例如 A->ab|ac|bd
    //则返回A->aA'|bd和A'->b|c的向量
    vector<LinkNode> afterDeal;
    for(size_t i = 0; i < node.right.size(); i++)
    {
        string iFrist = node.right[i][0];
        vector< vector<string> > temp;
        vector< vector<string> > noCommonTemp;
        bool noCommon = true;
        //在i前面的部分，如果是和i没有公因子的，那么就要加入到noCommonTemp
        //如果有公因子，不做处理，防止重复
        for(size_t j = 0; j < i; j++)
        {
            if(node.right[j][0] != iFrist)
                noCommonTemp.push_back(node.right[j]);
        }
        //在i后面的部分，不论有没有公因子，都要处理
        for(size_t j = i+1; j < node.right.size(); j++)
        {
            string jFirst = node.right[j][0];
            //如果第一个字符相同
            if(iFrist == jFirst)
            {
                    noCommon = false;
                    //提取一个公因子后剩下的部
                    //如ab|ac，则noCommonLefts中是{c}
                    vector<string> noCommonLefts;
                    if(node.right[j].begin() + 1 == node.right[j].end())
                        //a|ab，提取完之后是a(EPSILON | b)
                        noCommonLefts.push_back(EPSILON);
                    // 保存提取左公因子之后剩余的部分
                    else
                        noCommonLefts.assign(node.right[j].begin()+1, node.right[j].end());
                    //temp:{{c}}
                    temp.push_back(noCommonLefts);
            }
            else
            {
                //不是公因子的部分
                //noCommonTemp：{{bd}}
                noCommonTemp.push_back(node.right[j]);
            }
        }
        if(noCommon == false)
        {
            vector<string> noCommonLefts;
            if(node.right[i].begin()+1 == node.right[i].end())
                noCommonLefts.push_back(EPSILON);
            else
                noCommonLefts.assign(node.right[i].begin()+1, node.right[i].end()); // 保存提取左公因子之后剩余的部分
            //temp：{{a},{c}}
            temp.push_back(noCommonLefts);
            //A'
            string newNodeName = generateNewName(node.left);
            LinkNode newNode(newNodeName);
            //A'->a|c
            newNode.right = temp;
            vector<string> link;
            //link:{aA'}
            link.push_back(iFrist);
            link.push_back(newNodeName);
            //noCommonTemp:{{aA'},{bd}}
            noCommonTemp.insert(noCommonTemp.begin(), link);
            //A'->a|c
            LinkNode oriNode(node.left);
            oriNode.right = noCommonTemp;
            //A->aA'|bd
            afterDeal.push_back(oriNode);
            afterDeal.push_back(newNode);
            break;
        }
    }
    return afterDeal;
}

string Processor::generateNewName(string source)
{
    string str = source + "'";
    while(true)
    {
        for(size_t uu = 0; uu < grammers.size(); uu++)
        {
            if(str == grammers[uu].left)
            {
                str += "'";
                break;
            }
        }
        return str;
    }
}

/**
    将非终结符号str1的follow集合添加到str2的follow集合
*/
void Processor::append(const string &str1, const string &str2)
{
    set<string> &from = follow[str1];
    set<string> &to = follow[str2];
    set<string>::iterator it = from.begin();
    for( ; it!=from.end(); it++)
    {
        to.insert(*it);
    }
}

void Processor::getFollow()
{
    follow[grammers[startWordIndex].left].insert("$");
    //用于标记follow集合的值是否发生变化
    bool changed;
    while(true)
    {
        changed = false;
        //遍历每条文法
        for(size_t i = 0; i < grammers.size(); i++)
        {
            //左边部分
            string &left = grammers[i].left;
            //右边部分
            vector<vector<string>> &right = grammers[i].right;
            vector<vector<string>>::iterator it = right.begin();
            //遍历A -> bcd|efg|hij的后边三个部分，分别是bcd，efg，hij
            while(it != right.end())
            {
                //first(Xi+1,...,Xn)是否包含EPSILON
                bool flag = false;
                vector<string> &words = *it;
                //遍历每一个Xi
                for(size_t j = 0; j < it->size(); j++)
                {
                    flag = false;
                    string word = words[j];
                    // 如果当前符号是非终结符号
                    if(isFinalWord(words[j]) == false)
                    {
                        if(j == it -> size() - 1)
                        {
                            //如果是最后一个符号
                            //A->aB，就把Follow(A)加入Follow(B)
                            int sizeBefore = follow[word].size();
                            append(left, it->at(j));
                            int sizeAfter = follow[word].size();
                            if(sizeAfter > sizeBefore)
                                changed = true;
                        }
                        //将First(Xi+1,...,Xn)-EPSILON加入到follow(Xi)
                        for(size_t k = j + 1; k < it->size(); k++)
                        {
                            //first(Xi)是否包含EPSILON
                            bool flagi = false;
                            string nextWord = words[k];
                            if(isFinalWord(nextWord) == false)
                            {
                                set<string> &from = first[nextWord];
                                set<string> &to = follow[word];
                                set<string>::iterator it1 = from.begin();
                                int sizeBefore = follow[word].size();
                                for(; it1 != from.end(); it1++)
                                {
                                    if(*it1 != EPSILON)
                                        to.insert(*it1);
                                    else
                                    {
                                        //包含了EPSILON
                                        flagi = true;
                                    }
                                }
                                int sizeAfter = follow[word].size();
                                if(sizeAfter > sizeBefore)
                                    changed = true;
                                //如果first(Xi+1)中包含EPSILON，则继续把first(Xi+2)加入到follow(Xi)
                                //如果不包含EPSILON，则直接break;
                                if(flagi == false)
                                    break;
                                if(flagi == true && k == it -> size() - 1)
                                    flag = true;
                            }
                            else
                            {
                                int sizeBefore = follow[word].size();
                                follow[word].insert(nextWord);
                                int sizeAfter = follow[word].size();
                                if(sizeAfter > sizeBefore)
                                    changed = true;
                                break;
                            }
                        }
                        // 如果First(Xi+1,...Xn)包含EPSILON
                        if(flag)
                        {
                            size_t sizeBefore = follow[word].size();
                            //把A的follow集合加入到B中
                            append(left, it->at(j));
                            size_t sizeAfter = follow[word].size();
                            if(sizeAfter > sizeBefore)
                                //还在变化，要继续
                                changed = true;
                        }
                    }
                }
                it++;
            }
        }
        if(changed == false)
            break;
    }
    //输出follow集合到log文件
    map<string, set<string>>::iterator it = follow.begin();
    for(; it != follow.end(); it++)
    {
        log << "FOLLOW(" << it->first << ")={";
        set<string> &temp = it->second;
        bool flag = false;
        for(set<string>::iterator it1 = temp.begin(); it1 != temp.end(); it1++)
        {
            if(flag) log << ",";
            log << *it1;
            flag = true;
        }
        log << "}" << endl;
    }
}

vector<vector<string>> Processor::leftMostDerivation()
{
    input.push_back("$");
    //作为输出
    vector<vector<string>>output;
    string cur = grammers[startWordIndex].left;
    vector<string> v;
    v.push_back(cur);
    output.push_back(v);
    //表示目前已经对应到目标字符串的第几个
    int curIndex = 0;
    //如果没有达到就一直执行，除非发现语法错误
    while(true)
    {
        int size = output.size();
        //取出最近一次的结果
        vector<string> now = output[size - 1];
        vector<string> modified = now;
        //取出第一个非终结符号
        string word = "";
        for(size_t i = curIndex; i < now.size(); i++)
        {
            if(!isFinalWord(now[i]))
            {
                word = now[i];
                break;
            }
            else
            {
                curIndex++;
            }
        }
        for(vector<string> temp : grammers[getIndex(word)].right)
        {
            if(temp[0] == EPSILON)
            {
                //follow集合包含
                if(follow[word].count(input[curIndex]) == 1)
                {
                    //对应的点删掉，代表选择了curIndex
                    modified.erase(modified.begin() + curIndex);
                    break;
                }
            }
            else
            {
                //如果是终结符
                if(isFinalWord(temp[0]))
                {
                    //如果对应上
                    if(temp[0] == input[curIndex])
                    {
                        //对应的点删掉
                        modified.erase(modified.begin() + curIndex);
                        //插入匹配的文法
                        modified.insert(modified.begin()+curIndex, temp.begin(), temp.end());
                        curIndex++;
                        break;
                    }
                }
                    //不是终结符
                    else
                    {
                        //如果first集合包含待匹配的字符
                        if(first[temp[0]].count(input[curIndex]) == 1)
                        {
                            //对应的点删掉
                            modified.erase(modified.begin() + curIndex);
                            //插入匹配的文法
                            modified.insert(modified.begin()+curIndex, temp.begin(), temp.end());
                            break;
                        }
                        //输入字符串有误
                        else
                        {
                            exit(1);
                        }
                    }
            }
        }
        output.push_back(modified);
        bool isSame = false;
        if(modified.size() == input.size() - 1)
        {
            for(size_t i = 0;i<modified.size();i++)
            {
                if(modified[i] != input[i])
                {
                    isSame = false;
                    break;
                }
                if(i == modified.size() -1)
                    isSame = true;
            }
        }
        if(isSame)
            break;
    }
    return output;
}
