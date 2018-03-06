//
//  Source: bpt3.c written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(c): 2010, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2010-05-12 B+Tree �ڵ��� �����ϴ�.
//		2010-12-06 ���� �� ������ bpt_find_leaf() �Լ��� 2�� ȣ��Ǵ� ���� 1������ ����.(Ʈ��Ž�� �ߺ�����)
//		2010-12-07 ��� ������ ��й� �Լ� _bpt_redistribute_nodes()�� �ִ� ���׸� �����ϴ�.
//		2010-12-11 ��� ������ ���� �Լ� _bpt_coalesce_nodes()���� ���ʿ��� �ڵ�(split)�� �����ϴ�.
//		2010-12-17 ���ڿ�Ű�� ����Ű�� ���� �۾��� �� �ֵ��� B+Tree �Լ����� �����ϴ�.
//		2010-12-18 ��� ������ �޸� �������� �ʴ� ���� �����ϴ�. _bpt_remove_entry_from_node() ����.
//		2011-01-17 �ߺ�Ű ����� ���� B+Ʈ�� ����� �Լ��� �����ϴ�.
//		yyyy-mm-dd ...
//

#include <stdio.h>
#include <stdlib.h>

#include "dtype.h"
#include "ustr.h"
#include "tw1.h"
#include "bpt3.h"

//alloca() �Լ� ���
#ifdef __LINUX
	#include <alloca.h>	//Linux
#else
	#include <malloc.h>		//Win
#endif

// OUTPUT ---------------------------------------------------------------------------------------

//new_node�� ť(qnode)�� �߰� (bpt_print �Լ����� ���)
NODE5* _bpt_enqueue (NODE5* qnode, NODE5* new_node)
{
	NODE5* node;

	if (qnode == NULL) {
		qnode = new_node;
		qnode->next = NULL;

	} else {
		node = qnode;
		while(node->next != NULL) node = node->next;

		node->next = new_node;
		new_node->next = NULL;
	}
	return qnode;
}

//node�� ť(qnode)���� ������ (bpt_print �Լ����� ���)
NODE5* _bpt_dequeue (NODE5** qnode)
{
	NODE5* node = *qnode;

	*qnode = (*qnode)->next;
	node->next = NULL;
	return node;
}

//B+ Ʈ�� ���, ��带 �����ϱ� ���� '|' ��ȣ ���, Ű�� ���� �����ʹ� 16������ ���
void bpt_print (BTREE* btree)
{
	NODE5* node = NULL;
	NODE5* qnode = NULL;  //��¿� ť ���

	register int i = 0, j = 0;
	int rank = 0;
	int new_rank = 0;

	if (!btree->root) {
		printf("Empty.\n");
		return;
	}

	qnode = _bpt_enqueue (qnode, btree->root);
	while (qnode)
	{
		node = _bpt_dequeue (&qnode);
		if (node->parent && node == node->parent->pointers[0]) {
			new_rank = _bpt_path_to_root (btree->root, node);
			if (new_rank != rank) {
				rank = new_rank;
				printf("\n");
			}
		}
		for (i = 0; i < node->num_keys; i++) {
			btree->outkey (node->keys[i]);
			printf (" ");
			if (!node->is_leaf)
				for (j = 0; j <= node->num_keys; j++)
					qnode = _bpt_enqueue (qnode, node->pointers[j]);
		} //for
		printf("| ");
	} //while
	printf ("\n");
	printf ("Height=%d.\n", _bpt_height (btree->root));
	printf ("\n");
}

//���� ��常 ���
void bpt_print_leaves (BTREE* btree)
{
	register int i, height;
	unsigned int keys_cnt = 0;
	NODE5*		 node = btree->root;

	if (!node) {
		printf("Empty.\n");
		return;
	}

	//ù��° ���� ��� ã��
	height = 1;
	while (!node->is_leaf) {
		node = (NODE5*)node->pointers[0];
		height++;  //Ʈ�� ����
	}

	while (true) {
		keys_cnt += node->num_keys;  //������ �ִ� Ű���� �� ����
		for (i = 0; i < node->num_keys; i++) {
			btree->outkey (node->keys[i]);
			printf (":");
			btree->outdata (node->pointers[i]);
			printf (" ");
		}

		if (!(keys_cnt % 50))
			if (tw1_qn_answer ("\n** Would you like to see more? [Y/n]", FLAG_YES) == FLAG_NO) break;

		if (node->pointers[btree->order - 1]) {
			printf(" | ");
			node = (NODE5*)node->pointers[btree->order - 1];	//���� ���� ���
		} else break;
	} //while

	printf ("\n");
	printf ("Height(%d), Kno(%d), Kcnt(%d==%d)\n", height, btree->kno, keys_cnt, btree->kcnt);
}

//����Ű���� ����Ű B+Ʈ�� ���� (wi --> ws)
//��Ʈ���� 100% ä������ ����
NODE5* bpt_init_key (BTREE* wi, BTREE* ws)
{
	register int i;
	NODE5*  leaf;
	NODE5*	node = wi->root;

	if (!node) {
		printf("Empty.\n");
		return NULL;
	}
	while (!node->is_leaf)
		node = (NODE5*)node->pointers[0];  //����Ű ���� ������ �̵�
	while (true) {
		//������带 ó������ ������ ����
		for (i = 0; i < node->num_keys; i++) {
			leaf = bpt_find_leaf (ws, node->pointers[i], ws->compare);
			ws->root = bpt_insert (ws, leaf, node->pointers[i], node->keys[i], FLAG_INSERT);	//����Ű ����
		}
		if (node->pointers[wi->order - 1])
			node = (NODE5*)node->pointers[wi->order - 1];	//����Ű�� ���� ���� ���
		else break;
	} //while

	return ws->root;
}

//����Ű1 --> ����Ű2 ����
NODE5* bpt_init_trans_key (BTREE* t1, BTREE* hb[], int sh)
{
	register int i;
	NODE5*  leaf;
	NODE5*	node = t1->root;
	unsigned int h;

	if (!node) {
		printf("Empty.\n");
		return NULL;
	}
	while (!node->is_leaf)
		node = (NODE5*)node->pointers[0];  //���� ������ �̵�
	while (true) {
		//������带 ó������ ������ ����
		for (i = 0; i < node->num_keys; i++) {
			h = (sh == 0) ? sh : hash_value (node->pointers[i]); //�ؽð�
			leaf = bpt_find_leaf (hb[h], node->pointers[i], hb[h]->compare);
			hb[h]->root = bpt_insert (hb[h], leaf, node->pointers[i], node->keys[i], FLAG_INSERT);
		}
		if (node->pointers[t1->order - 1])
			node = (NODE5*)node->pointers[t1->order - 1];	//���� ���� ���
		else break;
	} //while

	return t1->root;
}

