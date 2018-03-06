//
//  Source: dtype.h written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2010, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2011-01-17 ������ Ÿ�� ������� ����
//		2011-01-18 BTREE ����ü�� �ߺ�Ű ��뿩�� �ʵ�(samek)�� �߰��ϴ�.
//		2011-02-11 �Լ� ��Ī�� ��Ģ�� �ο��ϴ�.(��Ī ����� ��Ģ)
//					�Լ����� ���ξ�� �Ҽ� ���ϸ��̰�, private �Լ��� ���ξ� �տ� _�� ���δ�.
//		2011-02-24 �ؽð� ���(HASH)�� �߰��ϴ�.
//		2011-03-16 ���� ���� ������ ����(StackTW)�� ���������� �߰��ϴ�.
//		yyyy-mm-dd ...
//

#ifndef __DTYPE
#define __DTYPE

#define __LINUX
//#define __WIN32

//��������: V.yyyy.mm
//���߹���: V.yyyy.mm.dd
#define TWVersion "**TransWorks V1.2011.04"

#define ASIZE		  80	//�ܾ� ����
#define SSIZE		1000	//���� ���� (1000����, ��100�ܾ�)

#define HashSizeStr	 "10"	//������ ���� ����� ���
#define HASHSIZE	  10	//(0 < �ؽð� < HASHSIZE), 0�� ĸ�� �����, �ؽð��� ���ҽ�Ű�� ���� �����͸� ��������.
#define HASH		  31	//�ؽð� ���� �������� ��(������ �ؽð� ������ ����)

#define	LF	0x0A	//Line Feed
#define CR	0x0D	//Carige Return

//�۾� ���а�(flag), tw1.c���� ��� -------------------------------------------
#define	FLAG_NONE	0
#define	FLAG_INSERT	1
#define	FLAG_DELETE	2
#define	FLAG_UPDATE	3
#define	FLAG_TRANS	4
#define	FLAG_VIEW	5
#define	FLAG_AUTO	6
#define	FLAG_CAP	7

#define	FLAG_YES	 1
#define	FLAG_NO		-1

//unsigned int �ִ밪(0�� ����)
#define UIFAIL		~0

//����� ���ϸ� --------------------------------------------------------------
#define DIR_WORKS	"./works/"	//����� �۾� ���

//�Ʒ� ���ϵ��� TW ������ ���� �̹Ƿ� ������(./)���� �۾���
#define FNAME_DIC0	"twd0.twd"  //�ܾ���� A
#define FNAME_DIC1	"twd1.twd"  //�ܾ���� B

//��ũ�δ� �Ű������� ������� ���޵Ǿ�� ��
//#define MR_FNAME(num) "twd" ## num ## ".twi"

#define FNAME_IDX	"twd"
#define FNAME_EXT	".twi"

#define FNAME_DAT0	"twd0.twa"  //������(�����ܾ� ��ȯ��)
#define FNAME_DAT1	"twd1.twa"  //������(������ �ܾ� ������ȣ ������ A)
#define FNAME_DAT2	"twd2.twa"  //������(������ �ܾ� ������ȣ ������ B)
#define FNAME_DAT3	"twd3.twa"	//���� ����
#define FNAME_DAT4	"twd4.twa"	//���� ����(ȸ��)

#define FNAME_KEY	"twdk.twa"	//����ȸ�� ����Ű ����

#define MANAGER		"J##)&J#"	//������
#define TRANS_CNT	     5		//��ȸ�� ������� ����
#define PKEY_DDAY	   365		//����Ű ��ȿ��(1��)
#define SIMILAR			 5		//Similar �˻�(full scan) ��� �ܾ��

#define STACK_HEIGHT   100		//�������� ������ ������ ����

//boolean ������ Ÿ��(gcc: #include <stdbool.h>) ------------------------------
typedef enum {
    true = 1,
    TRUE = 1,
    false = 0,
    FALSE = 0
} bool;

//�ڷᱸ��(����, ť, ����Ʈ) ����ü ���� --------------------------------------

//�ܹ��� ���(�Ϲ����� ����, ť, ����Ʈ)
typedef struct node
{
	void*			data;
	struct node*	link;
} NODE;

