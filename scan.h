/****************************************************/
/* File: scan.h                                     */
/* The scanner interface for the TINY compiler      */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SCAN_H_
#define _SCAN_H_

/* MAXTOKENLEN is the maximum size of a token */
#define MAXTOKENLEN 40

// 两条返回路径，token的类型通过函数返回值告知
// token的值通过全局变量传递（只有id/num两种类型要用到）

/* tokenString array stores the lexeme of each token */
// 变量声明，将这个全局变量的作用域扩展到其他文件
extern char tokenString[MAXTOKENLEN+1];

/* function getToken returns the 
 * next token in source file
 */
// 函数声明，一次返回一个token
TokenType getToken(void);

#endif
