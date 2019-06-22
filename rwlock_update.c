#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM 1000

// Declare mutex, condition vars etc. necessary to implement 'readwrite lock'
// <-- Write your code here

typedef struct {
	int readers;
	int writer;
	pthread_cond_t readers_proceed;
	pthread_cond_t writer_proceed;
	int pending_writers;
	pthread_mutex_t read_write_lock;
} mylib_rwlock_t;

typedef struct{
	int id;
	mylib_rwlock_t *l;
} mydata;

// Implement a function rwlock_init()
// to initialise mutex, condition vars etc. that you decleared 
// earlier to implement 'readwrite lock'
void rwlock_init(mylib_rwlock_t *l)
{
	// <--Write your code here
	l->readers = l->writer = l->pending_writers = 0;
	pthread_mutex_init(&(l -> read_write_lock), NULL);
	pthread_cond_init(&(l -> readers_proceed), NULL);
	pthread_cond_init(&(l -> writer_proceed), NULL);
}

// Implement the functiontions readlock(), writelock() and unlock()
void readlock(mylib_rwlock_t *l)
{
	//printf("Read lock %d %d\n",l->readers,l->writer);
	// <-- Write your code here
	pthread_mutex_lock(&(l->read_write_lock));
	while((l->writer>0) || (l->pending_writers>0))
		pthread_cond_wait(&(l->readers_proceed),&(l->read_write_lock));
	l->readers++;
	pthread_mutex_unlock(&(l->read_write_lock));
}

void writelock(mylib_rwlock_t *l)
{
	//printf("Write lock %d %d\n",l->readers,l->writer);
	// <-- Write your code here	
	pthread_mutex_lock(&(l->read_write_lock));
	l->pending_writers++; 
	//***************//
	// pending writers outside the loop
	// because of spurious wakeups
	//***************//
	while((l->readers>0) || (l->writer>0)){
		//l->pending_writers++;
		pthread_cond_wait(&(l->writer_proceed),&(l->read_write_lock));
		//l->pending_writers--;
	}
	l->pending_writers--;
	l->writer++;
	pthread_mutex_unlock(&(l->read_write_lock));
}

void unlock(mylib_rwlock_t *l)
{
	//printf("Unlock %d %d\n",l->readers,l->writer);
	// <-- Write your code here
	pthread_mutex_lock(&(l->read_write_lock));
	if(l->writer>0) l->writer=0;
	else if(l->readers>0) l->readers--;
	pthread_mutex_unlock(&(l->read_write_lock));
	
	if((l->readers==0) && (l->pending_writers>0))
		pthread_cond_signal(&(l->writer_proceed));
	else if(l->readers>=0) // Also careful with ifelse just use if if
		pthread_cond_broadcast(&(l->readers_proceed));
		
	//***************//
	// Readers >=0 I think only ==0 will suffice
	//***************//
	
	//pthread_cond_signal(&(l->writer_proceed));
	//pthread_cond_broadcast(&(l->readers_proceed));
		
	/*if((l->readers==0) || (l->pending_writers>0))
		pthread_cond_signal(&(l->writer_proceed));
	else if(l->readers>=0)
		pthread_cond_broadcast(&(l->readers_proceed));*/
}


struct node { int d; struct node *n; } *list;
void insert(int d)
{
	//printf("To insert: %d\n",d);
    struct node *x = (struct node *)malloc(sizeof(struct node));
	x->d = d; x->n = list; list = x;
}

struct node* search(int d)
{
	//printf("To search: %d\n",d);
	struct node *nd;
	for(nd = list; (nd != NULL)&&(nd->d != d); nd = nd->n);
	return nd;
}

void testlist()
{
	for(int j=0; j<100; j++) insert(j);
	struct node *x;
	for(int j=0; j<110; j++)
       	{
	       	x = search(j); printf("%d:",j);
		if (x == NULL) printf("NOT FOUND, ");
	       	else printf("%d ,",x->d);
	}
	puts("\n");
}


void *foo(void *x)
{
	//int id = *((int *)x);
	
	mydata* d = ((mydata *) x);
	int id=d->id;
	mylib_rwlock_t *l=d->l;
	
	printf("Thread Id: %d\n",id);
	
	int i, j = -1;
	for(i = 0; i < 10*NUM; i++){
		//printf("Thread %d i=%d\t",id,i);
		if(i%10 == 0)
		{
			//printf("Writing %d %d\n",id,1000*id+j);
			writelock(l);
			j++; insert (1000*id+j); 
			unlock(l);
			
		}
		else
		{			
			//printf("Reading %d %d\n",id,1000*id+j);
			readlock(l);
			search (1000*id+j);
			unlock(l);
		}
	}
		
	printf("Ending Thread Id: %d\n",id);
	pthread_exit(NULL);
}

int main()
{
	list = NULL;
	// testlist();

	mylib_rwlock_t *rwl = (mylib_rwlock_t*)malloc(sizeof(mylib_rwlock_t));
	rwlock_init(rwl);
	
	int i, a[5];
	mydata d[5];

	pthread_t t[5];
	//for(i = 0; i < 5; i++) {a[i] = i; pthread_create(&t[i],NULL,foo,&a[i]);}
	
	for(i = 0;i < 5;i++){
		d[i].id=i;
		d[i].l=rwl;
		pthread_create(&t[i],NULL,foo,&d[i]);
	}
	for(i = 0; i < 5; i++){
		pthread_join(t[i],NULL);
		printf("Thread %d joined\n",i);
	}

		
	int count = 0;
	for(struct node *nd = list; nd != NULL; nd = nd->n) { printf("%d, ",nd->d); count++;}
	puts("");
	if(count == 5*NUM) printf("OK\n"); else printf("ERROR\n");
}
