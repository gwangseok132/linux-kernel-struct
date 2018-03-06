//
//  Source: tw2.c written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2011, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2011-02-11 tw2.c ����� �ڵ��ϴ�.
//		2011-02-16 �ܾ� ���� �Լ�(tw2_word_update) �߰��ϴ�.
//		2011-02-22 ���Ͼ��� ���� ���� �߰��ϴ�.(tw2_file_translation: �����Է� >> ���� >> �������)
//		yyyy-mm-dd ...
//

#include <stdio.h>
#include <stdlib.h>	//malloc

#include "dtype.h"
#include "ustr.h"		//���ڿ�
#include "bpt3.h"
#include "tw1.h"
#include "fileio.h"
#include "utime.h"
#include "stack.h"

//����(revision) �ܾ� B+Ʈ���� �Է�
bool tw2_rev_word_insert (BTREE* rs, char akey[][ASIZE])
{
	char *pkey1, *pkey2, *mkey1, *mkey2;
	NODE5 *leaf;

	pkey1 = str_lower (akey[0]);	//�ҹ��ڷ� ��ȯ
	pkey1 = str_trim (pkey1);		//�յ� whitespace �߶�
	if (! *pkey1) return false;

	pkey2 = str_lower (akey[1]);	//�ҹ��ڷ� ��ȯ
	pkey2 = str_trim (pkey2);		//�յ� whitespace �߶�
	if (! *pkey2) return false;
	
	if (bpt_find_leaf_key (rs, pkey1, &leaf) < 0) {	
		//������ ������ Ű�� ���ٸ� �޸��Ҵ��� ����
		mkey1 = malloc (str_len (pkey1) + 1);
		if (!mkey1) {
			printf ("## Failure to allocate memory.\n");
			return false;
		}
		str_copy (mkey1, pkey1);

		mkey2 = malloc (str_len (pkey2) + 1);
		if (!mkey2) {
			printf ("## Failure to allocate memory.\n");
			return false;
		}
		str_copy (mkey2, pkey2);
		//B+Ʈ���� �Է�
		rs->root = bpt_insert (rs, leaf, mkey1, mkey2, FLAG_INSERT);
		//printf ("** Inserted.\n");
		return true;

	} else {
		printf ("** Exist same key.\n");
		return false;
	}
}

//����(revision) �ܾ� Ű������ �Է�
int tw2_rev_word_input (BTREE* rs)
{
	char akey[2][ASIZE];
	int	 c, cnt=0;
	register int i;

	while (1) {
		printf ("Word1: ");
		i = 0;
		while ((c = getchar()) != (int)'\n' && i < ASIZE-2)	akey[0][i++] = c;
		akey[0][i] = '\0';
		if (i < 2 || i == ASIZE-2) break;

		printf ("Word2: ");
		i = 0;
		while ((c = getchar()) != (int)'\n' && i < ASIZE-2)	akey[1][i++] = c;
		akey[1][i] = '\0';
		if (i < 2 || i == ASIZE-2) break;

		if (tw2_rev_word_insert (rs, akey)) cnt++;
	}
	return cnt;
}

///����(revision) �ܾ� �������� ����
int tw2_rev_word_delete (BTREE* rs)
{
	char akey1[ASIZE], *pkey1;
	int	 c, cnt=0;
	register int i;
	bool deleted;

	while (1) {
		printf ("Word1: ");
		i = 0;
		while ((c = getchar()) != (int)'\n' && i < ASIZE-2)
			akey1[i++] = c;
		akey1[i] = '\0';

		if (i < 2 || i == ASIZE-2) break;
		pkey1 = str_lower (akey1);	//�ҹ��ڷ� ��ȯ
		pkey1 = str_trim (pkey1);	//�յ� whitespace �߶�
		if (! *pkey1) break;

		//�������� �̵��Ͽ� ����
		//deleted = false;
		rs->root = bpt_delete (rs, pkey1, &deleted);
		if (deleted) {
			printf ("** Deleted.\n");
			cnt++;
		}
	} //while
	return cnt;
}

//����(revision) �ܾ� �Է�(����)
int tw2_rev_word_import (BTREE* rs) 
{
	char fname[ASIZE];
	if (! fio_getchar_fname ("Import(txt) FileName: ", fname)) return 0;
	return fio_import_revision (fname, rs);	//���尳�� ��ȯ
}

