#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    int shm_fd = shm_open("/memorie_comuna", O_RDWR, 0666);

    if (shm_fd == -1) {
        perror("Eroare shm_open");
        return 1;
    }

    int* memorie = (int*)mmap(NULL, sizeof(int),
        PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t* sem = sem_open("/semafor_comun", 0);

    if (sem == SEM_FAILED) {
        perror("Eroare sem_open");
        return 1;
    }

    srand(time(NULL) + getpid());

    while (true) {
        sem_wait(sem);

        if (*memorie >= 1000) {
            sem_post(sem);
            break;
        }

        if (rand() % 2 + 1 == 2) {
            (*memorie)++;
            cout << "Procesul 2: " << *memorie << endl;
        }

        sem_post(sem);
        usleep(10000);
    }

    cout << "Procesul 2 s-a terminat." << endl;

    munmap(memorie, sizeof(int));
    close(shm_fd);
    sem_close(sem);

    return 0;
}
