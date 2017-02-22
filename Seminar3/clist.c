#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

// list will consist of values between 0 and 999
#define MAX 1000

// definition of list-nodes
typedef struct cell {
	int val;
	struct cell *next;
	pthread_mutex_t mutex;
} cell;

/* 	
	two nodes that makes sure that the list is never empty (dummy), and prevents
	us from running past the end of the list without checking (sentinel)
*/
cell sentinel = {MAX, NULL, PTHREAD_MUTEX_INITIALIZER};
cell dummy = {-1, &sentinel, PTHREAD_MUTEX_INITIALIZER};

cell *global = &dummy;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void toggle(cell *lst, int r) {
	cell *prev = lst;
	pthread_mutex_lock(&prev->mutex);
	cell *this = prev->next;
	pthread_mutex_lock(&this->mutex);

	cell *removed = NULL;

	//pthread_mutex_lock(&mutex);

	// Sökning i listan
	while(this->val < r) {
		pthread_mutex_unlock(&prev->mutex);
		prev = this;
		this = this->next;
		pthread_mutex_lock(&this->mutex);
	}

	// Nod med värde r hittas, tar bort den
	if(this->val == r) {
		prev->next = this->next;
		removed = this;
	} else {
		// Nod med värde r hittas inte, skapa den
		cell *new = malloc(sizeof(cell));
		new->val = r;
		new->next = this;
		pthread_mutex_init(&new->mutex, NULL);
		prev->next = new;
		new = NULL; // vad gör den här?
	}
	pthread_mutex_unlock(&prev->mutex);
	pthread_mutex_unlock(&this->mutex);
	if(removed != NULL) free(removed);
	return;
}

typedef struct {
	int inc; 
	int id; 
	cell *list;
} args;

void *bench(void *arg) {
	int inc = ((args*)arg)->inc;
	int id = ((args*)arg)->id;
	cell *lstp = ((args*)arg)->list;

	for(int i = 0; i < inc; i++) {
		int r = rand() % MAX;
		toggle(lstp,r);
	}
}

int main(int argc, char *argv[]) {
	struct timespec t_start, t_stop;

	if(argc != 3) {
		printf("usage: list <total><threads>\n");
		exit(0);
	}
	int n = atoi(argv[2]);
	int inc = (atoi(argv[1]) / n);

	printf("%d threads doing %d operations each \n", n, inc);

	pthread_mutex_init(&mutex, NULL);

	args *thra = malloc(n * sizeof(args));

	for(int i = 0; i < n; i++) {
		thra[i].inc = inc;
		thra[i].id = i;
		thra[i].list = global;
	}

	pthread_t *thrt = malloc(n * sizeof(pthread_t));
	for(int i = 0; i < n; i++) {
		pthread_create(&thrt[i], NULL, bench, &thra[i]);
	}

	clock_gettime(CLOCK_MONOTONIC_COARSE, &t_start);

	for(int i = 0; i < n; i++) {
		pthread_join(thrt[i], NULL);
	}

	clock_gettime(CLOCK_MONOTONIC_COARSE, &t_stop);

	long wall_sec = t_stop.tv_sec - t_start.tv_sec;
	long wall_nsec = t_stop.tv_nsec - t_start.tv_nsec;
	long wall_msec = (wall_sec *1000) + (wall_nsec / 1000000);

	printf("done in %ld ms\n", wall_msec);
	return 0;
}
