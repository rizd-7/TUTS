# Pthread Tutorial

## Introduction
This repository serves as a tutorial for learning and practicing POSIX Threads (Pthreads) in C. Below are examples demonstrating various aspects of Pthreads, including thread creation, parameter passing, memory allocation, synchronization, and more.

---

## 1. Hello World Example
This is the simplest example of using Pthreads to print "Hello, world!" from multiple threads.

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 4

void* sayHello() {
    printf("hello world! \n");
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[N];
    for (int i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, sayHello, NULL);
    }
    pthread_exit(NULL);
}
```

---

## 2. Parameter Passing
In Pthreads, all functions must follow the signature `void* function(void* arg)`. Here's an example of passing parameters to threads:

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 4

void* sayHello(void* arg) {
    int* id = (int*)arg;
    printf("hello world from thread: %d \n", *id);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[N];
    int thread_ids[N];
    for (int i = 0; i < N; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, sayHello, (void*)&thread_ids[i]);
    }
    pthread_exit(NULL);
}
```

---

## 3. Struct Usage and Memory Allocation
### Example 1: Direct Declaration
```c
struct Person {
    char* name;
    int age;
};

int main() {
    struct Person p;
    p.name = "John";
    p.age = 30;
    printf("Name: %s, Age: %d\n", p.name, p.age);
    return 0;
}
```

### Example 2: Allocating Memory on the Heap
```c
#include <stdio.h>
#include <stdlib.h>

struct Person {
    char* name;
    int age;
};

int main() {
    struct Person* p = (struct Person*)malloc(sizeof(struct Person));
    p->age = 21;
    p->name = "RIAD";
    printf("Name: %s, Age: %d\n", p->name, p->age);
    free(p);
    return 0;
}
```

---

## 4. Multiple Argument Passing with Structs
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 4

struct Person {
    char* name;
    int age;
};

void* sayHello(void* arg) {
    struct Person* pers = (struct Person*)arg;
    printf("hello %s of age %d! \n", pers->name, pers->age);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[N];
    struct Person* p = (struct Person*)malloc(sizeof(struct Person));
    p->age = 21;
    p->name = "RIAD";

    for (int i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, sayHello, (void*)p);
    }
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    free(p);
    return 0;
}
```
---
## 6. Synchronization with Join 
Using a Join to prevent parent finishing before children:
example run this multiple time to see that at any time count is printed 

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 10

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int count=0;


void* sayHello(){
    printf("hello world! \n");

    pthread_mutex_lock (&mutex1);
    count ++;
    pthread_mutex_unlock (&mutex1);

    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[N];

    int i;
    for(i=0;i<N;i++){
        pthread_create(&threads[i],NULL,sayHello,NULL);
    }


    printf("%d ",count);

    pthread_exit(NULL);

    return 0;
}

```
Now run this ; 
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 10

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int count=0;


void* sayHello(){
    printf("hello world! \n");

    pthread_mutex_lock (&mutex1);
    count ++;
    pthread_mutex_unlock (&mutex1);

    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[N];

    int i;
    for(i=0;i<N;i++){
        pthread_create(&threads[i],NULL,sayHello,NULL);
    }

    for(i=0;i<N;i++){
        pthread_join(threads[i],NULL);
    }

    printf("%d ",count);


    return 0;
}
```


---

## 6. Synchronization with Mutex
Using a mutex to protect shared data and avoid race conditions:


```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 10

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int count = 0;

void* sayHello() {
    printf("hello world! \n");
    pthread_mutex_lock(&mutex1);
    count++;
    pthread_mutex_unlock(&mutex1);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[N];
    for (int i = 0; i < N; i++) {
        pthread_create(&threads[i], NULL, sayHello, NULL);
    }
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Count: %d\n", count);
    return 0;
}
```

---

## 7. Full Example: Array Processing with Threads
This example demonstrates dividing work among threads, using a mutex to update a global sum.

```c
#include <stdio.h>
#include <pthread.h>
#define SIZE 256
#define NUM_THREADS 4

double a[SIZE], b[SIZE];
double sum = 0.0;
pthread_mutex_t sum_mutex;

void* fill_and_compute(void* arg) {
    int thread_id = *(int*)arg;
    int start = (SIZE / NUM_THREADS) * thread_id;
    int end = (SIZE / NUM_THREADS) * (thread_id + 1);

    double partial_sum = 0.0;
    for (int i = start; i < end; i++) {
        a[i] = i * 0.5;
        b[i] = i * 2.0;
        partial_sum += a[i] * b[i];
    }

    pthread_mutex_lock(&sum_mutex);
    sum += partial_sum;
    pthread_mutex_unlock(&sum_mutex);

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    pthread_mutex_init(&sum_mutex, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, fill_and_compute, &thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("sum = %g\n", sum);
    pthread_mutex_destroy(&sum_mutex);
    return 0;
}
```

---

## Key Concepts Covered:
1. Thread creation and basic usage.
2. Passing arguments to threads.
3. Struct usage and memory allocation.
4. Synchronization with `pthread_mutex`.
5. Combining thread tasks (e.g., filling arrays and computation).
6. Proper cleanup using `pthread_join` and freeing allocated memory.

Feel free to modify these examples and experiment further!
