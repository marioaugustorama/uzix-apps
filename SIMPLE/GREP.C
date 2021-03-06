/* grep.c	24.05.94	by BAS
 *
 * Get Regular Expression and Print
 *
 *	GREP -CNVFD -P��ࠧ�� �����
 *
 * Grep ��ᬠ�ਢ��� 㪠����� 䠩�� (��� stdin), � ��� ᮢ�����騥
 * � ��ࠧ殬 ��ப�. ��ᯮ������� ᫥���騥 ����:
 *
 *	?	����� 奫��
 *	C	����� ⮫쪮 �᫠ ᮢ����� ��ப
 *	N	������ ��। ������ ��ப�� �� �����
 *	V	������ ��ᮢ���訥 ��ப�
 *	F	������ ����� 䠩���
 *	D	�⫠���
 *
 *	P	������� ��ࠧ� (����� ������ ��᪮�쪮)
 *
 * ��ࠧ�� �������� ॣ���� ��ࠦ�����.
 * ���孨� � ������ ॣ���� ࠧ�������.
 * ����� ��ப� ᮢ������ � ��ࠧ殬 "$" (�. ����).
 *
 * �����୮� ��ࠦ���� ��ந��� �� ᫥����� ������⮢:
 *
 *	x	����� ᨬ��� (�� ���ᠭ�� �����), ᮢ������ � ᮡ��.
 *	\	��࠭ ��� ᨬ����.  "\$" ᮢ������ � �����஬.
 *		\nnn (n = 0..7) - ᨬ��� � ����� nnn.
 *	^	� ��砫� ��ࠦ���� ᮢ������ � ��砫�� ��ப�.
 *	$	� ���� ��ࠦ���� ᮢ������ � ���殬 ��ப�.
 *	.	�� ᨬ���, �஬� LF.
 *	:r	�� ���᪠� �㪢�
 *	:p	�� �㭪��樮��� ᨬ���
 *	:l	�����쪠� ������᪠� �㪢�.
 *	:u	������ ������᪠� �㪢�.
 *	:a	�� ������᪠� �㪢�.
 *	:d	�� ���.
 *	:n	�� �㪢� �/��� ���.
 *	:c	�� ����஫, �஬� LF � TAB.
 *	:s	�஡��.
 *	:t	�������.
 *	:e	��砫� �����ࠦ����.
 *	*	��ࠦ����, �� ����� ���� '*', ᮢ������ � ����� ���
 *		����� ������ �⮣� ��ࠦ����: "fo*" ᮢ������ � "f",
 *		"fo", "foo"...
 *	+	��ࠦ����, �� ����� ���� '+', ᮢ������ � ����� ���
 *		����� ������ �⮣� ��ࠦ����: "fo+" ᮢ������ � "fo"...
 *	-	��ࠦ���� ��। '-' ᮢ������ � ����� ��� ����� �������
 *		��ࠦ����.
 *	[]	��ப� � ᪮���� ᮢ������ ⮫쪮 � ᨬ������ �� ��ப�,
 *		� �� � ������ ��㣨��. �᫨ ���� ᨬ��� ��ப� '^',
 *		��ࠦ���� �㤥� ᮢ������ � ��묨 ᨬ������, �᪫��� '\n'
 *		� �� ᨬ����, �室�騥 � ��ப�.
 *
 * ��� �ਬ��, "A[xyz]+B" ᮢ����� � "AxxB" � "AxyzzyB", � "A[^xyz]+B"
 * ᮢ����� � "AbcB" �� �� � "AxB".  �������� ᨬ����� ����� ���� 㪠���
 * ��� ��� ᨬ����, ࠧ�������� '-'. �������, �� [a-z] ᮢ����� �
 * �����쪨�� �㪢���, � [z-a] �� ᮢ����� �� � 祬.
 *
 * �����⥭��� ॣ����� ��ࠦ���� - ᭮�� ॣ��୮� ��ࠦ����.
 *
 * �������⨪�:
 *
 *	Unknown switch
 *
 *	No pattern
 *
 *	Illegal occurrence op. ...
 *		- ������ � ����୮� ���⥪��.  ���ਬ��, ��ࠧ�� "*foo"
 *	�������⨬, ⠪ ��� '*' ������ ������஢��� ���饥 ��। ���
 *	��ࠦ����.
 *
 *	No type
 *		- �����稥 � ������ �����䨪��஬.
 *
 *	Class terminates badly
 *		- �������� ����� "[...]" �����襭 �����४⭮.
 *	���ਬ��, "[A-]" - ����୮.
 *
 *	Unterminated class
 *		- �������� ����� �� �����襭 ']'.
 *
 *	Class too large
 *		- ����७��� ���� ��९�����.
 *
 *	Empty class
 *		- �������� ����� ����� ᮤ�ঠ�� �� �����."[]" - ����୮.
 *
 *	Pattern too complex
 *		- ����७��� ���� ��९�����.
 */

