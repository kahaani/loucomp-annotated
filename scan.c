/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in scanner DFA */
// 有限状态机的状态通过枚举实现
typedef enum
   { START,INASSIGN,INCOMMENT,INNUM,INID,DONE }
   StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1]; // 加一以容纳结束符

/* BUFLEN = length of the input buffer for
   source code lines */
// 缓冲区大小，256个字符
#define BUFLEN 256

// 静态变量保证安全
static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void)
{ if (!(linepos < bufsize))
  { lineno++; // 行号加一
    if (fgets(lineBuf,BUFLEN-1,source)) // 源代码一行最多255个字符
    { if (EchoSource) fprintf(listing,"%4d: %s",lineno,lineBuf);
      bufsize = strlen(lineBuf);
      linepos = 0; // 指针置成零
      return lineBuf[linepos++];
    }
    else //遇到文件结束
    { EOF_flag = TRUE;
      return EOF;
    }
  }
  else return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
// 注意行首的字符是不能unget的，靠使用者保证。
// 这里只检查文件末尾不能unget。
static void ungetNextChar(void)
{ if (!EOF_flag) linepos-- ;}

/* lookup table of reserved words */
// TokenType的宏定义在globals.h中，真正的对照表在这里
static struct
    { char* str;
      TokenType tok;
    } reservedWords[MAXRESERVED]
   = {{"if",IF},{"then",THEN},{"else",ELSE},{"end",END},
      {"repeat",REPEAT},{"until",UNTIL},{"read",READ},
      {"write",WRITE}};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup (char * s)
{ int i;
  for (i=0;i<MAXRESERVED;i++)
    if (!strcmp(s,reservedWords[i].str))
      return reservedWords[i].tok;
  return ID; // ID意为标识符，没有保留字匹配时的处理
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
   { int c = getNextChar();
     save = TRUE;
     switch (state)
     { case START:
         if (isdigit(c))
           state = INNUM;
         else if (isalpha(c))
           state = INID;
         else if (c == ':')
           state = INASSIGN;
         else if ((c == ' ') || (c == '\t') || (c == '\n'))
           save = FALSE; // 空格字符放弃，仍留在START状态
         else if (c == '{')
         { save = FALSE; // 注释字符放弃
           state = INCOMMENT; //但跳到INCOMMENT状态
         }
         else // 下面是可以直接到DONE的情况
         { state = DONE;
           switch (c)
           { case EOF:
               save = FALSE; // 文件结尾放弃
               currentToken = ENDFILE;
               break;
             case '=':
               currentToken = EQ;
               break;
             case '<':
               currentToken = LT;
               break;
             case '+':
               currentToken = PLUS;
               break;
             case '-':
               currentToken = MINUS;
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
             default:
               currentToken = ERROR; // 如果都不匹配，是错误
               break;
           }
         }
         break;
       case INCOMMENT:
         save = FALSE; // 注释括号中的字符放弃
         if (c == EOF) // 注释了一半遇到文件结束符，可以正常结束
         { state = DONE;
           currentToken = ENDFILE;
         }
         else if (c == '}') state = START; // 注释结束，重新开始
         break;
       case INASSIGN:
         state = DONE;
         if (c == '=')
           currentToken = ASSIGN;
         else
         { /* backup in the input */
           ungetNextChar();
           save = FALSE;
           currentToken = ERROR; // 出现单独的冒号是错误
         }
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
       case DONE: // 错误处理，状态DONE/ERROR都会匹配到这里
       // 出错处理步骤：(1)打印信息；(2)返回类型为ERROR
       default: /* should never happen */
         fprintf(listing,"Scanner Bug: state= %d\n",state);
         state = DONE;
         currentToken = ERROR;
         break;
     }
     // 把字符保存起来
     if ((save) && (tokenStringIndex <= MAXTOKENLEN))
       tokenString[tokenStringIndex++] = (char) c;
     // 如果是结束状态，加上结束符形成一个完整的字符串
     if (state == DONE)
     { tokenString[tokenStringIndex] = '\0';
       // 对保留字问题做处理
       if (currentToken == ID)
         currentToken = reservedLookup(tokenString);
     }
   }
   // 打印调试信息
   if (TraceScan) {
     fprintf(listing,"\t%d: ",lineno);
     printToken(currentToken,tokenString);
   }
   // 返回值是token的类型，但ID/NUM需要得到token的值，可以另外到全局变量tokenString[]处取
   return currentToken;
} /* end getToken */

