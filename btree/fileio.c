//
//  Source: fileio.c written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2010, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2010-12-21 ���� ����� �Լ����� �ۼ��ϴ�.
//		2011-01-07 ���� ����� �Լ����� �����Ͽ� �������� �����쿡�� ȣȯ�ǵ��� �ϴ�.
//		2011-01-08 �������� �� �б�� Ű������� ����ǹǷ� B+Tree�� �Է��ϴ� �ӵ��� �����ϴ�.
//		2011-02-11 ������ �ܾ��� ������ȣ ť�� ���� ������ϴ� �Լ��� �߰��ϴ�.
//		2011-02-22 ������ ���� import/export �Լ� �߰��ϴ�.
//		2011-02-22 ���Ͼ��� ���� ��������� �߰��ϴ�.(fio_translation: �����Է� --> ���� --> �������)
//		2011-02-25 ���� ����� ���� ���ڿ�(TWVersion)�� �߰��Ͽ� �����Ϳ� ���α׷��� ������ üũ�Ѵ�.
//		2011-03-01 ����ȸ�� ����� ����Ű�� ��ȣȭ/��ȣȭ�Ͽ� ���� ������ϴ� ��� �߰��ϴ�.
//		yyyy-mm-dd ...
//

#include <stdio.h>
#include <stdlib.h>

#include "dtype.h"
#include "umac.h"
#include "ustr.h"
#include "bpt3.h"
#include "queue.h"
#include "tw1.h"
#include "tw2.h"
#include "utime.h"

#ifdef __LINUX
	#include <alloca.h>		//alloca() �Լ�
	#include <sys/stat.h>	//file, directory ���� �Լ�
	#include <sys/types.h>
#else
	#include <malloc.h>		//alloca() �Լ�
	#include <direct.h>		//file, directory ���� �Լ�
#endif


//Win32����, warning C4996: deprecated�� ����... �޼��� ���Ÿ� ����
//������Ʈ�Ӽ�/�����Ӽ�/C,C++/��ó���⿡ _CRT_SECURE_NO_DEPRECATE �߰�


//���� ��� �б�
int _fio_read_header (FILE *fp, char *fname, char *buf)
{
	char *sbuf = buf;

	//������ fgets �Լ����� ����(LF,CR) �д� ����� �޶� �Ʒ� �ڵ�� ��ü
	//������ ���ϸ� �б�
	while ( (*buf = fgetc (fp)) != CR) buf++;
	*buf = '\0';
	if (str_cmp (sbuf, fname)) return -1;	//���ϸ��� �ٸ����

	//���� �б�
	buf = sbuf;
	while ( (*buf = fgetc (fp)) != CR) buf++;
	*buf = '\0';

	//fgetc (fp);	//EOF(-1)

	//HASHSIZE �б�(����Ű ������ ȣȯ ����)
	buf = sbuf;
	while ( (*buf = fgetc (fp)) != CR) buf++;
	*buf = '\0';
	if (str_cmp (sbuf, HashSizeStr)) {	//HASHSIZE�� �ٸ� ���
		printf ("Hash Size (%s != %s), ", sbuf, HashSizeStr);
		return 0;
	}
	return 1;
}

//���� ��� ����
void _fio_write_header (FILE *fp, char *fname)
{
	char* sbuf;

	//������ fputs �Լ����� ����(LF,CR) ���� ����� �޶� �Ʒ� �ڵ�� ��ü
	//���ϸ� ����
	while (*fname) fputc (*fname++, fp);
	fputc (CR, fp);		//CR

	//���� ����
	sbuf = TWVersion;
	while (*sbuf) fputc (*sbuf++, fp);
	fputc (CR, fp);		//CR

	//fputc (EOF, fp);	//EOF(-1)

	//HASHSIZE ����(����Ű ������ ȣȯ)
	sbuf = HashSizeStr;
	while (*sbuf) fputc (*sbuf++, fp);
	fputc (CR, fp);		//CR
}

