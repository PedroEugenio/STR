#include <stdio.h>
#include <math.h>

int N = 3;
int n = 0;

float C[3] = {30,20,4};
float w[3][10];
float T[3] = {200, 83.3, 20};
float R[3];

struct hp{
	int p[2];
};

struct hp hp[3];

//int hp[1][2] = {2,3};
//int hp[2][1] = {3};
//int hp[3][1] = {0};

float aux=0;

int main(){
	
	hp[0].p[0]=2;
	hp[0].p[1]=3;
	hp[1].p[0]=3;
	hp[2].p[0]=0;
	for(int i=0; i<N; i++){
		n=0;
		w[i][n]=C[i];
		//printf("w[%i][%i]=%f\n",i,n,w[i][n]);		
		while(1){
			for(int j=0; j < hp[i].p[i]; j++){
				aux = ceil(w[i][n]/T[j])*C[j];
				printf("j-%i, hp-%i\n", j, hp[i].p[i]);
			}
			w[i][n+1] = C[i] + aux;
			if(w[i][n+1] == w[i][n]){
				R[i]=w[i][n];
				printf("R[%i]=%f\n",i,R[i]);
				break;
			}
			
			if(w[i][n+1] > T[i]){
				printf("O processo %i nao cumpre a meta!\n", i);
				break;
			}
			
			n = n+1;		
		} 	
	}

return 0;
}

