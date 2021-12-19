/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "GLOBALS.h"
#include "UTIL.h"
#include "SCAN.h"

/* states in scanner DFA */
/*
 * 增加状态
 * INREGULAREXPR   :=
 * INEQ   ==
 * INLE   <=,<,<>
 * INGE   >,>=
 * INME   -,-=
 */
typedef enum
{
    START, INCOMMENT, INNUM, INID, DONE, INREGULAREXPR, INEQ, INLE, INGE, INME
}
StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void)
{
    if (!(linepos < bufsize))
    {
        lineno++;
        if (fgets(lineBuf, BUFLEN - 1, source))
        {
            if (EchoSource) fprintf(listing, "%4d: %s", lineno, lineBuf);
            bufsize = strlen(lineBuf);
            linepos = 0;
            return lineBuf[linepos++];
        }
        else
        {
            EOF_flag = TRUE;
            return EOF;
        }
    }
    else return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character in lineBuf */
static void ungetNextChar(void)
{
    if (!EOF_flag) linepos--;
}

/* lookup table of reserved words */
static struct
{
    char* str;
    TokenType tok;
} reservedWords[MAXRESERVED]
/*增加保留字*/
= { {"if",IF},{"then",THEN},{"else",ELSE},{"end",END},
   {"repeat",REPEAT},{"until",UNTIL},{"read",READ},
 {"write",WRITE},{"while",WHILE},{"endwhile",ENDWHILE},
 {"do",DO},{"for",FOR},{"to",TO},{"enddo",ENDDO},
 {"downto",DOWNTO},{"and",AND},{"or",OR},{"not",NOT} };

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup(char* s)
{
    int i;
    for (i = 0; i < MAXRESERVED; i++)
        if (!strcmp(s, reservedWords[i].str))
            return reservedWords[i].tok;
    return ID;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the
 * next token in source file
 */
TokenType getToken(void)
{  /* index for storing into tokenString */
    int tokenStringIndex = 0;
    /* holds current token to be returned */
    TokenType currentToken;
    /* current state - always begins at START */
    StateType state = START;
    /* flag to indicate save to tokenString */
    int save;
    while (state != DONE)
    {
        int c = getNextChar();
        save = TRUE;
        switch (state)
        {
        case START:
            if (isdigit(c))
                state = INNUM;
            else if (isalpha(c))
                state = INID;
            else if ((c == ' ') || (c == '\t') || (c == '\n'))
                save = FALSE;
            else if (c == '{')
            {
                save = FALSE;
                state = INCOMMENT;
            }
            //修改语义
            else if (c == ':')
                state = INREGULAREXPR;
            else if (c == '=')
                state = INEQ;
            else if (c == '<')
                state = INLE;
            else if (c == '>')
                state = INGE;
            else if (c == '-')
                state = INME;
            else
            {
                state = DONE;
                switch (c)
                {
                case EOF:
                    save = FALSE;
                    currentToken = ENDFILE;
                    break;
                case '+':
                    currentToken = PLUS;
                    break;
                case '*':
                    currentToken = TIMES;
                    break;
                case '/':
                    currentToken = OVER;
                    break;
                case '(':
                    currentToken = LPAREN;
                    break;
                case ')':
                    currentToken = RPAREN;
                    break;
                case ';':
                    currentToken = SEMI;
                    break;
                case '%':
                    currentToken = MOD;
                    break;
                case '^':
                    currentToken = POW;
                    break;
                case '|':
                    currentToken = ROR;
                    break;
                case '&':
                    currentToken = RAND;
                    break;
                case '#':
                    currentToken = RC;
                    break;
                default:
                    currentToken = ERROR;
                    break;
                }
            }
            break;
        case INCOMMENT:
            save = FALSE;
            if (c == EOF)
            {
                state = DONE;
                currentToken = ENDFILE;
            }
            else if (c == '}') state = START;
            break;
        case INNUM:
            if (!isdigit(c))
            { /* backup in the input */
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = NUM;
            }
            break;
        case INID:
            if (!isalpha(c))
            { /* backup in the input */
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = ID;
            }
            break;
        case INREGULAREXPR:
            state = DONE;
            if(c == '=')
            {

                currentToken = RE;
            }
            else
            {
                ungetNextChar();
                save = FALSE;
                currentToken = ERROR;
            }
            break;
        case INEQ:
            state = DONE;
            if(c == ' ')
            {
                save = FALSE;
                currentToken = ASSIGN;
            }
            else if(c == '=')
            {
                currentToken = EQ;
            }
            else
            {
                ungetNextChar();
                save = FALSE;
                currentToken = ERROR;
            }
            break;
        case INLE:
            state = DONE;
            if(c == ' ')
            {
                save = FALSE;
                currentToken = LT;
            }
            else if(c == '>')
            {
                currentToken = NE;
            }
            else if(c == '=')
            {
                currentToken = LE;
            }
            else
            {
                ungetNextChar();
                save = FALSE;
                currentToken = ERROR;
            }
            break;
        case INGE:
            state = DONE;
            if(c == ' ')
            {
                save = FALSE;
                currentToken = GT;
            }
            else if(c == '=')
            {
                currentToken = GE;
            }
            else
            {
                ungetNextChar();
                save = FALSE;
                currentToken = ERROR;
            }
            break;
        case INME:
            state = DONE;
            if(c == ' ')
            {
                save = FALSE;
                currentToken = MINUS;
            }
            else if(c == '=')
            {
                currentToken = ME;
            }
            else
            {
                ungetNextChar();
                save = FALSE;
                currentToken = ERROR;
            }
            break;
        case DONE:
        default: /* should never happen */
            fprintf(listing, "Scanner Bug: state= %d\n", state);
            state = DONE;
            currentToken = ERROR;
            break;
        }
        if ((save) && (tokenStringIndex <= MAXTOKENLEN))
            tokenString[tokenStringIndex++] = (char)c;
        if (state == DONE)
        {
            tokenString[tokenStringIndex] = '\0';
            if (currentToken == ID)
                currentToken = reservedLookup(tokenString);
        }
    }

    if (TraceScan) {
        fprintf(listing, "\t%d: ", lineno);
        printToken(currentToken, tokenString);
    }
    return currentToken;
} /* end getToken */


void resetScan()
{
    linepos = 0;
    bufsize = 0;
    EOF_flag = FALSE;
}