#undef	DEBUG
#define DEBUG	0

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>

#define IDENT	"GREP V01.03"

#define LMAX	4096
#define PMAX	4096
#define NPATS	16

#define CHAR	1
#define BOL	2
#define EOL	3
#define ANY	4
#define CLASS	5
#define NCLASS	6
#define STAR	7
#define PLUS	8
#define MINUS	9
#define LOWER	10
#define UPPER	11
#define ALPHA	12
#define DIGIT	13
#define NALPHA	14
#define CNTRL	15
#define RANGE	16
#define ENDPAT	17
#define RUSSL	18
#define START	19

char	*items[] = {
	"NULL\n", "'", "^ ", "$ ", "? ", "[", "[^", "\n*( ",
	"\n+( ", "\n-( ", "L ", "U ", "A ", "D ",
	"AN ", "CTRL ", "", ")\n", "R ", "! ",
};

int	cflag = 0;
int	nflag = 0;
int	vflag = 0;
int	fflag = 0;
#if DEBUG
int	debug = 0;	/* >0 ��� �⫠��� */
#endif

long	t_count = 0;
long	t_lno = 0;
int	t_fil = 0;

char	*pp = 0;
char	emp[] = {EOL,ENDPAT,0};

char	*lbuf;
char	*pbuf;
char	*pats[NPATS];
int	npats = 0;

static int nullp = 0;

static char *h1[] = {
	IDENT,
	"grep -cnfv {-p<pattern>} files",
	"\t-c - print only a count of matched lines",
	"\t-n - preceed each line by its line number",
	"\t-f - print file names",
	"\t-v - print non-matching lines",
#if DEBUG
	"\t-d - debug (may be 1, 2 times)",
#endif
	"\t-p - give pattern, may be repeated",
	"",
	"In the pattern yor may use following elements:",
	"\tx\t- ordinary character\t\\\t- quotes any char",
	"\t^\t- beginning of line\t$\t- end of line",
	"\t.\t- any char\t\t\\nnn\t- numeric (C-style) value",
	"\t:l\t- lower-case\t\t:u\t- upper-case",
	"\t:a\t- alphabetic\t\t:d\t- digits",
	"\t:n\t- alphanumeric\t\t:r\t- any russian letter",
	"\t:s\t- space\t\t\t:t\t- tab",
	"\t:c\t- any control, except LF & TAB",
	"\t:e\t- start of subexpression",
	"\t*\t- repeat zero or more\t+\t- repeat one or more",
	"\t-\t- optionally matches the expr",
	"\t[..]\t- any from this (may be ranges FROM-TO)",
	"\t[^..]\t- any except this",
	NULL
};

void help(register char *s[]) {
	register int i;

	for (i = 0; s[i]; i++)
		fprintf(stderr,"%s\n",s[i]);
	exit(1);
}

void error(char *s1,char *s2) {
	fprintf(stderr, "grep: %s %s\n", s1, s2 ? s2 : "");
	exit(1);
}

void store(int op) {
	if (pp >= pbuf+PMAX)
		error("Pattern too complex",0);
	*pp++ = op;
}

void badpat(char *message, char *source, char *stop) {
	/* Error message */
	/* Pattern start */
	/* Pattern end */
	fprintf(stderr,"grep: %s, pattern is \"%s\"\n", message, source);
	fprintf(stderr,"\tStopped at byte %d, '%c'\n",stop-source, stop[-1]);
	error("Stop",0);
}

char *skippat(char *p) {
	int level = 1;

	while (*p != 0 && (*p != ENDPAT || --level > 0)) {
		if (*p == START || *p == STAR || *p == PLUS || *p == MINUS)
			++level;
		++p;
	}
	return p;
}