//Ʈ�� ����: ��Ʈ���� �������� ����
int _bpt_height (NODE5* root)
{
	register int h = 0;

	NODE5*	node = root;

	while (!node->is_leaf) {
		node = node->pointers[0];
		h++;
	}
	return h+1;
}

//�ڽĳ�忡�� ��Ʈ������ ����
int _bpt_path_to_root (NODE5* root, NODE5* child)
{
	register int length = 0;
	NODE5*	node = child;

	while (node != root) {
		node = node->parent;
		length++;
	}
	return length;
}

///������
NODE5* bpt_find_leaf_debug (BTREE* btree, void* key, bool debug)
{
	int		i=0, height=0;
	NODE5*	node = btree->root;

	if (node == NULL) {
		if (debug) printf("Empty tree.\n");
		return node;
	}
	//����������(Ʈ������ ��ŭ)
	while (!node->is_leaf) {
		height++;
		if (debug) {
			printf ("%d: (", height);
			for (i = 0; i < node->num_keys; i++) {	//����� Ű���� ���(debug)
				printf (",");
				btree->outkey (node->keys[i]);
			}
			printf (",) ");
		}
		i = 0;
		while (i < node->num_keys) {  //������ Ű�迭�� ���������� Ž��
			if (btree->compare (key, node->keys[i]) < 0) break;  //(key < node->keys[i])
			else i++;
		}
		if (debug) printf("-> %d th\n", i);

		node = (NODE5 *)node->pointers[i];
	} //while

	if (debug) {
		printf ("%d: [", ++height);  //Leaf
		for (i = 0; i < node->num_keys; i++) {
			printf (",");
			btree->outkey (node->keys[i]);
			if (btree->compare (key, node->keys[i]) == 0) printf ("(*)");  //ã��(node->keys[i] == key)
			//printf (":");
			//btree->outdata (node->pointers[i]);
		}
		printf (" *]\n\n");
	}
	return node;
}

//����Ű(�ߺ�����) �˻�
NODE5* bpt_find_leaf_unique (BTREE* btree, void* key, int (*compare)(void* p1, void* p2))
{
	register int i;
	NODE5*	node = btree->root;

	if (!node) return NULL;

	//����������(Ʈ������ ��ŭ)
	while (!node->is_leaf) {
		i = 0;
		while (i < node->num_keys) {  //������ Ű�迭�� ���������� Ž��
			if (compare (key, node->keys[i]) < 0) break;
			else i++;
		}
		node = (NODE5 *)node->pointers[i];
	} //while
	return node;
}

//��Ʈ���� pointers�� ���󰡸� �������� �̵�
//�ߺ�Ű ��� �ڵ� �߰�(2011-01-18)
NODE5* bpt_find_leaf (BTREE* btree, void* key, int (*compare)(void* p1, void* p2))
{
	register int i, k = 1;
	NODE5	*node;
	bool	in_key = false;	//internal key ����

	node = btree->root;
	if (!node) return NULL;

	//���������� �̵�
	while (!node->is_leaf) {
		i = 0;
		while (i < node->num_keys) {  //������ Ű�迭�� ���������� Ž��
			k = btree->compare (key, node->keys[i]);
			if (btree->samek && k <= 0) break;	//k==0,����Ű
			else if (k < 0) break;
			else i++;
		}
		if (k == 0) in_key = true;	//����Ű
		node = (NODE5 *)node->pointers[i];
	} //while

	//����Ű�� �߻��߰� �ߺ�Ű ����̶��, �������� Ű�� ���� �˻�
	if (in_key && btree->samek) {
		do {
			for (i = 0; i < node->num_keys; i++)
				if (compare (key, node->keys[i]) <= 0) break;
		} while ((i == node->num_keys) && (node = node->pointers [btree->order - 1]));
	}
	return node;  //���� ���
}

//���� ��带 ã�Ƽ� ������ Ű�� �ִ��� �˻�
int bpt_find_leaf_key (BTREE* btree, void* key, NODE5** leaf)
{
	register int i;

	*leaf = bpt_find_leaf (btree, key, btree->compare);
	if (! *leaf) return -2;  //������� ����

	for (i = 0; i < (*leaf)->num_keys; i++)
		if (btree->compare (key, (*leaf)->keys[i]) == 0) break;

	return (i < (*leaf)->num_keys) ? i : -1;
}

//������ Ű�� �־��� ȸ�� ��ŭ �ִ��� �˻�
int bpt_find_leaf_key_next (BTREE* btree, void* key, NODE5** leaf, int count)
{
	register int i;
	int idx=0, cnt=0;

	*leaf = bpt_find_leaf (btree, key, btree->compare);
	if (! *leaf) return -2;  //������� ����

	for (i = 0; i < (*leaf)->num_keys; i++) {
		if (btree->compare (key, (*leaf)->keys[i]) == 0) {
			idx = i;
			cnt++;
		}
		if (cnt == count) break;
	}
	while ((cnt < count) && (idx == i-1) ) {
		*leaf = (*leaf)->pointers [btree->order - 1];  //���� ����
		if (!*leaf) return -1;  //Ű ����
		for (i = 0; i < (*leaf)->num_keys; i++) {
			if (btree->compare (key, (*leaf)->keys[i]) == 0) {
				idx = i;
				cnt++;
			}
			if (cnt == count) break;
		}
	} //while

	return (i < (*leaf)->num_keys) ? i : -1;
}

//����Ű ���� ��带 ã�� (Ű�ߺ��� �����Ƿ� ������ �����ͱ��� ��)
int bpt_find_leaf_key_trans (BTREE* btree, void* key, void* ptr, NODE5** leaf)
{
	register int i;

	*leaf = bpt_find_leaf (btree, key, btree->compare);
	if (! *leaf) return -2;  //������� ����

	for (i = 0; i < (*leaf)->num_keys; i++)
		//if ( (btree->compare ((*leaf)->keys[i], key) == 0) && ((*leaf)->pointers[i] == ptr) ) break;
		if ( ((*leaf)->keys[i] == key) && ((*leaf)->pointers[i] == ptr) ) break;

	return (i < (*leaf)->num_keys) ? i : -1;
}

