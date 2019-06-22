#include<pthread.h>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>

int n,t;
int* arr;

void *f(void *s){
	int *ptr=(int*)s;
	int id=*ptr;
	id++;
	int start=(n/t)*(id-1);
	int end=(n/t)*id-1;
	if(id==t) end=n-1;
	int psum=0;
	for(int i=start;i<end+1;i++){
		psum+=arr[i];
	}
	//printf("%d %d, st=%d end=%d\t",psum,id,start,end);
	*ptr=psum;
	pthread_exit(0);
}

int main(){
	int ans=0,sum=0;
	scanf("%d",&n);
	arr=(int*)malloc(sizeof(int)*n);
	for(int i=0;i<n;i++) arr[i]=rand()%10,ans+=arr[i];
	int *chunk;
	scanf("%d",&t);
	pthread_t *tid=(pthread_t*)malloc(sizeof(pthread_t)*t);
	chunk=(int*)malloc(sizeof(int)*t);
	for(int i=0;i<t;i++){
		chunk[i]=i;
		pthread_create(&tid[i],NULL,f,(void*)&chunk[i]);
	}
	for(int i=0;i<t;i++){
		pthread_join(tid[i],NULL);
		sum+=chunk[i];
	}
	printf("\n%d %d\n",ans,sum);
}
