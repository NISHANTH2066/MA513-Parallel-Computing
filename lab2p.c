#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<mpi.h>

int n=1;

void sort(int *arr){
    for(int i=0;i<n;i++) for(int j=i+1;j<n;j++)
        if(arr[i]>arr[j]){
            int t=arr[i];
            arr[i]=arr[j];
            arr[j]=t;
        }
}

void merge_split(int *A0,int *A1){
    int *data=(int*)malloc(2*n*sizeof(int));
    int i=0,j=0,k=0;
    while(i<n && j<n){
        if(A0[i]<=A1[j]) data[k++]=A0[i++];
        else data[k++]=A1[j++];
    }
    while(i<n) data[k++]=A0[i++];
    while(j<n) data[k++]=A1[j++];
    for(int i=0;i<n;i++) A0[i]=data[i],A1[i]=data[n+i];
}

int main(int argc,char **argv){
    int rank,p;

    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&p);


    int *A0=(int*)malloc(n*sizeof(int));
    int *A1=(int*)malloc(n*sizeof(int));
    int *arr,*data=(int*)malloc(2*n*sizeof(int));
    int *temp=(int*)malloc(n*sizeof(int));

    if(rank==0){
        srand(time(NULL)+rank);
        arr=(int*)malloc(2*n*p*sizeof(int));
        for(int i=0;i<2*n*p;i++) arr[i]=rand()%5;
        for(int i=0;i<2*n*p;i++) printf("%d ",arr[i]);
        printf("\n");
    }
    MPI_Scatter(arr,2*n,MPI_INT,data,2*n,MPI_INT,0,MPI_COMM_WORLD);
    //printf("R=%d  %d %d\n",rank,data[0],data[1]);
    for(int i=0;i<n;i++) A0[i]=data[i],A1[i]=data[n+i];
    sort(A0);sort(A1);

    MPI_Status status;

    for(int i=0;i<2*p;i++){
        int left=rank-1,right=rank+1;
        if(left==-1) left=MPI_PROC_NULL;
        if(right==p) right=MPI_PROC_NULL;
        if(i&1){
            MPI_Sendrecv(A0,n,MPI_INT,left,1,temp,n,MPI_INT,right,1,
                    MPI_COMM_WORLD,&status);
            if(rank!=p-1) merge_split(A1,temp);
            MPI_Sendrecv(temp,n,MPI_INT,right,1,A0,n,MPI_INT,left,1,
                    MPI_COMM_WORLD,&status);
        }
        else{
            merge_split(A0,A1);
        }
        //printf("it=%d   R=%d  %d %d\n",i,rank,A0[0],A1[0]);
        for(int i=0;i<n;i++) data[i]=A0[i],data[i+n]=A1[i];
        MPI_Gather(data,2*n,MPI_INT,arr,2*n,MPI_INT,0,MPI_COMM_WORLD);
        if(rank==0){
            for(int i=0;i<2*n*p;i++) printf("%d ",arr[i]);
            printf("\n");
        }
    }
    for(int i=0;i<n;i++) data[i]=A0[i],data[i+n]=A1[i];
    MPI_Gather(data,2*n,MPI_INT,arr,2*n,MPI_INT,0,MPI_COMM_WORLD);
    if(rank==0){
        for(int i=0;i<2*n*p;i++) printf("%d ",arr[i]);
        printf("\n");
    }
    //printf("R=%d  %d %d\n",rank,A0[0],A1[0]);
    MPI_Finalize();
}