char *pmatch(char *line, char *pattern) {
	/* (����) ��ப� */
	/* (����) ��ࠧ�� */
	register char *l;	/* 㪠��⥫� ��ப� */
	register char *p;	/* 㪠��⥫� ��ࠧ� */
	register char c;	/* ⥪�騩 ᨬ��� */
	char *e;	/* ����� ��� STAR � PLUS ���祭�� */
	int op; 	/* ������ �� ��ࠧ� */
	int n;		/* ���稪 ����� */
	char *are;	/* ��砫� STAR ���祭�� */

	l = line;
#if DEBUG
	if (debug > 2)
		printf("pmatch(\"%s\")\n", line);
#endif
	p = pattern;
	while ((op = *p++) != ENDPAT) {
#if DEBUG
		if (debug > 2) {
			char *s = items[op];
			printf("byte[%d] = '%c' ~ %s",
					l-line, *l, *s < ' ' ? s+1 : s);
			if (op == CHAR)
				printf("%c'",*p);
			printf("\n");
		}
#endif
		c = *l++;
		switch(op) {
		case START:
			break;
		case CHAR:
			if (c != *p++)
				return 0;
			break;
		case BOL:
			if (--l != lbuf)
				return 0;
			break;
		case EOL:
			if (*(--l))
				return 0;
			break;
		case ANY:
			if (!c)
				return 0;
			break;
		case DIGIT:
			if (!isdigit(c))
				return 0;
			break;
		case UPPER:
			if (!isupper(c))
				return 0;
			break;
		case LOWER:
			if (!islower(c))
				return 0;
			break;
		case ALPHA:
			if (!isalpha(c))
				return 0;
			break;
		case NALPHA:
			if (!isalnum(c))
				return 0;
			break;
		case RUSSL:
			if ((c >= '�' && c <= '�') ||
			    (c >= '�' && c <= '�'))
				break;
			return 0;
		case CNTRL:
			if (c == '\t' || c >= 040)
				return 0;
			break;
		case CLASS:
		case NCLASS:
			n = *p++ & 0377;
			do {
				if (*p == RANGE) {
					p += 3;
					n -= 2;
					if (c >= p[-2] && c <= p[-1])
						break;
				}
				else if (c == *p++)
					break;
			} while (--n > 1);
			if ((op == CLASS) == (n <= 1))
				return 0;
			if (op == CLASS)
				p += n - 2;
			break;
		case MINUS:
			e = pmatch(--l, p);	/* �஢�ਬ */
			p = skippat(p); 	/* �ய��⨬ ��ࠧ�� */
			if (e)			/* ���稬? */
				l = e;		/* ��, ᬥ���� ��ப� */
			break;			/* �ᥣ�� �� */
		case PLUS:			/* 1 ��� ����� ... */
			if ((l = pmatch(--l, p)) == 0)
				return 0;	/* ��易�� ������ */
		case STAR:			/* 0 ��� ����� ... */
			are = --l;		/* �������� ��砫� ��ப� */
			while (*l && (e = pmatch(l, p)) != 0)
				l = e;		/* ᠬ�� ������� ���祭� */
			p = skippat(p); 	/* �ய��⨬ ��ࠧ�� */
			while (l >= are) {	/* ��⠥��� ������ ���⮪ */
				if ((e = pmatch(l, p)) != 0)
					return e;
				--l;		/* �� 㤠����, ����� */
			}
			return 0;		/* ��祣� �� ����稫��� */
		default:
			fprintf(stderr,"grep: Bad op code %03o for line\n%s\n",
				op,lbuf);
			exit(0);
		}
	}
	return l;
}

/* �஢�ઠ ��ப� �� lbuf, ��୥� 1, �᫨ ��ப� ����� */
int match(void) {
	register char *l = lbuf;	/* 㪠��⥫� ��ப� */
	register char *p = pbuf;

	if (p[0] == BOL && p[1] == CHAR && l[0] != p[2] ||
	    p[0] == CHAR && (l = strchr(l,p[1])) == NULL) {
#if DEBUG
		if (debug > 1)
			printf("Fast test failed for\n\"%s\"\n",l);
#endif
		return 0;
	}
	if (l[0] == '\0' && strcmp(pbuf,emp) == 0)
		return 1;
	while (*l) {
		if (pmatch(l++, pbuf))
			return 1;
	}
	return 0;
}

