#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int n,p,nsub;
int *A,*B,*C;
int *tC;

struct index{
    int x,y,z;
};

int d2(int i,int j){
    return i*nsub+j;
}

int d3(int i,int j,int k){
    return i*nsub*nsub+j*nsub+k;
}

int func1(int I,int J,int i,int j){
    //return (I*p+J)*nsub*nsub+i*nsub+j;
    return I*n*nsub+J*nsub+i*n+j;
    //Be careful with indexing
}

int func2(int I,int J,int K,int i,int j){
    //return (I*p*p+J*p+K)*nsub*nsub+i*nsub+j;
    //This is wrong implement this func again
}


void *f(void *data){
    struct index *arg = (struct index *)data;
    int I=arg->x;
    int J=arg->y;
    int K=arg->z;

    /*printf("%d %d %d --> ",I,J,K);
    for(int i=0;i<nsub;i++){
        for(int j=0;j<nsub;j++){
            printf("(%d,%d)",A[func1(I,J,i,j)],B[func1(J,K,i,j)]);
        }
    }
    printf("\n");*/

    int tempC[nsub][nsub];

    for(int i=0;i<nsub;i++){
        for(int j=0;j<nsub;j++){
            tempC[i][j]=0;
            for(int k=0;k<nsub;k++){
                //tempC[i][j]+=A[i][k]*B[k][j];
                tempC[i][j]+=A[func1(I,K,i,k)]*B[func1(K,J,k,j)];
            }
            //tC[(I*p*p+J*p+K)*nsub*nsub+i*nsub+j]=tempC[i][j];
            C[func1(I,J,i,j)]+=tempC[i][j];
        }
    }

    pthread_exit(NULL);
}

int main(){
    printf("Enter matrix size : ");scanf("%d",&n);
    printf("Enter no of threads : ");scanf("%d",&p);
    A=(int*)malloc(sizeof(int)*n*n);
    B=(int*)malloc(sizeof(int)*n*n);
    C=(int*)calloc(n*n,sizeof(int));
    for(int i=0;i<n*n;i++) A[i]=rand()%10;
    for(int i=0;i<n*n;i++) B[i]=rand()%10;

    for(int i=0;i<n*n;i++) printf("%d ",A[i]);
    printf("\n");
    for(int i=0;i<n*n;i++) printf("%d ",B[i]);
    printf("\n");

    nsub=n/p;
    tC=(int*)malloc(sizeof(int)*nsub*nsub*p*p*p);

    pthread_t pid[p*p*p];
    struct index *arr=(struct index*)malloc(p*p*p*sizeof(struct index));

    for(int i=0;i<p;i++){
        for(int j=0;j<p;j++){
            for(int k=0;k<p;k++){
                int I=i*p*p+j*p+k;
                arr[I].x=i;
                arr[I].y=j;
                arr[I].z=k;
                pthread_create(&pid[I],NULL,f,&arr[I]);
            }
        }
    }

    for(int i=0;i<p;i++){
        for(int j=0;j<p;j++){
            for(int k=0;k<p;k++){
                pthread_join(pid[i*p*p+j*p+k],NULL);
            }
        }
    }

    /*for(int I=0;I<p;I++){ for(int J=0;J<p;J++){ for(int K=0;K<p;K++){
        for(int i=0;i<nsub;i++){ for(int j=0;j<nsub;j++){
            C[func1(I,J,i,j)]+=tC[func2(I,J,K,i,j)];
        }}
    }}}*/

    for(int i=0;i<n*n;i++) printf("%d ",C[i]);
    printf("\n");
    return 0;
}