//����(revision) �ܾ� ���(����)
void tw2_rev_word_export (BTREE* rs) 
{
	char fname[ASIZE];
	if (! fio_getchar_fname ("Export(txt) FileName: ", fname)) return;
	fio_export_revision (fname, rs);
}

///���� ����(������ ������ȣ�� �Էµ�, ������ȣ ��ȯ ����)
unsigned int _tw2_word_update_run (BTREE* ws, BTREE* wi, char *in_word, unsigned int kno)
{
	NODE5	*leaf_ps, *leaf_pi;
	char*	pword;
	unsigned int *pno;  //�Է¿� �Ϸù�ȣ(��������)
	int		err=0;

	if (ws->kno != wi->kno)	{	//�ε��� ������ȣ �ٸ�(�������� �߻��ϸ� �ʵ�)
		printf ("## Failure to index serial number.(ws:%u, wi:%u)\n", ws->kno, wi->kno);	
		return UIFAIL;
	}

	//������ �� ���� �ٽ� �˻�
	if (bpt_find_leaf_key (ws, in_word, &leaf_ps) >= 0) {
		printf ("## Exist same word.\n");
		return UIFAIL;
	}

	//������ ������ Ű�� ���ٸ� �޸��Ҵ��� ����
	pword = malloc (str_len(in_word) + 1);
	if (!pword) {
		printf ("## Failure to allocate memory.\n");
		err++;  //�޸� �Ҵ� ����
	}
	str_copy (pword, in_word);
	pno = malloc (sizeof(unsigned int));
	if (!pno) {
		printf ("## Failure to allocate memory.\n");
		err++;  //�޸� �Ҵ� ����
	}
	*pno = kno;

	if (bpt_find_leaf_key (wi, pno, &leaf_pi) >= 0) {
		//������ȣ ���� (�������� �߻��ϸ� �ʵ�)
		printf ("## Exist same number key.(s:%s, i:%u)\n", in_word, *pno);
		err++;
	}

	if (err) {
		free (pword);
		free (pno);
		return UIFAIL;
	}

	//����Ű �Է�
	ws->root = bpt_insert (ws, leaf_ps, pword, pno, FLAG_UPDATE);
	//����Ű �Է�
	wi->root = bpt_insert (wi, leaf_pi, pno, pword, FLAG_UPDATE);

	return *pno;
}

///�ܾ� ����(������ �Է�)
//����, ���� �ܾ�� �����Ǹ� �������� ����
int tw2_word_update (BTREE* ws, BTREE* wi, char* prompt)
{
	char akey1[ASIZE], akey2[ASIZE];
	char *pkey1, *pkey2;
	int	 c, cnt=0;
	register int i;
	unsigned int kno;
	NODE5 *leaf;
	bool deleted;

	while (1) {
		printf ("%s Word: ", prompt);
		i = 0;
		while ((c = getchar()) != (int)'\n' && i < ASIZE-2)
			akey1[i++] = c;
		akey1[i] = '\0';

		if (i < 2 || i == ASIZE-2) break;
		pkey1 = str_lower (akey1);	//�ҹ��ڷ� ��ȯ
		pkey1 = str_trim (pkey1);	//�յ� whitespace �߶�
		if (! *pkey1) break;

		printf ("%s To: ", prompt);
		i = 0;
		while ((c = getchar()) != (int)'\n' && i < ASIZE-2)
			akey2[i++] = c;
		akey2[i] = '\0';

		if (i < 2 || i == ASIZE-2) break;
		pkey2 = str_lower (akey2);	//�ҹ��ڷ� ��ȯ
		pkey2 = str_trim (pkey2);	//�յ� whitespace �߶�
		if (! *pkey2) break;

		if (bpt_find_leaf_key (ws, pkey2, &leaf) >= 0) {
			printf ("*# Exist same word.\n");	//�� ������ �߻��ϸ� ����Ű�� �����ؾ� ��(����� �׳� ����)
			break;
		}

		//����(������ �Է�)
		kno = tw1_drop_word_run (ws, wi, pkey1, &deleted);
		if (deleted) {
			//����(�Է�)
			if (_tw2_word_update_run (ws, wi, pkey2, kno) != UIFAIL) {
				printf ("** Updated.\n");
				cnt++;
			}
		} else printf ("** Not exist.\n");

		printf ("\n");
	} //while

	return cnt;
}