void printpattern(void) {
	unsigned char *lp;
	int c, n;

	printf("\n(");
	for (lp = pbuf; lp < pp-1;) {
		c = *lp++;
		printf("%s",items[c]);
		if (c == CHAR) {
			c = *lp++;
			printf("%s%c' ", c<' '?"^":"", c<' '? c + '@': c);
		}
		else if (c == CLASS || c == NCLASS) {
			for (n = *lp++; --n >= 0; ) {
				c = *lp++;
				printf("%s%c",c<' '?"^":"",c<' '? c+'@':c);
			}
			printf("] ");
		}
		else if (c == RANGE) {
			c = *lp++;
			printf("'%s%c'-", c<' '?"^":"", c<' '? c + '@': c);
			c = *lp++;
			printf("'%s%c' ", c<' '?"^":"", c<' '? c + '@': c);
		}
	}
	printf("\n");
}

int digit(int c, int r) {
	c = tolower(c);
	if (r > 10 && (c >= 'a' && c <= 'f'))
		return c-'a'+10;
	if (c >= '0' && c <= '0'+r)
		return c - '0';
	return -1;
}

char *escaped(char *source, int *val) {
	char *s = source;
	int c, v;

	if ((c = *s++) == 0)	/* ����室�� ᨬ��� */
		badpat("Class terminates badly", source, s);
	if ((v = digit(c,10)) >= 0) {
		int rad = 10;
		if (c == '0') {
			rad = 8;
			switch (*s) {
			case 'x':
			case 'X':
				rad = 16; ++s; break;
			case 'b':
			case 'B':
				rad = 2; ++s; break;
			}
			/* v = 0; */
		}
		for (; (c = digit(*s,rad)) >= 0; v = v*rad+c)
			++s;
	}
	else if (c == 't')
		v = '\t';
	else if (c == 'r')
		v = '\r';
	else if (c == 'n')
		v = '\n';
	else if (c == 'a')
		v = '\a';
	else if (c == 'f')
		v = '\f';
	else	v = c;
	if (val != NULL)
		*val = v;
	return s;
}

char *ordinar(char *source, int *val) {
	char *s = source;
	int v = 0;

	if (*s == 0)
		badpat("Non expected end of line",source,s);
	if (*s == '\\')
		s = escaped(++s,&v);
	else	v = *s++;
	if (*val != NULL)
		*val = v;
	else {
		store(CHAR);
		store(v);
	}
	return s;
}

char *aclass(char *source) {
	char *s = source;
	int c, c1;
	char *cp;

	c = CLASS;
	if (*s == '^') {
		++s;
		c = NCLASS;
	}
	cp = pp;
	store(0);				/* ���稪 ���⮢ */
	while (*s != 0 && *s != ']') {
		s = ordinar(s,&c);
		if (*s == '-' && s[1] != 0 && s[1] != ']') {
			s = ordinar(++s,&c1);
			if (c1 < c)
				badpat("Empty range",source,s);
			store(RANGE);
			store(c);
			store(c1);
		}
	}
	if (*s != ']')
		badpat("Unterminated class", source, s);
	if ((c = (int)(pp - cp)) >= 256)
		badpat("Class too large", source, s);
	if (c == 0)
		badpat("Empty class", source, s);
	*cp = c;
	return s+1;
}

char *special(char *source) {
	char *s = source;
	int c = *s++;

	switch(tolower(c)) {
	case 'l':
		store(LOWER); break;
	case 'u':
		store(UPPER); break;
	case 'a':
		store(ALPHA); break;
	case 'd':
		store(DIGIT); break;
	case 'n':
		store(NALPHA); break;
	case 'r':
		store(RUSSL); break;
	case 'c':
		store(CNTRL); break;
	case 's':
		store(CHAR); store(' '); break;
	case 't':
		store(CHAR); store('\t'); break;
	case 0:
		badpat("No type after ':'", source, s);
	default:
		badpat("Unknown type", source, s);
	}
	return s;
}

char *factor(char *source) {
	char *s = source;
	char *term(char *);
	char *lp = pp;
	int c;

	if (*s == ')')
		return s;
	store(START);			/* ���� �㤥� ����䨪��� */
	c = *s;
	if (c == '(') {
		s = term(++s);
		if (*s != ')')
			badpat("Unterminated subexpression",source,s);
		++s;
	}
	else if (c == '.') {
		++s;
		store(ANY);
	}
	else if (c == '[')
		s = aclass(++s);
	else if (c == ':')
		s = special(++s);
	else	s = ordinar(s, &nullp);
	c = *s;
	if (c == '+' || c == '-' || c == '*') {
		++s;
		*lp = (c == '+' ? PLUS : (c == '-' ? MINUS : STAR));
		store(ENDPAT);
	}
	else {
		memcpy(lp,lp+1,pp-lp);	/* ���� ��譨� START */
		--pp;
	}
	return s;
}

