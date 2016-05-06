#include <stdio.h>

#define MAX_ROW	16
#define MAX_COL	16

int max(int a, int b)
{
	return a>b?a:b;
}

int main()
{
	char *str1 = "abcdaf";
	char *str2 = "xacbcf";
	int result_array[1+MAX_ROW][1+MAX_COL] = {0};
	int i = 0;
	int j = 0;

	while(*(str1+i) != '\0'){
		j = 0;
		while(*(str2+j) != '\0'){
			if(*(str1+i) == *(str2+j)){
				result_array[i+1][j+1] = result_array[i-1+1][j-1+1] + 1;
			}else{
				result_array[i+1][j+1] = max(result_array[i+1-1][j+1], result_array[i+1][j+1-1]); 
			}
			j++;
		}
		i++;
	}

	printf("result %d\n", result_array[i][j]);

	//Print letters...
	while(i != 0 && j != 0){
		if((result_array[i][j] != result_array[i-1][j]) && result_array[i][j] != result_array[i][j-1]){
			printf("[%c]\t", *(str2+j-1));
			i = i - 1;
			j = j - 1;
			continue;
		}else{
			if(result_array[i][j] == result_array[i-1][j]){
				i = i - 1;
			}else{
				j = j - 1;
			}
		}
	}
	printf("\n");
}
