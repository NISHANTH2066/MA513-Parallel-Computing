#include<stdio.h>
#include<time.h>
#include<mpi.h>
#include<math.h>
#include<stdlib.h>

int main(int argc,char **argv){
    int rank,P,p;
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&P);
    p=(int)sqrt(1.0*P);
    //if(rank==0) printf("%d\n",p);
    int n=2,nlocal=n/p;

    int *A=(int*)malloc(nlocal*nlocal*sizeof(int));
    int *B=(int*)malloc(nlocal*nlocal*sizeof(int));
    int *C=(int*)calloc(nlocal*nlocal,sizeof(int));

    srand(time(NULL)+rank);
    for(int i=0;i<nlocal*nlocal;i++) A[i]=rand()%10,B[i]=rand()%10;

    MPI_Comm cartComm;
    int ndims=2,dims[]={p,p},wrap[]={1,1},reorder=1;
    MPI_Cart_create(MPI_COMM_WORLD,ndims,dims,wrap,reorder,&cartComm);

    int right,left,up,down,my2drank,coords[2];
    MPI_Cart_shift(cartComm,1,-1,&right,&left);
    MPI_Cart_shift(cartComm,0,-1,&down,&up);

    MPI_Comm_rank(cartComm,&my2drank);
    MPI_Cart_coords(cartComm,my2drank,ndims,coords);

    for(int i=0;i<nlocal*nlocal;i++) printf("%d %d %d\n",my2drank,A[i],B[i]);

    int src,dest;
    MPI_Status status;
    MPI_Cart_shift(cartComm,1,-coords[0],&src,&dest);
    MPI_Sendrecv_replace(A,nlocal*nlocal,MPI_INT,dest,1,src,1,cartComm,&status);
    MPI_Cart_shift(cartComm,0,-coords[1],&src,&dest);
    MPI_Sendrecv_replace(B,nlocal*nlocal,MPI_INT,dest,1,src,1,cartComm,&status);

    int Nlocal=nlocal*nlocal;

    for(int _=0;_<p;_++){
        for(int i=0;i<nlocal;i++)
            for(int j=0;j<nlocal;j++)
               for(int k=0;k<nlocal;k++)
                  C[i*nlocal+j]+=A[i*nlocal+k]*B[k*nlocal+j]; 
        MPI_Sendrecv_replace(A,Nlocal,MPI_INT,left,1,right,1,cartComm,&status);
        MPI_Sendrecv_replace(B,Nlocal,MPI_INT,up,1,down,1,cartComm,&status);
    }

    printf("2dRank=%d (x,y)=(%d,%d) ",my2drank,coords[0],coords[1]);
    for(int i=0;i<Nlocal;i++) printf("%d ",C[i]);
    printf(" --> %d\n",rank);

    MPI_Finalize();
}