//���Ͽ��� ���������� �о B+Ʈ���� �Ҵ� (����Ű)
unsigned int fio_read_from_file (char *fname, BTREE* btree)
{
	FILE	*fp;
	NODE5	*leaf, *leaf_left;
	register int i, j=0;
	int		 ch, ihalf, imod;
	unsigned int sno=0;
	unsigned int *pno, kno=0, *pnoa[ASIZE];	//Ű������ �迭�� B_ORDER�� ���� �����ǳ�, �ܾ� ���̷� �˳��ϰ�...
	char	in_key[ASIZE], in_word[ASIZE];	//�ܾ����
	char	*pword, *pworda[ASIZE];
	char	prompt[] = {'+', '-', '*'};

	printf ("Reading data from the %s...\n", fname);
	fp = fopen (fname, "r");
	if (fp == NULL) {
		printf (", Not exist!\n");
		return 0;
	}	
	if (_fio_read_header (fp, fname, in_word) < 0) {
		printf (", Header error!\n");
		return 0;
	}
	//������� ��Ʈ��(�迭)�� split ��ġ �ε���
	//B_ORDER�� ¦���϶�, �������ҵǴ� ������ ��������
	ihalf = _bpt_half_order (btree->order); 

	while ((ch = fgetc (fp)) != EOF) {  //-1
		i = 0;
		while (ch != '\0' && i < ASIZE-2) {
			in_key[i++] = ch;
			ch = fgetc (fp);
		}
		in_key[i] = '\0';

		ch = fgetc (fp);
		i = 0;
		while (ch != '\0' && i < ASIZE-2) {
			in_word[i++] = ch;
			ch = fgetc (fp);
		}
		in_word[i] = '\0';

		//���ڿ��� unsigned int�� ��ȯ
		kno = str_to_uint (in_key);
		pno = malloc (sizeof(unsigned int));
		if (!pno) {
			printf ("## Failure to allocate pno in fio_read_from_file().\n");
			break;  //�޸� �Ҵ� ����
		}
		*pno = kno; //Ű������ȣ

		pword = malloc (str_len (in_word) + 1);
		if (!pword) {
			printf ("## Failure to allocate pword in fio_read_from_file().\n");
			break;  //�޸� �Ҵ� ����
		}
		str_copy (pword, in_word);
		//printf ("%u:%s ", *pno, pword);		
		
		//����Ű ������� ����Ǳ� ������ �Է¼ӵ� ����������, ����� 50%�� ä������ ���� ����
		//leaf ��尡 ���������� ����� ������� ���ԵǹǷ� �ٽ� �˻��� �ʿ� ����(�ӵ� ����)
		//������� ã�� �ʿ� ����
		//leaf = bpt_find_leaf (btree, pno, btree->compare);
		//btree->root = bpt_insert (btree, leaf, pno, pword);
		//btree->root = bpt_insert_asc (btree, &leaf, pno, pword);

		//���� ����� ��Ʈ���� 100% ä��� ���ؼ� �Ʒ� �ڵ�� ���� (��, B_ORDER�� ¦������ ��)
		imod = (int)(sno % (btree->order-1));
		if (imod < ihalf) {	//�迭�� ����
			btree->root = bpt_insert_asc (btree, &leaf, pno, pword);
			if (sno == 0) leaf_left = btree->root;

			printf ("%c(%c)%u ", CR, prompt[sno%3], sno);  //�б�ǥ�� ������Ʈ

		} else {	//���͸� ������ ����
			pnoa[j] = pno;
			pworda[j++] = pword;
		}
		if (imod == (ihalf-1) && j > 0) {	//������ ���͸��� ���� ������ ä�� (���� ��尡 100% ä����)
			for (i = 0; i < j; i++)
				btree->root = bpt_insert_asc (btree, &leaf_left, pnoa[i], pworda[i]);
			leaf_left = leaf_left->pointers [btree->order - 1];
			j = 0;
		}
		sno++;	//����		
	} //while

	//������ ���͸��� �ִٸ� �ǿ����� ������ ����
	for (i = 0; i < j; i++) {
		btree->root = bpt_insert (btree, leaf, pnoa[i], pworda[i], FLAG_INSERT);
		if (leaf->pointers [btree->order -1]) leaf = leaf->pointers [btree->order -1];	//���������� �ٽ� �߻� �ߴٸ� 
	}

	fclose (fp);

	printf ("%c(%c)%u ", CR, prompt[0], sno);  //�б�ǥ�� ������Ʈ
	printf ("have read data: count (%u)\n", btree->kcnt);

	return sno;  //���� Ű ����
}

//���Ͽ��� ����Ű�� ������� �о ������� B+Ʈ���� �Ҵ� (����� ��Ʈ�� 100% ä����)
unsigned int fio_read_trans_asc (char *fname, BTREE* btree, int flag)
{
	FILE	*fp;
	NODE5	*leaf, *leaf_left;
	register int i, j=0;
	int		 ch, ihalf, imod;
	unsigned int sno = 0;
	char	in_key[SSIZE], in_word[SSIZE];	//�������� �ε��� ����
	char	*pkey, *pkeya[ASIZE], *pword, *pworda[ASIZE];
	char	prompt[] = {'+', '-', '*'};

	printf ("Reading data from the %s...\n", fname);
	fp = fopen (fname, "r");
	if (fp == NULL) {
		printf (", Not exist!\n");
		return 0;
	}	
	if ((ch = _fio_read_header (fp, fname, in_word)) <= 0) {
		if (ch < 0) {
			printf (", Header error!\n");
			return 0;
		}
		if (flag) return 0;	//ch==0: HASHSIZE �ٸ�
	}
	//������� ��Ʈ��(�迭)�� split ��ġ �ε���
	//B_ORDER�� ¦���϶�, �������ҵǴ� ������ ��������
	ihalf = _bpt_half_order (btree->order); 

	while ((ch = fgetc (fp)) != EOF) { //-1, 0x1A, ^Z
		i = 0;
		while (ch != '\0' && i < SSIZE-2) {
			in_key[i++] = ch;
			ch = fgetc (fp);
		}
		in_key[i] = '\0';

		ch = fgetc (fp);
		i = 0;
		while (ch != '\0' && i < SSIZE-2) {
			in_word[i++] = ch;
			ch = fgetc (fp);
		}
		in_word[i] = '\0';

		pkey = malloc (str_len (in_key) + 1);
		if (!pkey) {
			printf ("## Failure to allocate pkey in fio_read_trans_asc().\n");
			break;  //�޸� �Ҵ� ����
		}
		str_copy (pkey, in_key);

		pword = malloc (str_len (in_word) + 1);
		if (!pword) {
			printf ("## Failure to allocate pword in fio_read_trans_asc().\n");
			break;  //�޸� �Ҵ� ����
		}
		str_copy (pword, in_word);

		//����Ű ������� ����Ǳ� ������ �Է¼ӵ� ����������, ����� 50%�� ä������ ���� ����
		//leaf ��尡 ���������� ����� ������� ���ԵǹǷ� �ٽ� �˻��� �ʿ� ����(�ӵ� ����)
		//������� ã�� �ʿ� ����
		//leaf = bpt_find_leaf (btree, pkey, btree->compare);
		//btree->root = bpt_insert (btree, leaf, pkey, pword);
		//btree->root = bpt_insert_asc (btree, &leaf, pkey, pword);

		//���� ����� ��Ʈ���� 100% ä��� ���ؼ� �Ʒ� �ڵ�� ���� (��, B_ORDER�� ¦������ ��)
		imod = (int)(sno % (btree->order-1));
		if (imod < ihalf) {	//�迭�� ����
			btree->root = bpt_insert_asc (btree, &leaf, pkey, pword);
			if (sno == 0) leaf_left = btree->root;
			
			printf ("%c(%c)%u ", CR, prompt[sno%3], sno);  //�б�ǥ�� ������Ʈ

		} else {	//���͸� ������ ����
			pkeya[j] = pkey;
			pworda[j++] = pword;
		}
		if (imod == (ihalf-1) && j > 0) {	//������ ���͸��� ���� ������ ä�� (���� ��尡 100% ä����)
			for (i = 0; i < j; i++)
				btree->root = bpt_insert_asc (btree, &leaf_left, pkeya[i], pworda[i]);
			leaf_left = leaf_left->pointers [btree->order - 1];
			j = 0;
		}
		sno++;	//����
	} //while

	//������ ���͸��� �ִٸ� �ǿ����� ������ ����
	for (i = 0; i < j; i++) {
		btree->root = bpt_insert (btree, leaf, pkeya[i], pworda[i], FLAG_INSERT);
		if (leaf->pointers [btree->order -1]) leaf = leaf->pointers [btree->order -1];	//���������� �ٽ� �߻� �ߴٸ� 
	}

	fclose (fp);

	printf ("%c(%c)%u ", CR, prompt[0], sno);  //�б�ǥ�� ������Ʈ
	printf ("have read data: count (%u)\n", btree->kcnt);

	return sno;  //���� Ű ����
}