///���� ��带 ã�Ƽ� key������ ��(key������ ��ġ�ϴ��� ����, �ߺ�Ű ���)
int bpt_find_leaf_key_like (BTREE* btree, void* key, NODE5** leaf)
{
	register int i;
	char ucmax[ASIZE], key2[SSIZE];

	*leaf = bpt_find_leaf (btree, key, str_cmp_int_like);  //�ߺ�Ű ���
	if (! *leaf) return -2;  //������� ����
	for (i = 0; i < (*leaf)->num_keys; i++)
		if (str_cmp_int_like (key, (*leaf)->keys[i]) == 0) break;

	if (i < (*leaf)->num_keys) return i;  //Ű ����(Ű�ε���)

	//key�� ���� �ִ밪�� �ϳ� �߰��Ѵ�.
	//������ �ٽ� ã�´�.
	uint_to_str (UIFAIL, ucmax);	//unsigned int �ִ밪
	str_copy (key2, key);
	str_cat (key2, ucmax);
	str_cat (key2, "_");
	//printf ("** Finding like key2:%s\n", key2);

	*leaf = bpt_find_leaf (btree, key2, str_cmp_int_like);  //�ߺ�Ű ���
	//*leaf = bpt_find_leaf_debug  (btree, key2, true);
	if (! *leaf) return -2;  //������� ����

	for (i = 0; i < (*leaf)->num_keys; i++)
		if (str_cmp_int_like (key, (*leaf)->keys[i]) == 0) break;

	return (i < (*leaf)->num_keys) ? i : -1;
}

///key�� �߰��� ��ġ�ϴ� �� �˻�
//���� ��带 ó������ ������� �˻�(full scan)
int bpt_find_leaf_key_similar (BTREE* btree, void* key, NODE5** leaf)
{
	NODE5*	node = btree->root;
	register int i;
	char	prompt[] = {'+', '-', '*'};
	unsigned int sno = 0;

	//�������� �̵�
	while (!node->is_leaf) node = (NODE5 *)node->pointers[0];
	*leaf = node;
	if (! *leaf) return -2;  //������� ����

	//������� ��ü�˻�(full scan)
	do {
		for (i = 0; i < (*leaf)->num_keys; i++) {
			//btree->outkey ((*leaf)->keys[i]);
			sno++;
			printf ("%c**(%c)%u: %s", CR, prompt[sno%3], sno, "Finding similar words...");
			if (str_cmp_int_similar (key, (*leaf)->keys[i]) == 0 ) break;
		}
	} while ((i == (*leaf)->num_keys) && (*leaf = (*leaf)->pointers [btree->order -1]) );
	//printf ("\n\n");

	if (! *leaf) return -1;
	return (i < (*leaf)->num_keys) ? i : -1;
}

//������ ã�Ƽ� key�� �˻��Ͽ� �ش� record�� ��ȯ�Ѵ�.
void* bpt_search (BTREE* btree, void* key)
{
	register int i = 0;
	NODE5* leaf;

	leaf = bpt_find_leaf (btree, key, btree->compare);
	if (!leaf) return NULL;

	for (i = 0; i < leaf->num_keys; i++)
		if (btree->compare (key, leaf->keys[i]) == 0) break;

	return (i == leaf->num_keys) ? NULL : leaf->pointers[i];
}

///���ڿ� ���� ��ġ�ϴ� �ܾ� ��ȯ (��Ű �˻�)
void* bpt_search_str_unique_like (BTREE* btree, void* key)
{
	register int i = 0;
	NODE5	*leaf, *leaf2;
	int		k, k2, cnt = 0;

	//leaf = bpt_find_leaf_debug (btree, key, true);
	leaf = bpt_find_leaf_unique (btree, key, btree->compare);
	//�������� str_cmp_like�� �˻�
	do {
		for (i = 0; i < leaf->num_keys; i++)
			if (str_cmp_like (key, leaf->keys[i]) == 0) break;
	} while ((i == leaf->num_keys) && (leaf = leaf->pointers [btree->order -1]) );

	if (!leaf) return NULL;

	k = i;
	k2 = i;
	leaf2 = leaf;
	do {
		for (i = k2; i < leaf2->num_keys; i++) {
			if (str_cmp_like (key, leaf2->keys[i]) == 0) {
				btree->outkey (leaf2->keys[i]);	//��ġ �ϴ°��� ������ ��� ���
				printf ("\t");
				cnt++;
			} else break;
		}
		k2 = 0;
	} while ((i == leaf2->num_keys) && (leaf2 = leaf2->pointers [btree->order -1]) );

	return (cnt == 1) ? leaf->keys[k] : NULL;	//key ��ȯ(1���� ������)
}

//Order(������ �迭 ��Ҽ�)�� �߰� ��ġ ���
//((Order + 1) / 2) - 1
int _bpt_half_order (int length)
{
	return (length % 2 == 0) ? length / 2 : length / 2 + 1;
}


// INSERTION --------------------------------------------------------------------------------------
/*
1. ������ ����
2. ���� ����
3. �θ� ����
4. �θ����(3������ �ݺ�)
*/

/*
 1.bpt_insert ()
 2.	_bpt_make_root () //��Ʈ ��� ���� ����
 3.	bpt_find_leaf ()
 4.	_bpt_make_record ()

	//���� ��忡 ����� ��Ʈ���� ������
 5.	_bpt_insert_into_leaf () -->return

	//���� ��忡 ����� ��Ʈ���� ������(���� ������)
 6.	_bpt_insert_into_leaf_after_splitting ()  //��������(���ο� ��� right �Ҵ�)

_7.		_bpt_insert_into_parent ()  //���ҵ� ��带 �θ� ����
 8.			_bpt_insert_into_new_root ()
 9.			_bpt_get_left_index ()  //�θ��忡�� ���� �ڽ��� �ε���

			//�θ� ��忡 ����� ��Ʈ���� ������
10.			_bpt_insert_into_parent_node () --> return

			//�θ� ��忡 ����� ��Ʈ���� ������(���� ������)
11.			_bpt_insert_into_parent_after_splitting ()  //�θ��� �ٽ� ����(���ο� ��� right �Ҵ�)
12.				_bpt_insert_into_parent ()  //7������ �̵�(�ݺ�)
*/

//B+ Ʈ�� ���� ����
BTREE* bpt_create (int order, int (*compare)(void* p1, void* p2)
				   , void (*outkey)(void* p1), void (*outdata)(void* p1), bool samek )
{
	BTREE*  btree;

	btree = (BTREE*) malloc (sizeof(BTREE));
	if (btree) {
		btree->order = order;		//���͸� �迭ũ��
		btree->kno  = 0;			//Ű������ȣ(����)
		btree->kcnt = 0;			//Ű����(����)
		btree->root = NULL;
		btree->compare = compare;	//�� �Լ�������
		btree->outkey = outkey;		//Ű��� �Լ�������
		btree->outdata = outdata;	//��������� �Լ�������
		btree->samek = samek;		//�ߺ�Ű ��뿩��
	}
	return btree;
}

