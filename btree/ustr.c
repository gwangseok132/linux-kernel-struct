//
//  Source: ustr.c written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2010, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2011-01-05 ���ڿ� ó�� �Լ����� �ۼ��ϴ�.
//		2011-02-24 ���ڿ��� �ؽð��� �����ϴ� �Լ�(hash_value)�� �߰��ϴ�.
//		yyyy-mm-dd ...
//

#include "dtype.h"
#include "umac.h"

//���ڿ� ����
unsigned int str_len (char* t)
{
	char *s = t;
	while (*t++);
	t--;
	return (unsigned int)(t - s);
}

//���ڿ� �߰�(t <-- s)
void str_cat (char *t, char *s)
{ 
	while (*t++);			//t�� ����ġ
	t--;
	while (*t++ = *s++);	//����
} 

//�����ȣ �ΰ�?
int is_smark (char c)
{
	int mark[] = {33, 34, 44, 58, 59, 63, 96};	//! " , : ; ? ` ('�� �ܾ����ӿ� �����Ƿ� ����)
	register int i;

	for (i = 0; i < sizeof(mark)/sizeof(int); i++)
		if (c == mark[i]) return 1;
	return 0;
}

//���� ���ΰ�?
int is_end (char c)
{
	return (c==46 || c==63 || c==33) ? 1 : 0;	//. ? !
}

//whitespace �ΰ�?
int is_whitespace (char c)
{
	return (c > 0 && c < 33) ? 1 : 0;  //�ѱ��� ����, 2����Ʈ(0x80,0x80)
}

//whitespace Ȥ�� �����ȣ �ΰ�?
int is_space_mark (char c)
{
	return (c > 0 && c < 33) ? 1 : is_smark (c);
}

//�����ΰ�?
int is_digit (char c)
{
	return (c > 47 && c < 58) ? 1 : 0;
}

///unsigned int�� ���ڿ��� ��ȯ
char* uint_to_str (unsigned int n, char *s)
{
	register int i=0, j=0;
	char temp;

	do
		s[i++] = n - (n / 10 * 10) + '0';	//���ڸ� ���� �ƽ�Ű������
	while (n /= 10);
	s[i] = '\0';
	i--;

	while (i > j) {		//���� ���ڿ� �����ٲ�(reverse)
		temp = s[i];
		s[i--] = s[j];
		s[j++] = temp;
	}
	return s;
}

//unsigned int�� ���ڿ��� ��ȯ��, �Ű������� �־��� ���̰� �ǵ��� ���ʿ� '0'�� �е�
char* uint_to_str_len (unsigned int n, char *s, unsigned int length)
{
	register int i, gap;
	char sz[] = {"0000000000"}; //10�ڸ�(unsigned int)
	char *ps, *ps2;

	uint_to_str (n, s);
	ps = s;  //ó�� ��ġ ������
	ps2 = s;
	gap = length - str_len (s);
	if (gap > 0) {
		for (i = 0; i < gap; i++);  //�е��� ��ġ���� �̵�
		while (sz[i++] = *s++);
		i = 0;
		while (*ps++ = sz[i++]);
	}
	return ps2;
}

//���ڿ��� unsigned int�� ��ȯ
unsigned int str_to_uint (char *s) 
{ 
	unsigned int n = 0;	
	//while (*s >= '0' && *s <= '9')
	while (*s) {
		if (*s > 47 && *s < 58)	 n = 10 * n + (*s - '0');
		s++;
	}
	return n;
} 

//�ϳ��� �ƽ�Ű ���ڸ� �ҹ��ڷ�
char a_lower (char c) 
{ 
	return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
} 

//�ϳ��� �ƽ�Ű ���ڸ� �빮�ڷ�
char a_upper (char c) 
{ 
	return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
} 

//���ڿ��� �ҹ��ڷ�
char* str_lower (char *t)
{
	char* s = t;

	while (*t) {
		if (*t >= 'A' && *t <= 'Z') 
			*t = *t + 'a' - 'A';
		t++;
	}
	return s;
} 

