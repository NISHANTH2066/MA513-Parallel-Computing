#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

int main(){

    int rank,size,root;
    int n=4,p=2,nsub=n/p;
    int dims[3]={p,p,p},wrap[3]={1,1,1},cartRank,grid[3];
    MPI_Comm mesh3d,meshij,meshik,meshjk,ringi,ringj,ringk;

    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    MPI_Cart_create(MPI_COMM_WORLD,3,dims,wrap,1,&mesh3d);

    dims[0]=1;dims[1]=0;dims[2]=0;
    MPI_Cart_sub(mesh3d,dims,&ringi);

    dims[0]=0;dims[1]=1;dims[2]=0;
    MPI_Cart_sub(mesh3d,dims,&ringj);

    dims[0]=0;dims[1]=0;dims[2]=1;
    MPI_Cart_sub(mesh3d,dims,&ringk);

    MPI_Comm_rank(mesh3d,&cartRank);
    MPI_Cart_coords(mesh3d,cartRank,3,grid);
    int I=grid[0],J=grid[1],K=grid[2];

    int *Asub=(int*)malloc(nsub*nsub*sizeof(int));
    int *Bsub=(int*)malloc(nsub*nsub*sizeof(int));
    int *Csub=(int*)calloc(nsub*nsub,sizeof(int));

    // From Diagram in Textbook
    if(J==K){//if(J==0){ //Can also do in this way
        // Bcast from (I,K,K)
        for(int i=0;i<nsub;i++) 
            for(int j=0;j<nsub;j++) Asub[i*nsub+j]=rand()%10;

        printf("A (%d,%d,%d) --> ",I,J,K);
        for(int i=0;i<nsub*nsub;i++) printf("%d ",Asub[i]);
        printf("\n");

    }

    if(I==K){//if(I==0){ //Can also do in this way
        // Bcast from (K,J,K)
        for(int i=0;i<nsub;i++) 
            for(int j=0;j<nsub;j++) Bsub[i*nsub+j]=rand()%10;

        printf("B (%d,%d,%d) --> ",I,J,K);
        for(int i=0;i<nsub*nsub;i++) printf("%d ",Bsub[i]);
        printf("\n");
    }

    MPI_Barrier(mesh3d);

    // recv from J==K
    // Always give 1st dimension for Bcast
    dims[0]=grid[2];dims[1]=grid[2];dims[2]=grid[2];
    MPI_Cart_rank(ringj,dims,&root);
    //printf("-- (%d,%d,%d) --> %d\n",I,J,K,root);
    MPI_Bcast(Asub,nsub*nsub,MPI_INT,root,ringj);

    // recv from I==K
    dims[0]=grid[2];dims[1]=grid[1];dims[2]=grid[2];
    MPI_Cart_rank(ringi,dims,&root);
    //printf("(%d,%d,%d) --> %d\n",I,J,K,root);
    MPI_Bcast(Bsub,nsub*nsub,MPI_INT,root,ringi);

    for(int i=0;i<nsub;i++)
        for(int j=0;j<nsub;j++)
            for(int k=0;k<nsub;k++)
                Csub[i*nsub+j]+=Asub[i*nsub+k]*Bsub[k*nsub+j];

    
    /*printf("A (%d,%d,%d) --> ",I,J,K);
    for(int i=0;i<nsub*nsub;i++) printf("%d ",Asub[i]);
    printf("\n");*/
    
    dims[0]=grid[0];dims[1]=grid[1];dims[2]=0;
    // Always give 1st dimension for Ring
    dims[0]=0;
    MPI_Cart_rank(ringk,dims,&root);
    int *Cf;
    Cf=(int*)malloc(nsub*nsub*sizeof(int));
    MPI_Reduce(Csub,Cf,nsub*nsub,MPI_INT,MPI_SUM,root,ringk);

    MPI_Barrier(mesh3d);

    if(K==0){
        printf("(%d,%d,%d) --> ",I,J,K);
        for(int i=0;i<nsub*nsub;i++) printf("%d ",Cf[i]);
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