//HASHSIZE�� ����� ��� �ؽð����� B+Ʈ���� ����(����� ��Ʈ�� 100% ä������ ����)
unsigned int fio_read_trans_hash (char *fname, BTREE** hb[], int sh)
{
	FILE	*fp;
	NODE5	*leaf;
	register int i;
	int		 ch;
	unsigned int sno=0, h;
	char	in_key[SSIZE], in_word[SSIZE];	//�������� �ε��� ����
	char	*pkey, *pword;
	char	prompt[] = {'+', '-', '*'};

	printf ("Reading data(hash) from the %s...\n", fname);
	fp = fopen (fname, "r");
	if (fp == NULL) {
		printf (", Not exist!\n");
		return 0;
	}	
	if (_fio_read_header (fp, fname, in_word) < 0) {
		printf (", Header error!\n");
		return 0;
	}
	while ((ch = fgetc (fp)) != EOF) { //-1
		i = 0;
		while (ch != '\0' && i < SSIZE-2) {
			in_key[i++] = ch;
			ch = fgetc (fp);
		}
		in_key[i] = '\0';

		ch = fgetc (fp);
		i = 0;
		while (ch != '\0' && i < SSIZE-2) {
			in_word[i++] = ch;
			ch = fgetc (fp);
		}
		in_word[i] = '\0';

		pkey = malloc (str_len (in_key) + 1);
		if (!pkey) {
			printf ("## Failure to allocate pkey in fio_read_trans_asc().\n");
			break;  //�޸� �Ҵ� ����
		}
		str_copy (pkey, in_key);

		pword = malloc (str_len (in_word) + 1);
		if (!pword) {
			printf ("## Failure to allocate pword in fio_read_trans_asc().\n");
			break;  //�޸� �Ҵ� ����
		}
		str_copy (pword, in_word);

		//�ؽð��� ���� B+Ʈ�� A ����ã���� ����
		h = (sh==0) ? sh : hash_value (pkey);
		leaf = bpt_find_leaf (hb[0][h], pkey, hb[0][h]->compare);
		hb[0][h]->root = bpt_insert (hb[0][h], leaf, pkey, pword, FLAG_INSERT);

		//�ؽð��� ���� B+Ʈ�� B ����ã���� ����
		h = (sh==0) ? sh : hash_value (pword);
		leaf = bpt_find_leaf (hb[1][h], pword, hb[1][h]->compare);
		hb[1][h]->root = bpt_insert (hb[1][h], leaf, pword, pkey, FLAG_INSERT);
		
		printf ("%c(%c)%u ", CR, prompt[sno%3], sno);  //�б�ǥ�� ������Ʈ
		sno++;	//����
	} //while

	fclose (fp);

	printf ("%c(%c)%u ", CR, prompt[0], sno);  //�б�ǥ�� ������Ʈ
	printf ("have read data(hash): count (%u)\n", sno);

	return sno;  //���� Ű ����
}

