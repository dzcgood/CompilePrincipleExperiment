# TINY语言的扩充

## 问题描述

+ 扩充语法规则：`do-while`循环, `for`循环
+ 扩充算数运算符：`-=`, `%`, `^`
+ 扩充比较运算符：`==`, `>`, `<=`, `>=`, `<>`
+ 新增正则表达式支持，语法为`ID := RegExp `
+ 扩充逻辑运算符：`and`, `or`, `not`
+ 修改原来的赋值运算符符号，`:=`改为`=`，等于的比较符号`=`改为`==`

## 实现思路

### TokenType

新增以下`TokenType`：

|  标识符  |   含义   |
| :------: | :------: |
|  WHILE   |  while   |
| ENDWHILE | endwhile |
|    DO    |    do    |
|   FOR    |   for    |
|    TO    |    to    |
|  ENDTO   |  endto   |
|  DOWNTO  |  downto  |
|   MOD    |    %     |
|    ME    |    -=    |
|    RE    |    :=    |
|    GT    |    >     |
|    LE    |    <=    |
|    GE    |    >=    |
|    NE    |    <>    |
|   AND    |   and    |
|    OR    |    or    |
|   NOT    |   not    |
|   ROR    |    \|    |
|   RAND   |    &     |
|    RC    |    #     |
|   POW    |    ^     |

### StmtKind

新增以下`StmtKind`：

|  标识符  |     含义     |
| :------: | :----------: |
| DoWhileK | do-while语句 |
|   ForK   |   for语句    |

### MAXREVERSED

保留字个数由原先的8个扩展为18个，新增了`do`, `while`, `for`, `and`, `or`, `not`, `endwhile`, `to`, `enddo`, `downto`

### MAXCHILDREN

最大孩子数修改为4个，因为for语句的语法树需要4个孩子，具体如下：

![for语句语法树](https://gitee.com/dzcgood/img-bed/raw/master/img/for-syntax.jpg)

### SCAN

因为新增支持的运算符，所以需要新增几个状态并修改原有的状态转换图。

新增如下状态：`INREGULAREXPER`, `INEQ`, `INLE`, `INGE`, `INME`

修改后的状态转换图如下：

![状态转换图](https://gitee.com/dzcgood/img-bed/raw/master/img/state-change.jpg)

### 文法规则

新增了文法规则和修改了原有的文法规则，主要变动（文法使用EBNF表示）如下：

为了新增`%`和`^`，算数表达式的文法规则修改如下：

+ simple_exp -> term { ( + | - ) term }
+ term -> power { ( * | / | % ) power }
+ power -> factor { ^ factor }
+ factor -> NUM | ID | ( simple_exp )

为了增加对`and`, `or`, `not`的支持，修改exp的文法规则如下：

+ exp1 -> exp2 { or exp2 }
+ exp2 -> exp2 { and exp3 }
+ exp3 -> [ not ] exp4
+ exp4 -> simple_exp ( < | <= | > | >= | <> | == ) simple_exp | (exp1)

新增正则表达式的文法：

+ regExp1 -> regExp2 { | regExp2 }
+ regExp2 -> regExp3 { & regExp3 }
+ regExp3 -> regExp4 [ # ]
+ regExp4 -> ID | ( regExp1 )

新增`for`语句的文法：

+ for_stmt -> for ID = simple_exp ( to | downto ) exp1 do stmt_sequence enddo

新增`do-while`语句的文法：

+ dowhile_stmt -> do stmt_sequence while ( exp1 );

修改原有赋值语句的文法为：

+ assgin_stmt -> ID ASSSIGN | ME simple_exp
+ assgin_stmt -> ID RE regExp1

综合以上说明，可以列出所有的文法规则：

| 文法规则                                                     |
| :----------------------------------------------------------- |
| stmt_sequence -> statement { statement}                      |
| statement -> if_stmt \| repeat_stmt \| assgin_stmt \| read_stmt \| write_stmt \| write_stmt \| dowhile_stmt \| for_stmt |
| if_stmt -> if exp1 then stmt_sequence [ else stmt_sequence ] |
| repeat_stmt -> repeat stmt_sequence until exp1               |
| assgin_stmt -> ID ASSSIGN \| ME simple_exp                   |
| assgin_stmt -> ID RE regExp1                                 |
| read_stmt -> read ID;                                        |
| write_stmt -> write exp1;                                    |
| dowhile_stmt -> do stmt_sequence while ( exp1 );             |
| for_stmt -> for assgin_stmt ( to \| downto ) exp1 do stmt_sequence |
| exp1 -> exp2 { or exp2 }                                     |
| exp2 -> exp2 { and exp3 }                                    |
| exp3 -> [ not ] exp4                                         |
| exp4 -> simple_exp ( < \| <= \| > \| >= \| <> \| == ) simple_exp \| ( exp1 ) |
| simple_exp -> term { ( + \| - ) term }                       |
| term -> power { ( * \| / \| % ) power }                      |
| power -> factor { ^ factor }                                 |
| factor -> ID \| NUM \| ( simple_exp )                        |
| regExp1 -> regExp2 { \| regExp2 }                            |
| regExp2 -> regExp3 { & regExp3 }                             |
| regExp3 -> regExp4 [ # ]                                     |
| regExp4 -> ID ( regExp1 )                                    |

### 其他说明

#### assign_stmt

根据token的取值`:=`, `=`和`-=`，生成不同的语法树：

+ `:=`
  + 以`a := b#&c;`举例
  + ![:=](https://gitee.com/dzcgood/img-bed/raw/master/img/3.jpg)

+ `=`
  + 以`a = b;`举例
  + ![=](https://gitee.com/dzcgood/img-bed/raw/master/img/1.jpg)

+ `-=`
  + 以`a -= b;`举例
  + ![-=](https://gitee.com/dzcgood/img-bed/raw/master/img/2.jpg)

#### 关于if_stmt

我采取的语法树输出方法是文字输出，导致了输出语法树的时候没有办法区分是then中的语句还是else中的语句，但是在实际生成的语法树中，二者的区分是很明显的，因为他们是在不同的子树中。

例如`if x > 0 then x = 1; else x = 2; end`，实际的语法树中，有三个叉，第一个叉是`x > 0`，第二个叉是`x = 1;`，第三个叉是`x = 2;`但是输出的结果如下：

![关于if_stmt](https://gitee.com/dzcgood/img-bed/raw/master/img/image-20211219231408120.png)

#### 关于代码规范

+ 严格要求赋值语句、read语句、write语句和do-while后面都有分号作为分隔符号
+ 输入运算符前后请加上空格，例如`x >= 1;`，`y <> 2;`如果没有空格会出错

## 界面设计

![界面设计](https://gitee.com/dzcgood/img-bed/raw/master/img/image-20211219231808528.png)

## 使用说明

+ open按钮用于打开代码文件，支持的文件格式是`.txt`和`.tiny`
+ 目前只支持从文件中读取代码，不支持在文本框输入的代码作为source
+ 从文件中读取代码后，点击analyse按钮，即可分析
+ 点击save按钮，可以将左边文本编辑框编辑的代码保存
+ 2022/1/20补充：其实关于`;`的处理我做的并不好，大家可以自己写逻辑