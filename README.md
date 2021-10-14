# 编译原理实验一：C++单词拼装器

## 实验内容

+ 把C++源代码中的各类单词（记号）进行拼装分类
+ C++语言包含了几种类型的单词（记号）：标识符，关键字，数（包括整数、浮点数），字符串、注释、特殊符号（分界符）和运算符号等（详细的单词类别及拼装规则见另外的文件说明）
+ 要求应用程序应为Windows界面
+ 打开一个C++源文件，列出所有可以拼装的单词（记号）
+ 应该书写完善的软件设计文档

## 运行效果样本

C++源程序：

```c++
#include<iostream.h>

main()
{
   int i;
   cin>>i;
   if (i>3) cout<<“ok”;
}
```


扫描结果：

```
#       特殊符号
Include    关键字
<iostream. h>    特殊符号
main    关键字
(        特殊符号
)        特殊符号
{      特殊符号
int    关键字
 i      标识符
;       特殊符号
cin   关键字
>>    特殊符号
i       标识符
;       特殊符号
if      关键字       
(       特殊符号
i       标识符
>      特殊符号
3       数
)        特殊符号
cout   关键字
<<     特殊符号
“ok”   串
;        特殊符号
}        特殊符号
```



## 程序流程图

![流程图](程序流程图.jpg)





