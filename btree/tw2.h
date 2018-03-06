//
//  Source: tw2.h written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2011, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2011-02-11 tw2.h ����� �ڵ��ϴ�.
//		yyyy-mm-dd ...
//

#include "dtype.h"

///����(revision) �ܾ B+Ʈ���� �Է�
bool tw2_rev_word_insert (BTREE* rs, char akey[][ASIZE]);
///����(revision) �ܾ� Ű������ �Է�
int tw2_rev_word_input (BTREE* rs);
///�����ܾ� B+Ʈ������ ����(�ܾ����)
int tw2_rev_word_delete (BTREE* rs);
///����(revision) �ܾ� �Է�(����)
int tw2_rev_word_import (BTREE* rs);
///����(revision) �ܾ� ���(����)
void tw2_rev_word_export (BTREE* rs);

///���� ����(�Է�)
unsigned int _tw2_word_update_run (BTREE* ws, BTREE* wi, char *in_word, unsigned int kno);
///�ܾ� ����(������ �Է�)
int tw2_word_update (BTREE* ws, BTREE* wi, char* prompt);

///���Ͽ��� ������ ����� �Է�
int tw2_import_from_file (BTREE** ws, BTREE** wi, BTREE** hb[], int mode, BTREE* rs, QUEUE** qk, int flag);
///������� ���Ͽ� ���(k=0: �ܾ�, k=2: ����)
void tw2_export_to_file (BTREE** hb[], BTREE** wi, int mode, int sh);

///���Ͼ��� �������
void tw2_file_translation (BTREE** ws, BTREE** wi, BTREE** hb[], int mode, BTREE* rs, QUEUE** qk, int flag);

///�ؽð��� �ϳ� �Է¹���
int tw2_getchar_hash_value (void);
//������ ��й�ȣ�� �Է¹���
bool tw2_getchar_manager_pw (void);

//����Ű ��ȣȭ/��ȣȭ
int _tw2_getchar_encode_key (char* msg, char* akey, int flag);
void _tw2_get_decode_key (char* skey, char* out_key, char* out_key_time, int cnt, int flag);
int tw2_member_key_input (char *fname, int flag);
void tw2_member_key_output (char *fname);
int tw2_getchar_member_pw (char *fname);

//CAPTION �� rs Ʈ���� ����
int tw2_insertion_caption (BTREE** ws, BTREE** wi, int mode, BTREE* rs, QUEUE** qk, char rows[][ASIZE]);

//������ ���� ����
void tw2_stack_review (void);
int tw2_stack_push (char* str);
void tw2_stack_pop (void);
