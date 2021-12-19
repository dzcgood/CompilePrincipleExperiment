/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "GLOBALS.h"
#include "UTIL.h"
#include "SCAN.h"
#include "PARSE.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode* stmt_sequence(void);
static TreeNode* statement(void);
static TreeNode* if_stmt(void);
static TreeNode* repeat_stmt(void);
static TreeNode* assign_stmt(void);
static TreeNode* read_stmt(void);
static TreeNode* write_stmt(void);
//添加and or not 文法
static TreeNode* exp1(void);
static TreeNode* exp2(void);
static TreeNode* exp3(void);
static TreeNode* exp4(void);
static TreeNode* simple_exp(void);
static TreeNode* term(void);
//添加^文法
static TreeNode* power(void);
static TreeNode* factor(void);
//添加do-while和for
static TreeNode* dowhile_stmt(void);
static TreeNode* for_stmt(void);
//添加正则表达式
static TreeNode* regExp1(void);
static TreeNode* regExp2(void);
static TreeNode* regExp3(void);
static TreeNode* regExp4(void);

static void syntaxError(char* message)
{
    fprintf(listing, "\n>>> ");
    fprintf(listing, "Syntax error at line %d: %s", lineno, message);
    Error = TRUE;
}

static void match(TokenType expected)
{
    if (token == expected) token = getToken();
    else {
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        fprintf(listing, "      ");
    }
}

TreeNode* stmt_sequence(void)
{
    TreeNode* t = statement();
    TreeNode* p = t;
    //modified
    //增加了WHILE和ENDDO作为stmt_sequence的分割符
    while ((token != ENDFILE) && (token != END) && (token != ELSE) && (token != UNTIL)
        && (token != WHILE) && (token != ENDDO))
    {
        TreeNode* q;
        q = statement();
        if (q != NULL) {
            if (t == NULL) t = p = q;
            else /* now p cannot be NULL either */
            {
                p->sibling = q;
                p = q;
            }
        }
    }
    return t;
}


//P394
//lineno: 961
TreeNode* statement(void)
{
    TreeNode* t = NULL;
    switch (token) {
    case IF: t = if_stmt(); break;
    case REPEAT: t = repeat_stmt(); break;
    case ID: t = assign_stmt(); break;
    case READ: t = read_stmt(); break;
    case WRITE: t = write_stmt(); break;
    case DO: t = dowhile_stmt(); break;
    case FOR: t = for_stmt(); break;
    default: syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    } /* end case */
    return t;
}


//P394
//lineno: 977
TreeNode* if_stmt(void)
{
    TreeNode* t = newStmtNode(IfK);
    match(IF);
    if (t != NULL) t->child[0] = exp1();
    match(THEN);
    if (t != NULL) t->child[1] = stmt_sequence();
    if (token == ELSE) {
        match(ELSE);
        if (t != NULL) t->child[2] = stmt_sequence();
    }
    match(END);
    return t;
}

//P394
//lineno:991
TreeNode* repeat_stmt(void)
{
    TreeNode* t = newStmtNode(RepeatK);
    match(REPEAT);
    if (t != NULL) t->child[0] = stmt_sequence();
    match(UNTIL);
    if (t != NULL) t->child[1] = exp1();
    return t;
}

/*
 * modified
 * 改成 ID ASSIGN|ME simple_exp和 ID RE regExp1
 */
TreeNode* assign_stmt(void)
{
    TreeNode* t = newStmtNode(AssignK);
    if ((t != NULL) && (token == ID))
        t->attr.name = copyString(tokenString);
    match(ID);
    switch (token)
    {
    case RE:
        match(RE);
        if (t != NULL) t->child[0] = regExp1();
        break;
    case ASSIGN:
        match(ASSIGN);
        if (t != NULL) t->child[0] = simple_exp();
        break;
    case ME:
        match(ME);
        if (t != NULL)
        {
            TreeNode* p = newExpNode(OpK);
            t->child[0] = p;
            if (p != NULL)
            {
                p->attr.op = MINUS;
                TreeNode* q = newExpNode(IdK);
                if (q != NULL)
                {
                    q->attr.name = t->attr.name;

                }
                p->child[0] = q;
                p->child[1] = simple_exp();
            }
        }
        break;
    default:
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    }
    match(SEMI);
    return t;
}


TreeNode* read_stmt(void)
{
    TreeNode* t = newStmtNode(ReadK);
    match(READ);
    if ((t != NULL) && (token == ID))
        t->attr.name = copyString(tokenString);
    match(ID);
    match(SEMI);
    return t;
}

TreeNode* write_stmt(void)
{
    TreeNode* t = newStmtNode(WriteK);
    match(WRITE);
    if (t != NULL) t->child[0] = exp1();
    match(SEMI);
    return t;
}

/*
 * modified
 * 用于实现and or not
 * 改成：
 * exp1 -> exp2{or exp2}
 * exp2 -> exp3{and exp3}
 * exp3 -> [not]exp4
 * exp4 -> simple_exp <|<=|>|>=|==|<> simple_exp | (exp1)
 */

 //exp1 -> exp2{or exp2}
TreeNode* exp1(void)
{
    TreeNode* t = exp2();
    while (token == OR)
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL)
        {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
        }
        match(token);
        if (t != NULL)
        {
            t->child[1] = exp2();
        }
    }
    return t;
}

