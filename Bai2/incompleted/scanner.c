/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
	readChar();
}

Token* checkComment() {
	int ln = lineNo, cl = colNo;
	readChar();
	if (charCodes[currentChar] == CHAR_TIMES) {
		int pastChar = 0;
		//Check for end of comment
		while (currentChar != EOF) {
			if (charCodes[currentChar] == CHAR_RPAR && charCodes[pastChar] == CHAR_TIMES) {
				readChar();
				return makeToken(TK_NONE, ln, cl);
			}
			pastChar = currentChar;
			readChar();
		}
		error(ERR_ENDOFCOMMENT, ln, colNo);
		return makeToken(TK_NONE, ln, cl);
	}
	else return makeToken(SB_LPAR, ln, cl);
}

void copy_string(char *target, char *source) {
	while (*source) {
		*target = *source;
		source++;
		target++;
	}
	*(target) = '\0';
}

///<summary>
///Letter detected, from current char, determine what token it is then return it
///</summary>
Token* readIdentKeyword(void) {
	Token *token;
	int ln = lineNo, cl = colNo;
	int count = 0;

	char string[MAX_IDENT_LEN + 1];
	while (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT) {
		if (count > MAX_IDENT_LEN)
			error(ERR_IDENTTOOLONG, ln, cl);
		string[count] = currentChar;
		readChar();
		count++;
	}

	string[count] = '\0';
	//Check token
	TokenType temp = checkKeyword(string);
	switch (temp) {
	case TK_NONE: //Is not keyword
		token = makeToken(TK_IDENT, ln, cl);
		copy_string(token->string, string);
		break;
	default:	//Is keyword
		token = makeToken(temp, ln, cl);
		break;
	}

	return token;
}

Token* checkNEQ(void) {
	int ln = lineNo, cl = colNo;
	readChar();
	if (charCodes[currentChar] == CHAR_EQ)
		return makeToken(SB_NEQ, ln, cl);
	else {
		error(ERR_INVALIDSYMBOL, ln, cl);
		return makeToken(TK_NONE, ln, cl);
	}
}

Token* checkAssign(void) {
	int ln = lineNo, cl = colNo;
	readChar();
	if (charCodes[currentChar] == CHAR_EQ)
		return makeToken(SB_ASSIGN, ln, cl);
	else return makeToken(SB_COLON, ln, cl);
}

Token* checkLE(void) {
	int ln = lineNo, cl = colNo;
	readChar();
	if (charCodes[currentChar] == CHAR_EQ)
		return makeToken(SB_LE, ln, cl);
	else return makeToken(SB_LT, ln, cl);
}

Token* checkGE(void) {
	int ln = lineNo, cl = colNo;
	readChar();
	if (charCodes[currentChar] == CHAR_EQ)
		return makeToken(SB_GE, ln, cl);
	else return makeToken(SB_GT, ln, cl);
}

Token* readNumber(void) {
	Token *token;
	int ln = lineNo, cl = colNo;
	long int value = 0;

	while (charCodes[currentChar] == CHAR_DIGIT) {
		value = value * 10 + currentChar - '0';
		if (value > INT_MAX) {
			error(ERR_OVERFLOW, ln, cl);
			return makeToken(TK_NONE, ln, cl);
		}
		readChar();
	}

	token = makeToken(TK_NUMBER, ln, cl);
	token->value = value;
	return token;
}

Token* readConstChar(void) {
	Token* token;
	int ln = lineNo, cl = colNo;
	readChar();
	if (charCodes[currentChar] != CHAR_UNKNOWN) {
		char constchar = currentChar;
		readChar();
		if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
			token = makeToken(TK_CHAR, ln, cl);
			token->string[0] = constchar;
			return token;
		}
	}	
	error(ERR_INVALIDCHARCONSTANT, ln, cl);
	return makeToken(TK_NONE, ln, cl);
}

Token* getToken(void) {
  Token* token;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);
  
  switch (charCodes[currentChar]) {//Ordered by alphabet
  //Complex
  case CHAR_COLON: return (token = checkAssign())->tokenType != TK_NONE ? token : getToken();
  case CHAR_DIGIT: return readNumber();
  case CHAR_EXCLAIMATION: return (token = checkNEQ())->tokenType != TK_NONE ? token : getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_LPAR: return (token = checkComment())->tokenType != TK_NONE ? token : getToken();
  case CHAR_GT: return (token = checkGE())->tokenType != TK_NONE ? token : getToken();
  case CHAR_LT: return (token = checkLE())->tokenType != TK_NONE ? token : getToken();
  case CHAR_SINGLEQUOTE: readConstChar();
  case CHAR_SPACE: skipBlank(); return getToken();
  //Simple
  case CHAR_COMMA: readChar(); return makeToken(SB_COMMA, lineNo - 1, colNo - 1);
  case CHAR_EQ: readChar(); return makeToken(SB_EQ, lineNo - 1, colNo - 1);
  case CHAR_MINUS: readChar(); return makeToken(SB_MINUS, lineNo - 1, colNo - 1);
  case CHAR_PERIOD: readChar(); return makeToken(SB_PERIOD, lineNo - 1, colNo - 1);
  case CHAR_PLUS: readChar(); return makeToken(SB_PLUS, lineNo - 1, colNo - 1);
  case CHAR_RPAR: readChar(); return makeToken(SB_RPAR, lineNo - 1, colNo - 1);
  case CHAR_SEMICOLON: readChar(); return makeToken(SB_SEMICOLON, lineNo - 1, colNo - 1);
  case CHAR_SLASH: readChar(); return makeToken(SB_SLASH, lineNo - 1, colNo - 1);
  case CHAR_TIMES: readChar();  return makeToken(SB_TIMES, lineNo - 1, colNo - 1);
  default:
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return makeToken(TK_NONE, lineNo, colNo);;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%d)\n", token->value); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}



