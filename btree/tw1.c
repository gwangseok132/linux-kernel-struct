//
//  Source: tw1.c written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2010, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2010-12-06 tw1.c, ���� ���� ����� �ڵ��ϴ�.
//		2010-12-11 B+Tree�� ���� Ű�� �� 40��� ���� �� �����Ͽ� ���Ἲ�� �����ϴ�.
//		2010-12-17 ���ڿ�Ű�� ����Ű�� �۾��� �� �ֵ��� B+Tree�� ���� �����ϴ�.
//		2010-12-18 Ű �Է� �� ������ �޸� �������� �ʴ� ���� �����ϴ�.
//		2010-12-21 ���� �����(fileio) �Լ����� �߰��ϴ�.
//		2010-12-25 �������� �ڵ��ϴ�.
//		2011-01-14 ������ ����Ű �����Լ�(_tw1_delete_trans_key) �߰��ϴ�. --> V1.2011.01
//		2011-01-18 ����Ű ���Լ����� �ߺ�Ű�� ����ϴ�.
//		2011-01-26 �ܾ���� ���� ����Ű�� 2���� �й��ϴ�.
//		2011-01-28 ���ں�ȣ(! " , ? : ; `) ó�� ����� �߰��ϴ�.(is_space_mark �Լ�)
//		2011-02-09 �����ܾ� ������� �߰��ϰ� �����Է��� �ϳ��� �Լ�(tw1_words_input)�� �����ϴ�.
//		2011-02-11 �ܾ� ������ ������ȣ ������ ť(QUEUE* qk[])�� �߰��ϴ�.
//		2011-02-11 tw2.c ����� �и��Ͽ� �ڵ� �����ϴ�.
//		yyyy-mm-dd ...
//

#include <stdlib.h>
#include <stdio.h>

#include "dtype.h"
#include "umac.h"
#include "ustr.h"
#include "utime.h"
#include "fileio.h"
#include "bpt3.h"
#include "queue.h"
#include "stack.h"
#include "tw1.h"
#include "tw2.h"

//alloca() �Լ� ���
#ifdef __LINUX
	#include <alloca.h>
	#include "umem.h"
#else
	#include <malloc.h>
#endif


int tw1_compare_int (void* p1, void* p2)
{
	if (*(unsigned int*)p1 <  *(unsigned int*)p2) return -1;
	if (*(unsigned int*)p1 == *(unsigned int*)p2) return 0;
	return 1;
}

int tw1_compare_str (void* p1, void* p2)
{
	return str_cmp ((char *)p1, (char *)p2);
}

//����Ű ���ڿ��� unsigned int�� ��ȯ�� ��
int tw1_compare_str_int (void* p1, void* p2)
{
	return str_cmp_int ((char *)p1, (char *)p2);
}

void tw1_output_int (void* p1)
{
	printf ("%u", *(unsigned int*)p1);
}

void tw1_output_str (void* p1)
{
	printf ("%s", (char*)p1);
}

//����(Yes/No)�� ���� �亯��
int tw1_qn_answer (char *msg, int ans)
{
	register int i=0;
	char c, str[ASIZE];

	printf ("%s", msg);
	while ((c = getchar()) != (int)'\n') str[i++] = a_lower (c);

	if (*str == 'y') ans = FLAG_YES;
	else if (*str == 'n') ans = FLAG_NO;

	printf ("\n");
	return ans;
}

//�ܾ� �Է��� ����Ű ��ȯ
unsigned int _tw1_insert_to_btree (BTREE* ws, BTREE* wi, char *in_word, QUEUE* que_kno)
{
	NODE5	*leaf_ps, *leaf_pi;
	char*	pword;
	unsigned int *pno;  //�Է¿� �Ϸù�ȣ(��������)
	int		idx, err=0, flag;
	bool	is_qk;

	if (! *in_word) return UIFAIL;

	if (ws->kno != wi->kno)	{	//�ε��� ������ȣ �ٸ�(�������� �߻��ϸ� �ʵ�)
		printf ("## Failure to index serial number.(ws:%u, wi:%u)\n", ws->kno, wi->kno);
		return UIFAIL;
	}

	idx = bpt_find_leaf_key (ws, in_word, &leaf_ps);
	if (idx >= 0) return *(unsigned int*)leaf_ps->pointers[idx];	///���� Ű ����

	///������ ������ Ű�� ���ٸ� �޸��Ҵ��� ����
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

	///ť�� ������ ������ȣ�� ������ ������
	is_qk = que_dequeue (que_kno, (void*)&pno);
	if (!is_qk)	{
		*pno = ws->kno;  ///ť�� ���ٸ� ���� ���(���Խ� ������)
		if (UIFAIL == *pno + 1) {
			printf ("## Key serial number(kno) exceed!!\n");
			err++;
		}
	}
	if (bpt_find_leaf_key (wi, pno, &leaf_pi) >= 0) {
		///������ȣ ���� (�������� �߻��ϸ� �ʵ�)
		printf ("## Exist same number key.(s:%s, i:%u)\n", in_word, *pno);
		err++;
	}

	if (err) {
		free (pword);
		free (pno);
		return UIFAIL;
	}

	///ť�� �ִ� ������ȣ�� ����ߴٸ� kno �����ʵ�(FLAG_UPDATE)
	flag = (is_qk) ? FLAG_UPDATE : FLAG_INSERT;

	///����Ű �Է�
	ws->root = bpt_insert (ws, leaf_ps, pword, pno, flag);

	///����Ű �Է�
	wi->root = bpt_insert (wi, leaf_pi, pno, pword, flag);

	return *pno;
}

//������ �Է�
bool _tw1_insert_to_btree_trans (BTREE* pts1, BTREE* pts2, char* keys1, char* keys2)
{
	NODE5 *leaf, *leaf2;
	char  *pw1, *pw2;

	if (bpt_find_leaf_key (pts1, keys1, &leaf) >= 0) {	//ù��° Ű ����
		if (bpt_find_leaf_key (pts2, keys2, &leaf2) >= 0) {	//�ι�° Ű ����
			//printf ("** Exist same sentence.\n");
			return false;	//���� �ߺ� ȸ��
		}
	}

	//pts1�� pts2�� Ű���߿� ���ο� Ű�� �����ؾ� �Ѵٸ�, �޸� �Ҵ��� ����
	pw1 = malloc (str_len (keys1) + 1);
	if (!pw1) {
		printf ("## Failure to allocate memory.\n");
		return false;  //�޸� �Ҵ� ����
	}
	str_copy (pw1, keys1);

	pw2 = malloc (str_len (keys2) + 1);
	if (!pw2) {
		printf ("## Failure to allocate memory.\n");
		return false;  //�޸� �Ҵ� ����
	}
	str_copy (pw2, keys2);

	//������ pts1�� ����
	pts1->root = bpt_insert (pts1, leaf, pw1, pw2, FLAG_INSERT);

	leaf2 = bpt_find_leaf (pts2, pw2, pts2->compare);
	//������ pts2�� ����
	pts2->root = bpt_insert (pts2, leaf2, pw2, pw1, FLAG_INSERT);

	return true;
}

//�ܾ� Ű ����
unsigned int tw1_drop_word_run (BTREE* ws, BTREE* wi, char *key, bool *deleted)
{
	void *ptr1, *ptr2;	//�Ѱ� �޴� ������
	unsigned int pno;
	int idx;
	NODE5 *leaf1, *leaf2;

	*deleted = false;
	if ((idx = bpt_find_leaf_key (ws, key, &leaf1)) >= 0) {
		ptr1 = leaf1->keys[idx];		//������(Ű)
		ptr2 = leaf1->pointers[idx];	//������(������)
		pno = *((unsigned int*)ptr2);

		if ((idx = bpt_find_leaf_key (wi, ptr2, &leaf2)) >= 0) {	//���� ������
			//ws ��Ʈ��(�迭)���� Ű ����
			ws->root = bpt_delete_entry (ws, leaf1, ptr1, ptr2, FLAG_NONE);	//flag:FLAG_NONE(�ߺ�Ű ����)
			//ws->kno--;	//������ȣ�� ť�� ���� �ǹǷ� �������� ����
			ws->kcnt--;		//Ű ���� ����

			//wi ��Ʈ��(�迭)���� Ű ����
			wi->root = bpt_delete_entry (wi, leaf2, ptr2, ptr1, FLAG_NONE);	//flag:FLAG_NONE(�ߺ�Ű ����)
			//wi->kno--;	//������ȣ�� ť�� ���� �ǹǷ� �������� ����
			wi->kcnt--;		//Ű ���� ����

			//����Ű ������ �޸� ����
			free (ptr1);
			//����Ű ������ �޸� ����
			free (ptr2);

			*deleted = true;
			return pno;		//������ Ű ������ȣ

		} else {
			printf ("## Don't find pointer key in the deletion.(%s, %u)\n", key, pno);
			bpt_find_leaf_debug (wi, ptr2, true);
			return UIFAIL;
		}
	}
	return 0;	//UIFAIL;	//������� ����(unsigned int �ִ밪)
}

