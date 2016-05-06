#include <stdio.h>

int g_coins[] = {7,2,3,6};
int g_target = 13;
int g_found_way = 0;
int tmp_array[100][100] = {0};

int min(int a, int b)
{
	return a>b?b:a;
}

int count_coins(int m, int target)
{
	int a, b;
	int x;	

	if(tmp_array[m][target] != 0){
		return tmp_array[m][target];
	}

	if(target == 0){
		g_found_way++;
		return 0;
	}

	if(target < 0 )
		return g_target;

	if(m == 0 && target)
		return g_target;

	a = count_coins(m, target - g_coins[m-1]);
	b = count_coins(m-1, target);

	x = min(a,b);

	if( x == a){
		tmp_array[m][target] = x+1;
		return x+1;
	}else{
		tmp_array[m][target] = x;
		return x;
	}
}

int main()
{
	printf("Count Coins %d\n", count_coins(4, g_target));
//	printf("Found way %d\n", g_found_way);
}
