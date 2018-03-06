//
//  Source: utime.h written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2010, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2010-12-18 �ð� ���� �Լ����� �ۼ��ϴ�.
//		yyyy-mm-dd ...
//

void time_rand_seed_init (void);

//p�� r������ ���� �߻�
unsigned int time_random_between (unsigned int p, unsigned int r);

//������ ���ڿ�(���� �ҹ���)�� cnt ���� ��ŭ ����
char* time_get_random_str (int cnt);


//�� ������ ����ð��� ������
long time_get_sec (void);

//�̸��� ������ ����ð��� ������
double time_get_msec (void);

void time_sleep (int sec);