//���� Ű ����(�ߺ��� Ű ������ bpt_delete_entry���� �����ͷ� ��)
int _tw1_delete_trans_key_run (BTREE* pt1, BTREE* pt2, NODE5* leaf, int k)
{
	void  *ptr1, *ptr2;		//�Ѱ� �޴� ������
	NODE5 *leaf2;	//�Ѱ� �޴� ����
	double msec1, msec2;

	ptr1 = leaf->keys[k];		//������(Ű)
	ptr2 = leaf->pointers[k];	//������(������)

	//�̸� �� ����(����)
	msec1 = time_get_msec ();

	if (bpt_find_leaf_key_trans (pt2, ptr2, ptr1, &leaf2) >= 0) {
		//pt1 ��Ʈ��(�迭)���� Ű ����
		pt1->root = bpt_delete_entry (pt1, leaf, ptr1, ptr2, FLAG_YES);	//flag:FLAG_YES(������ Ű ��, �ߺ�Ű ����)
		pt1->kno--;
		pt1->kcnt--;  //Ű ���� ����

		//pt2 ��Ʈ��(�迭)���� Ű ����
		pt2->root = bpt_delete_entry (pt2, leaf2, ptr2, ptr1, FLAG_YES);	//flag:FLAG_YES(������ Ű ��, �ߺ�Ű ����)
		pt2->kno--;
		pt2->kcnt--;  //Ű ���� ����

		//pt1�� Ű �޸� ����
		free (ptr1);
		//pt2�� Ű �޸� ����
		free (ptr2);

		msec2 = time_get_msec ();
		//����ð�
		printf ("** Deletion Time: %.3f Secs\n\n", msec2 - msec1);

		return 1;

	} else {
		printf ("## Don't find pointer key in the deletion.\n");
		bpt_find_leaf_debug (pt2, ptr2, true);
		return -1;	//UIFAIL
	}
}

///�Է��۾�(���� Ű���� �Է�)
int tw1_insertion (BTREE** ws, BTREE** wi, BTREE** hb[], int mode, BTREE* rs, QUEUE** qk)
{
	char keys[2][SSIZE];	//�������� ����Ű �迭
	NODE5 *leaf;
	BTREE *t1, *t2;		//������ �ε���
	int cnt, isave=0, idx;
	unsigned int h;
	double msec1, msec2;

	do {
		//�̸� �� ����(����)
		msec1 = time_get_msec ();

		if ((cnt = tw1_words_input (_tw1_prompt(mode), keys[mode], ws[mode], wi[mode], rs, qk[mode], FLAG_INSERT)) > 0) {
			//�ؽð����� ����Ű B+Ʈ�� ����
			h = hash_value (keys[mode]);
			t1 = hb[mode][h];

			//�̹� ������ ���� �ִٸ� ���
			if ((idx = bpt_find_leaf_key (t1, keys[mode], &leaf)) >= 0)
				isave = _tw1_trans_key_finder (leaf, idx, keys[mode], t1, hb[!mode], wi[!mode], FLAG_INSERT);	//������ Ű�� �ִٸ� ���

			if ((cnt = tw1_words_input (_tw1_prompt(!mode), keys[!mode], ws[!mode], wi[!mode], rs, qk[!mode], FLAG_INSERT)) > 0) {
				//�̸� �� ����(����)
				msec1 = time_get_msec ();

				h = hash_value (keys[!mode]);
				t2 = hb[!mode][h];

				//������ B+Ʈ���� ����
				if (_tw1_insert_to_btree_trans (t1, t2, keys[mode], keys[!mode])) isave++;
			}
		} //if
		//�̸� �� ����(����)
		msec2 = time_get_msec ();
		//����ð�
		printf ("** Insertion Time: %.3f Secs\n\n", msec2 - msec1);

	} while (cnt > 0);

	return isave;
}

//�Է��۾�(���Ͽ��� �ڵ� �Է�)
//int sh: ������ �ؽð� (�����̸� �ؽð� ������ ����)
int tw1_insertion_from_file (BTREE** ws, BTREE** wi, BTREE** hb[], int mode, BTREE* rs, QUEUE** qk, char rows[2][SSIZE], int sh)
{
	char keys[2][SSIZE];	//�������� ����Ű �迭
	//NODE5 *leaf;
	BTREE *t1, *t2;		//������ �ε���
	int saved = 0, idx;
	unsigned int h;

	str_copy (keys[mode], rows[mode]);	//����� ���ڿ� ����1

	//����Ű�� ��ȯ
	if ((idx = tw1_words_input (_tw1_prompt(mode), keys[mode], ws[mode], wi[mode], rs, qk[mode], FLAG_AUTO)) > 0) {
		//Ű������ �ؽð� �� B+Ʈ�� ����
		if (sh < 0) {
			h = hash_value (keys[mode]);
			t1 = hb[mode][h];
		} else t1 = hb[mode][sh];

		//������ Ű�� �ִ��� ����
		//if ((idx = bpt_find_leaf_key (t1, keys[mode], &leaf)) < 0)	{
			str_copy (keys[!mode], rows[!mode]);	//����� ���ڿ� ����2

			//����Ű�� ��ȯ ���� ����
			if ((idx = tw1_words_input (_tw1_prompt(!mode), keys[!mode], ws[!mode], wi[!mode], rs, qk[!mode], FLAG_AUTO)) > 0) {
				if (sh < 0) {
					h = hash_value (keys[!mode]);
					t2 = hb[!mode][h];
				} else t2 = hb[!mode][sh];
				//������ ����Ű�� ������ (�ߺ�ȸ��)
				if (_tw1_insert_to_btree_trans (t1, t2, keys[mode], keys[!mode])) {
					saved = 1;
					//printf ("%s\n", rows[mode]);
					//printf (">> %s\n", rows[!mode]);
				}
			}
		//}
	}
	return saved;
}

//����Ű���� �˻��Ͽ� �������� ���
int _tw1_trans_key_data (BTREE* wi, void* keys, int kcnt)
{
	char	*ckeys, adigit[ASIZE], buf[SSIZE];
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

	buf[0] = '\0';
	ckeys = (char*)keys;
	while (*ckeys) {
		i = 0;
		while ( (adigit[i++] = *ckeys++) != '_');
		i--;
		if (kcnt-- > 0) continue;	//������ ���� �˻���(caption) ����
		str_cat (buf, " ");	//�ܾ���� ��ĭ �ٿ�

		adigit[i] = '\0';
		*pkey = str_to_uint (adigit);
		data = bpt_search (wi, pkey);
		if (data) {
			cnt++;
			if (cnt == 1) {
				*(char*)data = a_upper (*(char*)data);	//ù�ܾ��� ���۹��ڴ� �빮�ڷ�
				//wi->outdata (data);
				str_cat (buf, data);
				*(char*)data = a_lower (*(char*)data);	//�ҹ��ڷ� �ٽ� �ǵ���
			} else {
				//wi->outdata (data);
				str_cat (buf, data);
			}
		} else str_cat (buf, "~");  //����
	}
	//free (pkey);  //alloca ���� ���ÿ� �Ҵ�� �޸𸮴� scope�� ����� �ڵ����� ������

	if (cnt) {
		str_cat (buf, ".\n");
		printf ("%s", buf);
		//���� ���� ������ ���ÿ� �Է�
		tw2_stack_push (buf);
	}
	return cnt;
}

