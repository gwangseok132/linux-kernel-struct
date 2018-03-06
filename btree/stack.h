//
//  Source: stack.h written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2010, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2010-03-18 ���� ������� ����
//		yyyy-mm-dd ...
//

#include "dtype.h"

//���� �Լ��� ���� ------------------------------------------------------------
//���� ����
STACK* stack_create (void);
//���� ����
int stack_destroy (STACK* stack);
//top���� ���� ����
int stack_drop_from_top (STACK* stack, int flag);
//bottom���� ���� ����
int stack_drop_from_bottom (STACK* stack, int flag);

//������ top�� push�Ѵ�.
bool stack_push_top (STACK* stack, void* data_in);
//������ bottom�� push�Ѵ�.
bool stack_push_bottom (STACK* stack, void* data_in);

//���� ����(height)�� ������ �ξ� top�� push�Ѵ�.
bool stack_push_limit (STACK* stack, void* data_in, int height);
//������ height ���� ��ŭ �ǵ��� bottom���� �����Ѵ�.
void stack_remove_bottom (STACK* stack, int height);

//���� ��ܿ��� ������ ������
void* stack_pop_top (STACK* stack, int flag);
void* stack_top (STACK* stack, int flag);

//���� �ϴܿ��� ������ ������
void* stack_pop_bottom (STACK* stack, int flag);
void* stack_bottom (STACK* stack, int flag);

bool stack_is_empty (STACK* stack);
bool stack_is_full (STACK* stack);
int stack_count (STACK* stack);
