#ifndef IMPLEMENTATIONS_H
#define IMPLEMENTATIONS_H
#include"QString"
#include"qmap.h"

QMap<QString, QString> letter_map;
QMap<char, QString> punctuation_map;
QMap<QString, QString>::iterator it1;
QMap<char, QString>::iterator it2;
const int KEY_NUMBER = 64;//关键字数量
const QString KEY_SET[] = {"asm", "else", "new", "this",
                           "auto", "enum", "operator", "throw",
                          "bool", "explicit", "private", "true",
                          "break", "export", "protected", "try",
                          "case", "extern", "public", "typedef",
                          "catch", "false", "register", "typeid",
                          "char", "float", "reinterpret_cast", "typename",
                          "class", "for", "return", "union",
                          "const", "friend", "short", "unsigned",
                          "const_cast", "goto", "signed", "using",
                          "continue", "if", "sizeof", "virtual",
                          "default", "inline", "static", "void",
                          "delete", "int", "static_cast", "volatile",
                          "do",  "long", "struct", "wchar_t",
                          "double", "mutable", "switch", "while",
                          "dynamic_cast", "namespace", "template", "include"};
const char PUNCTUATION[] = {'[',']','(',')','~',',','{','}',';',' ','\"','\'', '#'};//标点，在转换过程中没有分支
int state = 0;//当前的状态号；状态转换过程请看实验报告
int lineNumber = 0;//行号，用于定位行位置
char str[1024];//存储读入的一行字符串
int index = 0;//标记当前位置
QString letter;//当前单词

char getChar(char str[]){//从字符串中获取指针当前所指位置的字符
    return str[index++];
}

bool isDigit(char c){//判断是不是数字
    return c >= '0' && c <= '9';
}

bool isLetter(char c){//判断是不是字母
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'z');
}

bool isPunctuation(char c){//判断是不是标点符号
    for(int i = 0; i < 13; i++){
        if(c == PUNCTUATION[i])return true;
    }
    return false;
}

bool isKey(QString str){//判断是不是关键字
    for(int i = 0; i< KEY_NUMBER; i++){
        if(str == KEY_SET[i]){
            return true;
        }
    }
    return false;
}


