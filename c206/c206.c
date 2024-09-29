/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/*           Daniel Dolejška, září 2022                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
** uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c206.h"

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error(void) {
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init( DLList *list ) {
	list->firstElement = NULL;
	list->lastElement = NULL;
	list->activeElement = NULL;
	list->currentLength = 0;
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose( DLList *list ) {
	DLLElementPtr currentElement = list->firstElement; // set the current element to be the first element

	while(currentElement != NULL) {
		DLLElementPtr temp = currentElement; // temporarily save the current element
		currentElement = currentElement->nextElement; // move onto to the next element
		free(temp); // free the temp element
	}

	list->firstElement = NULL;
	list->lastElement = NULL;
	list->activeElement = NULL;
	list->currentLength = 0;
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst( DLList *list, long data ) {
	DLLElementPtr newElement = malloc(sizeof(struct DLLElement)); // allocate memory for the new element
	if(newElement != NULL) {
		newElement->data = data; // set the data of the new element
		newElement->previousElement = NULL; // set the previous element of the new element to NULL
		newElement->nextElement = list->firstElement; // set the next element to be the first element of the list

		if(list->firstElement != NULL) {
			// if the first element is not NULL, set the previous element of the first element to be the new element
			list->firstElement->previousElement = newElement;
		} else {
			// if the first element is NULL, set the last element to be the new element
			list->lastElement = newElement;
		}

		list->firstElement = newElement; // set the first element to be the new element
		list->currentLength++; // increment the current length of the list
	} else {
		DLL_Error();
		return; // return if there is not enough memory
	}
}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k DLL_InsertFirst).
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast( DLList *list, long data ) {
	DLLElementPtr newElement = malloc(sizeof(struct DLLElement)); // allocate memory for the new element
	if(newElement != NULL) {
		newElement->data = data; // set the data of the new element
		newElement->previousElement = list->lastElement; // set the previous element of the new element to be the last element
		newElement->nextElement = NULL; // set the next element of the new element to NULL

		if(list->lastElement != NULL) {
			// if the last element is not NULL, set the next element of the last element to be the new element
			list->lastElement->nextElement = newElement;
		} else {
			// if the last element is NULL, set the first element to be the new element
			list->firstElement = newElement;
		}

		list->lastElement = newElement; // set the last element to be the new element
		list->currentLength++; // increment the current length of the list
	} else {
		DLL_Error();
		return; // return if there is not enough memory
	}
}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First( DLList *list ) {
	list->activeElement = list->firstElement; // set the active element to the first element
}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last( DLList *list ) {
	list->activeElement = list->lastElement; // set the active element to the last element
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst( DLList *list, long *dataPtr ) {
	if (list->firstElement != NULL) {
		*dataPtr = list->firstElement->data; // set the data pointer to the data of the first element
	} else {
		DLL_Error();
		return; // return if the list is empty
	}
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast( DLList *list, long *dataPtr ) {
	if (list->lastElement != NULL) {
		*dataPtr = list->lastElement->data;
	} else {
		DLL_Error();
		return;
	}
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst( DLList *list ) {
	if (list->firstElement != NULL) {
		DLLElementPtr temp = list->firstElement; // temporarily save the first element
		DLLElementPtr nextElement = list->firstElement->nextElement; // set the next element to be the next element of the first element
		list->firstElement = nextElement; // set the first element to be the next element
		free(temp); // free the first element
		
		if (list->firstElement != NULL) {
			// if the first element is not NULL, set the previous element of the first element to NULL
			list->firstElement->previousElement = NULL;
		} else {
			// if the first element is NULL, set the last element to NULL
			list->lastElement = NULL;
		}

		if (list->activeElement == temp) {
			// if the active element is the first element, set the active element to NULL
			list->activeElement = NULL;
		}

		list->currentLength--; // decrement the current length of the list
	}
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast( DLList *list ) {
	if(list->lastElement != NULL) {
		DLLElementPtr temp = list->lastElement; // temporarily save the last element
		DLLElementPtr previousElement = list->lastElement->previousElement; // set the previous element to be the previous element of the last element
		list->lastElement = previousElement; // set the last element to be the previous element
		free(temp); // free the last element
		
		if(list->lastElement != NULL) {
			// if the last element is not NULL, set the next element of the last element to NULL
			list->lastElement->nextElement = NULL;
		} else {
			// if the last element is NULL, set the first element to NULL
			list->firstElement = NULL;
		}

		if (list->activeElement == temp) {
			// if the active element is the last element, set the active element to NULL
			list->activeElement = NULL;
		}

		list->currentLength--; // decrement the current length of the list
	}
}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter( DLList *list ) {
	if(list->activeElement != NULL && list->activeElement != list->lastElement) {
		DLLElementPtr temp = list->activeElement->nextElement; // save the next element of the active element
		DLLElementPtr nextElement = list->activeElement->nextElement->nextElement; // set the next element of the next element
		list->activeElement->nextElement = nextElement; // set the next element of the active element to be the next element of the next element
		free(temp); // free the next element of the active element

		if(nextElement != NULL) {
			// if the next element is not NULL, set the previous element of the next element to be the active element
			nextElement->previousElement = list->activeElement;
		} else {
			// if the next element is NULL, set the last element to be the active element
			list->lastElement = list->activeElement;
		}

		list->currentLength--; // decrement the current length of the list
	}
}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore( DLList *list ) {
	if(list->activeElement != NULL && list->activeElement != list->firstElement) {
		DLLElementPtr temp = list->activeElement->previousElement; // save the previous element of the active element
		DLLElementPtr previousElement = list->activeElement->previousElement->previousElement; // set the previous element of the previous element
		list->activeElement->previousElement = previousElement; // set the previous element of the active element to be the previous element of the previous element
		free(temp); // free the previous element of the active element

		if(previousElement != NULL) {
			// if the previous element is not NULL, set the next element of the previous element to be the active element
			previousElement->nextElement = list->activeElement;
		} else {
			// if the previous element is NULL, set the first element to be the active element
			list->firstElement = list->activeElement;
		}

		list->currentLength--; // decrement the current length of the list
	}
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter( DLList *list, long data ) {
    if (list->activeElement != NULL) {
        DLLElementPtr newElement = malloc(sizeof(struct DLLElement)); // allocate memory for the new element
        if (newElement != NULL) {
            newElement->data = data; // set the data of the new element
            newElement->previousElement = list->activeElement; // set the previous element of the new element to be the active element
            newElement->nextElement = list->activeElement->nextElement; // set the next element of the new element to be the next element of the active element

            if (list->activeElement->nextElement != NULL) {
				// if the next element of the active element is not NULL, set the previous element of the next element to be the new element
                list->activeElement->nextElement->previousElement = newElement;
            } else {
				// if the next element of the active element is NULL, set the last element to be the new element
				list->lastElement = newElement;
			}

            list->activeElement->nextElement = newElement; // set the next element of the active element to be the new element
            list->currentLength++; // increment the current length of the list
        } else {
            DLL_Error();
			return; // return if there is not enough memory
        }
    }
}


/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore( DLList *list, long data ) {
    if (list->activeElement != NULL) {
        DLLElementPtr newElement = malloc(sizeof(struct DLLElement)); // allocate memory for the new element
        if (newElement != NULL) {
            newElement->data = data; // set the data of the new element
            newElement->nextElement = list->activeElement; // set the next element of the new element to be the active element
            newElement->previousElement = list->activeElement->previousElement; // set the previous element of the new element to be the previous element of the active element

            if (list->activeElement->previousElement != NULL) {
				// if the previous element of the active element is not NULL, set the next element of the previous element to be the new element
                list->activeElement->previousElement->nextElement = newElement;
            } else {
				// if the previous element of the active element is NULL, set the first element to be the new element
                list->firstElement = newElement;
            }

            list->activeElement->previousElement = newElement; // set the previous element of the active element to be the new element
            list->currentLength++; // increment the current length of the list
        } else {
            DLL_Error();
			return; // return if there is not enough memory
        }
    }
}


/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue( DLList *list, long *dataPtr ) {
	if(list->activeElement != NULL) {
		*dataPtr = list->activeElement->data; // if the active element exists, set the data pointer to the data of the active element
	} else {
		DLL_Error();
		return; // return if the list is not active
	}
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue( DLList *list, long data ) {
	if(list->activeElement != NULL) {
		list->activeElement->data = data; // if the active element exists, overwrite the data of the active element with the new data
	}
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next( DLList *list ) {
	if(list->activeElement != NULL) {
		list->activeElement = list->activeElement->nextElement; // if the active element exists, set the active element to the next element
	}
}


/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous( DLList *list ) {
	if(list->activeElement != NULL) {
		list->activeElement = list->activeElement->previousElement; // if the active element exists, set the active element to the previous element
	}
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
bool DLL_IsActive( DLList *list ) {
	return list->activeElement != NULL; // return 1 if the list is active, 0 otherwise
}

/* Konec c206.c */