//������忡 Ű�� ������ �Է�
NODE5* bpt_insert (BTREE* btree, NODE5* leaf, void* key, void* data, int flag)
{
	if (flag==FLAG_INSERT)	btree->kno++;	//Ű ������ȣ ����
	btree->kcnt++;  //Ű ���� ����

	//B+ Ʈ���� ���� �������� �ʴ´ٸ� ó������ ����
	if (btree->root == NULL) {
		//���ڵ带 �޸𸮿� �Ҵ��Ѵ�.
		//pointer = _bpt_make_record (btree->record);
		btree->root = _bpt_make_root (btree, key, data);
		return btree->root;
	}

	//���ڵ带 �޸� �Ҵ��Ѵ�.
	//pointer = _bpt_make_record (key, data);

	//������� overflow �ƴ�: ���� ��� �߰��� ����
	if (leaf->num_keys < btree->order - 1) {
		leaf = _bpt_insert_into_leaf (leaf, btree, key, data);
		return btree->root;
	}
	//������� overflow: ������ �и����� �����Ѵ�.
	return _bpt_insert_into_leaf_after_splitting (btree, leaf, key, data);
}

//Ű ������� �ԷµǴ� ��� �Է� �ӵ� ����(leaf ������ �״�� ���)
NODE5* bpt_insert_asc (BTREE* btree, NODE5** leaf, void* key, void* data)
{
	btree->kno++;	//Ű ������ȣ ����
	btree->kcnt++;  //Ű ���� ����

	//B+ Ʈ���� ���� �������� �ʴ´ٸ� ó������ ����
	if (btree->root == NULL) {
		btree->root = _bpt_make_root (btree, key, data);
		*leaf = btree->root;
		return btree->root;
	}

	//������� overflow �ƴ�: ���� �迭 ���� ����
	if ((*leaf)->num_keys < btree->order - 1) {
		*leaf = _bpt_insert_into_leaf_asc (*leaf, btree, key, data);
		return btree->root;
	}
	//������� overflow: ������ �и����� �����Ѵ�.
	btree->root = _bpt_insert_into_leaf_after_splitting (btree, *leaf, key, data);

	//������ �и��� ���, ���� ����(���������� ����� ����) ��ȯ
	*leaf = (*leaf)->pointers[btree->order - 1];

	return btree->root;
}

NODE5* _bpt_make_root (BTREE* btree, void* key, void* data)
{
	NODE5* root = _bpt_make_leaf (btree);	///������� ����(�޸��Ҵ�)

	root->keys[0] = key;
	root->pointers[0] = data;
	root->pointers[btree->order - 1] = NULL;
	root->parent = NULL;
	root->num_keys++;

	return root;
}

///���� ��� ����
NODE5* _bpt_make_leaf (BTREE* btree)
{
	NODE5* leaf = _bpt_make_node (btree);
	leaf->is_leaf = true;
	return leaf;
}

///��� ����(�޸� �Ҵ�)
NODE5* _bpt_make_node (BTREE* btree)
{
	NODE5* new_node;

	new_node = malloc(sizeof(NODE5));
	if (new_node == NULL) {
		printf ("## Node creation error in _bpt_make_node().\n");
		exit(EXIT_FAILURE);
	}
	new_node->keys = malloc ( (btree->order - 1) * sizeof(void*) );
	if (new_node->keys == NULL) {
		printf ("## New NODE5 keys array in _bpt_make_node().\n");
		exit(EXIT_FAILURE);
	}
	new_node->pointers = malloc ( btree->order * sizeof(void*) );
	if (new_node->pointers == NULL) {
		printf ("## New NODE5 pointers array in _bpt_make_node().\n");
		exit(EXIT_FAILURE);
	}
	new_node->is_leaf = false;
	new_node->num_keys = 0;
	new_node->parent = NULL;
	new_node->next = NULL;

	return new_node;
}

//RECORD �ϳ��� �����Ѵ�.(�޸� �Ҵ�)
/*
RECORD* _bpt_make_record (void* record)
{
	RECORD* new_record = (RECORD *)malloc (sizeof(RECORD));

	if (new_record == NULL) {
		printf ("## Record creation error in _bpt_make_record().\n");
		exit (EXIT_FAILURE);
	} else {
		new_record->key = ((RECORD *)record)->key;
		new_record->word = ((RECORD *)record)->word;
	}
	return new_record;
}
*/

//������忡 �����Ѵ�.
NODE5* _bpt_insert_into_leaf (NODE5* leaf, BTREE* btree, void* key, void* data)
{
	register int i, idx;

	idx = 0;
	//������ ��ġ�� ã�´�.(=�� �ߺ�Ű)
	while (idx < leaf->num_keys && btree->compare (leaf->keys[idx], key) <= 0)
		idx++;

	for (i = leaf->num_keys; i > idx; i--) {
		leaf->keys[i] = leaf->keys[i - 1];	//���������� �̵�
		leaf->pointers[i] = leaf->pointers[i - 1];
	}
	//������ġ(idx)�� ����
	leaf->keys[idx] = key;
	leaf->pointers[idx] = data;
	leaf->num_keys++;

	return leaf;
}

//Ű�� ū ������� �ԷµǴ� ���, �迭 ���� �׳� ����
NODE5* _bpt_insert_into_leaf_asc (NODE5* leaf, BTREE* btree, void* key, void* data)
{
	//�迭���� ����
	leaf->keys[leaf->num_keys] = key;
	leaf->pointers[leaf->num_keys] = data;
	leaf->num_keys++;

	return leaf;
}

