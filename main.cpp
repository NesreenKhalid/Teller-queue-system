#include <iostream>
#include <stdlib.h>
#include "Goto.c"
#define X0 30
#define Y0 8
#define DELTA_Y 2
#define UP -72
#define DOWN -80
#define LEFT -75
#define RIGHT -77
#define ENTER 13
#define EXIT 111
#define MENU_SIZE 3
#define GREEN 10
#define WHITE 15
#define NO_OF_DISKS 3
using namespace std;
template <typename T>
struct LinkedList{
    T* first;
    T* last;
    int noOfNodes;
    LinkedList();
    void add(T* newNode);

    void insertAfter(T* newNode, T* prevNode);
    void insertBefore(T* newNode, T* nextNode);
    void remove(T* pRemove);
    void Delete(T* pDelete);
    void selectSort();
};
struct Interrupt{
    Interrupt *next, *prev;
    int time;
    int period;
//    Interrupt();
//    Interrupt(const int& _arrival, const int& _transaction);
};
struct Client{
    Client *next, *prev;
    int arrival;
    int arrivalToQueue;
    int transaction;
    LinkedList<Interrupt> interrupts;
    Client();
    Client(const int& _arrival, const int& _transaction);
};
struct Disk{
    LinkedList<Client> clients;
    int waitingTime;
    int totalTransTime;
    int Departure;
    int noOfClients;
    int interruptFlag;
    Disk();
};
void getClient(Client* client);
void getInterruptValues(Client& client);
void drawMenu(int x, int y);
char getKey();
void newClient(LinkedList<Client>& list);
void enterKey(LinkedList<Client>& list, int& exit);
void excuteInput(const char& input, LinkedList<Client>& list, int& exit);
int getShortestQ(Disk d[]);
int getLongestQ(Disk d[]);
void endService(Disk& disk);
void handleNextClient(Disk& disk);
void handleInterrupt(Disk& disk);
void switchDisk(Disk& longest, Disk& shortest);
void assignClientToDisk(Disk& disk , Client* client);
int getDiskToBeLeft(Disk d[]);
void calcBankReport(Disk d[]);
void printReports(Disk d[]);
void printDiskReport(const Disk& d, int x, int y);
void printBankReport(int x, int y);

///////////////////////////////////////////////////////////////////////
char menu[MENU_SIZE+1][50] = {
    "Menu: (Navigate using arrows)",
    "1- New",
    "2- Finish",
    "3- Exit"
};
int lineNo = 1;
/////////////////
int totalClients = 0;
int totalBankWaiting = 0;
int totalBankTrans = 0;
int remainingClients;
int eventTime = 0;
LinkedList<Client> inputClients;
////////////////////////////////////////////////////////////////////////
int main()
{
    int exit = 1;
    char input;
    int shortest, longest, toBeLeft =0 ;
    Disk d[NO_OF_DISKS];

    do{
        drawMenu(X0,Y0);
        input = getKey();
        excuteInput(input, inputClients, exit);
        if(exit == EXIT){
            system("cls");
            SetColor(WHITE);
            return 0;
        }
    } while(exit);

    Client* nextArriving = inputClients.first;
    totalClients = inputClients.noOfNodes;
    remainingClients = totalClients;
    while(remainingClients){
        nextArriving = inputClients.first;
        if(nextArriving != NULL  && nextArriving->arrival < d[toBeLeft].Departure){
            eventTime = nextArriving->arrival;
            assignClientToDisk(d[getShortestQ(d)] , nextArriving);
        }
        else if (d[toBeLeft].interruptFlag == 0){
            eventTime = d[toBeLeft].Departure;
            endService(d[toBeLeft]);
            shortest = getShortestQ(d);
            longest = getLongestQ(d);
            if(d[shortest].clients.noOfNodes < d[longest].clients.noOfNodes-1)
                switchDisk(d[longest], d[shortest]);
        }
        else{
            eventTime = d[toBeLeft].Departure;
            handleInterrupt(d[toBeLeft]);
        }
        toBeLeft = getDiskToBeLeft(d);
    }
    calcBankReport(d);
    printReports(d);
    system("cls");

    return 0;
}
/////////////////////////////////////////////////////////////////////

