//
//  Source: fileio.h written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2010, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2010-12-21 ���� ����� �Լ����� �ۼ��ϴ�.
//		yyyy-mm-dd ...
//

#include "dtype.h"	//B+ Tree ���

//���Ͽ��� �ܾ �о B+Ʈ���� �Ҵ��� �� ������ ��ȯ
int _fio_read_header (FILE *fp, char *fname, char *buf);  //���� ��� �б�
unsigned int fio_read_from_file (char *fname, BTREE* btree);
unsigned int fio_read_trans_asc (char *fname, BTREE* btree, int flag);
unsigned int fio_read_trans_hash (char *fname, BTREE** hb[], int sh);

//B+Ʈ���� ������ ���Ͽ� ����
void _fio_write_header (FILE *fp, char *fname);  //���� ��� ����
unsigned int fio_write_to_file (char *fname, BTREE* btree);
unsigned int fio_write_to_file_trans (char *fname, BTREE* btree);

///������ �ܾ��� ������ȣ�� ���Ͽ��� �о queue�� ����
unsigned int fio_read_from_file_kno (char *fname, QUEUE* queue);
///queue�� �ִ� ������ �ܾ��� ������ȣ�� ���Ͽ� ����
unsigned int fio_write_to_file_kno (char *fname, QUEUE* queue);

///���� ������ ����(txt)���� ��������� �о ����(�Է�)
int fio_import (char *fname, BTREE** ws, BTREE** wi, BTREE** hb[], int mode, BTREE* rs, QUEUE** qk);
///���������� ���Ͽ� ���
int _fio_export_data (FILE *fp, BTREE* wi, void* keys);
///���������� ���ۿ� ����
int _fio_export_buffer (char sbuf[], BTREE* wi, void* keys);
///Ű�� �ϳ��� �и��Ͽ� �ܾ� ������ ���
int _fio_trans_key_each (FILE *fp, char* keys, BTREE** hb[], BTREE** wi, int mode, int kcnt);
///���� �������� ���Ͽ� ���(����)
void fio_export (char* fname, BTREE* pt, BTREE* pi_src, BTREE* pi_trg);
///����(revision) �ܾ ����(txt)���� �Է�(�б�)
int fio_import_revision (char *fname, BTREE* rs);
///����(revision) �ܾ ����(txt)�� ���(����)
void fio_export_revision (char* fname, BTREE* rs);

//���Ͽ� �ִ� ���� ����(mode=0�� ��������, 1�� �ѱ۹���)
int fio_translation (char *fname, char *fname2, BTREE** ws, BTREE** wi, BTREE** hb[], int mode, BTREE* rs, QUEUE** qk, int flag);

//���� ������ �о ���
bool fio_read_help (char *fname);

//��� �� ���ϸ� ���� �Լ�
int fio_mkdir (char *dir);
int fio_getchar_fname (char* msg, char* fname);

//����ȸ�� ����Ű ����
int fio_write_member_key (char* fname, char* skey);
//����ȸ�� ����Ű �б�
int fio_read_member_key (char* fname, char* skey);

///���� ������ ����(txt)���� CAPTION ������� �о ����(�Է�)
int fio_import_caption (char *fname, BTREE** ws, BTREE** wi, BTREE** hb[], int mode, BTREE* rs, QUEUE** qk);