//exp2 -> exp3{and exp3}
TreeNode* exp2(void)
{
    TreeNode* t = exp3();
    while (token == AND)
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL)
        {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
        }
        match(token);
        if (t != NULL)
        {
            t->child[1] = exp3();
        }
    }
    return t;
}

//exp3 -> [not]exp4
TreeNode* exp3(void)
{
    TreeNode* t = NULL;
    if (token == NOT)
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL)
        {
            p->attr.op = token;
        }
        match(token);
        if (p != NULL)
        {
            p->child[0] = exp4();
            t = p;
        }
    }
    else
    {
        t = exp4();
    }
    return t;
}

//exp4 -> simple_exp <|<=|>|>=|==|<> simple_exp | (exp1)
TreeNode* exp4(void)
{
    TreeNode* t = NULL;
    if (token == LPAREN)
    {
        match(LPAREN);
        t = exp1();
        match(RPAREN);
    }
    else
    {
        t = simple_exp();
        if ((token == LT) || (token == EQ) || (token == GT)
            || (token == LE) || (token == GE) || (token == NE)) {
            TreeNode* p = newExpNode(OpK);
            if (p != NULL) {
                p->child[0] = t;
                p->attr.op = token;
                t = p;
            }
            match(token);
            if (t != NULL)
                t->child[1] = simple_exp();
        }
    }
    return t;
}


TreeNode* simple_exp(void)
{
    TreeNode* t = term();
    while ((token == PLUS) || (token == MINUS))
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            t->child[1] = term();
        }
    }
    return t;
}


/*
 * modified
 * 为了加入^操作，需要将term中的factor改成power
 * 并且增加%操作
 */
TreeNode* term(void)
{
    TreeNode* t = power();
    while ((token == TIMES) || (token == OVER) || (token == MOD))//modified
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            t->child[1] = power();
        }
    }
    return t;
}

//power
// power -> factor{^factor}
TreeNode* power(void)
{
    TreeNode* t = factor();
    while (token == POW)
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            t->child[1] = factor();
        }
    }
    return t;
}

//将exp改成simple_exp
TreeNode* factor(void)
{
    TreeNode* t = NULL;
    switch (token) {
    case NUM:
        t = newExpNode(ConstK);
        if ((t != NULL) && (token == NUM))
            t->attr.val = atoi(tokenString);
        match(NUM);
        break;
    case ID:
        t = newExpNode(IdK);
        if ((t != NULL) && (token == ID))
            t->attr.name = copyString(tokenString);
        match(ID);
        break;
    case LPAREN:
        match(LPAREN);
        t = simple_exp();//modified
        match(RPAREN);
        break;
    default:
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    }
    return t;
}

/*
 * do-while语句
 * DO stmt_sequence WHILE(exp1)
 */
TreeNode* dowhile_stmt(void)
{
    TreeNode* t = newStmtNode(DoWhileK);
    match(DO);
    if (t != NULL) t->child[0] = stmt_sequence();
    match(WHILE);
    match(LPAREN);
    if (t != NULL) t->child[1] = exp1();
    match(RPAREN);
    match(SEMI);
    return t;
}

TreeNode* for_stmt(void)
{
    TreeNode* t = newStmtNode(ForK);
    match(FOR);
    if (t != NULL) t->child[0] = assign_stmt();
    TreeNode* p = newExpNode(OpK);
    if (p != NULL) p->attr.op = token;
    if (t != NULL) t->child[1] = p;
    match(token);
    if (t != NULL) t->child[2] = exp1();
    match(DO);
    if (t != NULL) t->child[3] = stmt_sequence();
    match(ENDDO);
    return t;
}

/*
 * 添加正则表达式
 * regExp1 -> regExp2{| regExp2}
 * regExp2 -> regExp3{& regExp3}
 * regExp3 -> regExp4[#]
 * regExp4 -> ID | (RE)
 */

 //regExp1 -> regExp2{| regExp2}
TreeNode* regExp1(void)
{
    TreeNode* t = regExp2();
    while (token == ROR)
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL)
        {
            p->attr.op = token;
            p->child[0] = t;
            t = p;
        }
        match(token);
        if (t != NULL)
            t->child[1] = regExp2();
    }
    return t;
}

//regExp2 -> regExp3{& regExp3}
TreeNode* regExp2(void)
{
    TreeNode* t = regExp3();
    while (token == RAND)
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL)
        {
            p->attr.op = token;
            p->child[0] = t;
            t = p;
        }
        match(token);
        if (t != NULL)
            t->child[1] = regExp3();
    }
    return t;
}

//regExp3 -> regExp4[#]
TreeNode* regExp3(void)
{
    TreeNode* t = regExp4();
    if (token == RC)
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL)
        {
            p->attr.op = token;
            p->child[0] = t;
            t = p;
        }
        match(token);
    }
    return t;
}

//regExp4 -> ID | (RE)
TreeNode* regExp4(void)
{
    TreeNode* t = NULL;
    switch (token) {
    case ID:
        t = newExpNode(IdK);
        if ((t != NULL) && (token == ID))
            t->attr.name = copyString(tokenString);
        match(ID);
        break;
    case LPAREN:
        match(LPAREN);
        t = simple_exp();//modified
        match(RPAREN);
        break;
    default:
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    }
    return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode* parse(void)
{
    TreeNode* t;
    token = getToken();
    t = stmt_sequence();
    if (token != ENDFILE)
        syntaxError("Code ends before file\n");
    return t;
}