//������ �и����� �����Ѵ�.
NODE5* _bpt_insert_into_leaf_after_splitting (BTREE* btree, NODE5* leaf, void* key, void* data)
{
	NODE5*	new_leaf;
	void**	temp_keys;
	void**	temp_pointers;
	void*	new_key;
	int		idx, split, i, j;

	new_leaf = _bpt_make_leaf (btree);	//���ο� ������� �޸��Ҵ�

	//�ӽ� �޸� �Ҵ�
	//temp_keys = malloc (btree->order * sizeof(void*) );
	temp_keys = alloca (btree->order * sizeof(void*) );
	if (temp_keys == NULL) {
		printf ("## Temporary keys allocation error in _bpt_insert_into_leaf_after_splitting().\n");
		exit(EXIT_FAILURE);
	}
	//temp_pointers = malloc (btree->order * sizeof(void*) );
	temp_pointers = alloca (btree->order * sizeof(void*) );
	if (temp_pointers == NULL) {
		printf ("## Temporary pointers allocation error in _bpt_insert_into_leaf_after_splitting().\n");
		exit(EXIT_FAILURE);
	}

	idx = 0;
	//������ ��ġ�� ã�´�.(=�� �ߺ�Ű)
	while (idx < btree->order - 1 && btree->compare (leaf->keys[idx], key) <= 0)
		idx++;

	//leaf�� temp�� ����
	for (i = 0, j = 0; i < leaf->num_keys; i++, j++) {
		if (j == idx) j++; //key��ġ�� �ǳʶ�
		temp_keys[j] = leaf->keys[i];
		temp_pointers[j] = leaf->pointers[i];
	}
	temp_keys[idx] = key;	//key����
	temp_pointers[idx] = data;

	leaf->num_keys = 0;
	//������� �и� �ε���
	split = _bpt_half_order (btree->order - 1);

	//split ����(Left)�� left�� �̵�
	for (i = 0; i < split; i++) {
		leaf->pointers[i] = temp_pointers[i];
		leaf->keys[i] = temp_keys[i];
		leaf->num_keys++;
	}
	//split �̻�(Right)�� new_left�� �̵�
	for (i = split, j = 0; i < btree->order; i++, j++) {
		new_leaf->pointers[j] = temp_pointers[i];
		new_leaf->keys[j] = temp_keys[i];
		new_leaf->num_keys++;
	}

	//�ӽ� �޸� ���� (alloca�� ���ÿ� �Ҵ�� �޸𸮴� scope ����� �ڵ� ������)
	//free(temp_pointers);
	//free(temp_keys);

	//���� ������ ������(�̿� ����)
	new_leaf->pointers[btree->order - 1] = leaf->pointers[btree->order - 1];
	leaf->pointers[btree->order - 1] = new_leaf;

	//�޺κ� NULL ó��
	for (i = leaf->num_keys; i < btree->order - 1; i++)
		leaf->pointers[i] = NULL;
	for (i = new_leaf->num_keys; i < btree->order - 1; i++)
		new_leaf->pointers[i] = NULL;

	//�θ��� ó��
	new_leaf->parent = leaf->parent;
	//���ο� ���(Right)�� ù��° Ű
	new_key = new_leaf->keys[0];

	//new_key�� �θ��忡 �����Ѵ�.
	return _bpt_insert_into_parent (btree, leaf, new_key, new_leaf);	//(root, left, key, right)
}

//���ο� Ű(������ ����� ù��° Ű)�� �θ��忡 �����Ѵ�.
NODE5* _bpt_insert_into_parent (BTREE* btree, NODE5* left, void* key, NODE5* right)
{
	register int left_index;
	NODE5*	parent;

	parent = left->parent;
	if (parent == NULL) //���ο� �θ� ��� ���� (Ʈ������ ����)
		return _bpt_insert_into_new_root (btree, left, key, right);

	//�θ� ��忡�� ���� ����� ��ġ�� ã��
	left_index = _bpt_get_left_index (parent, left);

	if (parent->num_keys < btree->order - 1)
		//�θ��忡 ����
		return _bpt_insert_into_parent_node (btree->root, parent, left_index, key, right);

	//�θ��� ����(overflow)
	return _bpt_insert_into_parent_after_splitting (btree, parent, left_index, key, right);
}

//���ο� �θ� ��� ���� (Ʈ������ ����)
NODE5* _bpt_insert_into_new_root (BTREE* btree, NODE5* left, void* key, NODE5* right)
{
	NODE5* root = _bpt_make_node (btree);

	root->keys[0] = key;
	root->pointers[0] = left;
	root->pointers[1] = right;
	root->num_keys++;
	root->parent = NULL;
	left->parent = root;
	right->parent = root;

	return root;
}

//�θ� ��忡�� ���� ����� ��ġ�� ã��
int _bpt_get_left_index (NODE5* parent, NODE5* left)
{
	register int left_index = 0;

	while (left_index <= parent->num_keys && parent->pointers[left_index] != left)
		left_index++;

	return left_index;
}

//�θ��忡 ����
NODE5* _bpt_insert_into_parent_node (NODE5* root, NODE5* node, int left_index, void* key, NODE5* right)
{
	register int i;

	for (i = node->num_keys; i > left_index; i--) {
		node->pointers[i + 1] = node->pointers[i];	//���������� �̵�
		node->keys[i] = node->keys[i - 1];
	}
	node->pointers[left_index + 1] = right;
	node->keys[left_index] = key;
	node->num_keys++;

	return root;
}

//�θ��� ����
NODE5* _bpt_insert_into_parent_after_splitting (BTREE* btree, NODE5* left, int left_index, void* key, NODE5* right)
{
	register int i, j, split;
	NODE5*	new_node, * child;
	void**	temp_keys;
	NODE5** temp_pointers;
	void*	kp;

	//�ӽ� �޸� �Ҵ�(split ���� �����ʹ� Order���� �Ѱ� ����)
	//temp_pointers = malloc ((btree->order + 1) * sizeof(NODE5*) );
	temp_pointers = alloca ((btree->order + 1) * sizeof(NODE5*) );
	if (temp_pointers == NULL) {
		printf ("## Temporary pointers allocation error in _bpt_insert_into_parent_after_splitting().\n");
		exit(EXIT_FAILURE);
	}
	//temp_keys = malloc (btree->order * sizeof(void*) );
	temp_keys = alloca (btree->order * sizeof(void*) );
	if (temp_keys == NULL) {
		printf ("## Temporary keys allocation error in _bpt_insert_into_parent_after_splitting().\n");
		exit(EXIT_FAILURE);
	}

	//�����͸� �ӽ� �޸𸮿� ����
	for (i = 0, j = 0; i < left->num_keys + 1; i++, j++) {
		if (j == left_index + 1) j++;
		temp_pointers[j] = left->pointers[i];
	}
	//Ű�� �ӽ� �޸𸮿� ����
	for (i = 0, j = 0; i < left->num_keys; i++, j++) {
		if (j == left_index) j++;
		temp_keys[j] = left->keys[i];
	}
	//����
	temp_pointers[left_index + 1] = right;
	temp_keys[left_index] = key;

	//���ο� ��� �Ҵ�(right)
	new_node = _bpt_make_node (btree);

	//���γ��� �и� �ε����� �ϳ� �� ����
	split = _bpt_half_order (btree->order);
	left->num_keys = 0;
	//������ ����(left)
	for (i = 0; i < split - 1; i++) {
		left->pointers[i] = temp_pointers[i];
		left->keys[i] = temp_keys[i];
		left->num_keys++;
	}
	left->pointers[i] = temp_pointers[i];
	kp = temp_keys[split - 1];  //�θ�� �ö󰡴� Ű

	//������ ����(right)
	for (++i, j = 0; i < btree->order; i++, j++) {
		new_node->pointers[j] = temp_pointers[i];
		new_node->keys[j] = temp_keys[i];
		new_node->num_keys++;
	}
	new_node->pointers[j] = temp_pointers[i];

	//�ӽø޸� ���� (alloca�� ���ÿ� �Ҵ�� �޸𸮴� �ڵ����� ������)
	//free(temp_pointers);
	//free(temp_keys);

	new_node->parent = left->parent;
	for (i = 0; i <= new_node->num_keys; i++) {
		child = new_node->pointers[i];
		child->parent = new_node;
	}

	//��尡 �и� �Ǿ����Ƿ� �θ� �ٽ� ����
	return _bpt_insert_into_parent (btree, left, kp, new_node);
}