//���Ͽ��� ������ ����� �Է�
int tw2_import_from_file (BTREE** ws, BTREE** wi, BTREE** hb[], int mode, BTREE* rs, QUEUE** qk, int flag)
{
	char fname[ASIZE];
	int c;
	double	msec1, msec2;

	if (! fio_getchar_fname ("Import(txt) FileName: ", fname)) return 0;

	//�̸� �� ����(����)
	msec1 = time_get_msec ();

	if (flag == FLAG_CAP)
		c = fio_import_caption (fname, ws, wi, hb, mode, rs, qk);
	else
		c = fio_import (fname, ws, wi, hb, mode, rs, qk);

	//�̸� �� ����(����)
	msec2 = time_get_msec ();
	//����ð�
	printf ("** Run Time: %.3f Secs\n\n", msec2 - msec1);

	return c;
}

//������� ���Ͽ� ���(k=0: �ܾ�, k=2: ����)
void tw2_export_to_file (BTREE** hb[], BTREE** wi, int mode, int sh)
{
	char fname[ASIZE], *pfname, buf[ASIZE];
	register int i = 0, h;
	double	msec1, msec2;

	if (!(i = fio_getchar_fname ("Export(txt) FileName: ", fname))) return;

	//�̸� �� ����(����)
	msec1 = time_get_msec ();

	if (sh == 0) {
		//ĸ�� ���� ���
		fio_export (fname, hb[mode][sh], wi[mode], wi[!mode]);
	} else {
		fname[i] = '_';
		pfname = fname;
		for (h = 1; h < HASHSIZE; h++) {		
			fname[i+1] = '\0';
			str_cat (pfname, uint_to_str (h, buf));	//���ϸ� ���� �ؽù�ȣ �߰�
			fio_export (pfname, hb[mode][h], wi[mode], wi[!mode]);
		}
	}
	//�̸� �� ����(����)
	msec2 = time_get_msec ();
	//����ð�
	printf ("** Run Time: %.3f Secs\n\n", msec2 - msec1);
}

//���Ͼ��� ����� ����
void tw2_file_translation (BTREE** ws, BTREE** wi, BTREE** hb[], int mode, BTREE* rs, QUEUE** qk, int flag)
{
	char fname[ASIZE], fname2[ASIZE];
	int cnt;
	double	msec1, msec2;

	if (! fio_getchar_fname ("Input (txt) FileName: ", fname)) return;
	if (! fio_getchar_fname ("Output(txt) FileName: ", fname2)) return;

	//�̸� �� ����(����)
	msec1 = time_get_msec ();

	cnt = fio_translation (fname, fname2, ws, wi, hb, mode, rs, qk, flag);	//������ ���尳�� ��ȯ

	//�̸� �� ����(����)
	msec2 = time_get_msec ();
	//����ð�
	printf ("** %d Run Time: %.3f Secs\n\n", cnt, msec2 - msec1);
}

//�ؽð��� �ϳ� �Է¹���
int tw2_getchar_hash_value (void)
{
	char value[ASIZE];
	register int i=0;
	int c;

	printf ("Input (0 <= h < %d): ", HASHSIZE);
	while ((c = getchar()) != (int)'\n' && i < ASIZE-2)
		value[i++] = c;
	value[i] = '\0';

	return (i==0) ? -1 : str_to_uint (value);
}