//���ē��ΰ�?
int is_alpha (char c)
{
	c = a_lower (c);
	return (c > 96 && c < 123) ? 1 : 0;
}

//���ڿ� ���� �ٲ�(reverse)
void str_reverse (char *s)
{
	register int i, j;
	char temp;

	i = str_len (s) - 1;
	j = 0;
	while (i > j) {
		temp = s[i];
		s[i--] = s[j];
		s[j++] = temp;
	}
	//return s;
}

//���ڿ� ����
void str_copy (char *t, char *s) 
{ 
	while (*t++ = *s++);
} 

//���ڿ� ��
int str_cmp (char *s, char *t) 
{ 
	for ( ; *s == *t; s++, t++) 
		if (*s == '\0' || *t == '\0') break;
	return *s - *t;
}

//���ڿ� s���� �����ϴ��� ��
int str_cmp_like (char *s, char *t) 
{ 
	for ( ; *s == *t; s++, t++) ;
	if (*s == '\0') return 0; 
	return *s - *t;
}

//���ڿ��� �������(����Ű �ε���)�� unsigned int�� ��ȯ�� ��
int str_cmp_int (char *s, char *t) 
{ 
	register int i;
	char sa[11], ta[11];  //�迭, unsigned int�� ���ڰ��� ��ŭ�� ����
	unsigned int su=0, tu=0;

	while (*s && *t) {
		i = 0;
		while (*s != '_') sa[i++] = *s++;
		sa[i] = '\0';
		su = str_to_uint (sa);
		s++;
		
		i = 0;
		while (*t != '_') ta[i++] = *t++;
		ta[i] = '\0';
		tu = str_to_uint (ta);
		t++;

		if (su != tu) break;
	}

	//unsigned int�� ������ �����Ƿ� if�� ��
	if (su < tu) return -1;
	if (su > tu) return 1;
	return *s - *t;
}

//���ڿ��� �������(����Ű �ε���)�� unsigned int�� ��ȯ�� ��
//*s ���� ��ġ�ϴ��� ��
int str_cmp_int_like (void *p1, void *p2) 
{ 
	char *s, *t;
	register int i;
	char sa[11], ta[11];  //�迭, unsigned int�� ���ڰ��� ��ŭ�� ����
	unsigned int su, tu;

	s = (char*)p1;
	t = (char*)p2;	

	while (*s && *t) {
		i = 0;
		while (*s != '_') sa[i++] = *s++;
		sa[i] = '\0';
		su = str_to_uint (sa);
		s++;
		
		i = 0;
		while (*t != '_') ta[i++] = *t++;
		ta[i] = '\0';
		tu = str_to_uint (ta);
		t++;

		if (su != tu) break;
	}

	if (*s && !*t) return 1;

	//unsigned int�� ������	�����Ƿ� if�� ��
	if (su < tu) return -1;  
	if (su > tu) return 1;
	return 0;
}

//���ڿ� t�ȿ� ���ڿ� s�� ���ԵǾ� ������ 0�� ����
int str_ncmp (char *t, char *s, int num)
{
	register int cnt = 0;

	for ( ; *s != *t; t++) 
		if (*t == '\0') return 1; 

	for ( ; *s == *t; s++, t++) {
		if (++cnt == num) return 0;
		if (*s == '\0') return 1;
	}

	return 1;
}