// DELETION ---------------------------------------------------------------------------------------
/*
1.���(����,�θ�) ����
2.	��� ����(��ģ�� ����)
3.		��尡 ���� ���ʿ� �ִ°�?
4.			���� ����
5.			�θ� ����
		��� ����(1������ �ݺ�)
6.	��� ��й�
7.		��尡 ���� ���ʿ� �ִ°�?
8.			���� ��й�
9.			�θ� ��й�
*/

/*
1.bpt_delete ()
2.	bpt_delete_entry ()
3.		_bpt_remove_entry_from_node ()	//��Ʈ�� ����
4.		_bpt_adjust_root ()				//��Ʈ ��� ������
5.		_bpt_get_neighbor_index ()		//�̿�(����) ��� �ε���

		//�̿�(����) ��忡 ����� ��Ʈ���� ������ ��� ����(��ħ, ���� ���� ����)
6.		_bpt_coalesce_nodes ()
			bpt_delete_entry () //2������ �̵�(�ݺ�)

		//�̿�(����) ��忡 ����� ��Ʈ���� ������(���� ������) ��� ��й�(Ű �̵�)
7.		_bpt_redistribute_nodes ()
*/

///�ܵ� B+Ʈ�� ��� ����
NODE5* bpt_delete (BTREE* btree, void* key, bool *deleted)
{
	NODE5* leaf;
	int idx;
	void *pkey, *pointer;

	*deleted = false;
	//�������� �̵��Ͽ� key�� ã��
	if ((idx = bpt_find_leaf_key (btree, key, &leaf)) >= 0) {
		pkey = leaf->keys[idx];		//������(Ű)
		pointer = leaf->pointers[idx];	//������(������)

		//��Ʈ��(�迭)���� Ű ����
		//Ű�� �θ��忡�� �����Ƿ� �޸� ������ �����߻�(���⼭ �޸� ���� ���ɼ� ����)
		//�����Ǵ� Ű�� �θ��忡 ������ _bpt_remove_entry_from_node ����(2010-12-18)
		btree->root = bpt_delete_entry (btree, leaf, pkey, pointer, 0);	//key�� ������Ű �ƴ�(���ڿ�)

		//�޸� ����
		free (pkey);
		free (pointer);

		btree->kno--;	//������ȣ ����
		btree->kcnt--;  //Ű ���� ����
		*deleted = true;
	}
	return btree->root;
}

///������ ��Ʈ�� ����(�ݺ� ȣ���)
//�ߺ�Ű ������ flag �Ű����� �߰�(2011-01-17)
NODE5* bpt_delete_entry (BTREE* btree, NODE5* node, void* key, void* pointer, int flag)
{
	int		min_keys, max_keys;
	NODE5*	neighbor;
	int		neighbor_index;
	int		kp_index;
	void*	kp;

	//����� ���͸�(�迭)���� Ű-������ ����
	node = _bpt_remove_entry_from_node (btree, node, key, pointer, flag);

	//��Ʈ��尡 ��� �ִٸ� ������
	if (node == btree->root) return _bpt_adjust_root (btree->root);

	//��忡 �ּ����� Ű��(����)�� �ִٸ� ����(���� ����)
	min_keys = node->is_leaf ? _bpt_half_order (btree->order - 1) : _bpt_half_order (btree->order) - 1;
	if (node->num_keys >= min_keys)	return btree->root;

	//��忡 �ּ����� Ű��(����)�� ���ٸ�, ��� ���� �� ��й� ����

	//�θ��忡�� ���� ���� �̿�(����)�ϴ� ��� �ε���(-1�϶��� �����尡 ���� ���ʳ��)
	neighbor_index = _bpt_get_neighbor_index (node);
	kp_index = neighbor_index == -1 ? 0 : neighbor_index;
	//�θ��� Ű
	kp = node->parent->keys[kp_index];

	//���� ���� �̿��ϴ� ���
	neighbor = neighbor_index == -1 ? node->parent->pointers[1] : node->parent->pointers[neighbor_index];

	max_keys = node->is_leaf ? btree->order : btree->order - 1;
	if (neighbor->num_keys + node->num_keys < max_keys)
		//�̿�(����) ��忡 ����� ��Ʈ���� ������ ��� ����(��ħ, ���� ��� ����)
		return _bpt_coalesce_nodes (btree, node, neighbor, neighbor_index, kp);
	else
		//�̿�(����) ��忡 ����� ��Ʈ���� ������(���� ������) ��� ��й�(Ű �̵�)
		return _bpt_redistribute_nodes (btree->root, node, neighbor, neighbor_index, kp_index, kp);
}