//_tw1_trans_key_data() �Լ��� ������ ������
int _tw1_trans_key_data_each (BTREE* wi, void* keys)
{
	char	*ckeys, adigit[ASIZE];  //�ܾ����
	void	*data;
	register int i;
	int		cnt = 0;
	unsigned int *pkey;

	pkey = alloca (sizeof(unsigned int));	//���ÿ� �޸� �Ҵ� (�ڵ�������)
	if (!pkey) {
		printf ("## Failure to allocate alloca in _tw1_trans_key_data().\n");
		return 0;  //�޸� �Ҵ� ����
	}

	ckeys = (char*)keys;
	while (*ckeys) {
		i = 0;
		while ( (adigit[i++] = *ckeys++) != '_');
		i--;
		printf (" ");

		adigit[i] = '\0';
		*pkey = str_to_uint (adigit);
		data = bpt_search (wi, pkey);
		if (data) {
			cnt++;
			if (cnt == 1) {
				*(char*)data = a_upper (*(char*)data);	//ù�ܾ��� ���۹��ڴ� �빮�ڷ�
				wi->outdata (data);
				*(char*)data = a_lower (*(char*)data);	//�ҹ��ڷ� �ٽ� �ǵ���
			} else wi->outdata (data);
		} else printf ("~");  //����
	}
	//free (pkey);  //alloca ���� ���ÿ� �Ҵ�� �޸𸮴� scope�� ����� �ڵ����� ������
	return cnt;
}

//������ ����Ű(�����ġ)�� �� ������ ����
int _tw1_trans_key_finder (NODE5* leaf, int k, char *keys, BTREE* pt1, BTREE* hb[], BTREE* pi_trg, int flag)
{
	register int isave=0, cnt=1;
	BTREE* pt2;

	do {
		//����
		if (_tw1_trans_key_data (pi_trg, leaf->pointers[k], 0) == 0) break;

		pt2 = hb[hash_value (leaf->pointers[k])];	//pt�� �ؽð����� �ٽ� ����

		//����Ű �����Ǹ� leaf->num_keys �پ��
		if (flag) {
			if (tw1_qn_answer ("* Would you like to delete this? [y/N] ", FLAG_NO) == FLAG_YES) {
				isave = _tw1_delete_trans_key_run (pt1, pt2, leaf, k);
				cnt -= isave;
			}
		} else {
			isave = _tw1_delete_trans_key_run (pt1, pt2, leaf, k);
			cnt -= isave;
		}
		cnt++;
		//idx = tw1_qn_answer ("\n** Would you like to see more? [Y/n] ", FLAG_YES);  //FLAG_NO(-1)
	} while (isave >= 0 && (k = bpt_find_leaf_key_next (pt1, keys, &leaf, cnt)) >= 0);

	return (isave > 0) ? 1 : 0;
}

//������ Ű(�����ġ) ����
void _tw1_trans_key_equal (NODE5* leaf, char *keys, int k, BTREE* pt1, BTREE* pi_trg)
{
	register int i, idx;

	idx = -1;
	for (i = k; i < leaf->num_keys; i++) {
		idx = pt1->compare (keys, leaf->keys[i]);
		if (idx == 0)	//������ Ű�� �ִٸ� ����
			_tw1_trans_key_data (pi_trg, leaf->pointers[i], 0);
		else break;
	} //for

	//���� ������忡�� ������ Ű�� �ִٸ� ����
	while (idx == 0) {
		if (leaf->pointers [pt1->order - 1]) {
			leaf = leaf->pointers [pt1->order - 1];
			for (i = 0; i < leaf->num_keys; i++) {
				idx = pt1->compare (keys, leaf->keys[i]);
				if (idx == 0) {
					if (i == leaf->num_keys-1)
						idx = tw1_qn_answer ("* Would you like to see more? [Y/n] ", FLAG_YES) - 1;  //FLAG_YES(1), FLAG_NO(-1)
					if (idx) break;
					_tw1_trans_key_data (pi_trg, leaf->pointers[i], 0);	//������ Ű�� �ִٸ� ����
				} else break;
			} //for
		} else idx = -1;
	} //while
}

//�պκ� ��ġ Ű�� �ִٸ� ����(range scan)
int _tw1_trans_key_like (char *keys, BTREE* pt, BTREE* pi_src, BTREE* pi_trg, int kcnt)
{
	NODE5* leaf;
	register int i;
	int idx=0, ifind=0;

	i = bpt_find_leaf_key_like (pt, keys, &leaf);	//������ Ű�� �������� �ѹ� �� ã��
	if (i < 0) return 0;

	do {
		//�������� ��ȯ
		if (_tw1_trans_key_data (pi_src, leaf->keys[i], kcnt)) {
			_tw1_trans_key_data (pi_trg, leaf->pointers[i], kcnt);	//����
			printf ("\n");
			ifind++;
		}
	} while ( (++i < leaf->num_keys) &&
		      (idx = str_cmp_int_like (keys, leaf->keys[i])) == 0 );	//Ű������ ��(���� �պκ� ��ġ)

	//���� ������忡�� ������ Ű�� �ִٸ� ����
	while (idx == 0) {
		if (leaf->pointers [pt->order - 1]) {
			leaf = leaf->pointers [pt->order - 1];
			for (i = 0; i < leaf->num_keys; i++) {
				idx = str_cmp_int_like (keys, leaf->keys[i]);
				if (idx == 0) {
					//�������� ��ȯ
					if (_tw1_trans_key_data (pi_src, leaf->keys[i], kcnt)) {
						_tw1_trans_key_data (pi_trg, leaf->pointers[i], kcnt);	//����
						ifind++;
						printf ("\n");

						if (!(ifind%pt->order))
							idx = tw1_qn_answer ("* Would you like to see more? [Y/n] ", FLAG_YES) - 1;
						if (idx) break;
					}
				} else break;
			} //for
		} else idx = -1;
	} //while

	//return ifind;
	return (idx == -2) ? idx : ifind;	//-2: ����� ����
}

//������ ����Ű �˻�(������� ��ü�˻�: full scan)
int _tw1_trans_key_similar (char *keys, BTREE* pt, BTREE* pi_src, BTREE* pi_trg)
{
	NODE5* leaf;
	register int i;
	int idx=0, ifind=0;

	i = bpt_find_leaf_key_similar (pt, keys, &leaf);	//������ Ű�� �������� �ѹ� �� ã��
	if (i < 0) return 0;

	do {
		printf ("\n\n");
		_tw1_trans_key_data (pi_src, leaf->keys[i], 0);		//������ ����
		_tw1_trans_key_data (pi_trg, leaf->pointers[i], 0);	//����
		ifind++;
	} while ( (++i < leaf->num_keys) &&
		      (idx = str_cmp_int_similar (keys, leaf->keys[i])) == 0 );	//Ű������ ��

	//���� ������忡�� ������ Ű�� �ִٸ� ����
	while (idx == 0) {
		if (leaf->pointers [pt->order - 1]) {
			leaf = leaf->pointers [pt->order - 1];
			for (i = 0; i < leaf->num_keys; i++) {
				idx = str_cmp_int_similar (keys, leaf->keys[i]);
				if (idx == 0) {
					if (i == leaf->num_keys-1)
						idx = tw1_qn_answer ("* Would you like to see more? [Y/n] ", FLAG_YES) - 1;  //FLAG_YES(1), FLAG_NO(-1)
					if (idx) break;

					printf ("\n");
					_tw1_trans_key_data (pi_src, leaf->keys[i], 0);		//������ ����
					_tw1_trans_key_data (pi_trg, leaf->pointers[i], 0);	//����
					ifind++;
				} else break;
			} //for
		} else idx = -1;
	} //while

	//return ifind;
	return (idx == -2) ? idx : ifind;	//-2: ����� ����
}

//Ű�� �ϳ��� �и��Ͽ� �ܾ� ������ �˻�
int _tw1_trans_key_each (char* keys, BTREE** hb[], BTREE** wi, int mode, int kcnt)
{
	int		i=0, cnt=0, idx;
	unsigned int h, *akey2;
	BTREE	*t1;
	NODE5*	leaf;
	char	akey[ASIZE], *pkey;

	akey2 = alloca (sizeof(unsigned int)); //���ÿ� �Ҵ�(�ڵ�������)
	if (!akey2) {
		printf ("## Failure to allocate alloca in _tw1_trans_search_each().\n");
		return -1;  //�޸� �Ҵ� ����
	}

	printf (" *");
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
			_tw1_trans_key_data_each (wi[!mode], leaf->pointers[idx]);
			cnt++;
		} else printf (" -");	//Ű�� ����
		printf ("/");
	} //while
	printf ("\n\n");
	return cnt;
}