//B+Ʈ���� ���������� ���Ͽ� ����
unsigned int fio_write_to_file (char *fname, BTREE* btree)
{
	register int i, j, height;
	unsigned int keys_cnt = 0;
	NODE5*	node = btree->root;
	FILE	*fp;
	char	akey[ASIZE];
	char	prompt[] = {'+', '-', '*'};

	fp = fopen (fname, "w");
	if (fp == NULL) {
		printf ("File(%s) creating error!\n", fname);
		return 0;
	}	
	_fio_write_header (fp, fname);  //���� ��� ����

	printf ("Writing data to the %s...\n", fname);
	if (!node) {
		printf (", Empty.\n");
		fclose (fp);
		return 0;
	}

	//ù��° ���� ��� ã��
	height = 1;
	while (!node->is_leaf) {
		node = (NODE5*)node->pointers[0];
		height++;  //Ʈ�� ����
	}
	while (btree->kcnt > 0) {		
		keys_cnt += node->num_keys;  //������ �ִ� Ű���� �� ����
		for (i = 0; i < node->num_keys; i++) {			
			//btree->outkey (node->keys[i]);
			uint_to_str (*(unsigned int*)node->keys[i], akey);  //����Ű�� ���ڿ�Ű�� ��ȯ
			j = 0;
			while (fputc (akey[j++], fp));  //�������� �� �����
			//btree->outdata (node->pointers[i]);
			j = 0;
			while (fputc (*((char*)node->pointers[i] + j++), fp));  //�������� �� �����
		}
		printf ("%c(%c)%u ", CR, prompt[keys_cnt%3], keys_cnt);  //����ǥ�� ������Ʈ

		if (node->pointers[btree->order - 1])
			node = (NODE5*)node->pointers[btree->order - 1];	//���� ���� ���
		else break;		
	} //while

	fclose (fp);

	printf ("%c(%c)%u ", CR, prompt[0], keys_cnt);  //����ǥ�� ������Ʈ
	printf ("have written data: height (%d), count (%u)\n", height, btree->kcnt);

	return keys_cnt;
}

//B+Ʈ���� ���������� ���Ͽ� ����
unsigned int fio_write_to_file_trans (char *fname, BTREE* btree)
{
	register int i, j, height;
	unsigned int keys_cnt = 0;
	NODE5*	node = btree->root;
	FILE	*fp;
	char	prompt[] = {'+', '-', '*'};

	fp = fopen (fname, "w");
	if (fp == NULL) {
		printf ("File(%s) creating error!\n", fname);
		return 0;
	}	
	_fio_write_header (fp, fname);  //���� ��� ����

	printf ("Writing data to the %s...\n", fname);
	if (!node) {
		printf (", Empty.\n");
		fclose (fp);
		return 0;
	}

	//ù��° ���� ��� ã��
	height = 1;
	while (!node->is_leaf) {
		node = (NODE5*)node->pointers[0];
		height++;  //Ʈ�� ����
	}
	while (btree->kcnt > 0) {		
		keys_cnt += node->num_keys;  //������ �ִ� Ű���� �� ����
		for (i = 0; i < node->num_keys; i++) {
			j = 0;
			while (fputc (*((char*)node->keys[i] + j++), fp));  //�������� �� �����
			j = 0;
			while (fputc (*((char*)node->pointers[i] + j++), fp));  //�������� �� �����			
		}
		printf ("%c(%c)%u ", CR, prompt[keys_cnt%3], keys_cnt);  //����ǥ�� ������Ʈ

		if (node->pointers[btree->order - 1])
			node = (NODE5*)node->pointers[btree->order - 1];	//���� ���� ���
		else break;		
	} //while

	fclose (fp);

	printf ("%c(%c)%u ", CR, prompt[0], keys_cnt);  //����ǥ�� ������Ʈ
	printf ("have written data: height (%d), count (%u)\n", height, btree->kcnt);

	return keys_cnt;
}

///������ �ܾ��� ������ȣ�� ���Ͽ��� �о queue�� ����
unsigned int fio_read_from_file_kno (char *fname, QUEUE* queue)
{
	FILE	*fp;
	register int i;
	unsigned int kno, *pno;
	int		ch;
	char	in_key[ASIZE];

	fp = fopen (fname, "r");
	if (fp == NULL) return 0;	//printf (", Not exist!\n");
	printf ("Reading data from the %s...\n", fname);

	if (_fio_read_header (fp, fname, in_key) < 0) {
		printf (", Header error!\n");
		return 0;
	}

	while ((ch = fgetc (fp)) != EOF) {  //-1, 0x1A, ^Z
		i = 0;
		while (ch != '\0' && i < ASIZE-2) {
			in_key[i++] = ch;
			ch = fgetc (fp);
		}
		in_key[i] = '\0';

		//���ڿ��� unsigned int�� ��ȯ
		kno = str_to_uint (in_key);
		//ť�� �Է�
		pno = malloc (sizeof(unsigned int));
		if (pno) {	
			*pno = kno;
			que_enqueue (queue, pno);	//������ ������ȣ�� ť�� ����
		} else return 0;	//�޸� �Ҵ� ����
	} //while

	fclose (fp);
	printf ("have read data: queue (%d)\n", queue->count);

	return queue->count;  //���� Ű ����
}

///queue�� �ִ� ������ �ܾ��� ������ȣ�� ���Ͽ� ����
unsigned int fio_write_to_file_kno (char *fname, QUEUE* queue)
{
	register int i;
	unsigned int *pno, cnt=0;
	FILE	*fp;
	char	akey[ASIZE];

	fp = fopen (fname, "w");
	if (fp == NULL) {
		printf ("File(%s) creating error!\n", fname);
		return 0;
	}	
	_fio_write_header (fp, fname);  //���� ��� ����

	printf ("Writing data to the %s...\n", fname);
	if (queue->count == 0) {
		////printf (", Empty.\n");
		fclose (fp);
		return 0;
	}

	while (!que_is_empty (queue))	{
		if (que_dequeue (queue, (void*)&pno)) {
			uint_to_str (*pno, akey);  //����Ű�� ���ڿ�Ű�� ��ȯ
			i = 0;
			while (fputc (akey[i++], fp));  //�������� �� �����
			free (pno);	//ť�� ������ �޸� ����
			cnt++;
		} else break;
	} //while

	fclose (fp);
	printf ("have written data: queue (%u)\n", cnt);

	return cnt;
}