//����� ���͸�(�迭)���� Ű-������ ����
//�ߺ�Ű ������ flag �Ű����� �߰�(2011-01-17)
NODE5* _bpt_remove_entry_from_node (BTREE* btree, NODE5* node, void* key, void* pointer, int flag)
{
	register int i, k, idx_end;
	void*	dkey;  //�����Ǵ� Ű
	NODE5*	node_tmp = NULL;

	i = 0;
	k = 0;

	if (flag) {  //�ߺ�Ű�� �����ͷ� ��
		while (key != node->keys[i] ) {
			i++;
			k++;
		}
	} else {
		while (btree->compare (key, node->keys[i]) ) {
			i++;
			k++;
		}
	}
	dkey = node->keys[i];
	//Ű ��ġ���� �������� �̵�(key ����)
	for (++i; i < node->num_keys; i++)
		node->keys[i - 1] = node->keys[i];

	//���� ���� �����Ͱ� �ϳ� ����(�� ������ ������)
	idx_end = node->is_leaf ? node->num_keys : node->num_keys + 1;

	i = 0;
	while (node->pointers[i] != pointer) i++;
	//������ ��ġ���� �������� �̵�(pointer ����)
	for (++i; i < idx_end; i++)
		node->pointers[i - 1] = node->pointers[i];

	//���͸� ���� �ϳ� ����
	node->num_keys--;

	//�޺κ� �����ʹ� NULL ó��
	if (node->is_leaf)
		for (i = idx_end; i < btree->order - 1; i++)  //�������� �� ������ ������ �����
			node->pointers[i] = NULL;
	else
		for (i = idx_end; i < btree->order; i++)
			node->pointers[i] = NULL;

	//(2010-12-18, �߰�): ������ ���� ���� ��Ұ� ���� �Ǿ��ٸ�(k==0),
	//�̰��� �θ� ������ ���� �ǹǷ� �����Ѵ�.(������ Ű�� �θ� �����ϴ� ���� ����)
	if (k==0 && node->is_leaf) node_tmp = node->parent;
	while (node_tmp) {
		for (i = 0; i < node_tmp->num_keys; i++) {
			if (dkey == node_tmp->keys[i]) {	//������ ��
				node_tmp->keys[i] = node->keys[0];
				break;	//�θ� �ϳ��� ����
			}
		} //for
		node_tmp = node_tmp->parent;
	} //while

	return node;
}

//��Ʈ��尡 ��� �ִٸ� ������
NODE5* _bpt_adjust_root (NODE5* root)
{
	NODE5* new_root;

	//��� ���� ������ �״�� ��ȯ
	if (root->num_keys > 0) return root;

	//��� ������
	if (root->is_leaf) {  //��Ʈ�� �����̸� �ڽ� ���(������) ����
		new_root = NULL;
	} else {  //��Ʈ�� ������ �ƴ϶�� ù��° �ڽ� ��带 ���ο� ��Ʈ��
		new_root = root->pointers[0];
		new_root->parent = NULL;
	}

	//����ִ� ��Ʈ��� ����
	free (root->keys);
	free (root->pointers);
	free (root);

	//���ο� ��Ʈ��� ��ȯ
	return new_root;
}

//�̿�(����)��� �ε���
int _bpt_get_neighbor_index (NODE5* node)
{
	register int i;

	//���� ���� �̿��ϴ� ���ʳ�� ��ȣ(-1�϶��� �����尡 ���� ���ʳ��)
	for (i = 0; i <= node->parent->num_keys; i++)
		if (node->parent->pointers[i] == node)
			return i - 1;

	//�����޼��� ���
	printf ("There is no left-child-node of parent in the _bpt_get_neighbor_index().\n");
	exit (EXIT_FAILURE);
}

//�̿�(����) ��忡 ����� ��Ʈ���� ������ ��� ����(��ħ, ���� ���� ����)
NODE5* _bpt_coalesce_nodes (BTREE* btree, NODE5* node, NODE5* neighbor, int neighbor_index, void* kp)
{
	register int i, j;
	int idx, n_end;
	NODE5 *tmp;

	if (neighbor_index == -1) {  //���� ��尡 ���� ���� ����϶�, neighbor�� �ٲ�
		tmp = node;
		node = neighbor;
		neighbor = tmp;
	}

	//�̿� ��忡 �������� ��ġ �ε���
	idx = neighbor->num_keys;

	if (node->is_leaf) {
	//���� ��尡 �������
		//�����带 �̿������ ������ �̵�(��ħ)
		for (i = idx, j = 0; j < node->num_keys; i++, j++) {
			neighbor->keys[i] = node->keys[j];
			neighbor->pointers[i] = node->pointers[j];
			neighbor->num_keys++;
		}
		neighbor->pointers[btree->order - 1] = node->pointers[btree->order - 1];

	} else {
	//���� ��尡 ������ �ƴ϶��(���γ��)
		//�θ����� Ű�� �̿��� ������ �̵�(����)
		neighbor->keys[idx] = kp;
		neighbor->num_keys++;

		//���� ����� Ű��
		n_end = node->num_keys;
		//���� ��带 �̿� ����� ������ �̵�
		for (i = idx + 1, j = 0; j < n_end; i++, j++) {
			neighbor->keys[i] = node->keys[j];
			neighbor->pointers[i] = node->pointers[j];
			neighbor->num_keys++;
			node->num_keys--;
		} //for

		//�������� ���� Ű�� ������ �׻� �ϳ� ����.
		neighbor->pointers[i] = node->pointers[j];

		//������ ��� �ڽĵ��� �θ�(neighbor->pointers->parent)�� ������ ���(neighbor)�� �Ǿ�� �Ѵ�.
		for (i = 0; i < neighbor->num_keys + 1; i++) {
			tmp = (NODE5 *)neighbor->pointers[i];
			tmp->parent = neighbor;
		} //for
	} //if

	//���� ����� �θ� ����(��ȣ��)
	btree->root = bpt_delete_entry (btree, node->parent, kp, node, 1); //�θ��� Ű�� ������ ��.

	//���� ��� ����
	free (node->keys);
	free (node->pointers);
	free (node);

	return btree->root;
}