/* ����Ű�� ���ڿ��� ���ڷ� ��ȯ�� ���ؾ� ��
int str_cmp_int_similar (void *p1, void *p2) 
{ 
	return str_ncmp ((char*)p2, (char*)p1, str_len ((char*)p1));
}
*/
///���ڿ��� �������(����Ű �ε���)�� unsigned int�� ��ȯ�� ��
///�ܾ 2�� �̻� ��ġ�ϴ��� ����
///-1�� �����ϸ� ���� �˻� ����
int str_cmp_int_similar (void *p1, void *p2) 
{ 
	char *s, *t;
	register int i, j=0, sc=0, tc=0;
	char sa[11], ta[11];  //�迭, unsigned int�� ���ڰ��� ��ŭ�� ����
	unsigned int su=0, tu=0;
	unsigned int sca[SSIZE], tca[SSIZE];

	s = (char*)p1;
	t = (char*)p2;	

	while (*t) {
		i = 0;
		while (*t != '_') ta[i++] = *t++;
		ta[i] = '\0';
		tu = str_to_uint (ta);
		t++;
		tca[tc++] = tu;
	} //while
	//if (tc == 0) return 1;

	while (*s) {
		i = 0;
		while (*s != '_') sa[i++] = *s++;
		sa[i] = '\0';
		su = str_to_uint (sa);
		s++;
		sca[sc++] = su;
	}

	if (sc > tc) return 1;

	for (i = 0; i < tc; i++) {
		if (tca[i] == sca[j]) j++;
		if (j == sc) return 0;
	}

	return 1;
}

//���ڿ� �յ��� whitespace �߶�
char* str_trim (char *s)
{
	char *t;

	//t = s + str_len (s) - 1;
	t = s + str_len (s);
	*t-- = '\0';
	while (um_whites (*t) ) *t-- = '\0';  //�ںκ� whitespace ����
	while (um_whites (*s) ) s++;  //�պκ� whitespace ����

	return s;
}

//���ڿ��� ���� whitespace �߶�
char* str_trim_left (char *s)
{
	while (um_whites (*s) ) s++;  //�պκ� whitespace ����
	return s;
}

//���ڿ����� Ư�� ���� ��ü
void str_replace (char *s, char c1, char c2) 
{ 
	while (*s) {
		if (*s==c1) *s = c2;
		s++;
	}
} 

//���ڿ� s�� �ؽð� ����(sh�� �̹� ���� �ؽð�)
unsigned int hash_value (char *s) 
{ 
	unsigned int h;

	for (h = 0; *s != '\0'; s++)	//���ڿ� s�� ���̸�ŭ �ݺ�
		h = *s + HASH * h;			//���� *s�� �ƽ�Ű���� �ؽð� ����

	return (h % (HASHSIZE-1)) + 1;	//(0 < �ؽð� < HASHSIZE), 0�� ĸ�� ���忡�� ���
} 

//�ѱ��ΰ�?
bool is_kor (char* s)
{
	const char *sk1 = "��";	//0xB0A1
	const char *sk2 = "��";	//0xC8FE

	if (*s >= sk1[0] && *s <= sk2[0])
		if (*(s+1) >= sk1[1] && *(s+1) <= sk2[1]) return true;
	return false;
}

//�������ΰ�?
bool is_eng (char* s)
{
	return (*s >= 32 && *s <= 126) ? true : false;	//32�� ����
}

//���ڿ��� �ѱ��� ���ԵǾ� �ִ°�?
bool str_is_kor (char* s)
{
	while (*s) {
		if (is_kor (s)) return true;	//�ѱ� ����
		s++;
	}
	return false;	//�ѱ� ����
}

//���ڿ��� ��� �������ΰ�?
bool str_is_eng (char* s)
{
	while (*s) {
		if (*s < 32 || *s > 126) return false;	//������ �ƴ�(Ư������)
		s++;
	}	
	return true;	//��� ������
}

//���ڿ��� ���� �Ǵ� �ѱ��ΰ�?
int str_is_eng_kor (char* s)
{
	int cnt=0;

	if (!*s) return -1;

	while (*s) {
		if (*s > 126) return -1;	//Ư������
		if (*s < 32) {
			if (is_kor (s)) {
				s++;
				cnt++;
			} else return -1;	//Ư������
		}
		s++;
	}	
	return (cnt) ? 1 : 0;	//0:��� ������, 1:�ѱ�����
}
