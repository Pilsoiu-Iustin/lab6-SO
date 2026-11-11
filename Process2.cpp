
#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;

int main()
{
    // Deschidem memoria mapata creata de Procesul 1
    HANDLE hMapFile = OpenFileMappingW(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        L"Local\\Lab6SO_Memory"
    );

    if (hMapFile == NULL)
    {
        cout << "Eroare la deschiderea memoriei mapate!\n";
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

    // Deschidem semaforul creat de Procesul 1
    HANDLE hSemaphore = OpenSemaphoreW(
        SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
        FALSE,
        L"Local\\Lab6SO_Semaphore"
    );

    if (hSemaphore == NULL)
    {
        cout << "Eroare la deschiderea semaforului!\n";
        UnmapViewOfFile(numar);
        CloseHandle(hMapFile);
        return 1;
    }

    // Deschidem evenimentul creat de Procesul 1
    HANDLE hEvent = OpenEventW(
        EVENT_MODIFY_STATE,
        FALSE,
        L"Local\\Lab6SO_Ready"
    );

    if (hEvent == NULL)
    {
        cout << "Eroare la deschiderea evenimentului!\n";
        CloseHandle(hSemaphore);
        UnmapViewOfFile(numar);
        CloseHandle(hMapFile);
        return 1;
    }

    // Anuntam Procesul 1 ca suntem pregatiti
    cout << "Procesul 2 este pregatit!\n";
    SetEvent(hEvent);

    srand((unsigned int)time(NULL) + GetCurrentProcessId());

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

            cout << "Procesul 2: " << *numar << endl;
        }

        // Eliberam semaforul
        ReleaseSemaphore(hSemaphore, 1, NULL);

        Sleep(10);
    }

    cout << "Procesul 2 s-a terminat.\n";

    CloseHandle(hEvent);
    CloseHandle(hSemaphore);
    UnmapViewOfFile(numar);
    CloseHandle(hMapFile);

    return 0;
}