//���� ������ ����(txt)���� ��������� �о ����(�Է�)
//mode ����, 0:����(�ѱ۹���), 1:�ѱ�(��������)
int fio_import (char *fname, BTREE** ws, BTREE** wi, BTREE** hb[], int mode, BTREE* rs, QUEUE** qk)
{
	FILE	*fp;
	register int i=0;
	char	ch, row[SSIZE], rows[2][SSIZE], *rowp;
	int		idx, rown=0, cnt=0, ie;
	bool	brun;

	printf ("Reading data from the %s...\n", fname);
	fp = fopen (fname, "r");
	if (fp == NULL) {
		printf (", Not exist!\n");
		return 0;
	}
	rows[0][0] = '\0';
	rows[1][0] = '\0';
	//���๮��, ���� ������ ����
	ch = fgetc (fp);
	while (ch != EOF) 
	{
		ie = 0;
		while (ch != '\n' && ch != EOF && i < SSIZE-2) {	//���峡
			row[i++] = ch;
			if (um_end(ch)) ie = 1;	//���峡 ��ȣ(. ? !)�� �ִ��� ����
			ch = fgetc (fp);			
		}
		if (i >= SSIZE-2) break;

		row[i] = '\0';
		rowp = str_trim_left (row);
		if (*rowp == '/' && *(rowp+1) == '/') {	//�ּ�
			ch = fgetc (fp);
			i = 0;
			continue;
		}
		if (ch == '\n')	row[i++] =  ' ';
		ch = fgetc (fp);
		if (ch != EOF && !ie) continue;	//���峡�� �ƴ����� �Ǵ��ϰ� ��� ����

		row[i] = '\0';
		rowp = str_trim (row);
		
		//0:����, 1:�ѱ�, -1:Ư������
		idx = str_is_eng_kor (rowp);
		if (idx >= 0) {
			if (*rowp == '*') str_copy (rows[idx], rowp+1);
			else str_copy (rows[idx], rowp);

			brun = (mode) ? !idx: idx;	//0:����(�ѱ۹���), 1:�ѱ�(��������)
			if (brun && *rows[mode])
				cnt += tw1_insertion_from_file (ws, wi, hb, mode, rs, qk, rows, -1);	//���� ���� �� ����
		}
		printf ("%c*line: %d/%d: ", CR, cnt, ++rown);  //�����
		i = 0;
	}

	fclose (fp);
	printf ("%d rows have saved.\n", cnt);

	return cnt;	//������ ���� ��
}

///���������� ���Ͽ� ���
int _fio_export_data (FILE *fp, BTREE* wi, void* keys)
{
	char	*ckeys, adigit[ASIZE], sbuf[SSIZE];
	void	*data;
	register int i;
	int		cnt = 0;
	unsigned int *pkey;

	//pkey = malloc (sizeof(unsigned int));
	//Win: <malloc.h>
	//Linux: <alloca.h> ���ÿ� �޸𸮸� ������ �Ҵ�, scope�� ����� �ڵ����� �ǹǷ� free�� �ʿ����.
	pkey = alloca (sizeof(unsigned int)); 
	if (!pkey) {
		printf ("## Failure to allocate alloca in _tw1_trans_key_data().\n");
		return 0;  //�޸� �Ҵ� ����
	}

	sbuf[0] = '\0';
	ckeys = (char*)keys;
	while (*ckeys) {
		i = 0;
		while ( (adigit[i++] = *ckeys++) != '_');
		i--;
		adigit[i] = '\0';		

		*pkey = str_to_uint (adigit);
		data = bpt_search (wi, pkey);
		if (data) {
			cnt++;
			str_cat (sbuf, data);
		}
		str_cat (sbuf, " ");
	}
	sbuf[str_len (sbuf) - 1] = '\0';	//������ ���� ����

	//free (pkey);  //alloca ���� ���ÿ� �Ҵ�� �޸𸮴� scope�� ����� �ڵ����� ������
	str_cat (sbuf, ".\n");
	fputs (sbuf, fp);	//���Ͽ� ����

	//���� ���� ������ ���ÿ� �Է�
	if (cnt > 0) tw2_stack_push (sbuf);

	return cnt;
}

//���������� ���ۿ� ����
int _fio_export_buffer (char sbuf[], BTREE* wi, void* keys)
{
	char	*ckeys, adigit[ASIZE];
	void	*data;
	register int i;
	int		cnt = 0;
	unsigned int *pkey;

	//pkey = malloc (sizeof(unsigned int));
	//Win: <malloc.h>
	//Linux: <alloca.h> ���ÿ� �޸𸮸� ������ �Ҵ�, scope�� ����� �ڵ����� �ǹǷ� free�� �ʿ����.
	pkey = alloca (sizeof(unsigned int)); 
	if (!pkey) {
		printf ("## Failure to allocate alloca in _tw1_trans_key_data().\n");
		return 0;  //�޸� �Ҵ� ����
	}
	
	ckeys = (char*)keys;
	while (*ckeys) {
		i = 0;
		while ( (adigit[i++] = *ckeys++) != '_');
		i--;
		adigit[i] = '\0';		

		*pkey = str_to_uint (adigit);
		data = bpt_search (wi, pkey);
		if (data) {
			cnt++;
			str_cat (sbuf, data);
		} else str_cat (sbuf, "~");

		str_cat (sbuf, " ");
	}
	sbuf[str_len (sbuf) - 1] = '\0';	//������ ���� ����

	//free (pkey);  //alloca ���� ���ÿ� �Ҵ�� �޸𸮴� scope�� ����� �ڵ����� ������
	return cnt;
}