void doProcess(char * str){//处理一行字符串
    if(state != 21){ //如果当前不在多行注释内
        state = 0;
    }
    char c = '\0';
    c = getChar(str);//从str读取一个字符
    while(c != '\n' && c != '\0'){
        switch(state){
        case 0:
            if(isPunctuation(c)){
                state = 0;
                if(!punctuation_map.contains(c)){
                    if(c == 32){
                        punctuation_map.insert(c, "空格");
                    }
                    else{
                        punctuation_map.insert(c, "标点符号");
                    }
                }
                c = getChar(str);
            }
            else if(isLetter(c) || c == '_'){
                state = 1;//进入状态1
                letter = c;
                c = getChar(str);
            }else if(isDigit(c)){
                state = 2;//进入状态2
                letter = c;
                c =getChar(str);
            }else if(c == '<'){
                state = 3;//进入状态三
                letter = c;
                c = getChar(str);
            }else if(c == '>'){
                state = 5;//进入状态5
                letter = c;
                c = getChar(str);
            }else if(c == '='){
                state = 7;//进入状态7
                letter = c;
                c = getChar(str);
            }else if(c == '+'){
                state = 8;//进入状态8
                letter = c;
                c = getChar(str);
            }else if(c == '-'){
                state = 9;//进入状态9
                letter = c;
                c = getChar(str);
            }else if(c == '*'){
                state = 11;//进入状态11
                letter = c;
                c = getChar(str);
            }else if(c == '.'){
                state = 12;//进入状态12
                letter = c;
                c = getChar(str);
            }else if(c == '!'){
                state = 13;//进入状态13
                letter = c;
                c = getChar(str);
            }else if(c == '%'){
                state = 14;//进入状态14
                letter = c;
                c = getChar(str);
            }else if(c == '&'){
                state = 15;//进入状态15
                letter = c;
                c = getChar(str);
            }else if(c =='^'){
                state = 16;//进入状态16
                letter = c;
                c = getChar(str);
            }else if(c =='|'){
                state = 17;//进入状态17
                letter = c;
                c = getChar(str);
            }else if(c ==':'){
                state = 18;//进入状态18
                letter = c;
                c = getChar(str);
            }else if(c =='?'){
                state = 19;//进入状态19
                letter = c;
                c = getChar(str);
            }else if(c =='/'){
                state = 20;//进入状态20
                letter = c;
                c = getChar(str);
            }else{
                //其他情况，忽略此单词
                 c = getChar(str);
            }
            break;
        case 1:
            if(isLetter(c) || isDigit(c) || c == '_'){
                state = 1;
                letter += c;
                c = getChar(str);
            }else{//到分隔符
                if(isKey(letter)){
                    if(!letter_map.contains(letter)){
                        letter_map.insert(letter,"关键字");
                    }
                }else{
                    if(!letter_map.contains(letter)){
                        letter_map.insert(letter,"标识符");
                    }
                }
                state = 0;
            }
            break;
        case 2:
            if(isDigit(c)){
                state = 2;
                letter += c;
                c = getChar(str);
            }else{//到了分隔符
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"数字");
                }
                state = 0;
            }
            break;
        case 3:
            if(c =='<'){
                state = 4;//进入状态4
                letter += c;
                c = getChar(str);
            }else if(c == '='){
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"逻辑运算符");
                }
                letter += c;
                state = 0;
                c = getChar(str);
            }else{
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"逻辑运算符");
                }
                state = 0;
            }
            break;
        case 4:
            if(c == '='){
                letter += c;
                state = 0;
                c = getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"复合赋值运算符");
                }
            }else{
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"位运算符");
                }
                state = 0;
            }
            break;
        case 5:
            if(c == '='){
                letter += c;
                state = 0;
                c = getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"逻辑运算符");
                }
            }else if(c == '>'){
                state = 6;
                c = getChar(str);
                letter += c;
            }else{
                state = 0;
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"逻辑运算符");
                }
            }
            break;
        case 6:
            if(c == '='){
                letter += c;
                state = 0;
                c = getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"复合赋值运算符");
                }
            }else{
                state = 0;
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"逻辑运算符");
                }
            }
            break;
        case 7:
            if(c == '='){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"逻辑运算符");
                }
            }else{
                state = 0;
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"赋值运算符");

                }
            }
            break;
        case 8:
            if(c == '+'){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"自增运算符");
                }
            }else if(c == '='){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"复合赋值运算符");
                }
            }else{
                state = 0;
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"加号（正号）");
                }
            }
            break;
        case 9:
            if(c == '-'){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"自减运算符");
                }
            }else if(c == '='){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"复合赋值运算符");
                }
            }else if(c == '>'){
                state = 10;
                letter += c;
                c = getChar(str);
            }else{
                state = 0;
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"减号（负号）");
                }
            }
            break;
        case 10:
            if(c == '*'){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"成员指针运算符");
                }
            }else{
                state = 0;
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"指向成员运算符");
                }
            }
            break;
        case 11:
            if(c == '='){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"复合赋值运算符");
                }
            }else{
                state = 0;
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"乘号（地址访问运算符）");
                }
            }
            break;
        case 12:
            if(c == '*'){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"复合赋值运算符");
                }
            }else{
                state = 0;
                 /////////////////////////输出信息，.号////////////////////////////
            }
            break;
        case 13:
            if(c == '='){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"成员访问运算符");
                }
            }else{
                state = 0;
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"逻辑运算符");
                }
            }
            break;
        case 14:
            if(c == '='){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"复合赋值运算符");
                }
            }else{
                state = 0;
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"取余");
                }
            }
            break;
        case 15:
            if(c == '&'){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"逻辑运算符");
                }
            }else if(c == '='){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"复合赋值运算符");
                }
            }else{
                state = 0;
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"位运算符");
                }
            }
            break;
        case 16:
            if(c == '='){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"复合赋值运算符");
                }
            }else{
                state = 0;
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"位运算符");
                }
            }
            break;
        case 17:
            if(c == '|'){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"逻辑运算符");
                }
            }else if(c == '='){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"复合赋值运算符");
                }
            }else{
                state = 0;
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"位运算符");
                }
            }
            break;
        case 18:
            if(c == ':'){
                letter += c;
                state = 0;
                c =getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"作用域运算符");
                }
            }else{
                state = 0;
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"冒号");
                }
            }
            break;
        case 19:
            break;///////////////////////////////////////////做不来
        case 20:
            if(c == '*'){
                state = 21;
                letter += c;
                c = getChar(str);
            }else if(c =='/'){
                state = 23;
                letter += c;
                c = getChar(str);
            }else if(c == '='){
                state = 0;
                letter += c;
                c = getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"复合赋值运算符");
                }
            }else{
                state = 0;
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"除号");
                }
            }
            break;
        case 21:
            if(c == '*'){
                state = 22;
                letter += c;
                c = getChar(str);
            }else{
                state = 21;
                letter += c;
                c = getChar(str);
            }
            break;
        case 22:
            if(c == '*'){
                state = 22;
                letter += c;
                c = getChar(str);
            }else if(c == '/'){
                letter += c;
                state = 0;
                c = getChar(str);
                if(!letter_map.contains(letter)){
                    letter_map.insert(letter,"多行注释");
                }
            }else{
                state = 21;
                letter += c;
                c = getChar(str);
            }
            break;
        case 23:
            state = 23;
            letter += c; 
            if(str[index] == '\0'|| str[index] == '\n' ){
                state = 0;
                    if(!letter_map.contains(letter)){
                        letter_map.insert(letter,"单行注释");
                    }
            }
            c = getChar(str);
            break;
        }
    }
}
#endif // IMPLEMENTATIONS_H