//����� ���(������ ����-->����Ʈ)
typedef struct node2
{
	struct node2*	prev;	//prev link
	void*			data;
	struct node2*	next;	//next link
} NODE2;

//���� ����ü
typedef struct
{
	NODE2*	top;
	NODE2*	bottom;
	int		count;
} STACK;

//ť ����ü
typedef struct
{
	NODE* front;
	NODE* rear;
	int count;
} QUEUE;


STACK*	StackTW;	//������ ���� ����� ����(����)
bool StackTW_Enable;


//B+Tree ����ü ���� ------------------------------------------------------------------------------

//�������� ��������� ����Ǿ� �ִ� ���ܾ� ���� ��30����(�ִ� 100���� ����)
//8�� 6��      262144      ( 26��)
//8�� 7��     2097152      (209��) *
//8�� 8��    16777216     (1677��)
//8�� 9��   134217728  (1��3421��) #
//8��10��  1073741824 (10��7374��)

//6�� 8��  1,679,616
//6�� 9�� 10,077,696

//�迭 ���� ����(Ʈ�� ������ ��Ʈ�� ����)
//B_ORDER�� ¦���϶�, �������ҵǴ� ������ �������� (Ű������ �����Է½� �ʿ��� ����)
#define B_ORDER	4
///#define B_ORDER		8	//�Ϲݴܾ�
///#define R_ORDER		6	//�����ܾ�
#define R_ORDER		4	//�����ܾ�

//�׽�Ʈ�� ������:
//�ܾ��(key)�� ��30���� �϶�:
//(1) B+Tree ���� (������ 8�϶� 9, ������ 6�϶� 11)
//(2) ������ ���� ũ��� ��� �� 4x3=12M
//(3) �޸� �Ҵ� ũ��� ��� �� 240M

//�޸� �Ҵ�: �ܾ�� 10������ 100M

//key�� unsigned int ������ Ÿ��, �ִ� 0xFFFFFFFF(32��Ʈ: 10�ڸ�: 4,294,967,295(��42��))��


//B+ Ʈ���� ��� ����:
//����� �ٽ��� keys�� �迭�̸� pointers���� �������̴�.
//keys�� pointers�� ����� leaves�� ���γ�� ���̿��� �޶�����.
//�������� ������ Ű �ε����� �̰Ͱ� ������ pointer �ε����� �����ϸ�, �ִ� (B_ORDER - 1)���� Ű-������ ���� ������.
//������ pointer�� ������ ������ ����Ų��.
//���γ�忡���� ù��° pointer�� Ű �迭���� �������� Ű���� ���� Ű�ε� ������ ����Ű��,
//(i+1)��° pointer�� �ε��� i��° ����� Ű���� ũ�ų� ���� Ű���� ����Ų��.
//num_keys �ʵ�� ��ȿ�� Ű���� �����̴�.
//���γ�忡�� ��ȿ�� pointer���� ������ �׻� (num_keys + 1)�̴�.
//���������� �����Ϳ� ��ȿ�� pointer���� ������ �׻� num_keys �̴�.
//������ ���� pointer�� ���� ������ ����Ų��.
typedef struct node5
{
	struct node5*	parent;		//�θ��� ������
	bool			is_leaf;	//��������ΰ�?
	int				num_keys;	//������ Ű ����

	void**			pointers;	//���γ�� �� ���ڵ�(������) ������ ���͸�(�迭)
	void**			keys;		//Ű�� ���͸�(�迭)

	struct node5*	next;		//��带 ����Ҷ� Queue Node�� ���(����׿�)
} NODE5;

//B+Ʈ�� ���� ����ü
typedef struct
{
	NODE5*			root;
	int				order;		//B+Ʈ�� ����(���͸� �迭ũ��)
	unsigned int	kno;		//Ű������ȣ(���������)
	unsigned int	kcnt;		//Ű����(��������, ��������)
	int  (*compare) (void* p1, void* p2);	//Ű�� �Լ�������
	void (*outkey)  (void* p1);				//Ű��� �Լ�������
	void (*outdata) (void* p1);				//��������� �Լ�������
	bool			samek;		//�ߺ�Ű ��뿩��
} BTREE;

#endif