//������ ���� �˻�, keys �ε��� --> hb(������ �ε���) --> wi(���ڿ�)
//������ġ ����Ű�� ������ HASHSIZE ��ŭ B+Ʈ�� Ž���� �ݺ��ϹǷ� �ӵ� ���� ����
int _tw1_trans_search (char* keys, BTREE** hb[], BTREE** wi, BTREE* rs, int mode, int kcnt)
{
	register int i;
	unsigned int h;
	int		idx=0, cnt=0, cnt2=0, cnt3=0;
	BTREE	*t1, *t2;
	NODE5*	leaf;
	char*	cap;

	if (kcnt <= 0) return -1;

	//�ؽð����� ����Ű B+Ʈ�� ����
	h = hash_value (keys);
	t1 = hb[mode][h];

	if ((idx = bpt_find_leaf_key (t1, keys, &leaf)) >= 0) {
		//���� ��ġŰ�� �ִٸ� ����
		_tw1_trans_key_equal (leaf, keys, idx, t1, wi[!mode]);
		printf ("\n");
		cnt = 1;
	}

	//������ġ ������ ���ٸ�, Ű�� �ϳ��� �и��Ͽ� �ܾ� ������ �˻�
	if (cnt==0 && kcnt > 1)
		_tw1_trans_key_each (keys, hb, wi, mode, kcnt);


	//CAPTION �˻�(�պκ� ĸ�� ��ġŰ like% �˻�)
	cap = bpt_search (rs, keys);
	if (cap) {	//ĸ���� �ִٸ�
		//rs->outdata (cap);
		_tw1_trans_key_data (wi[!mode], cap, 0);
		printf ("\n");

		h = 0;	//ĸ�� ������ �ؽð��� 0���� ����.
		t2 = hb[mode][h];
		idx = _tw1_trans_key_like (keys, t2, wi[mode], wi[!mode], kcnt+2);
		cnt += (idx < 0) ? 0 : idx;	//�˻�����
	}

	//�˻������ ���ٸ�, �պκ� ��ġŰ(like%)�� �˻��ɶ� ���� �ؽ����̺��� B+Ʈ�� ��� Ž��
	if (cnt==0) {
		for (i = 1; i < HASHSIZE; i++) {
			t2 = hb[mode][i];
			idx = _tw1_trans_key_like (keys, t2, wi[mode], wi[!mode], 0);
			cnt2 += (idx < 0) ? 0 : idx;	//�˻�����
			if (idx < 0) break;	//����� ����
			if ((cnt3 < cnt2/10) && tw1_qn_answer ("* Would you like to see more? [Y/n] ", FLAG_YES) == FLAG_NO) break;
			cnt3 = cnt2 / 10;
		}
	}

	//�˻������ ���ٸ�, Ű�� �߰��� ���ԵǾ� �ִ� ���� �˻� (������� ��ü ��ĵ)
	if (!cap && !cnt2 && kcnt < SIMILAR) {
		for (i = 1; i < HASHSIZE; i++) {
			t2 = hb[mode][i];
			idx = _tw1_trans_key_similar (keys, t2, wi[mode], wi[!mode]);
			cnt += (idx < 0) ? 0 : idx;	//�˻�����
			if (idx) break;	//�˻��� Ȥ�� ���������
		}
	}
	printf ("\n");
	return cnt + cnt2;
}

//���� (���ڿ�Ű, ����Ű, ��������Ű(target, source))
void tw1_translation (BTREE** ws, BTREE** hb[], BTREE** wi, int mode, char *prompt, BTREE* rs, QUEUE** qk)
{
	char	keys[SSIZE];
	int		kcnt;	//����Ű ���հ���
	double	msec1, msec2;

	if (!ws[mode]->root) {
		printf("** Empty.\n");
		return;
	}
	while (true) {
		//���ڸ� �Է¹޾Ƽ� �˻��� �ε����� keys�� ����
		kcnt = tw1_words_input (prompt, keys, ws[mode], wi[mode], rs, qk[mode], FLAG_TRANS);
		if (kcnt < 0) break;	//�Է¹��ڰ� ������(�׳� ����)

		//�̸� �� ����(����)
		msec1 = time_get_msec ();

		if (_tw1_trans_search (keys, hb, wi, rs, mode, kcnt) <= 0) {
			printf ("** This words does not exist.\n");
			//���ÿ� ����� ������� ���� ����
			tw2_stack_pop ();
		}

		//�̸� �� ����(����)
		msec2 = time_get_msec ();
		//����ð�
		printf ("** Translation Time: %.3f Secs\n\n", msec2 - msec1);
	}
}

//������ ����Ű ����
int _tw1_delete_trans_key (char* keys, int cnt, BTREE** hb[], BTREE** wi, int mode)
{
	int		isave=0, idx;
	unsigned int h;
	NODE5	*leaf;
	BTREE	*t1;

	//�ؽð����� ����Ű B+Ʈ�� ����
	h = hash_value (keys);
	t1 = hb[mode][h];

	//������ Ű�� �ִٸ� ����
	if ((idx = bpt_find_leaf_key (t1, keys, &leaf)) >= 0)
		isave = _tw1_trans_key_finder (leaf, idx, keys, t1, hb[!mode], wi[!mode], FLAG_DELETE);

	return isave;
}

///������ ����Ű ����
int tw1_deletion (BTREE** ws, BTREE** hb[], BTREE** wi, int mode, char* prompt, BTREE* rs, QUEUE** qk)
{
	char keys[SSIZE];
	int  cnt, isave=0;

	if (!ws[mode]->root) {
		printf("** Empty.\n");
		return 0;
	}

	//���ڸ� �Է¹޾Ƽ� �˻��� �ε����� keys�� ����
	cnt = tw1_words_input (prompt, keys, ws[mode], wi[mode], rs, qk[mode], FLAG_DELETE);

	//������ �ε��� ����
	if (cnt > 0) {
		isave = _tw1_delete_trans_key (keys, cnt, hb, wi, mode);
		if (! isave) printf ("** Not Exist!\n");
	}
	return isave;
}

//�ܾ���� (�ܾ������ ���� �ϳ� ���� ������, �ʿ���� �ܾ��϶� ���)
//�ܾ� �����ϰ� ������ȣ�� ť�� ����(���� ����)
bool tw1_drop_word (BTREE* ws, BTREE* wi, QUEUE* queue)
{
	char *pstr, words[SSIZE];  //�������
	char aword[ASIZE];  //�ܾ����
	register int i, c;
	unsigned int kno, *pno;
	bool deleted = false;

	if (!ws->root) {
		printf("** Empty.\n");
		return false;
	}

	i = 0;
	words[0] = '\0';
	while ((c = getchar()) != (int)'\n'  && i < SSIZE-2)
		words[i++] = c;

	if (i > 0) {
		if (words[i-1] == '.') words[i-1] = '\0'; //�Ǹ����� ��ħǥ ����
		else words[i] = '\0';
	}
	pstr = words;
	while (um_whites (*pstr)) pstr++;  //�պκ� whitespace ����

	while (*pstr) {  //���� ����(��) ��ŭ �ܾ� ������ �ݺ�
		i = 0;
		while (!is_whitespace (*pstr) && *pstr && i < ASIZE-2)
			aword[i++] = *pstr++;
		aword[i] = '\0';

		//������ �ܾ��� ������ȣ�� ���� �޾Ƽ� ť�� ����(���� �ϱ� ����)
		kno = tw1_drop_word_run (ws, wi, aword, &deleted);
		if (deleted) {
			pno = malloc (sizeof(unsigned int));
			if (pno) {
				*pno = kno;
				que_enqueue (queue, pno);	//������ ������ȣ�� ť�� ����
			} else return false;	//�޸� �Ҵ� ����
		}
		while (um_whites (*pstr) ) pstr++;  //whitespace ����
	}
	return deleted;
}

//������忡 �ִ� Ű��(����_����)�� �˻��Ͽ� ���
void tw1_display (BTREE* hb[], BTREE* pi_src, BTREE* pi_trg)
{
	register int i, h;
	unsigned int cnt=0, sum=0;
	NODE5*		 node;

	for (h = 1; h < HASHSIZE; h++)
	{
		if (! hb[h]->root) continue;

		node = hb[h]->root;
		sum += hb[h]->kcnt;

		//ù��° ���� ���� �̵�
		while (!node->is_leaf)
			node = (NODE5*)node->pointers[0];	//Ʈ�� ���̸�ŭ �ݺ�

		while (true) {
			for (i = 0; i < node->num_keys; i++) {
				printf ("%u:", ++cnt);
				//pt->outkey (node->keys[i]);
				_tw1_trans_key_data (pi_src, node->keys[i], 0);  //�������� �˻�
				printf (" >>");
				//pt->outdata (node->pointers[i]);
				_tw1_trans_key_data (pi_trg, node->pointers[i], 0);  //�������� ����(�˻�)

				if (!(cnt % (node->num_keys*2))) {
					printf ("\n* %u/%u(%d/%d) ", cnt, sum, h, HASHSIZE-1);
					if (tw1_qn_answer ("Would you like to see more? [Y/n] ", FLAG_YES) == FLAG_NO) {
						h = HASHSIZE;
						break;
					}
				} //if
			} //for
			if (h == HASHSIZE) break; //while exit

			if (node->pointers[hb[h]->order - 1])
				node = (NODE5*)node->pointers[hb[h]->order - 1];	//���� ���� ���
			else break;
		} //while
	} //for

	if (cnt==0) printf("** Empty.\n");
	printf ("\n");
}