Client :: Client(){
    next = prev = NULL;
    arrival = transaction = 0;
}
Client :: Client(const int& _arrival, const int& _transaction){
    arrival = _arrival;
    transaction = _transaction;
}
template <typename T>
LinkedList<T> :: LinkedList(){
    first = last = NULL;
    noOfNodes = 0;
}
template <typename T>
void LinkedList<T> :: add(T* newNode){
    if(first == NULL)
        first = last = newNode;
    else{
        last -> next = newNode;
        newNode -> prev = last;
        last = newNode;
    }
    noOfNodes++;
}
template <typename T>
void LinkedList<T> ::  insertAfter(T* newNode, T* prevNode){
        if(prevNode != last){
            newNode -> prev = prevNode;
            newNode -> next = prevNode -> next;
            (prevNode -> next) -> prev = newNode;
            prevNode -> next = newNode;
        }
        else
            add(newNode);
        noOfNodes++;
}
template <typename T>
void LinkedList<T> ::  insertBefore(T* newNode, T* nextNode){
    if(nextNode != first){
        newNode -> next = nextNode;
        newNode -> prev = nextNode -> prev;
        (nextNode -> prev) -> next = newNode;
        nextNode -> prev = newNode;
    }
    else{
        newNode -> next = first;
        first = newNode;
        (newNode -> next) -> prev = newNode;
    }
    noOfNodes++;
}
template <typename T>
void LinkedList<T> :: remove(T* pRemove){
    if(pRemove == first){
        if(pRemove == last)
            first = last = NULL;
        else{
            first = pRemove -> next;
            first -> prev = NULL;
        }
    }
    else if(pRemove == last){
        last = pRemove -> prev;
        last -> next = NULL;
    }
    else{
        (pRemove -> prev) -> next = pRemove -> next;
        (pRemove -> next) -> prev = pRemove -> prev;
    }
    pRemove -> next = pRemove -> prev = NULL;
    noOfNodes--;
}
template <typename T>
void LinkedList<T> :: Delete(T* pDelete){
    remove(pDelete);
    delete [] pDelete;
}
template <typename T>
void LinkedList<T> :: selectSort(){

    T *i = first, *j, *min;
    while(i != NULL){
        min = i;
        j = i->next;
        while(j != NULL){
            if(j->arrival < min->arrival)
                min = j;
            j = j -> next;
        }

    if(min != i){
        remove(min);
        insertBefore(min, i);
    }
    else
        i = i -> next;
    }

}
Disk :: Disk(){
    waitingTime = 0;
    totalTransTime = 0;
    Departure = 999999;
    noOfClients = 0;
    interruptFlag = 0;
}
void drawMenu(int x, int y){
    system("cls");
    for(int i = 0; i < MENU_SIZE+1; i++){
        gotoxy(x, y);

        if(i == lineNo)
            SetColor(GREEN);
        else
            SetColor(WHITE);

        printf("%s", menu[i]);
        y+=DELTA_Y;
    }
}
void getClient(Client* client){
    system("cls");
    char form [2][20] = {
        "Arrival time: ",
        "Transaction time: "
    };
    // getting x distance for the cursor
    int lengths [2];
    for(int i = 0; i < 2; i++){
        lengths[i] = strlen(form[i]);
    }
    int x = X0, y = Y0; // starting coordinates
    for(int i = 0; i < 2; i++){
        gotoxy(x, y);
        printf("%s", form[i]);
        y = y + DELTA_Y;
    }
    y = Y0;
    gotoxy(x + lengths[0], y);
    cin >> client->arrival;

    y += DELTA_Y;
    gotoxy(x + lengths[1], y);
    cin >> client->transaction;

}
int getShortestQ(Disk d[]){
    int shortestQ = 0;
    for(int i=0; i<NO_OF_DISKS; i++)
        if(d[shortestQ].clients.noOfNodes > d[i].clients.noOfNodes)
            shortestQ = i;
    return shortestQ;
}
int getLongestQ(Disk d[]){
    int longestQ = 0;
    for(int i=0; i<NO_OF_DISKS; i++)
        if(d[longestQ].clients.noOfNodes < d[i].clients.noOfNodes)
            longestQ = i;
    return longestQ;
}
char getKey(){
    char input;
    input = getch();
    if(input != -32)
    {
        return input;
    }
    else
    {
        input = getch();
        return input*-1;
    }
}
void excuteInput(const char& input, LinkedList<Client>& list, int& exit){
    switch(input){
    case UP:
        lineNo--;
        break;
    case DOWN:
        lineNo++;
        break;
    case ENTER:
        enterKey(list, exit);
        break;
    default:
        break;
    }
    if(lineNo > MENU_SIZE) lineNo = 1;
    if(lineNo < 1) lineNo = MENU_SIZE;

}
void enterKey(LinkedList<Client>& list, int& exit){
    switch(lineNo){
    case 1: //new
        newClient(list);
        break;
    case 2: //finish
        exit = 0;
        list.selectSort();
        system("cls");
        break;
    case 3: //exit
        exit = EXIT;
        break;
    default:
        break;
    }
}
void newClient(LinkedList<Client>& list){
    Client* client = new Client;
    getClient(client);
    //ask for interrup
    getInterruptValues(*client);
    list.add(client);
}
void getInterruptValues(Client& client){
    int exit = 1;
    do{

        printf("\n\t\t\tAdd interrupt?! \t [y/n]");
        char input = getche();
        if(input == 'y' || input == 'Y'){
            system("cls");
            char form [2][20] = {
                "Interrupt time: ",
                "Interrupt period: "
            };
            // getting x distance for the cursor
            int lengths [2];
            for(int i = 0; i < 2; i++){
                lengths[i] = strlen(form[i]);
            }
            int x = X0, y = Y0; // starting coordinates
            for(int i = 0; i < 2; i++){
                gotoxy(x, y);
                printf("%s", form[i]);
                y = y + DELTA_Y;
            }
            y = Y0;
            gotoxy(x + lengths[0], y);
            Interrupt* interrupt = new Interrupt;
            cin >> interrupt->time;

            y += DELTA_Y;
            gotoxy(x + lengths[1], y);
            cin >> interrupt->period;

            client.interrupts.add(interrupt);
        }
        else if(input == 'n' || input == 'N')
            exit = 0;
    } while(exit);
}
void endService(Disk& disk){
    disk.totalTransTime += disk.clients.first->transaction;
    disk.clients.Delete(disk.clients.first);
    remainingClients--;
    if(disk.clients.first == NULL)
            disk.Departure = 999999;
    else handleNextClient(disk);
}
void handleInterrupt(Disk& disk){
    Client* interruptedClient = disk.clients.first;
    disk.totalTransTime += disk.clients.first->interrupts.first->time;
    interruptedClient->transaction = interruptedClient->transaction -(eventTime - interruptedClient->arrival);
    interruptedClient->arrival = eventTime + interruptedClient->interrupts.first->period;
    interruptedClient->interrupts.Delete(interruptedClient->interrupts.first);
    disk.clients.remove(interruptedClient);
    inputClients.add(interruptedClient);
    inputClients.selectSort();

    if(disk.clients.first == NULL)
            disk.Departure = 999999;
    else handleNextClient(disk);
}
void handleNextClient(Disk& disk){
        disk.waitingTime += eventTime - disk.clients.first->arrivalToQueue;
        if(disk.clients.first->interrupts.noOfNodes != 0){
            disk.interruptFlag = 1;
            disk.Departure = eventTime + disk.clients.first->interrupts.first->time;
        }
        else{
            disk.interruptFlag = 0;
            disk.Departure = eventTime + disk.clients.first->transaction;
        }
}
void switchDisk(Disk& longest, Disk& shortest){
    Client* movingClient;
    longest.waitingTime += eventTime - longest.clients.last->arrivalToQueue;
    movingClient = longest.clients.last;
    longest.clients.remove(movingClient);
    assignClientToDisk(shortest ,movingClient);
}
void assignClientToDisk(Disk& disk , Client* client){
    if(client == inputClients.first)
        inputClients.remove(client);
    disk.clients.add(client);
    disk.noOfClients++;
    client->arrivalToQueue = eventTime;
    if(client == disk.clients.first)
        handleNextClient(disk);
}
int getDiskToBeLeft(Disk d[]){
    int minIndex = 0;
    for(int i=0; i<NO_OF_DISKS; i++){
        if(d[i].Departure < d[minIndex].Departure){
            minIndex = i;
        }
    }
    return minIndex;
}
void calcBankReport(Disk d[]){
    for(int i=0; i<NO_OF_DISKS ; i++){
        totalBankTrans += d[i].totalTransTime;
        totalBankWaiting += d[i].waitingTime;
    }

}
void printReports(Disk d[]){
    int x, y, i = 0;
    char input;
    do{

        x = X0; y = Y0;
        system("cls");
        gotoxy(x,y);
        printf("Navigate using arrows [<--] [-->], Press [Enter] to exit");
        y += DELTA_Y;
        gotoxy(x,y);
        SetColor(GREEN);
        if (i != NO_OF_DISKS){
            printf("Disk [%d]:",i+1);
            y++;
            gotoxy(x,y);
            printf("--------");
            y += DELTA_Y;
            gotoxy(x,y);
            SetColor(WHITE);
            printDiskReport(d[i], x, y);
        }
        else{
            printf("Bank:");
            y++;
            gotoxy(x,y);
            printf("-----");
            y += DELTA_Y;
            gotoxy(x,y);
            SetColor(WHITE);
            printBankReport(x,y);
        }
        input = getKey();
        switch(input){
        case RIGHT:
            i++;
            if(i > NO_OF_DISKS)
                i = 0;
            break;
        case LEFT:
            i--;
            if(i < 0)
                i = NO_OF_DISKS;
            break;
        default:
            break;
        }

    } while(input != 13);
}
void printDiskReport(const Disk& d, int x, int y){
    gotoxy(x,y);
    cout << "\tTotal number of clients = " << d.noOfClients;
    y+= DELTA_Y;
    gotoxy(x,y);
    cout << "\tTotal transaction time = " << d.totalTransTime;
    y+= DELTA_Y;
    gotoxy(x,y);
    cout << "\tTotal waiting time = " << d.waitingTime;
    if(d.noOfClients != 0){
        y+= DELTA_Y;
        gotoxy(x,y);
        cout << "\tAverage transaction time = " << float(d.totalTransTime)/d.noOfClients;
        y+= DELTA_Y;
        gotoxy(x,y);
        cout << "\tAverage waiting time = " << float(d.waitingTime)/d.noOfClients;
    }
    else{
        y+= DELTA_Y;
        gotoxy(x,y);
        cout << "\tAverage transaction time = 0";
        y+= DELTA_Y;
        gotoxy(x,y);
        cout << "\tAverage waiting time = 0";
    }
}
void printBankReport(int x, int y){
    gotoxy(x,y);
    cout << "\tTotal number of clients = " << totalClients;
    y+= DELTA_Y;
    gotoxy(x,y);
    cout << "\tTotal transaction time = " << totalBankTrans;
    y+= DELTA_Y;
    gotoxy(x,y);
    cout << "\tTotal waiting time = " << totalBankWaiting;
    if(totalClients != 0){
        y+= DELTA_Y;
        gotoxy(x,y);
        cout << "\tAverage transaction time = " << float(totalBankTrans)/totalClients;
        y+= DELTA_Y;
        gotoxy(x,y);
        cout << "\tAverage waiting time = " << float(totalBankWaiting)/totalClients;
    }
    else{
        y+= DELTA_Y;
        gotoxy(x,y);
        cout << "\tAverage transaction time = 0";
        y+= DELTA_Y;
        gotoxy(x,y);
        cout << "\tAverage waiting time = 0";
    }
}
