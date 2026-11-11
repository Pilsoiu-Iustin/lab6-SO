
#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;

int main()
{
    // Cream memoria mapata
    HANDLE hMapFile = CreateFileMappingW(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(int),
        L"Local\\Lab6SO_Memory"
    );

    if (hMapFile == NULL)
    {
        cout << "Eroare la crearea memoriei mapate!\n";
        return 1;
    }

    // Accesam memoria
    int* numar = (int*)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        sizeof(int)
    );

    if (numar == NULL)
    {
        cout << "Eroare la accesarea memoriei!\n";
        CloseHandle(hMapFile);
        return 1;
    }

    // Initializam numarul
    *numar = 0;

    // Cream semaforul
    HANDLE hSemaphore = CreateSemaphoreW(
        NULL,
        1,
        1,
        L"Local\\Lab6SO_Semaphore"
    );

    if (hSemaphore == NULL)
    {
        cout << "Eroare la crearea semaforului!\n";
        UnmapViewOfFile(numar);
        CloseHandle(hMapFile);
        return 1;
    }

    // Cream evenimentul pentru sincronizarea pornirii
    HANDLE hEvent = CreateEventW(
        NULL,
        TRUE,
        FALSE,
        L"Local\\Lab6SO_Ready"
    );

    if (hEvent == NULL)
    {
        cout << "Eroare la crearea evenimentului!\n";
        CloseHandle(hSemaphore);
        UnmapViewOfFile(numar);
        CloseHandle(hMapFile);
        return 1;
    }

    cout << "Procesul 1 asteapta pornirea Procesului 2...\n";

    // Asteptam ca Procesul 2 sa fie pregatit
    WaitForSingleObject(hEvent, INFINITE);

    srand((unsigned int)time(NULL));

    while (true)
    {
        // Rezervam semaforul
        WaitForSingleObject(hSemaphore, INFINITE);

        if (*numar >= 1000)
        {
            ReleaseSemaphore(hSemaphore, 1, NULL);
            break;
        }

        // Dam cu banul: 1 sau 2
        while (*numar < 1000 && rand() % 2 + 1 == 2)
        {
            (*numar)++;

            cout << "Procesul 1: " << *numar << endl;
        }

        // Eliberam semaforul
        ReleaseSemaphore(hSemaphore, 1, NULL);

        Sleep(10);
    }

    cout << "Procesul 1 s-a terminat.\n";

    CloseHandle(hEvent);
    CloseHandle(hSemaphore);
    UnmapViewOfFile(numar);
    CloseHandle(hMapFile);

    return 0;
}