//�ܾ� ����(�����ܾ� ����)
char* tw1_revision (BTREE* rs, char* skey)
{
	void *data;
	data = bpt_search (rs, skey);
	if (data) {
		str_copy (skey, data);
		return skey;
	}
	return NULL;
}

//�ܾ words �迭�� �Է� ����
int _tw1_words_getchar (char* prompt, char words[], BTREE* ws)
{
	int	c;
	register int i, j, length=0;
	char akey[ASIZE], *pkey, *data;

	words[0] = '\0';
	printf (prompt);

	//Ű���忡�� �����Է� ����
	//while ((c = getchar()) != (int)'\n' && length < SSIZE-2) words[length++] = c;
	do {
		c = getchar();		//���Ͱ� �Էµɶ� ���� ���۸�
		if (length && c == '\t') {	//��Ű�� �ܾ� �˻�
			fflush (stdin);			//Ű������� ���
			j = 0;
			for (i = length-1; i >= 0; i--) {
				if (um_whites (words[i])) break;
				akey[j++] = words[i];
			}
			if (j < 2) break;

			akey[j] = '\0';
			str_reverse (akey);
			//printf ("%s\n", akey);
			pkey = str_lower (akey);	//�ҹ��ڷ� ��ȯ
			data = bpt_search_str_unique_like (ws, pkey);
			if (data) {	//�˻��ܾ 1���� ������
				data = (char*)data + str_len (pkey);
				//printf ("%s", (char*)data);		//�˻��� �ܾ��� �ںκ�
				while (*(char*)data) {
                    words[length] = *(char*)data;
                    length++;
                    data++;
                }
			}
			words[length] = '\0';
			printf ("\n");
			printf (prompt);
			printf ("%s", words);

		} else words[length++] = c;

	} while (c != '\n' && length < SSIZE-2);

	//��¡ ���� ����
	if (length >= SSIZE-2) {
		printf ("## This sentence is too long to insert.\n");
		return -1;
	}
	words[length] = '\0';
	if (is_space_mark (words[0])) return 0;

	//���� ���� ������ ���ÿ� �Է�
	tw2_stack_push (words);

	return length;
}

///���ڿ��� �Է¹޾� �ܾ� ������ �ε��� ������ keys �迭�� ����
//flag�� FLAG_INSERT�϶� �ܾ� ����
int tw1_words_input (char* prompt, char keys[], BTREE* ws, BTREE* wi, BTREE* rs, QUEUE* que_kno, int flag)
{
	char words[SSIZE], *pwords, akey[ASIZE], *pkey, *data;
	register int i;
	unsigned int kno;
	int		cnt=0, length;
	STACK*	stack1;	//�ܾ� ������ ����
	char akey2[ASIZE], *mkey;

	if (flag==FLAG_AUTO || flag==FLAG_NONE)		//���Ͽ��� �Է�(FLAG_AUTO:�����Է�, FLAG_NONE:����)
		str_copy (words, keys);
	else
		if (_tw1_words_getchar (prompt, words, ws) <= 0) return -1;	//�ܾ words �迭�� �Է� ����

	pwords = str_trim (words);		//�յ� whitespace ����
	length = str_len (pwords) - 1;
	if (length < 0) return -1;

	if (*(pwords+length) == '.') *(pwords+length) = '\0'; //�Ǹ����� ��ħǥ ����
	pwords = str_lower (pwords);	//�ҹ��ڷ� ��ȯ

	stack1 = stack_create ();		//�ܾ� ������ ���� ����

	i = 0;
	keys[0] = '\0';
	while (*pwords || *akey) {
		if (i == 0) {
			while (!is_space_mark (*pwords) && *pwords && i < ASIZE-2) akey[i++] = *pwords++;
			akey[i] = '\0';
		}
		//�ܾ� ���� ����
		if (i >= ASIZE-2) {
			printf ("## A word is too long to insert.\n");
			break;
		}
		if (*akey) {
			//�ܾ� ����
			pkey = tw1_revision (rs, akey);
			if (pkey) {	//�ܾ� ������ ���� �ִٸ� ���ÿ� ����
				while (*pkey) {
					i = 0;
					while (!is_whitespace (*pkey) && *pkey) akey2[i++] = *pkey++;
					akey2[i] = '\0';
					//���ÿ� �Է�
					mkey = malloc (str_len (akey2) + 1);
					str_copy (mkey, akey2);
					stack_push_top (stack1, mkey);

					while (um_whites (*pkey)) pkey++;  //whitespace ����
				}
				pkey = stack_bottom (stack1, FLAG_VIEW);

			} else pkey = akey;	//���� ����

			while (pkey) {
				if (flag == FLAG_INSERT || flag == FLAG_AUTO) {
					kno = _tw1_insert_to_btree (ws, wi, pkey, que_kno);	//�ܾ������ �߰�
					if (kno != UIFAIL) {
						str_cat (keys, uint_to_str (kno, akey));
						str_cat (keys, "_");
						cnt++;
					} else {
						printf ("## Key index number(kno) error!!\n");
						*pwords = '\0';
						break;
					}
				} else {
					data = bpt_search (ws, pkey);  //�ܾ�������� ����Ű �˻�
					if (data) {
						//btree1->outdata (data);  //����
						str_cat (keys, uint_to_str (*(int*)data, akey));
						str_cat (keys, "_");
						cnt++;
					}
				}
				pkey = stack_bottom (stack1, FLAG_VIEW);
			} //while

			//�޸� �Ҵ�� ������ �ִٸ� ��� ����
			if (stack1->count && stack_drop_from_top (stack1, FLAG_NONE))
				printf ("## Error occured at the stack deletion.\n");
		} //if

		//�����ȣ ó��
		i = 0;
		while (is_smark (*pwords)) akey[i++] = *pwords++;
		akey[i] = '\0';

		while (um_whites (*pwords)) pwords++;  //whitespace ����
	} //while

	//�޸� �Ҵ�� ���� ��� ����
	if (stack_drop_from_top (stack1, FLAG_DELETE))
		printf ("## Error occured at the stack destroying.\n");

	return cnt;	//��ȿ �ܾ� ��
}

//�ܾ �Է¹޾� B+Ʈ���� �����ϰ� ����Ű�� ��ȯ (random �׽�Ʈ���� ���)
int tw1_test_insert (BTREE* ws, BTREE* wi, char* words, char keys[], QUEUE* que_kno)
{
	char akey[ASIZE];
	register int i=0, cnt=0;
	unsigned int kno;

	while (um_whites (*words) ) words++;  //�պκ� whitespace ����
	if (! *words) return 0;

	keys[0] = '\0';
	akey[0] = '\0';
	while (*words || *akey) {
		if (i == 0) {
			while (!is_space_mark (*words) && *words && i < ASIZE-2)
				akey[i++] = *words++;
			akey[i] = '\0';
		}
		if (*akey) {
			//�ܾ� ���� ����
			if (i >= ASIZE-2) {
				printf ("## A word is too long to insert.\n");
				return -1;
			}
			kno = _tw1_insert_to_btree (ws, wi, akey, que_kno);	//�ܾ� ������ �߰�
			if (kno == UIFAIL) {
				printf ("## Key index number(kno) error!!\n");
				return -1;
			}
			//���� ���� ����
			if (str_len (keys) > SSIZE - ASIZE) {
				printf ("## This sentence is too long to insert.\n");
				return -1;
			}
			str_cat (keys, uint_to_str (kno, akey));	//��������Ű
			str_cat (keys, "_");
			cnt++;
		}
		//�����ȣ ó��
		i = 0;
		while (is_smark (*words)) akey[i++] = *words++;
		akey[i] = '\0';

		while (um_whites (*words)) words++;  //whitespace ����
	}
	return cnt;  //����Ű ����
}

//����(������ ���ڿ� �ִ� 10�ڸ�)�� �߻��Ͽ� ���԰� ������ �ݺ�����(loop_max: �ݺ�ȸ��)
//Ű�� 70% ���� ���Եǰ�, 30%�� ������.
//�ݺ��� 100���� ����� �� 100M ����Ʈ�� �޸� �Һ�. (���� ��100����Ʈ)

