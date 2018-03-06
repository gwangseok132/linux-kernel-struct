//
//  Source: tw1.h written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2010, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2011-01-01 tw(TransWorks) ������Ϸ� ������Ű��.
//		2011-02-09 �����ܾ� ������� �߰��ϴ�.
//		yyyy-mm-dd ...
//

#include "dtype.h"

//B+Tree ���� �Լ�
int tw1_compare_int (void* p1, void* p2);
int tw1_compare_str (void* p1, void* p2);
//����Ű ���ڿ��� unsigned int�� ��ȯ�� ��
int tw1_compare_str_int (void* p1, void* p2);
void tw1_output_int (void* p1);
void tw1_output_str (void* p1);

int tw1_qn_answer (char *msg, int ans);

//�Է�
unsigned int _tw1_insert_to_btree (BTREE*, BTREE*, char *in_word, QUEUE*);
bool _tw1_insert_to_btree_trans (BTREE*, BTREE*, char* keys1, char* keys2);
int tw1_insertion (BTREE**, BTREE**, BTREE** hb[], int mode, BTREE* rs, QUEUE**);
int tw1_insertion_from_file (BTREE**, BTREE**, BTREE** hb[], int, BTREE*, QUEUE**, char rows[2][SSIZE], int sh);

//���� ���
int  _tw1_trans_key_data (BTREE*, void* keys, int kcnt);
int  _tw1_trans_key_data_each (BTREE* wi, void* keys);
int  _tw1_trans_key_finder (NODE5* leaf, int k, char *keys, BTREE* pt1, BTREE* hb[], BTREE* pi_trg, int flag);
void _tw1_trans_key_equal (NODE5* leaf, char *keys, int k, BTREE* pt1, BTREE* pi_trg);
int  _tw1_trans_key_like (char *keys, BTREE* pt, BTREE* pi_src, BTREE* pi_trg, int kcnt);
int  _tw1_trans_key_similar (char *keys, BTREE* pt, BTREE* pi_src, BTREE* pi_trg);
int  _tw1_trans_key_each (char* keys, BTREE** hb[], BTREE** wi, int mode, int kcnt);
int  _tw1_trans_search (char* keys, BTREE** hb[], BTREE** wi, BTREE* rs, int mode, int kcnt);
void tw1_translation (BTREE**, BTREE** hb[], BTREE**, int mode, char* prompt, BTREE* rs, QUEUE**);

//����
unsigned int tw1_drop_word_run (BTREE*, BTREE*, char *key, bool *deleted);
bool tw1_drop_word (BTREE* ws, BTREE* wi, QUEUE* queue);
int _tw1_delete_trans_key_run (BTREE* pt1, BTREE* pt2, NODE5* leaf, int k);
int _tw1_delete_trans_key (char* keys, int cnt, BTREE** hb[], BTREE** wi, int mode);
int tw1_deletion (BTREE**, BTREE** hb[], BTREE**, int mode, char* prompt, BTREE* rs, QUEUE**);

//���
void tw1_display (BTREE* hb[], BTREE*, BTREE*);

//�׽�Ʈ
//����(������ ���ڿ� �ִ� 10�ڸ�)�� �߻��Ͽ� ���԰� ������ �ݺ�����(loop_max: �ݺ�ȸ��)
//Ű�� 70% ���� ���Եǰ�, 30%�� ������.
//�ݺ��� 100���� ����� �� 100M ����Ʈ�� �޸� �Һ�. (���� ��100����Ʈ)
int tw1_test_insert (BTREE* ws, BTREE* wi, char* words, char akeys[], QUEUE*);
///�ܾ� �Է� �׽�Ʈ
void _tw1_test_ins_word_random (BTREE* ws, BTREE* wi, unsigned int cnt_loop, QUEUE*);
///�ܾ� ���� �׽�Ʈ
void _tw1_test_del_word_random (BTREE* ws, BTREE* wi, unsigned int cnt_loop, QUEUE*);
void tw1_test_random (BTREE* ws[], BTREE* wi[], BTREE** hb[], unsigned int cnt_loop, QUEUE* qk[]);

//����
void tw1_save (BTREE* ai, BTREE* bi, BTREE* hbta[], BTREE* rs);

//�޴�
char* _tw1_prompt (int mode);
char* _tw1_menu (int mode, int isave);
void tw1_menu_run (BTREE* ws[], BTREE* wi[], BTREE** hb[], BTREE*, QUEUE* qk[], int isave);
//������ �޴�
char* _tw1_manager_menu (int mode);
int tw1_manager (BTREE* ws[], BTREE* wi[], BTREE** hb[], int mode, BTREE*, QUEUE* qk[]);
//ȸ�� �޴�(����)
char* _tw1_member_menu (int mode);
int tw1_member (BTREE* ws[], BTREE* wi[], BTREE** hb[], int mode, BTREE* rs, QUEUE* qk[]);

//���
void tw1_statis (BTREE* ws[], BTREE* wi[], BTREE** hb[], BTREE* rs, QUEUE** qk);

//�ܾ� ����(�����ܾ� ����)
char* tw1_revision (BTREE* rs, char* skey);

//�ܾ words �迭�� �Է� ����
int _tw1_words_getchar (char* prompt, char words[], BTREE* ws);
//���ڿ��� �Է¹޾� �ܾ� ������ �ε��� ������ keys �迭�� ����
int tw1_words_input (char* prompt, char keys[], BTREE*, BTREE*, BTREE* rs, QUEUE*, int flag);

