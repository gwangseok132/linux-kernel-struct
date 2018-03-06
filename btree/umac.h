//
//  Source: umac.h written by Jung,JaeJoon at the www.kernel.bz
//  Compiler: Standard C
//  Copyright(C): 2010, Jung,JaeJoon(rgbi3307@nate.com)
//
//  Summary:
//		2011-03-11 ��ũ�� �Լ����� �ۼ��ϴ�. (��ũ�� ��Ī()�� ���� ���δ�)
//		yyyy-mm-dd ...
//

//#include <stdio.h>
//#define um_dprint (param)  printf (#param " = %d\n", param)

#define um_max(a, b)  (a > b) ? a : b
#define um_min(a, b)  (a < b) ? a : b

#define um_square(x)  x * x

#define um_paste(p1, p2)  p1 ## p2

//���� ���ΰ�? (. ? !)
#define um_end(c)  (c==46 || c==63 || c==33) ? 1 : 0

//ȭ��Ʈ�����̽� �ΰ�?
#define um_whites(c)  (c > 0 && c < 33) ? 1 : 0

//�����ΰ�?
#define um_digit(c)  (c > 47 && c < 58) ? 1 : 0
