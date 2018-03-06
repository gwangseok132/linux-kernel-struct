//
//  Source: main.c written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2010, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2011-01-01 main �Լ��� ������Ű��.
//		2011-01-28 �������� �ε���Ű�� 2���� �и��ϴ�.
//		2011-02-08 �����ܾ� ��ȯ�� B+Ʈ��(btree_rs)�� �߰��ϴ�.
//		2011-02-11 �ܾ� ������ ������ȣ ������ ť(QUEUE* qk[])�� �߰��ϴ�.
//		2011-02-25 ����Ű B+Ʈ���� HASHSIZE ��ŭ �����Ͽ� �ؽð����� �����ϵ��� �ϴ�.(�뷮�� �ӵ����)
//		yyyy-mm-dd ...
//

#include <stdio.h>
#include <stdlib.h>

#include "dtype.h"		//������ Ÿ��
#include "bpt3.h"		//B+Ʈ��
#include "ustr.h"		//���ڿ�
#include "utime.h"		//�ð�
#include "fileio.h"		//���� �����
///#include "queue.h"		//ť
///#include "stack.h"		//����
#include "tw1.h"

//���� �Լ� ---------------------------------------------------------------------------------------
int main (int argc, char** argv)
{
	BTREE	*btree;
	register int i;
	int		err = 0;
	double	msec1, msec2;

	printf ("btree test running...\n");

	//�̸� �� ����(����)
	msec1 = time_get_msec ();

	//B_ORDER; //default 8
	//B_ORDER�� B+Ʈ�� ��忡�� entries(keys and pointers)�� �ִ� �� �ּ� ������ �����Ѵ�.

	//�ܾ�Ű B+Ʈ�� A ����
	btree = bpt_create (B_ORDER, tw1_compare_str, tw1_output_str, tw1_output_int, false);	//�ߺ�Ű ������
	if (!btree) err++;

	if (err) {
		printf ("## Memory allocation error(%) at the bpt_create().\n", err);
		exit (EXIT_FAILURE);
	}

    ///test block
    {
        NODE5	*leaf, *root;
        char	*pword, s[6];
        unsigned int *pno;

        for (i=0; i < 20; i++) {
            bpt_find_leaf_key (btree, pno, &leaf);

            pword = malloc (6);
            str_copy (pword, "dat");
            uint_to_str (i, s);
            strcat(pword, s);
            pno = malloc (sizeof(unsigned int));
            *pno = i;
            ///����Ű �Է�
            btree->root = bpt_insert (btree, leaf, pno, pword, FLAG_INSERT);
            root = btree->root;
        }
    }
    //�̸� �� ����(����)
    msec2 = time_get_msec ();
    //����ð�
    printf ("** Run Time: %.3f Secs\n", msec2 - msec1);

    printf ("btree test finished.\n\n");

	return EXIT_SUCCESS;
}
