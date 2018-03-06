//
//  Source: bpt2.h written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2010, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2010-05-12 B+Tree �ڵ��� �����ϴ�.
//		2010-12-08 ������Ͽ� ���� �ּ����� �߰��ϰ� �������ϴ�.
//		2010-12-17 ���ڿ�Ű�� ����Ű�� ���� �۾��� �� �ֵ��� NODE5�� �����ϰ� BTREE ����ü�� ���� �߰��ϴ�.
//		yyyy-mm-dd ...
//

#include "dtype.h"

// OUTPUT -----------------------------------------------------------------------------------------

//new_node�� ť(qnode)�� �߰� (bpt_print �Լ����� ���)
NODE5* _bpt_enqueue (NODE5* qnode, NODE5* new_node);

//node�� ť(qnode)���� ������ (bpt_print �Լ����� ���)
NODE5* _bpt_dequeue (NODE5** qnode);

//B+ Ʈ�� ���, ��带 �����ϱ� ���� '|' ��ȣ ���, Ű�� ���� �����ʹ� 16������ ���
void bpt_print (BTREE* btree);

//���� ��常 ���
void bpt_print_leaves (BTREE* btree);

//����� Ű ����
//����Ű --> ����Ű
NODE5* bpt_init_key (BTREE*, BTREE*);
//����Ű1 --> ����Ű2 ����
NODE5* bpt_init_trans_key (BTREE* t1, BTREE* hb[], int sh);

//Ʈ�� ����: ��Ʈ���� �������� ����
int _bpt_height (NODE5* root);

//�ڽĳ�忡�� ��Ʈ������ ����
int _bpt_path_to_root (NODE5* root, NODE5* child);

//��Ʈ���� pointers�� ���󰡸� ������带 ã�´�.
NODE5* bpt_find_leaf_debug (BTREE* btree, void* key, bool debug);
NODE5* bpt_find_leaf_unique (BTREE* btree, void* key, int (*compare)(void* p1, void* p2));
NODE5* bpt_find_leaf (BTREE* btree, void* key, int (*compare)(void* p1, void* p2));

//���� ��带 ã�Ƽ� key �˻�, leaf �Ѱ� ����
int bpt_find_leaf_key (BTREE* btree, void* key, NODE5** leaf);
int bpt_find_leaf_key_next (BTREE* btree, void* key, NODE5** leaf, int count);
int bpt_find_leaf_key_trans (BTREE* btree, void* key, void* ptr, NODE5** leaf);
int bpt_find_leaf_key_like (BTREE* btree, void* key, NODE5** leaf);
int bpt_find_leaf_key_similar (BTREE* btree, void* key, NODE5** leaf);

//������ ã�Ƽ� key�� �˻��Ͽ� �ش� record�� ��ȯ�Ѵ�.
void* bpt_search (BTREE* btree, void* key);
void* bpt_search_str_unique_like (BTREE* btree, void* key);

//Order(������ �迭 ��Ҽ�)�� �߰� ��ġ ���
//((Order + 1) / 2) - 1
int _bpt_half_order (int length);


// INSERTION --------------------------------------------------------------------------------------

//B+ Ʈ���� key�� value�� �����Ѵ�.
/*
 1.bpt_insert ()	
 2.	_bpt_make_root () //��Ʈ ��� ���� ����
 3.	bpt_find_leaf ()
 4.	_bpt_make_record ()

	//���� ��忡 ����� ��Ʈ���� ������
 5.	_bpt_insert_into_leaf () -->return
	
	//���� ��忡 ����� ��Ʈ���� ������(���� ������)
 6.	_bpt_insert_into_leaf_after_splitting ()  //��������

 7.		_bpt_insert_into_parent ()  //���ҵ� ��带 �θ� ����
 8.			_bpt_insert_into_new_root () 			
 9.			_bpt_get_left_index ()  //�θ��忡�� ���� �ڽ��� �ε���

			//�θ� ��忡 ����� ��Ʈ���� ������
10.			_bpt_insert_into_parent_node () --> return

			//�θ� ��忡 ����� ��Ʈ���� ������(���� ������)
11.			_bpt_insert_into_parent_after_splitting ()  //�θ��� �ٽ� ����
12.				_bpt_insert_into_parent ()  //7������ �̵�(�ݺ�)
*/

//btree ����ü ����
BTREE* bpt_create (int order, int (*compare)(void* p1, void* p2)
				   , void (*outkey)(void* p1), void (*outdata)(void* p1), bool samek );

//������忡 Ű�� ������ �Է�
NODE5* bpt_insert (BTREE* btree, NODE5* leaf, void* key, void* data, int flag);

//Ű ������� �ԷµǴ� ��� �Է� �ӵ� ����(leaf ������ �״�� ���)
NODE5* bpt_insert_asc (BTREE* btree, NODE5** leaf, void* key, void* data);