///���� �������� ���Ͽ� ���(����)
void fio_export (char* fname, BTREE* pt, BTREE* pi_src, BTREE* pi_trg) 
{
	FILE*	fp;
	register int i;
	unsigned int row = 0;
	NODE5*	node;
	//char	prompt[] = {'+', '-', '*'};

	if (!pt->root) {
		printf("** Empty.\n");
		return;
	}

	fp = fopen (fname, "w");
	if (fp == NULL) {
		printf ("File(%s) creating error!\n", fname);
		return;
	}	
	printf ("Writing data to the %s...\n", fname);

	node = pt->root;
	//ù��° ���� ��� ã��
	while (!node->is_leaf)  //Ʈ�� ���̸�ŭ �ݺ�
		node = (NODE5*)node->pointers[0];

	while (true) {		
		for (i = 0; i < node->num_keys; i++) {
			row++;
			//�ҽ� ����
			_fio_export_data (fp, pi_src, node->keys[i]);
			//���� ����
			_fio_export_data (fp, pi_trg, node->pointers[i]);
			fputc ('\n', fp);
		}
		printf ("%c(line)%u: ", CR, row);  //���� ��

		if (node->pointers[pt->order - 1])
			node = (NODE5*)node->pointers[pt->order - 1];	//���� ���� ���
		else break;		
	} //while	

	fclose (fp);
	printf ("%u rows have written.\n", row, fname);
	printf ("\n");
}

//����(revision) �ܾ ����(txt)���� �Է�(�б�)
int fio_import_revision (char *fname, BTREE* rs)
{
	FILE	*fp;
	char	row[ASIZE], rows[2][ASIZE], *rowp;
	int		length, skip=0;
	int		rown=0, cnt=0, idx, ins_cnt=0;

	printf ("Reading data from the %s...\n", fname);
	fp = fopen (fname, "r");
	if (fp == NULL) {
		printf (", Not exist!\n");
		return 0;
	}

	//�����(���๮�ڱ���) ����, ���н� NULL ��ȯ
	while (rowp = fgets (row, ASIZE, fp)) {		
		skip = 0;
		rowp = str_trim (rowp);	//����(\n)�� ���⼭ �߸�
		length = str_len (rowp) - 1;
		if (*(rowp+length) == '.') *(rowp+length) = '\0'; //�Ǹ����� ��ħǥ ����

		if (length < 0) skip++;
		if (*rowp == '/') skip++;	//�ּ�
			
		if (!skip) {
			idx = cnt % 2;
			str_copy (rows[idx], rowp);
			cnt++;
			if (idx) 
				if (tw2_rev_word_insert (rs, rows)) ins_cnt++;
		}
		printf ("%c(line)%u: ", CR, ++rown);  //���� ��
	}

	fclose (fp);
	printf ("%d have saved.\n", ins_cnt);

	return ins_cnt;	//���尳��
}

//����(revision) �ܾ ����(txt)�� ���(����)
void fio_export_revision (char* fname, BTREE* rs) 
{
	FILE*	fp;
	register int i;
	unsigned int row = 0;
	NODE5*		 node;

	if (!rs->root) {
		printf("** Empty.\n");
		return;
	}

	fp = fopen (fname, "w");
	if (fp == NULL) {
		printf ("File(%s) creating error!\n", fname);
		return;
	}	
	printf ("Writing data to the %s...\n", fname);

	node = rs->root;
	//ù��° ���� ��� ã��
	while (!node->is_leaf)  //Ʈ�� ���̸�ŭ �ݺ�
		node = (NODE5*)node->pointers[0];

	while (1) {		
		for (i = 0; i < node->num_keys; i++) {
			row++;
			//rs->outkey (node->keys[i]);
			fputs ((char*)node->keys[i], fp);
			fputc ('\n', fp);
			//printf (" >> ");
			//rs->outdata (node->pointers[i]);
			fputs ((char*)node->pointers[i], fp);
			fputc ('\n', fp);
			fputc ('\n', fp);
		}
		printf ("%c(line)%u: ", CR, row);

		if (node->pointers[rs->order - 1])
			node = (NODE5*)node->pointers[rs->order - 1];	//���� ���� ���
		else break;
	} //while
	fclose (fp);
	printf ("%u rows have written to the file.(%s)\n", row, fname);
	printf ("\n");
}

//Ű�� �ϳ��� �и��Ͽ� �ܾ� ������ �˻�
int _fio_trans_key_each (FILE *fp, char* keys, BTREE** hb[], BTREE** wi, int mode, int kcnt)
{
	int		i=0, cnt=0, idx;
	unsigned int h, *akey2;
	BTREE	*t1;
	NODE5*	leaf;
	char	akey[ASIZE], *pkey, sbuf[SSIZE];

	akey2 = alloca (sizeof(unsigned int)); //���ÿ� �Ҵ�(�ڵ�������)
	if (!akey2) {
		printf ("## Failure to allocate alloca in _tw1_trans_search_each().\n");
		return -1;  //�޸� �Ҵ� ����
	}

	sbuf[0] = '\0';
	while (i < kcnt) {
		pkey = akey;
		while (*keys != '_') *pkey++ = *keys++;
		*pkey++ = *keys++;
		*pkey = '\0';
		i++;

		//�ؽð����� ����Ű B+Ʈ�� ����
		h = hash_value (akey);
		t1 = hb[mode][h];
		if ((idx = bpt_find_leaf_key (t1, akey, &leaf)) >= 0) {
			if (_fio_export_buffer (sbuf, wi[!mode], leaf->pointers[idx]) > 0) {
				cnt++;
			}
		} else str_cat (sbuf, "-");	//Ű�� ����
		str_cat (sbuf, "/");
	} //while

	str_cat (sbuf, ".\n");
	fputs (sbuf, fp);	//���Ͽ� ����

	return cnt;
}