//�̿�(����) ��忡 ����� ��Ʈ���� ������(���� ������) ��� ��й�
NODE5* _bpt_redistribute_nodes (NODE5* root, NODE5* node, NODE5* neighbor, int neighbor_index, int kp_index, void* kp)
{
	register int i;

	if (neighbor_index == -1) {
		//���� ��尡 ���� ���ʳ���̸�(������ �̿����� �ϳ� ������)
		if (node->is_leaf) {
			//������ �̿������ ���� ���ʿ� �ִ� key-pointer ���� ���� ����� ���������� �ű�.
			node->keys[node->num_keys] = neighbor->keys[0];
			node->pointers[node->num_keys] = neighbor->pointers[0];
		} else {
			//�θ����� Ű�� �������� ������ �̵�(����)
			node->keys[node->num_keys] = kp;
			//�̿� ����� ù��° �����͸� ���� ����� ������ �̵�
			node->pointers[node->num_keys + 1] = neighbor->pointers[0];
			//�̿�(neighbor) ����� �θ�� ����(node) ��尡 ��
			((NODE5 *)node->pointers[node->num_keys + 1])->parent = node;

			//Ű�� �θ���� �̵�
			node->parent->keys[kp_index] = neighbor->keys[0];
		}
		//������ �̿������ Ű-�����͸� ��ĭ�� �������� �̵�
		for (i = 0; i < neighbor->num_keys; i++) {
			neighbor->keys[i] = neighbor->keys[i + 1];
			neighbor->pointers[i] = neighbor->pointers[i + 1];
		}
		if (node->is_leaf)
			node->parent->keys[kp_index] = neighbor->keys[0];  //Ű�� �θ���� �̵�
		else
			neighbor->pointers[i] = neighbor->pointers[i + 1];

	} else {
	//���� �̿� ��忡�� �ϳ� ������
		if (!node->is_leaf)
			node->pointers[node->num_keys + 1] = node->pointers[node->num_keys];
		//���������� �̵�
		for (i = node->num_keys; i > 0; i--) {
			node->keys[i] = node->keys[i - 1];
			node->pointers[i] = node->pointers[i - 1];
		}
		if (node->is_leaf) {
			node->pointers[0] = neighbor->pointers[neighbor->num_keys - 1];
			neighbor->pointers[neighbor->num_keys - 1] = NULL;
			node->keys[0] = neighbor->keys[neighbor->num_keys - 1];
			//Ű�� �θ���� �̵�
			node->parent->keys[kp_index] = node->keys[0];
		} else {
			node->pointers[0] = neighbor->pointers[neighbor->num_keys];
			((NODE5 *)node->pointers[0])->parent = node;
			neighbor->pointers[neighbor->num_keys] = NULL;
			node->keys[0] = kp;
			node->parent->keys[kp_index] = neighbor->keys[neighbor->num_keys - 1];
		}
	}
	//������ Ű ����
	node->num_keys++;
	//�̿���� Ű ����
	neighbor->num_keys--;

	return root;
}


// DESTROY ----------------------------------------------------------------------------------------

//������忡 �Ҵ�� �޸𸮿� ��� �����͵��� ���ÿ� ����(���ȣ��)
unsigned int bpt_drop_leaves_nodes (BTREE* btree, NODE5* node)
{
	register int i;
	char prompt[] = {'+', '-', '*'};
	static unsigned int cnt = 0;

	if (!node) {
		printf("** Empty.\n");
		return 0;
	}

	if (node->is_leaf) {
		for (i = 0; i < node->num_keys; i++) {
			free (node->pointers[i]);
			btree->kcnt--;
		}
	} else {
		for (i = 0; i < node->num_keys + 1; i++)
			bpt_drop_leaves_nodes (btree, node->pointers[i]);  //���ȣ��
	}

	free (node->pointers);
	free (node->keys);
	free (node);

	cnt++; //������ ����
	printf ("%c(%c)%u ", CR, prompt[cnt%3], cnt);  //CR, ������Ʈ, �ݺ���

	return cnt;
}

//������忡 �Ҵ�� �޸� ����
unsigned int bpt_drop_leaves (BTREE* btree, NODE5* node)
{
	register int i;
	unsigned int cnt = 0;
	char prompt[] = {'+', '-', '*'};

	if (!node) {
		printf("** Empty.\n");
		return cnt;
	}
	//���� ��忡 �Ҵ�� �޸� ����
	while (!node->is_leaf)
		node = (NODE5*)node->pointers[0];  //Ʈ�� ���̸�ŭ �ݺ�

	while (true) {
		cnt += node->num_keys;
		for (i = 0; i < node->num_keys; i++) {
			free (node->keys[i]);
			free (node->pointers[i]);
			btree->kcnt--;
		}
		printf ("%c(%c)%u ", CR, prompt[cnt%3], cnt);  //CR, ������Ʈ, �ݺ���

		if (node->pointers[btree->order - 1])
			node = (NODE5*)node->pointers[btree->order - 1];	//���� ���� ���
		else break;
	} //while

	return cnt; //������ �ִ� key ����
}

//��� ������ ����(���ȣ��)
unsigned int bpt_drop_nodes (NODE5* node)
{
	register int i;
	char prompt[] = {'+', '-', '*'};
	static unsigned int cnt = 0;

	if (!node) {
		printf("** Empty.\n");
		cnt = 0;
		return cnt;
	}

	if (!node->is_leaf) {
		for (i = 0; i < node->num_keys + 1; i++)
			bpt_drop_nodes (node->pointers[i]); //��� �����͸� ���ȣ��
	}

	free (node->pointers);
	free (node->keys);
	free (node);

	cnt++;
	printf ("%c(%c)%u ", CR, prompt[cnt%3], cnt);  //CR, ������Ʈ, �ݺ���

	return cnt; //��� ����
}


//B+ Ʈ�� ��� ��� ����
void bpt_drop (BTREE** ws, BTREE** wi)
{
	unsigned int cnt;

	cnt = bpt_drop_leaves_nodes (*ws, (*ws)->root);
	(*ws)->root = NULL;
	printf ("%c(-)%u s-nodes have removed.", CR, cnt);
	(*ws)->kno = (*ws)->kcnt;	//0

	cnt = bpt_drop_leaves_nodes (*wi, (*wi)->root);
	(*wi)->root = NULL;
	printf ("%c(-)%u i-nodes have removed.\n", CR, cnt);
	(*wi)->kno = (*wi)->kcnt;	//0

	/*
	//������忡 �Ҵ�� �޸� ���� (ws�� wi�� key,pointer ����)
	cnt = bpt_drop_leaves (*ws, (*ws)->root);
	printf ("%c(-)%u data leaves have removed.\n", CR, cnt);

	cnt = bpt_drop_nodes ((*ws)->root);		//ws ��� ������ ����
	(*ws)->root = NULL;
	(*ws)->kno = (*ws)->kcnt;	//0
	printf ("%c(-)%u s-nodes have removed.\n", CR, cnt);

	cnt = bpt_drop_nodes ((*wi)->root);		//wi ��� ������ ����
	(*wi)->root = NULL;
	(*wi)->kcnt = (*ws)->kcnt;	//0
	(*wi)->kno = (*wi)->kcnt;	//0
	printf ("%c(-)%u i-nodes have removed.\n", CR, cnt);
	*/
}

//B+ Ʈ�� ��� ���� B+Ʈ������ ����
void bpt_drop_all (BTREE** ws, BTREE** wi)
{
	bpt_drop (ws, wi);
	free (*ws);
	free (*wi);
	*ws = NULL;
	*wi = NULL;
}