///�ܾ� �Է� �׽�Ʈ
void _tw1_test_ins_word_random (BTREE* ws, BTREE* wi, unsigned int cnt_loop, QUEUE* queue)
{
	char *in_word;
	unsigned int sno, ino, dno;
	double fi=0.0, fd=0.0;

	time_rand_seed_init ();
	sno = 0;		//����
	ino = ws->kcnt;	//�Էµȼ�
	dno = 0;		//�����ȼ�
	while (sno++ < cnt_loop) {
		in_word = time_get_random_str (1);
		//�ܾ������ �߰�
		if (_tw1_insert_to_btree (ws, wi, in_word, queue) == UIFAIL) break;
		printf ("%c** Inserting... SK:%10u, IK:%10u, SC:%10u, IC:%10u", CR, ws->kno, wi->kno, ws->kcnt, wi->kcnt);
	} //while 2��32�� = 4294967296 = ��42��

	ino = ws->kcnt - ino;
	if (--sno > 0) {
		fi = (double)ino / (double)sno * 100;  //������
		fd = (double)dno / (double)sno * 100;  //������
	}
	printf ("\n** Completed(%u), Inserted(%u: %.2f), Deleted(%u: %.2f)\n\n", sno, ino, fi, dno, fd);
}

///�ܾ� ���� �׽�Ʈ
void _tw1_test_del_word_random (BTREE* ws, BTREE* wi, unsigned int cnt_loop, QUEUE* queue)
{
	char *in_word;
	unsigned int sno, ino, dno, kno;
	unsigned int *pno;
	double fi=0.0, fd=0.0;
	bool deleted = false;

	time_rand_seed_init ();
	sno = 0;		//����
	ino = 0;		//�Էµȼ�
	dno = 0;		//�����ȼ�
	while (sno++ < cnt_loop) {
		in_word = time_get_random_str (1);
		//�ܾ�������� ����
		if ((kno = tw1_drop_word_run (ws, wi, in_word, &deleted)) == UIFAIL) break;
		if (deleted) {
			//printf ("deleted: %s, %u\n", in_word, kno);
			dno++;
			pno = malloc (sizeof(unsigned int));
			if (pno) {
				*pno = kno;
				//������ ������ȣ�� ť�� ����
				que_enqueue (queue, pno);
			} else return;
		}
		printf ("%c** Deleting... SN:%10u, IN:%10u, DN:%10u, KW:%s", CR, sno, ino, dno, in_word);
	} //while 2��32�� = 4294967296 = ��42��

	printf ("\n** Deleted. SK:%10u, IK:%10u, SC:%10u, IC:%10u\n", ws->kno, wi->kno, ws->kcnt, wi->kcnt);
	if (--sno > 0) {
		fi = (double)ino / (double)sno * 100;  //������
		fd = (double)dno / (double)sno * 100;  //������
	}
	printf ("** Completed(%u), Inserted(%u: %.2f), Deleted(%u: %.2f)\n\n", sno, ino, fi, dno, fd);
}

//���� ���� �׽�Ʈ
void tw1_test_random (BTREE* ws[], BTREE* wi[], BTREE** hb[], unsigned int cnt_loop, QUEUE* qk[])
{
	char *in_word;
	unsigned int ucnt, h;
	int  mode = 0, k;
	char keys[2][SSIZE];
	NODE5 *leaf;
	//void *data;
	BTREE *t1, *t2;

	//B+Ʈ�� A,B�� ������ ���԰� ���� (�ܾ� ���� ����, ���� �׽�Ʈ)
	//�ܾ�Ű�� �����Ǹ� ������ ����Ű �����Ͱ� �޶���
	{
		//�ܾ���� A
		_tw1_test_ins_word_random (ws[0], wi[0], cnt_loop, qk[0]);	//�Է� �׽�Ʈ
		///_tw1_test_del_word_random (ws[0], wi[0], cnt_loop, qk[0]);	//���� �׽�Ʈ
		//�ܾ���� B
		///_tw1_test_ins_word_random (ws[1], wi[1], cnt_loop, qk[1]);	//�Է� �׽�Ʈ
		///_tw1_test_del_word_random (ws[1], wi[1], cnt_loop, qk[1]);	//���� �׽�Ʈ
	}

	//B+Ʈ�� A,B�� �Է� (����Ű �Է� �� ���� �׽�Ʈ)
	printf ("\n");
	ucnt = 0;
	while (ucnt++ < cnt_loop)
	{
		in_word = time_get_random_str (ucnt % 10 + 1);
		if (tw1_test_insert (ws[mode], wi[mode], in_word, keys[mode], qk[mode]) > 0) {
			in_word = time_get_random_str (ucnt % 10 + 1);
			if (tw1_test_insert (ws[!mode], wi[!mode], in_word, keys[!mode], qk[!mode]) > 0) {
				//������ B+Ʈ���� �Է�
				printf ("** Trans Key Inserting: %s: %s\n", keys[mode], keys[!mode]);
				h = hash_value (keys[mode]);
				t1 = hb[mode][h];
				h = hash_value (keys[!mode]);
				t2 = hb[!mode][h];
				_tw1_insert_to_btree_trans (t1, t2, keys[mode], keys[!mode]);

				if (!(ucnt%5) && (k = bpt_find_leaf_key (t1, keys[mode], &leaf)) >= 0) {
					printf ("** Trans Key Deleting: %s\n", keys[mode]);
					//������ B+Ʈ������ ����
					_tw1_trans_key_finder (leaf, k, keys[mode], t1, hb[!mode], wi[!mode], FLAG_NONE);
				}
			}
		}
	} //while

	/*
	//���� �׽�Ʈ
	printf ("\n");
	ucnt = 0;
	while (ucnt++ < cnt_loop)
	{
		keys[mode][0] = '\0';
		in_word = time_get_random_str (ucnt % 10 + 1);
		printf ("\n** Translating: %s\n", in_word);
		data = bpt_search (ws[mode], in_word);  //�ܾ�������� ����Ű �˻�
		if (data) {
			//btree1->outdata (data);  //����
			str_cat (keys[mode], uint_to_str (*(int*)data, keys[!mode]));
			str_cat (keys[mode], "_");
		}
		_tw1_trans_search (keys[mode], hb, wi, mode, 1);
	}
	*/
}

char* _tw1_prompt (int mode)
{
	static char *prompt[] = {"English>> ", "�ѱ�>> "};
	return prompt[mode];
}

//������ �޴�
char* _tw1_manager_menu (int mode)
{
	printf ("\n");
	printf ("%s for MANAGER\n", TWVersion);

	if (mode) {
		printf ("A: �ܾ����(i) ����\n");
		printf ("B: �ܾ����(s) ����\n");
		printf ("C: ����Ű ���� ����\n");
		printf ("\n");
		printf ("D: �����ܾ� �Է�\n");
		printf ("E: �����ܾ� ����\n");
		printf ("F: �����ܾ� �Է�(����)\n");
		printf ("G: �����ܾ� ���(����)\n");
		printf ("\n");
		printf ("H: ����ȸ�� ����Ű ���\n");
		printf ("I: ����ȸ�� ����Ű �Է�\n");
		printf ("\n");
		printf ("J: ĸ�ǹ��� �Է�(����)\n");
		printf ("K: ĸ�ǹ��� ���(����)\n");
		printf ("L: ĸ�ǹ��� ��� ����\n");

		printf ("x: ����\n");

		printf ("\n");
		printf ("~: Delete Word\n");
		printf ("!: Delete All TransKey\n");
		printf ("@: Delete All Dic Words\n");
		printf ("#: Delete All Revision\n");

	} else {
		printf ("A: View Words Index(i)\n");
		printf ("B: View Words Index(s)\n");
		printf ("C: View TransKey Index\n");
		printf ("\n");
		printf ("D: Insert Revision Words\n");
		printf ("E: Delete Revision Words\n");
		printf ("F: Import Revision Words(File)\n");
		printf ("G: Export Revision Words(File)\n");
		printf ("\n");
		printf ("H: Member Private Key Output\n");
		printf ("I: Member Private Key Input\n");
		printf ("\n");
		printf ("J: Import Caption(File)\n");
		printf ("K: Export Caption(File)\n");
		printf ("L: Delete All Caption\n");

		printf ("x: Exit\n");

		printf ("\n");
		printf ("~: Delete Word\n");
		printf ("!: Delete All TransKey\n");
		printf ("@: Delete All Dic Words\n");
		printf ("#: Delete All Revision\n");
	}
	printf ("\n");

	return _tw1_prompt (mode);
}

