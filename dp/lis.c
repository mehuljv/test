
#include <stdio.h>

#define MAX_SEQ	128
//int g_seq[] = {1, -1, 3, 4, 5};
int g_seq[] = {10, 22, 9, 33, 21, 50, 41, 60, 80};

int main()
{
	int tmp[MAX_SEQ] = {0};
	int i = 0;
	int	num_ele = sizeof(g_seq)/sizeof(int);
	int j = 0;
	int max_seq = 1;
	int max_seq_index = 0;
	int expected_lis;
	int last_num_in_seq;

	for(i=0; i<num_ele; i++){
		tmp[i] = 1;
		for(j=0; j<i; j++){
			if(g_seq[j] < g_seq[i]){
				if(tmp[i] <= tmp[j])
					tmp[i] = tmp[j] + 1;
				if(tmp[i] > max_seq){
					max_seq = tmp[i];
					max_seq_index = i;
				}
			}
		}
		printf("Value %d ==> MaxSeq %d\n", g_seq[i], tmp[i]);
	}
	printf("max_seq %d, max_seq_index %d\n", max_seq, max_seq_index);

	expected_lis = max_seq;
	last_num_in_seq = g_seq[max_seq_index];
	for(i=max_seq_index; i>=0; i--){
		if(tmp[i] == expected_lis && tmp[i] <= last_num_in_seq){
			printf("%d\t", g_seq[i]);
			last_num_in_seq = g_seq[i];
			expected_lis--;
		}
	}
	printf("\n");
}