//����Ű ��ȣȭ
int _tw2_getchar_encode_key (char* msg, char* akey, int flag)
{
	register int i=0, j;
	const int length = 60;	//Ű����
	const int codec[] = { 0, 1, 0, 2, 5, 2, 0, 3, 3, 0, 7 }; //11��
	int c, k, asize = sizeof(codec) / sizeof(codec[0]);

	if (flag) {
		printf ("%s", msg);
		while ((c = getchar()) != (int)'\n' && i < length-2)
			akey[i++] = c;
		akey[i] = '\0';
	} else i = str_len (akey);

	//��ȣȭ
	for (j=0; j < i; j++) {
		k = j + asize;
		akey[j] -= (k * (codec[j%asize])) % 33;
		akey[j] += 20;
	}
	str_reverse (akey);
	akey[i] = 19;	//Ű�� ��

	//�������ڷ� Ű����(length)���� ä��
	time_rand_seed_init ();	
	for (j = i+1; j < length; j++)
		akey[j] = time_random_between (33, 126);	//��°����� �ƽ�Ű����
	akey[j] = '\0';
	
	return i;	//�Է¹��� Ű�� ����
}

//����Ű ��ȣȭ
void _tw2_get_decode_key (char* skey, char* out_key, char* out_key_time, int cnt, int flag)
{
	char akey[ASIZE], *pkey;
	register int i, j;
	const int length = 60;	//Ű����
	const int codec[] = { 0, 1, 0, 2, 5, 2, 0, 3, 3, 0, 7 }; //11��
	int k, asize = sizeof(codec) / sizeof(codec[0]);
	char *str[] = {"key  ", "phone", "email", "time "};

	while (cnt--) {
		pkey = akey;
		i = 0;
		while (*skey != 19 && i++ < ASIZE-2) *pkey++ = *skey++;
		*pkey = '\0';

		//��ȣȭ
		str_reverse (akey);
		for (i=0; i < (int)str_len (akey); i++) {
			k = i + asize;
			akey[i] += (k * (codec[i%asize])) % 33;
			akey[i] -= 20;
		}
		//akey[i] = '\0';
		if (flag) printf ("%s: %s\n", str[cnt], akey);
		if (cnt==0) str_copy (out_key, akey);
		if (cnt==3) str_copy (out_key_time, akey);

		//Ű����(length)���� ä���� ��������
		for (j = i; j < length; j++) *skey++;
	}
}

//����Ű �Է� (��ȣȭ)
int tw2_member_key_input (char *fname, int flag)
{
	char skey[SSIZE], akey[ASIZE];
	int	 kc;

	skey[0] = '\0';

	//�����Ͻ�, ����ð�(�ʴ���)
	if (flag == FLAG_YES) {
		kc = _tw2_getchar_encode_key ("", uint_to_str (time_get_sec (), akey), FLAG_NONE);
		str_cat (skey, akey);

		//�̸��� �ּ�
		kc = _tw2_getchar_encode_key ("Input email: ", akey, FLAG_INSERT);
		str_cat (skey, akey);

		//��ȭ��ȣ
		kc = _tw2_getchar_encode_key ("Input phone: ", akey, FLAG_INSERT);
		str_cat (skey, akey);

		//����Ű(��ȣ)
		kc = _tw2_getchar_encode_key ("Input   key: ", akey, FLAG_INSERT);
		str_cat (skey, akey);

	} else {
		kc = _tw2_getchar_encode_key ("", uint_to_str (UIFAIL, akey), FLAG_NONE);	//��ȿ�Ⱓ ���
		str_cat (skey, akey);
	}	

	//0: ����Ű �Է� ����
	if (kc) {
		kc = fio_write_member_key (fname, skey);
		if (kc && flag == FLAG_YES) printf ("Private key have written.\n");
	}
	return kc;
}

//����Ű ��� (��ȣȭ)
void tw2_member_key_output (char *fname)
{
	char skey[SSIZE], akey[ASIZE], akey_time[ASIZE];

	if (fio_read_member_key (fname, skey)) 
		_tw2_get_decode_key (skey, akey, akey_time, 4, FLAG_VIEW);	//skey ��ȣȭ, ȭ�鿡 ���
}

//������ ��й�ȣ�� �Է¹���
bool tw2_getchar_manager_pw (void)
{
	char cpw[ASIZE];
	register int i=0;
	int c;

	printf ("Input password: ");
	while ((c = getchar()) != (int)'\n' && i < ASIZE-2) 
		cpw[i++] = c;
	cpw[i] = '\0';
	if (i==0) return false;

	if (str_cmp (MANAGER, cpw)) return false;

	//ȭ�� Ŭ����(�Էµ� ��ȣ�� �ʺ��̵���)
	#ifdef __LINUX
		system ("clear");	
	#else
		system ("cls");
	#endif

	return true;
}

