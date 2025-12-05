/* RandomArt
 * A lisp-like language for RandomArt
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mempool.h"
#include "node.h"

#include "lang.h"

typedef struct _Compiler {
	const char *start, *current;
	int line;
	int character;
} Compiler;

static bool _reachedEOF(Compiler *cc) {
	return *cc->current == '\0';
}

static char _peek(Compiler *cc) {
	return *cc->current;
}

static char _advance(Compiler *cc) {
	++cc->character;
	return *(cc->current++);
}

static bool _isDigit(const char CHAR) {
	return CHAR >= '0' && CHAR <= '9';
}

static bool _isAlpha(const char CHAR) {
	return (CHAR >= 'a' && CHAR <= 'z') || (CHAR >= 'A' && CHAR <= 'Z')
		|| CHAR == '_';
}

static bool _isAlphanumeric(const char CHAR) {
	return _isAlpha(CHAR) || _isDigit(CHAR);
}

static void _skipSpace(Compiler *cc) {
	while( true ) {
		switch( _peek(cc) ) {
		case ' ':
		case '\t':
		case '\r':
			++cc->character;
			_advance(cc);
			break;
		case '\n':
			++cc->line;
			cc->character = 1;
			_advance(cc);
			break;
		default:
			return;
		}
	}
}

static bool _check(Compiler *cc, int start, int len, const char *REST) {
	return ((cc->current - cc->start) == (start + len)
		&& memcmp(cc->start + start, REST, len) == 0);
}

#define N1(M) return M(_expr(cc, pool))

#define N2(M)                                                                  \
	Node *a = _expr(cc, pool);                                                 \
	Node *b = _expr(cc, pool);                                                 \
	return M(a, b)

#define N3(M)                                                                  \
	Node *a = _expr(cc, pool);                                                 \
	Node *b = _expr(cc, pool);                                                 \
	Node *c = _expr(cc, pool);                                                 \
	return M(a, b, c)

static Node *_expr(Compiler *cc, MemPool *pool);

static Node *_identifier(Compiler *cc, MemPool *pool) {
	while( _isAlphanumeric(_peek(cc)) ) {
		_advance(cc);
	}

	switch( *cc->start ) {
	case 'a':
		if( _check(cc, 1, 2, "bs") ) {
			N1(NODE_ABS);
		}
		if( _check(cc, 1, 2, "dd") ) {
			N2(NODE_ADD);
		}
		break;
	case 'c':
		if( _check(cc, 1, 2, "os") ) {
			N1(NODE_COS);
		}
		break;
	case 'd':
		if( _check(cc, 1, 2, "iv") ) {
			N2(NODE_DIV);
		}
		break;
	case 'e':
		if( _check(cc, 1, 2, "xp") ) {
			N1(NODE_EXP);
		}
		break;
	case 'f':
		if( _check(cc, 1, 4, "ract") ) {
			N1(NODE_FRACT);
		}
		break;
	case 'i':
		if( cc->current[-1] == 'f' ) {
			N3(NODE_IF);
		}
		break;
	case 'l':
		if( _check(cc, 1, 2, "og") ) {
			N1(NODE_LOG);
		}
		break;
	case 'm':
		if( _check(cc, 1, 2, "ax") ) {
			N2(NODE_MAX);
		}
		if( _check(cc, 1, 2, "in") ) {
			N2(NODE_MIN);
		}
		if( _check(cc, 1, 2, "ix") ) {
			N3(NODE_MIX);
		}
		if( _check(cc, 1, 2, "od") ) {
			N2(NODE_MOD);
		}
		if( _check(cc, 1, 2, "ul") ) {
			N2(NODE_MUL);
		}
		break;
	case 'r':
		if( _check(cc, 1, 2, "gb") ) {
			N3(NODE_RGB);
		}
		if( _check(cc, 1, 2, "nd") ) {
			return NODE_RAND();
		}
		break;
	case 's':
		if( _check(cc, 1, 2, "in") ) {
			N1(NODE_SIN);
		}
		if( _check(cc, 1, 3, "qrt") ) {
			N1(NODE_SQRT);
		}
		if( _check(cc, 1, 2, "ub") ) {
			N2(NODE_SUB);
		}
		break;
	case 'x':
		return NODE_X();
	case 'y':
		return NODE_Y();
	}

	printf("err:%d:%d unknown identifier near '%c'\n", cc->line, cc->character,
		*cc->start);
	exit(EXIT_FAILURE);
}

static Node *_parseNumber(Compiler *cc, MemPool *pool) {
	char *end;
	double num = strtod(cc->start, &end);
	cc->start = cc->current = end;

	return NODE_NUM(num);
}

static Node *_expr(Compiler *cc, MemPool *pool) {
	_skipSpace(cc);

	cc->start = cc->current;
	if( _reachedEOF(cc) ) {
		printf("err:%d:%d unexpected EOF\n", cc->line, cc->character);
		exit(EXIT_FAILURE);
	}

	const char CHAR = _advance(cc);
	if( _isDigit(CHAR) ) {
		return _parseNumber(cc, pool);
	}

	if( _isAlpha(CHAR) ) {
		return _identifier(cc, pool);
	}

	switch( CHAR ) {
	case '(':
	case ')':
		return _expr(cc, pool);
	case '<': {
		if( _peek(cc) == '=' ) {
			_advance(cc);
			N2(NODE_LTEQ);
		}
		N2(NODE_LT);
	}
	case '>': {
		if( _peek(cc) == '=' ) {
			_advance(cc);
			N2(NODE_GTEQ);
		}
		N2(NODE_GT);
	}
	case '!': {
		if( _peek(cc) == '=' ) {
			_advance(cc);
			N2(NODE_NEQ);
		}
	} break;
	case '=': {
		if( _peek(cc) == '=' ) {
			_advance(cc);
			N2(NODE_EQ);
		}
	} break;
	case '+': {
		if( _isDigit(_peek(cc)) ) {
			return _parseNumber(cc, pool);
		}

		printf("function\n");
		N2(NODE_ADD);
	}
	case '-': {
		if( _isDigit(_peek(cc)) ) {
			return _parseNumber(cc, pool);
		}

		printf("function\n");
		N2(NODE_SUB);
	}
	case '*': {
		N2(NODE_MUL);
	}
	case '/': {
		N2(NODE_DIV);
	}
	case '%': {
		N2(NODE_MOD);
	}
	}

	printf("err:%d:%d: unexpected character '%c' met\n", cc->line,
		cc->character, CHAR);
	exit(EXIT_FAILURE);
}

Node *langCompile(const char *EXPR) {
	MemPool pool = poolNew();

	Compiler cc;
	cc.start = EXPR;
	cc.current = EXPR;
	cc.line = 1;
	cc.character = 1;

	Node *ast = _expr(&cc, &pool);

	return ast;
}