//������ �޴� ����
int tw1_manager (BTREE* ws[], BTREE* wi[], BTREE** hb[], int mode, BTREE* rs, QUEUE* qk[])
{
	char cmd, *prompt;
	int  isave = 0, h;

	prompt = _tw1_manager_menu (mode);
	cmd = 'h';
	while (cmd != 'x')
	{
		printf ("Command> ");
		cmd = a_lower ((char)getchar());
		if (cmd != '\n') while (getchar() != (int)'\n');

		switch (cmd) {
			case 'a':  //�ܾ���� A ��ȸ
				bpt_print (wi[mode]);
				bpt_print_leaves (wi[mode]);
				break;
			case 'b':  //�ܾ���� B ��ȸ
				bpt_print (ws[mode]);
				bpt_print_leaves (ws[mode]);
				break;
			case 'c':  //����Ű ��ȸ
				h = tw2_getchar_hash_value ();
				if (h >= 0) {
					bpt_print (hb[mode][h]);
					bpt_print_leaves (hb[mode][h]);
				}
				break;

			case 'd':	//�����ܾ� �Է�
				if (tw2_rev_word_input (rs) > 0) {
					fio_write_to_file_trans (FNAME_DAT0, rs);
					printf ("** Saved.\n");
				}
				break;
			case 'e':	//�����ܾ� ����
				if (tw2_rev_word_delete (rs) > 0) {
					fio_write_to_file_trans (FNAME_DAT0, rs);
					printf ("** Saved.\n");
				}
				break;
			case 'f':	//�����ܾ� �Է�(����)
				if (tw2_rev_word_import (rs) > 0) {
					fio_write_to_file_trans (FNAME_DAT0, rs);
					printf ("** Saved.\n");
				}
				break;
			case 'g':	//�����ܾ� ���(����)
				tw2_rev_word_export (rs);
				break;

			case 'h':	//����ȸ�� ����Ű(��ȣȭ) ���
				tw2_member_key_output (FNAME_KEY);
				break;
			case 'i':	//����ȸ�� ����Ű(��ȣȭ) �Է�(����)
				tw2_member_key_input (FNAME_KEY, FLAG_YES);
				break;

			case 'j':  //���Ͽ��� CAPTION ����� �Է�(mode ����, 0:����(�ѱ۹���), 1:�ѱ�(��������))
				if (tw2_import_from_file (ws, wi, hb, mode, rs, qk, FLAG_CAP) > 0) {
					fio_write_to_file_trans (FNAME_DAT0, rs);
					printf ("** Saved.\n");
					isave++;
				}
				break;
			case 'k':	//����Ű���� ĸ�� ������� ���Ͽ� ���
				tw2_export_to_file (hb, wi, mode, 0);
				break;
			case 'l':	//����Ű���� ĸ�� ����� ���� (�����ܾ ���� ������)
				bpt_drop (&hb[0][0], &hb[1][0]);
				isave++;
			case '#':	//�����ܾ� ����
				bpt_drop_leaves_nodes (rs, rs->root);
				rs->root = NULL;
				fio_write_to_file_trans (FNAME_DAT0, rs);
				printf ("\n");
				isave++;
				break;

			case '~':  //�ܾ���� (�ܾ������ ���� �ϳ� ���� ������, �ʿ���� �ܾ��϶� ���)
				if (tw1_qn_answer ("������ȣ�� ť�� �����˴ϴ�. �۾����� �Ͻðڽ��ϱ�? [y/N] ", FLAG_NO) == FLAG_YES) {
					printf (prompt);
					if (tw1_drop_word (ws[mode], wi[mode], qk[mode])) {
						printf ("\n** Deleted.\n");
						isave++;
					} else printf ("\n** Not exist.\n");
				}
				break;
			case '!':  //����Ű ��� ����
				for (h=1; h < HASHSIZE; h++)
					bpt_drop (&hb[0][h], &hb[1][h]);
				isave++;
				break;
			case '@':  //�ܾ���� A,B ��� ����
				bpt_drop (&ws[0], &wi[0]);
				bpt_drop (&ws[1], &wi[1]);
				isave++;
				break;

			case 'x': break;	//����
			default:
				prompt = _tw1_manager_menu (mode);

		} //switch

	} //while

	printf("\n");
	return isave;
}

//ȸ�� �޴� (����)
char* _tw1_member_menu (int mode)
{
	printf ("\n");
	printf ("%s for MEMBER\n", TWVersion);

	if (mode) {
		printf ("F: ���� ����\n");
		printf ("I: �������� �Է�(����)\n");
		printf ("E: �������� ���(����)\n");
		printf ("U: �ܾ� ����\n");
		//printf ("D: �������� ��� ����\n");
		printf ("H: ����\n");
		printf ("x: ����\n");
	} else {
		printf ("F: File Translation\n");
		printf ("I: Import (File)\n");
		printf ("E: Export (File)\n");
		printf ("U: Update Word\n");
		//printf ("D: Delete All\n");
		printf ("H: Help\n");
		printf ("x: Exit\n");
	}
	printf ("\n");

	return _tw1_prompt (mode);
}

//ȸ�� �޴�(����) ����
int tw1_member (BTREE* ws[], BTREE* wi[], BTREE** hb[], int mode, BTREE* rs, QUEUE* qk[])
{
	char cmd, *prompt;
	int  isave = 0;
	static bool bManager = false;	//������ �α��� ����

	prompt = _tw1_member_menu (mode);
	cmd = 'h';
	while (cmd != 'x')
	{
		printf ("Command> ");
		cmd = a_lower ((char)getchar());
		if (cmd != '\n') while (getchar() != (int)'\n');

		StackTW_Enable = false;

		switch (cmd) {
			case 'f':	//���Ͼ��� ����� ����(����-�ѿ� ��� ����)
				StackTW_Enable = true;
				tw2_file_translation (ws, wi, hb, mode, rs, qk, FLAG_AUTO);	//ȸ��(���Ѿ��� ����)
				break;
			case 'i':  //���Ͽ��� ������ ����� �Է�(mode ����, 0:����(�ѱ۹���), 1:�ѱ�(��������))
				isave += tw2_import_from_file (ws, wi, hb, mode, rs, qk, FLAG_INSERT);
				break;
			case 'e':	//����Ű���� ����(�ܾ�)���� ���Ͽ� ���
				tw2_export_to_file (hb, wi, mode, -1);
				break;
			case 'u':  //�ܾ� ���� (������ȣ �״�� ����)
				isave += tw2_word_update (ws[mode], wi[mode], prompt);
				break;
			/*
			case 'd':  //�������� ��� ���� (ĸ�� ������ ����)
				if (tw1_qn_answer ("������������ ��� ���ϴ�. �۾����� �Ͻðڽ��ϱ�? [y/N] ", FLAG_NO) == FLAG_YES) {
					for (h=1; h < HASHSIZE; h++)
						bpt_drop (&hb[0][h], &hb[1][h]);	//����Ű ��� ����
					//�ܾ���� A,B ��� ����
					bpt_drop (&ws[0], &wi[0]);
					bpt_drop (&ws[1], &wi[1]);
					isave++;
				}
				break;
			*/

			case 'h':  //����
				if (! fio_read_help (FNAME_DAT4))
					prompt = _tw1_member_menu (mode);
				//printf ("\n");
				break;

			case 'x': break;	//����

			case '@':	//������ �޴��� �̵�
				if (bManager || tw2_getchar_manager_pw ()) {
					isave += tw1_manager (ws, wi, hb, mode, rs, qk);
					bManager = true;
				}
			default:
				prompt = _tw1_member_menu (mode);

		} //switch

	} //while

	printf("\n");
	return isave;
}

void tw1_save (BTREE* ai, BTREE* bi, BTREE* hbta[], BTREE* rs)
{
	register int i;
	char afname[ASIZE], anum[ASIZE];

	//�����ܾ� �� ����ĸ�� ����
	//fio_write_to_file_trans (FNAME_DAT0, rs);

	//�ܾ����(����Ű) ���Ͽ� ����
	fio_write_to_file (FNAME_DIC0, ai);
	fio_write_to_file (FNAME_DIC1, bi);

	//����Ű ���Ͽ� ����
	for (i = 0; i < HASHSIZE; i++) {
		afname[0] = '\0';
		str_cat (afname, FNAME_IDX);	//"twd"
		str_cat (afname, uint_to_str (i, anum));
		str_cat (afname, FNAME_EXT);	//".twi"
		fio_write_to_file_trans (afname, hbta[i]);
	}
}