//��Ʈ ��� ����
NODE5* _bpt_make_root (BTREE* btree, void* key, void* data);

//���� ��� ����
NODE5* _bpt_make_leaf (BTREE* btree);

//��� ����(�޸� �Ҵ�)
NODE5* _bpt_make_node (BTREE* btree);

//RECORD �ϳ��� �����Ѵ�.(�޸� �Ҵ�)
//RECORD* _bpt_make_record (void* record);

//������忡 �����Ѵ�.
NODE5* _bpt_insert_into_leaf (NODE5* leaf, BTREE* btree, void* key, void* data);

//Ű�� ū ������� �ԷµǴ� ���, �迭 ���� �׳� ����(�ӵ� ����)
NODE5* _bpt_insert_into_leaf_asc (NODE5* leaf, BTREE* btree, void* key, void* data);

//������ �и����� �����Ѵ�.
NODE5* _bpt_insert_into_leaf_after_splitting (BTREE* btree, NODE5* leaf, void* key, void* data);

//���ο� Ű(������ ����� ù��° Ű)�� �θ��忡 �����Ѵ�.
NODE5* _bpt_insert_into_parent (BTREE* btree, NODE5* left, void* key, NODE5* right);

//���ο� �θ� ��� ���� (Ʈ������ ����)
NODE5* _bpt_insert_into_new_root (BTREE* btree, NODE5* left, void* key, NODE5* right);

//�θ� ��忡�� ���� ����� ��ġ�� ã��
int _bpt_get_left_index (NODE5* parent, NODE5* left);

//�θ��忡 ����
NODE5* _bpt_insert_into_parent_node (NODE5* root, NODE5* node, int left_index, void* key, NODE5* right);

//�θ��� ����
NODE5* _bpt_insert_into_parent_after_splitting (BTREE* btree, NODE5* left, int left_index, void* key, NODE5* right);


// DELETION ---------------------------------------------------------------------------------------

//key�� �ش��ϴ� ��� ����
/*
1.bpt_delete ()
2.	bpt_delete_entry ()
3.		_bpt_remove_entry_from_node ()  //��Ʈ�� ����
4.		_bpt_adjust_root ()  //��Ʈ ��� ������
5.		_bpt_get_neighbor_index ()  //�̿�(����) ��� �ε���

		//�̿�(����) ���� ����� ��Ʈ���� ������ ��� ����(��� �ٽ� ���� �߻���)
6.		_bpt_coalesce_nodes ()
			bpt_delete_entry () //2������ �̵�(�ݺ�)

		//�̿�(����) ��忡 ����� ��Ʈ���� ������(���� ������) ��� ��й�
7.		_bpt_redistribute_nodes ()
*/
//�ܵ� B+Ʈ�� ��� ����
NODE5* bpt_delete (BTREE* btree, void* key, bool *deleted);

//������ ��Ʈ�� ����(�ݺ� ȣ���)
NODE5* bpt_delete_entry (BTREE* btree, NODE5* node, void* key, void* pointer, int flag);

//����� ���͸�(�迭)���� Ű-������ ����
NODE5* _bpt_remove_entry_from_node (BTREE* btree, NODE5* node, void* key, void* pointer, int flag);

//��Ʈ��尡 ��� �ִٸ� ������
NODE5* _bpt_adjust_root (NODE5* root);

//�̿�(����)��� �ε���
int _bpt_get_neighbor_index (NODE5* node);

//�̿�(����) ��忡 ����� ��Ʈ���� ������ ��� ����(��ħ, ���� ���� ����)
NODE5* _bpt_coalesce_nodes (BTREE* btree, NODE5* node, NODE5* neighbor, int neighbor_index, void* kp);

//�̿�(����) ��忡 ����� ��Ʈ���� ������(���� ������) ��� ��й�
NODE5* _bpt_redistribute_nodes (NODE5* root, NODE5* node, NODE5* neighbor, int neighbor_index, int kp_index, void* kp);


// DESTROY ----------------------------------------------------------------------------------------

//������忡 �Ҵ�� �޸𸮿� ��� �����͵��� ���ÿ� ����(���ȣ��)
unsigned int bpt_drop_leaves_nodes (BTREE* btree, NODE5* node);

//������忡 �Ҵ�� �޸� ����
unsigned int bpt_drop_leaves (BTREE* btree, NODE5* node);
//��� ������ ����(���ȣ��)
unsigned int bpt_drop_nodes (NODE5* node);

//��� ��� ����
void bpt_drop (BTREE**, BTREE**);

//��� ���� B+Ʈ������ �޸� ����
void bpt_drop_all (BTREE** ws, BTREE** wi);

