//
//  Source: utime.c written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2010, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2011-01-25 �ð� ���� �Լ����� �ۼ��ϴ�.
//		yyyy-mm-dd ...
//

#include <time.h>

#include "dtype.h"

#ifdef __LINUX
	#include <sys/time.h>
	#include <unistd.h>		//sleep()
#else
	#include <sys/timeb.h>
	#include <windows.h>	//Sleep()
#endif


#define RND_SIZE     25		//���� �ִ� ũ��
#define RND_DIV   10000		//4�ڸ����� ����(10��4��)

unsigned int RandSeed;	//���� �߻��� seed

void time_rand_seed_init (void)
{
	//2��15�� = 32768
	//2��16�� = 65536
	//2��30�� = 1073741824 (10��)
	//2��31�� = 2147483648
	//2��32�� = 4294967296
	unsigned int time_value;
	time_t st;

	time_value = (unsigned int)time (&st);  //10�ڸ�(��10��)
	RandSeed = time_value - (unsigned int)(time_value / RND_DIV) * RND_DIV;  //�� 4�ڸ���
}

//p�� r������ ���� �߻�
unsigned int time_random_between (unsigned int p, unsigned int r)
{
	RandSeed = RandSeed * 1103515245 + 12345; 
	return (unsigned int) p + (RandSeed/65536) % (r-p); //double�� ���������� ����
}

//������ ���ڿ�(���� �ҹ���)�� cnt ���� ��ŭ ����
char* time_get_random_str (int cnt)
{
	static char str[SSIZE];
	register int i=0, j=0;
	int length;

	while (--cnt >= 0) {
		length = time_random_between (0, RND_SIZE) % 10 + 2;	//�ܾ����
		for (i = 0; i < length; i++, j++)
			str[j] = 'a' + time_random_between (0, RND_SIZE);  //0~25
		str[j++] = ' ';
	}
	str[--j] = '\0';

	return str;
}

/*
struct timeval {
        time_t       tv_sec;	//seconds
        suseconds_t  tv_usec;	//microseconds
};

struct timespec {
        time_t  tv_sec;		//seconds
        long    tv_nsec;	//nanoseconds
};
*/

//�� ������ ����ð��� ������
long time_get_sec (void)
{
	//typedef long time_t;
	time_t t;

	//printf ("** current time: %ld\n", (long) time (&t));
	return (long)time (&t);
}

//�̸��� ������ ����ð��� ������
double time_get_msec (void)
{
	double msec = 0;

	#ifdef __LINUX

		struct timeval tv;

		gettimeofday (&tv, NULL);	//timezone
		//printf ("** sec=%ld, usec=%ld\n", (long)tv.sec, (long)tv.usec);
		msec = (long)tv.tv_sec + (double)((long)tv.tv_usec) / 1000000;

	#else
    
		struct timeb timebuffer;
	    
		ftime (&timebuffer);
		//printf ("** sec=%ld, usec=%d\n", (long)timebuffer.time, (unsigned short)timebuffer.millitm);
		msec = (long)timebuffer.time + (double)((unsigned short)timebuffer.millitm) / 1000;
    
	#endif

	return msec;
}

void time_sleep (int sec)
{
	#ifdef __LINUX
		struct timeval tv;	
		tv.tv_sec = sec;
		tv.tv_usec = 0;
		select (0, NULL, NULL, NULL, &tv);	
		//sleep (sec);
	#else
		Sleep (sec * 1000);	//�ʴ���
	#endif
}