char *term(char *s) {
	while (*s != 0 && *s != '$' && *s != ')')
		s = factor(s);
	return s;
}

/* ��������� ��ࠧ� � ���� pbuf */
void compile(char *source) {	/* ��ࠧ�� */
	char *s = source;
#if DEBUG
	if (debug)
		printf("\nPattern = \"%s\"\n", s);
#endif
	pp = pbuf;
	if (*s == '^') {
		store(BOL);
		++s;
	}
	s = term(s);
	if (*s == '$') {
		store(EOL);
		++s;
	}
	store(ENDPAT);
	store(0);	/* �����訬 ��ப� */
	if (*s != 0)
		badpat("Unterminated syntax",source,s);
#if DEBUG
	if (debug)
		printpattern();
#endif
}

void compile1(char *s) {
	char *p;
	int n;

	if (npats >= NPATS-1)
		error("Too many patterns",0);
	compile(s);
	n = strlen(pbuf)+1;
	if ((p = malloc(n)) == NULL)
		error("No room for pattern",s);
	pats[npats] = memcpy(p,pbuf,n);
	++npats;
}

/* �����஢���� 䠩�� ��� ���᪠ ��ࠧ� �� pbuf */
void grep(FILE *fd, char *fn) {
	/* ���ਯ�� 䠩�� */
	/* ��� 䠩�� */
	register int lno, count, m, i, wasfn;

	++t_fil;
	lno = count = wasfn = 0;
	while (fgets(lbuf,LMAX,fd)) {
		lbuf[strlen(lbuf)-1] = 0;	/* ���� LF */
		++lno; ++t_lno;
		for (m = 0, i = 0; m == 0 && i < npats; ++i) {
			pbuf = pats[i];
			m |= match();
		}
		if ((m && !vflag) || (!m && vflag)) {
			++count; ++t_count;
			if (!cflag) {
				if (fflag && !wasfn && *fn) {
					printf("\f%s\n",fn);
					++wasfn;
				}
				if (nflag)
					printf("%5d\t", lno);
				puts(lbuf);
			}
		}
	}
	if (ferror(fd))
		error("Read error file", *fn ? fn : "");
	if (cflag)
		printf("%s %d lines matched, %d total\n",
			*fn ? fn : "", count, lno);
}

int main(int argc, char *argv[]) {
	register int i, c;
	register int fcnt = 0;	/* �᫮ 䠩��� */
	char *ap;
	FILE *fd;

	if ((lbuf = malloc(LMAX)) == NULL ||
	    (pbuf = malloc(PMAX)) == NULL)
		error("No enought memory",0);
	for (i = 1; i < argc; ++i) {
		ap = argv[i];
		if (*ap == '-') {
			argv[i] = NULL;
			++ap;
			while ((c = *ap++) != 0) {
				switch (tolower(c)) {
				case '?':
					help(h1); break;
				case 'c':
					++cflag; break;
#if DEBUG
				case 'd':
					++debug; break;
#endif
				case 'n':
					++nflag; break;
				case 'f':
					++fflag; break;
				case 'v':
					++vflag; break;
				case 'p':
					if (*ap != 0)
						strcpy(lbuf,ap);
					else if (++i < argc) {
						strcpy(lbuf,argv[i]);
						argv[i] = NULL;
					}
					ap = "";
					compile1(lbuf);
					break;
				default:
					error("Unknown switch",--ap);
				}
			}
		}
		else	++fcnt; /* ��� 䠩�� */
	}
	if (npats == 0)
		error("No pattern",0);

	if (fcnt == 0)
		grep(stdin,0);
	else {
		for (i = 1; fcnt > 0 && i < argc; ++i) {
			ap = argv[i];
			if (ap) {
				--fcnt;
				if ((fd = fopen(ap,"r")) == NULL)
					error("Can't open",ap);
				grep(fd,ap);
				fclose(fd);
			}
		}
		if (cflag)
			printf("%ld lines matched, %ld total in %d file(s)\n",
				t_count, t_lno, t_fil);
	}
	return 0;
}