//��� ���� ���
void tw1_statis (BTREE* ws[], BTREE* wi[], BTREE** hb[], BTREE* rs, QUEUE** qk)
{
	register int i;
	unsigned int sum0=0, sum1=0;	//�հ��̹Ƿ� ������ Ÿ���� Ŀ����.
	unsigned int min_a, min_b, max_a, max_b;

	printf ("%s\n\n", TWVersion);

	printf ("* Dictionay Tree Order  :%6d\n", ws[0]->order);	//B_ORDER
	printf ("* Translation Hash Size :%6d\n", HASHSIZE);		//HASHSIZE
	printf ("* Revision Index Count  :%6d\n", rs->kcnt);		//�ܾ��Ű ����
	printf ("* Words A Queue Count   :%6d\n", qk[0]->count);	//�ܾ���� A ������ ������ȣ ����
	printf ("* Words B Queue Count   :%6d\n", qk[1]->count);	//�ܾ���� A ������ ������ȣ ����

	printf ("\n");
	printf ("* Words A Index(i) Count            :%10u\n", wi[0]->kcnt);
	if (wi[0]->kcnt != ws[0]->kcnt)	//�׻� ���ƾ� ��(�ߺ�Ű ����)
	printf ("# Words A Index(s) Count            :%10u\n", ws[0]->kcnt);

	printf ("* Words A Index(i) Serial           :%10u\n", wi[0]->kno);
	if (wi[0]->kno != ws[0]->kno)	//�׻� ���ƾ� ��(�ߺ�Ű ����)
	printf ("# Words A Index(s) Serial           :%10u\n", ws[0]->kno);

	printf ("* Words B Index(i) Count            :%10u\n", wi[1]->kcnt);
	if (wi[1]->kcnt != ws[1]->kcnt)	//�׻� ���ƾ� ��(�ߺ�Ű ����)
	printf ("# Words B Index(s) Count            :%10u\n", ws[1]->kcnt);

	printf ("* Words B Index(i) Serial           :%10u\n", wi[1]->kno);
	if (wi[1]->kno != ws[1]->kno)	//�׻� ���ƾ� ��(�ߺ�Ű ����)
	printf ("# Words B Index(s) Serial           :%10u\n", ws[1]->kno);

	printf ("\n");
	printf ("* Translation[0] Index(A) Count     :%10u\n", hb[0][0]->kcnt);
	printf ("* Translation[0] Index(B) Count     :%10u\n", hb[1][0]->kcnt);
	printf ("\n");
	min_a = hb[0][1]->kcnt;
	min_b = hb[1][1]->kcnt;
	max_a = hb[0][1]->kcnt;
	max_b = hb[1][1]->kcnt;
	for (i = 1; i < HASHSIZE; i++) {
		min_a = um_min(min_a, hb[0][i]->kcnt);	//�ּ�
		min_b = um_min(min_b, hb[1][i]->kcnt);
		max_a = um_max(max_a, hb[0][i]->kcnt);	//�ִ�
		max_b = um_max(max_b, hb[1][i]->kcnt);
		//����Ű ���� �հ�
		sum0 += hb[0][i]->kcnt;
		sum1 += hb[1][i]->kcnt;
	}
	printf ("* Translation[#] Index(A) Min Count :%10u\n", min_a);
	printf ("* Translation[#] Index(B) Min Count :%10u\n", min_b);
	printf ("* Translation[#] Index(A) Max Count :%10u\n", max_a);
	printf ("* Translation[#] Index(B) Max Count :%10u\n", max_b);
	printf ("\n");
	printf ("* Translation[#] Index(A) All Sum   :%10u\n", sum0);
	if (sum0 != sum1)	//�հ�� �׻� ���ƾ� ��
		printf ("# Translation[#] Index(B) All Sum   :%10u\n", sum1);

	printf ("\n");

	#ifdef __LINUX
		mem_info ();	//�޸� ��� ����
	#endif
}

char* _tw1_menu (int mode, int isave)
{
	printf ("\n");
	printf ("%s for natural language translation\n", TWVersion);
	printf ("  written by Jung,JaeJoon(rgbi3307@nate.com) on the www.kernel.bz.\n");
	if (mode) {
		printf ("**�����̳� ��ȭ(010-2520-3307)�� �� ���α׷��� ���Ͽ� ���� �ϼ���.\n\n");

		printf ("*MENU for �ѱ�-����\n");
		printf ("T: �����ϱ�\n");
		printf ("I: �Է��ϱ�\n");
		printf ("D: �����ϱ�\n");
		printf ("V: ��ȸ�ϱ�\n");
		printf ("F: ���Ϲ���\n");
		printf ("R: ��������\n");
		printf ("C: ������ȯ\n");

		if (isave) printf ("S: �����ϱ�\n");
		printf ("A: ���\n");
		printf ("M: ȸ���޴�\n");
		printf ("H: ����\n");
		printf ("Q: ����\n");

	} else {
		printf ("**Please email or call me(010-2520-3307) to ask about Algorithms.\n\n");

		printf ("*MENU for English-Korean\n");
		printf ("T: Translation\n");
		printf ("I: Insertion\n");
		printf ("D: Deletion\n");
		printf ("V: View\n");
		printf ("F: File Trans\n");
		printf ("R: Review\n");
		printf ("C: Change\n");

		if (isave) printf ("S: Save\n");
		printf ("A: Statistic\n");
		printf ("M: Member\n");
		printf ("H: Help\n");
		printf ("Q: Quit\n");
	}
	printf ("\n");

	return _tw1_prompt (mode);
}

//isave;  //����Ǽ�
void tw1_menu_run (BTREE* ws[], BTREE* wi[], BTREE** hb[], BTREE* rs, QUEUE* qk[], int isave)
{
	char cmd, *prompt;
	int  mode, idx;
	static bool bMember = false;	//����ȸ�� �α��� ����

	mode = 0;   //�޴����(0:English, 1:Korean)
	prompt = _tw1_menu (mode, isave);

	cmd = 'h';
	while (cmd != '!')
	{
		printf ("Command> ");
		cmd = a_lower ((char)getchar());
		if (cmd != '\n') while (getchar() != (int)'\n');

		StackTW_Enable = false;

		switch (cmd) {
			case 't':  //���� ���� --> ���� �۾�
				StackTW_Enable = true;
				tw1_translation (ws, hb, wi, mode, prompt, rs, qk);
				prompt = _tw1_menu (mode, isave);
				break;
			case 'i':  //���� �Է�(�ܾ� �������� �Էµ�) --> ���� �۾�
				isave += tw1_insertion (ws, wi, hb, mode, rs, qk);
				prompt = _tw1_menu (mode, isave);
				break;
			case 'd':  //���� ����
				isave += tw1_deletion (ws, hb, wi, mode, prompt, rs, qk);
				break;
			case 'v':  //���� ����
				tw1_display (hb[mode], wi[mode], wi[!mode]);
				break;
			case 'f':	//���Ͼ��� ����� ����(����-�ѿ� ��� ����)
				StackTW_Enable = true;
				tw2_file_translation (ws, wi, hb, mode, rs, qk, FLAG_NONE);	//��ȸ��(����������� ����)
				break;
			case 'r':	//Review: ��������
				tw2_stack_review ();	//StackTW ���(��������)
				break;

			case 'c':  //������ȯ
				prompt = _tw1_menu (mode = !mode, isave);
				break;
			case 's':  //����
				tw1_save (wi[0], wi[1], hb[0], rs);
				isave = 0;
				break;
			case 'a':  //��� ���
				tw1_statis (ws, wi, hb, rs, qk);
				break;

			case 'h':  //����
				if (! fio_read_help (FNAME_DAT3))
					prompt = _tw1_menu (mode, isave);
				//printf ("\n");
				break;
			case 'q':	//����
				if (isave) tw1_save (wi[0], wi[1], hb[0], rs);	//���� �Ǽ��� ������, �������� ����
				cmd = '!';
			case '!': break;

			case 'm':  //ȸ���޴�(����)
				idx = 1;
				if (bMember || (idx = tw2_getchar_member_pw (FNAME_KEY)) >= 0) {
					if (idx > 0) {
						isave += tw1_member (ws, wi, hb, mode, rs, qk);
						bMember = true;
					} else {
						printf ("** ����Ű ��ȣ�� Ʋ���ϴ�.\n");
						break;
					}
				} else fio_read_help (FNAME_DAT4);

			default:
				prompt = _tw1_menu (mode, isave);

		} //switch

	} //while

	printf("\n");
}