//���Ͽ� �ִ� ���� ����(���� mode�� ������� ����)
int fio_translation (char *fname, char *fname2, BTREE** ws, BTREE** wi, BTREE** hb[], int mode, BTREE* rs, QUEUE** qk, int flag)
{
	FILE	*fp, *fp2;
	char	rows[SSIZE], keys[SSIZE], *rowp;
	register int i=0;
	int		ch, rown=0, kcnt, trans_cnt=0;
	unsigned int h;
	BTREE*	t1;
	void*	data;

	printf ("File (%s >> %s) Translating...\n", fname, fname2);
	fp = fopen (fname, "r");
	if (fp == NULL) {
		printf (", Input file(%s) not exist!\n", fname);
		return 0;
	}

	fp2 = fopen (fname2, "w");
	if (fp2 == NULL) {
		printf (", Output file(%s) creating error!\n", fname2);
		return 0;
	}

	//���� ������ ����
	ch = fgetc (fp);
	while (ch != EOF) 
	{
		while (ch != '\n' && ch != EOF && i < SSIZE-2) {	//���峡
			rows[i++] = ch;
			if (um_end(ch)) break;	//���峡 ��ȣ(. ? !)
			ch = fgetc (fp);			
		}
		if (i >= SSIZE-2) break;

		rows[i] = '\0';
		rowp = str_trim_left (rows);
		if (*rowp == '/' && *(rowp+1) == '/') {	//�ּ�			
			fputs (rowp, fp2);	//�������
			fputc ('\n', fp2);
			ch = fgetc (fp);
			i = 0;
			continue;
		}

		if (ch == '\n') {
			rows[i++] = ' ';
			//fputc ('\n', fp2);
		}
		ch = fgetc (fp);
		if (ch != EOF && !um_whites(ch)) continue;	//���峡�� �ƴ����� �Ǵ��ϰ� ��� ����

		rows[i] = '\0';
		rowp = str_trim (rows);
		mode = str_is_eng_kor (rowp);	//0:����, 1:�ѱ�, -1:Ư������(���๮�ڵ� Ư�����ڷ� ���)
		if (mode >= 0) {
			fputs (rowp, fp2);	//�������
			fputc ('\n', fp2);

			//������ �ε���Ű�� ��ȯ
			str_copy (keys, rowp);
			if ((kcnt = tw1_words_input ("", keys, ws[mode], wi[mode], rs, qk[mode], FLAG_NONE)) > 0) {
				//printf (">> %s\n", keys);
				//�ؽð����� ����Ű B+Ʈ�� ����
				h = hash_value (keys);
				t1 = hb[mode][h];
				data = bpt_search (t1, keys);	//����Ű				
				if (data) {	
					//���� ���� ������ ���ÿ� �Է�
					tw2_stack_push (rowp);

					///��������(������ġ)�� ���Ͽ� ���
					if (_fio_export_data (fp2, wi[!mode], data) > 0) trans_cnt++;
					else tw2_stack_pop ();	//���ÿ� ����� ������� ���� ����

				} else {
					//������ġŰ�� ������, ����Ű�� �ϳ��� �и��Ͽ� �ܾ������ �˻��Ͽ� ���
					if (_fio_trans_key_each (fp2, keys, hb, wi, mode, kcnt) > 0) trans_cnt++;				
				}
			}
			fputc ('\n', fp2);
		}
		printf ("%c*line: %u: ", CR, ++rown);  //���� ��
		i = 0;

		if (flag==FLAG_NONE && trans_cnt==TRANS_CNT) {	//����
			//���Ͽ� ���
			fputs ("** ������� ������ �����մϴ�.\n", fp2);
			fputs ("** ���� ȸ������ ����Ͻø� ���Ѿ��� �����մϴ�.\n\n", fp2);
			//ȭ�鿡 ���
			printf ("** ��������� %d���� �����մϴ�.\n", TRANS_CNT);
			printf ("** ���� ȸ������ ����Ͻø� ���Ѿ��� �����մϴ�.\n\n");
			break;
		}
	} //while

	fclose (fp2);
	fclose (fp);
	return trans_cnt;	//������ ���� ��
}

//���� ������ �о ���
bool fio_read_help (char *fname)
{
	FILE	*fp;
	char	row[SSIZE], *rowp;

	//printf ("Reading data from the %s...\n", fname);
	fp = fopen (fname, "r");
	if (fp == NULL) {
		//printf (", Not exist!\n");
		return false;
	}

	//�����(���๮�ڱ���) ����, ���н� NULL ��ȯ
	while (rowp = fgets (row, SSIZE, fp)) {
		if (*rowp == '/' && *(rowp+1) == '/') {
			printf ("%s", (rowp+2));
			getchar ();
		} else {
			printf ("%s", rowp);
		}
	}
	
	fclose (fp);
	return true;
}

//��� ����
int fio_mkdir (char *dir)
{
	int iret;

	#ifdef __LINUX
		if ((iret = mkdir (dir, 0755)) >= 0) 
	#else
		if ((iret = mkdir (dir)) >= 0) 
	#endif
			printf ("** %s directory created.\n", dir);
		//else
			//printf ("** %s directory creating failed.\n", dir);
	return iret;
}

