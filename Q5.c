#include <pthread.h>
#include <stdio.h>
#define NUM_ITER (10000000)
#define N (6)

//Matriz deve diagonalmente dominante e também é bom que haja solução única
double A[N][N] = {{10, -1,  2,  0,  0,  0},
                  {-1, 11, -1,  3,  0,  0},
                  { 2, -1, 10, -1,  0,  0},
                  { 0,  3, -1,  8, -1,  0},
                  { 0,  0,  0, -1,  6, -1},
                  { 0,  0,  0,  0, -1,  5}},
       b[N] = {5, 6, 7, 8, 9, 10}, 
       x[NUM_ITER+1][N];

pthread_barrier_t barrier;
 

static inline double compute_xi(int k, int i){
    double new_xi = b[i];
    for (int j = 0; j < N; j++){
        if (i == j) continue;
        new_xi -= A[i][j]*x[k-1][j];
    }
    return new_xi/A[i][i];
}

void* solve(void* args){
    int* start_end_array = (int *) args;
    int start = start_end_array[0], end = start_end_array[1];

    for (int k = 1; k <= NUM_ITER; k++){
        for (int i = start; i < end; i++){
            x[k][i] = compute_xi(k, i);
        }
        pthread_barrier_wait(&barrier);        
    }
}

int main(){
    for (int i = 0; i < N; i++)
        x[0][i] = 1;
    
    puts("Quantos cores voce possui?");
    int num_cores; scanf("%d", &num_cores);            
    int args[num_cores][2]; 
    int num_th = 0; 
    {   //Definindo range em x que cada thread será responsavel
        int ratio = N/num_cores, offset = N % num_cores;
        for (int i = 0; i < num_cores; i++){
            args[i][0] = ((i == 0)? 0: args[i-1][1]);
            args[i][1] = args[i][0] + ratio + (offset-- >= 1);
            if (args[i][0] != args[i][1])
                num_th++;
            else
                break;
            //printf("%d %d\n", args[i][0], args[i][1]);
        }
    }   

    printf("Usando %d threads\n", num_th);
    pthread_t th[num_th];
    pthread_barrier_init(&barrier, NULL, num_th);
    for (int i = 0; i < num_th; i++)
        pthread_create(th+i, NULL, solve, (void*) args[i]);

    for (int i = 0; i < num_th; i++)
        pthread_join(th[i], NULL);

    pthread_barrier_destroy(&barrier);
    
    puts("Os valores de X:");
    for (int k = NUM_ITER; k < NUM_ITER+1; k++){
        printf("k: %d\n", k);
        for (int i = 0; i < N; i++)
            printf("%lf ", x[k][i]);
        putchar('\n');
    }

}
