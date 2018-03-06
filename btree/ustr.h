//
//  Source: ustr.h written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2010, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2010-12-18 ���ڿ� ó�� �Լ����� �ۼ��ϴ�.
//		2011-02-24 ���ڿ��� �ؽð��� �����ϴ� �Լ�(hash_value)�� �߰��ϴ�.
//		yyyy-mm-dd ...
//

//unsigned int�� ���ڿ��� ��ȯ
char* uint_to_str (unsigned int num, char *s);

//unsigned int�� ���ڿ��� ��ȯ��, �Ű������� �־��� ���̰� �ǵ��� ���ʿ� '0'�� �е�
char* uint_to_str_len (unsigned int n, char *s, unsigned int length);

//�����ȣ �ΰ�?
int is_smark (char c);

//���� ���ΰ�?
int is_end (char c);

//whitespace �ΰ�?
int is_whitespace (char c);

//whitespace Ȥ�� �����ȣ �ΰ�?
int is_space_mark (char c);

//�����ΰ�?
int is_digit (char c);

//���ڿ��� unsigned int�� ��ȯ
unsigned int str_to_uint (char *s);

//�ϳ��� �ƽ�Ű ���ڸ� �ҹ��ڷ�
char a_lower (char c);

//�ϳ��� �ƽ�Ű ���ڸ� �빮�ڷ�
char a_upper (char c);

//���ڿ��� �ҹ��ڷ�
char* str_lower (char *t);

//���ē��ΰ�?
int is_alpha (char c);

//���ڿ� ���� �ٲ�(reverse)
void str_reverse (char *s);

//���ڿ� ����
void str_copy (char *t, char *s);

//���ڿ� �߰�
char* str_cat (char *t, char *s);

//���ڿ� ����
unsigned int str_len (char* t);

//���ڿ� ��
int str_cmp (char *s, char *t);

//���ڿ� s������ ��
int str_cmp_like (char *s, char *t);

//���ڿ��� �������(����Ű �ε���)�� unsigned int�� ��ȯ�� ��
int str_cmp_int (char *s, char *t);

//���ڿ��� �������(����Ű �ε���)�� unsigned int�� ��ȯ�� ��
//*s ���� ��ŭ�� ��
int str_cmp_int_like (void*, void*);

//�ܾ 2�� �̻� ��ġ�ϴ��� ����
int str_cmp_int_similar (void*, void*);

///���ڿ� �յ��� whitespace �߶�
char* str_trim (char *s);

///���ڿ��� ���� whitespace �߶�
char* str_trim_left (char *s);

//���ڿ����� Ư�� ���� ��ü
void str_replace (char *s, char c1, char c2);


///���ڿ� s�� �ؽð� ����
unsigned int hash_value (char *s);


///�ѱ��ΰ�?
bool is_kor (char* s);

///�������ΰ�?
bool is_eng (char* s);

///���ڿ��� �ѱ��� ���ԵǾ� �ִ°�?
bool str_is_kor (char* s);

///���ڿ��� ��� �������ΰ�?
bool str_is_eng (char* s);

///���ڿ��� ���� �Ǵ� �ѱ��ΰ�?
int str_is_eng_kor (char* s);