//����� �۾���ο� ���ϸ� �Է�(���ϸ� ���� ��ȯ)
int fio_getchar_fname (char* msg, char* fname)
{
	register int i;
	int c;
	char *pstr = fname;

	pstr = (char*)getcwd (NULL, 0);	//Current Directory
	str_replace (pstr, '\\', '/');
	*pstr = a_upper (*pstr);
	printf ("(Path)%s", pstr);

	str_copy (fname, DIR_WORKS);
	printf ("%s\n", fname+1);

	printf (msg);

	i = str_len (fname);
	while ((c = getchar()) != (int)'\n' && i < ASIZE-2)
		fname[i++] = c;
	fname[i] = '\0';

	if (i == ASIZE-2) return 0;

	str_trim (fname);
	return (str_cmp (DIR_WORKS, fname)) ? i : 0;
}

//����ȸ�� ����Ű ���� (��ȣȭ)
int fio_write_member_key (char* fname, char* skey)
{
	FILE	*fp;

	fp = fopen (fname, "w");
	if (fp == NULL) {
		printf ("File(%s) creating error!\n", fname);
		return 0;
	}
	_fio_write_header (fp, fname);  //���� ��� ����

	fputs (skey, fp);

	fclose (fp);
	return 1;
}

//����ȸ�� ����Ű �б� (��ȣȭ)
int fio_read_member_key (char* fname, char* skey)
{
	FILE *fp;
	char buf[ASIZE];

	fp = fopen (fname, "r");
	if (fp == NULL) {
		//printf ("File(%s) reading error!\n", fname);
		return 0;
	}
	if (_fio_read_header (fp, fname, buf) < 0) {
		//printf (", Header error!\n");
		return 0;
	}

	fgets (skey, SSIZE, fp);

	fclose (fp);
	return 1;
}

//���� ������ ����(txt)���� CAPTION ������� �о ����(�Է�)
//mode ����, 0:����(�ѱ۹���), 1:�ѱ�(��������)
int fio_import_caption (char *fname, BTREE** ws, BTREE** wi, BTREE** hb[], int mode, BTREE* rs, QUEUE** qk)
{
	FILE	*fp;
	register int i=0;
	char	ch, row[SSIZE], rows[2][SSIZE], *rowp;
	char	caps[2][ASIZE], capn[ASIZE];
	int		idx, skip=0, ie;
	int		rown=0, cnt=0, cnt_sum=0;
	bool	brun, bcap;

	printf ("Reading data from the %s...\n", fname);
	fp = fopen (fname, "r");
	if (fp == NULL) {
		printf (", Not exist!\n");
		return 0;
	}

	rows[0][0] = '\0';
	rows[1][0] = '\0';

	//���๮��, ���� ������ ����
	ch = fgetc (fp);
	while (ch != EOF) 
	{
		ie = 0;
		while (ch != '\n' && ch != EOF && i < SSIZE-2) {	//���峡
			row[i++] = ch;
			if (um_end(ch)) ie = 1;	//���峡 ��ȣ(. ? !)�� �ִ��� ����
			ch = fgetc (fp);			
		}
		if (i >= SSIZE-2) break;

		row[i] = '\0';
		rowp = str_trim_left (row);
		if (*rowp == '/' && *(rowp+1) == '/') {	//�ּ�
			ch = fgetc (fp);
			i = 0;
			continue;
		}
		if (ch == '\n')	row[i++] =  ' ';
		ch = fgetc (fp);
		if (ch != EOF  && !ie) continue;	//���峡�� �ƴ����� �Ǵ��ϰ� ��� ����

		row[i] = '\0';
		rowp = str_trim (row);

		//0:����, 1:�ѱ�, -1:Ư������
		idx = str_is_eng_kor (rowp);
		if (idx >= 0) {
			//ĸ�� �����ΰ�?
			bcap = (*rowp == '*') ? true : false;

			str_copy (rows[idx], rowp);
			if (bcap) str_copy (caps[idx], rowp+1);

			brun = (mode) ? !idx: idx;	//0:����(�ѱ۹���), 1:�ѱ�(��������)
			if (brun && *rows[mode]) {
				if (bcap) {
					tw2_insertion_caption (ws, wi, mode, rs, qk, caps);	//ĸ�� ���� ����
					idx = str_len (caps[0]) - 1;
					caps[0][idx] = ' ';		//��ħǥ ����
					caps[0][idx+1] = '\0';
					idx = str_len (caps[1]) - 1;
					caps[1][idx] = ' ';		//��ħǥ ����
					caps[1][idx+1] = '\0';

					cnt = 0;

				} else {
					tw1_insertion_from_file (ws, wi, hb, mode, rs, qk, rows, -1);	//���� ���� �� ����

					//ĸ���� ���� �Ӹ��� �ٿ� ����					
					str_copy (row, caps[0]);
					str_cat (row, uint_to_str (cnt, capn));
					str_cat (row, ": ");
					str_cat (row, rows[0]);
					str_copy (rows[0], row);
					
					str_copy (row, caps[0]);
					str_cat (row, uint_to_str (cnt, capn));
					str_cat (row, ": ");
					str_cat (row, rows[1]);
					str_copy (rows[1], row);

					//ĸ�� ������ �ؽð��� 0���� ����.
					cnt += tw1_insertion_from_file (ws, wi, hb, mode, rs, qk, rows, 0);
					cnt_sum++;
				}
			}
		}
		printf ("%c*line: %d/%d: ", CR, cnt_sum, ++rown);  //���� ��
		i = 0;
	} //while

	fclose (fp);
	printf ("%d rows have saved.\n", cnt_sum);

	return cnt_sum;	//������ ���� ��
}
