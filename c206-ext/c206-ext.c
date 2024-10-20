/*
 *  Předmět: Algoritmy (IAL) - FIT VUT v Brně
 *  Rozšíření pro příklad c206.c (Dvousměrně vázaný lineární seznam)
 *  Vytvořil: Daniel Dolejška, září 2024
 */

#include "c206-ext.h"

bool error_flag;
bool solved;

/**
 * Tato metoda simuluje příjem síťových paketů s určenou úrovní priority.
 * Přijaté pakety jsou zařazeny do odpovídajících front dle jejich priorit.
 * "Fronty" jsou v tomto cvičení reprezentovány dvousměrně vázanými seznamy
 * - ty totiž umožňují snazší úpravy pro již zařazené položky.
 * 
 * Parametr `packetLists` obsahuje jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Pokud fronta s odpovídající prioritou neexistuje, tato metoda ji alokuje
 * a inicializuje. Za jejich korektní uvolnení odpovídá volající.
 * 
 * V případě, že by po zařazení paketu do seznamu počet prvků v cílovém seznamu
 * překročil stanovený MAX_PACKET_COUNT, dojde nejdříve k promazání položek seznamu.
 * V takovémto případě bude každá druhá položka ze seznamu zahozena nehledě
 * na její vlastní prioritu ovšem v pořadí přijetí.
 * 
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param packet Ukazatel na strukturu přijatého paketu
 */
void receive_packet(DLList *packetLists, PacketPtr packet) {
    // pointer which will be used to traverse the list of packet lists
    DLLElementPtr currentNode = packetLists->firstElement;
    QosPacketListPtr qosList = NULL;

    // checks if there is an existing list with the same priority
    while (currentNode) {
        QosPacketListPtr currentList = (QosPacketListPtr)currentNode->data;
        if (currentList->priority == packet->priority) {
            qosList = currentList; // found a matching priority list, set it to qosList
            break;
        }
        currentNode = currentNode->nextElement; // move on to the next list if no match was found
    }

    // if we didnt find a list with the same priority, create a new one
    if (qosList == NULL) {
        qosList = (QosPacketListPtr)malloc(sizeof(QosPacketList));
        if (qosList != NULL) {
            qosList->priority = packet->priority;
            qosList->list = (DLList*)malloc(sizeof(DLList));
            // check if the memory was allocated correctly
            if (qosList->list != NULL) {
                DLL_Init(qosList->list); // initialize the list
                DLL_InsertLast(packetLists, (long)qosList); // add the list to the list of packet lists
            } else {
                free(qosList); // free the allocated memory
                return; // if the memory was not allocated correctly, return
            }
        } else {
            free(qosList); // free the allocated memory
            return; // if the memory was not allocated correctly, return
        }
    }

    // if the list is full, delete every other packet
    if (qosList->list->currentLength == MAX_PACKET_COUNT) {
        DLL_First(qosList->list);
        // loop through the list and delete every other packet
        for (int i = 0; DLL_IsActive(qosList->list) && i < MAX_PACKET_COUNT; i++) {
            if (i % 2 == 0) {
                DLL_DeleteAfter(qosList->list); // delete every other packet
            } else {
                DLL_Next(qosList->list); // move on to the next packet
            }
        }
    }
    DLL_InsertLast(qosList->list, (long)packet); // insert the packet to the list
}

/**
 * Tato metoda simuluje výběr síťových paketů k odeslání. Výběr respektuje
 * relativní priority paketů mezi sebou, kde pakety s nejvyšší prioritou
 * jsou vždy odeslány nejdříve. Odesílání dále respektuje pořadí, ve kterém
 * byly pakety přijaty metodou `receive_packet`.
 * 
 * Odeslané pakety jsou ze zdrojového seznamu při odeslání odstraněny.
 * 
 * Parametr `packetLists` obsahuje ukazatele na jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Parametr `outputPacketList` obsahuje ukazatele na odeslané pakety (`PacketPtr`).
 * 
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param outputPacketList Ukazatel na seznam paketů k odeslání
 * @param maxPacketCount Maximální počet paketů k odeslání
 */
void send_packets(DLList *packetLists, DLList *outputPacketList, int maxPacketCount) {
    int packetCnt = 0; // counter for the amount of packets sent

    // loop until we reach the maximum packet count
    while (packetCnt < maxPacketCount) {
        DLLElementPtr currentNode = packetLists->firstElement;
        QosPacketListPtr highestPriorityL = NULL;

        // find the list with the highest priority
        while (currentNode) {
            QosPacketListPtr currentList = (QosPacketListPtr)currentNode->data;
            if ((highestPriorityL == NULL || currentList->priority > highestPriorityL->priority) && currentList->list->currentLength > 0) {
                highestPriorityL = currentList; // set the list with the highest priority
            }
            currentNode = currentNode->nextElement; // move on to the next list
        }

        if (highestPriorityL != NULL) {
            long packetData;
            DLL_GetFirst(highestPriorityL->list, &packetData); // get the first packet from the list
            DLL_InsertLast(outputPacketList, packetData); // insert the packet to the output list at the last place
            DLL_DeleteFirst(highestPriorityL->list); // delete the first packet from the list
            packetCnt++; // increment the packet count
        } else {
            break; // if no list was found, break the loop
        }
    }
}