//����ȸ�� ����Ű �б�(��)
int tw2_getchar_member_pw (char *fname)
{
	char skey[SSIZE];
	char akey1[ASIZE], akey2[ASIZE], akey_time[ASIZE];
	register int i=0;
	unsigned int utime;
	int c, dtime;

	if (! fio_read_member_key (fname, skey)) return -1;
	//skey ��ȣȭ
	_tw2_get_decode_key (skey, akey1, akey_time, 4, FLAG_NONE);	//ȭ�鿡 ��¾���

	utime = (unsigned int)time_get_sec ();
	dtime = (utime - str_to_uint (akey_time)) / 3600 / 24;	//�ʸ� �Ϸ������ ȯ��
	printf ("* ����Ű ���� ��ȿ��: %d��/%d��\n", PKEY_DDAY - dtime, PKEY_DDAY);
	
	if (dtime > PKEY_DDAY) {
		tw2_member_key_input (FNAME_KEY, FLAG_NO);
		printf ("* ����Ű ��ȿ�Ⱓ�� �������ϴ�. ��߱� ��û�� �ֽñ� �ٶ��ϴ�.\n\n");
		return -1;
	}	

	printf ("Input password: ");
	while ((c = getchar()) != (int)'\n' && i < ASIZE-2)
		akey2[i++] = c;
	akey2[i] = '\0';
	if (i==0) return 0;

	if (str_cmp (akey1, akey2)) return 0;

	//ȭ�� Ŭ����(�Էµ� ��ȣ�� �ʺ��̵���)
	#ifdef __LINUX
		system ("clear");	
	#else
		system ("cls");
	#endif

	return 1;
}

//CAPTION �� rs Ʈ���� ����
int tw2_insertion_caption (BTREE** ws, BTREE** wi, int mode, BTREE* rs, QUEUE** qk, char rows[][ASIZE])
{
	char keys[2][ASIZE];	//�������� ����Ű �迭
	int saved = 0, idx;

	str_copy (keys[mode], rows[mode]);	//����� ���ڿ� ����1

	//����Ű�� ��ȯ 
	if ((idx = tw1_words_input (_tw1_prompt(mode), keys[mode], ws[mode], wi[mode], rs, qk[mode], FLAG_AUTO)) > 0) {
		str_copy (keys[!mode], rows[!mode]);	//����� ���ڿ� ����2

		//����Ű�� ��ȯ ���� ����
		if ((idx = tw1_words_input (_tw1_prompt(!mode), keys[!mode], ws[!mode], wi[!mode], rs, qk[!mode], FLAG_AUTO)) > 0) {
			//rs Ʈ���� ����
			if (tw2_rev_word_insert (rs, keys)) saved = 1;
		}
	}
	return saved;
}

void tw2_stack_review (void)
{
	unsigned int i=0;
	char *ps;
	NODE2* sb;

	sb = StackTW->bottom;
	if (!sb) {
		printf ("** ������ ������ �����ϴ�.\n");
		return;
	}

	while (true) {
		if (!StackTW->bottom) StackTW->bottom = sb;	//��� �ݺ�

		ps = (char*)stack_bottom (StackTW, FLAG_VIEW);
		printf ("%s\n", ps);

		time_sleep (str_len (ps) / 10);	//������̿� ���� ���

		if (!(++i%8) && tw1_qn_answer ("* Would you like to see more? [Y/n] ", FLAG_YES) == FLAG_NO) break;
	}
	StackTW->bottom = sb;
}

//���� ���� ������ ���ÿ� �Է�
int tw2_stack_push (char* str)
{
	char* sm;

	if (StackTW_Enable) {
		sm = malloc (str_len (str) + 2);
		if (!sm) {
			printf ("## StackTW memory allocation error!\n");
			return -1;
		}
		str_copy (sm, str);
		stack_push_limit (StackTW, sm, STACK_HEIGHT);	//���� ������ ž�� �Է�
		
		return 1;
	}
	return 0;
}

//���ÿ� ����� ������� ���� ����
void tw2_stack_pop (void)
{
	if (StackTW_Enable) 
		stack_pop_top (StackTW, FLAG_DELETE);
}
