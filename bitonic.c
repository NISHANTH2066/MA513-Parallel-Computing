#include<stdio.h>
#include<omp.h>
#include<stdlib.h>

const int ASCENDING=1;
const int DESCENDING=0;

void compare(int i,int j,int *arr,int dir){
    if(dir==(arr[i]>arr[j])){
        int temp=arr[i];
        arr[i]=arr[j];
        arr[j]=temp;
    }
}

void sortBitonicSeq(int first,int last,int *arr,int dir){
    int n=last-first+1;
    for(int j=n/2;j>0;j/=2){
        int cnt=0;
        for(int i=first;i+j<=last;i++){
            if(cnt<j){
                cnt++;
                compare(i,i+j,arr,dir);
            }else{
                cnt=0;
                i=i+j-1;
            }
        }
    }
}

void bitonicSort(int first,int last,int* arr){
    int n=last-first+1;
    for(int j=2;j<=n;j*=2){
        #pragma omp parallel for
        for(int i=0;i<n;i+=j){
            int t=i/j;
            if(t%2==0) sortBitonicSeq(i,i+j-1,arr,ASCENDING);
            else sortBitonicSeq(i,i+j-1,arr,DESCENDING);
        }
    }
}

int main(){
    int no_threads=omp_get_num_procs();
    omp_set_num_threads(no_threads);
    int n;
    printf("No. of elements :: ");
    scanf("%d",&n);
    int *arr=(int*)malloc(n*sizeof(int));
    printf("Initial arr :: ");
    for(int i=0;i<n;i++) arr[i]=rand()%100,printf("%d ",arr[i]);
    printf("\n");
    double stime,etime;
    stime=omp_get_wtime();
    bitonicSort(0,n-1,arr);
    etime=omp_get_wtime();
    printf("Sorted arr  :: "); for(int i=0;i<n;i++) printf("%d ",arr[i]);
    printf("\n");
    printf("Time taken :: %f\n",etime-stime);
}
