//Progetto API 2023
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define LEN 512
#define MAX_DIM 100000

typedef struct Stazione {
    int distanza;
    int cont;
    int id;
    int parco[LEN];
    int max_autonomia;

    struct Stazione* prev;
    struct Stazione* next;
} Stazione;

typedef struct Autostrada {
    int n_staz;
    Stazione* Head;
    Stazione* Tail;
} node_autostrada;

//creo un vettore statico ordinato per salvare le distanze delle stazioni
typedef struct vettore{
    int distanza_km;
    Stazione* pointer;
} vettore_distanza;

void inizializzazione_autostrada(node_autostrada *list){
    list->n_staz=0;
    list->Tail = NULL;
    list->Head = NULL;
}

int legginum (FILE* file) {
    char c = getc_unlocked(file);
    int num = 0;
    while (c >= '0' && c <= '9') {
        num = num * 10 + (c - '0');
        c = getc_unlocked(file);
    }
    return num;
}

int binary_search_distanze(vettore_distanza v[], int low, int high, int key) {

    if(high==-1) //caso base della lista vuota con n_staz = 0, devo interrompere la ricerca e ritornare lo 0
        return 0;

    if (high-low<=1) { //caso base della binary_search
        if(v[high].distanza_km<=key)
            return high;
        return low;
    }
    else {
        //codice binary search...
        int medium = (low + high) / 2;
        if (v[medium].distanza_km == key)
            return medium;
        else if (v[medium].distanza_km > key)
            return binary_search_distanze(v, low, medium, key); // devo cercare a sx essendo il medio più grande della key
        else
            return binary_search_distanze(v, medium , high, key); //cerco a dx
    }
}

int trova_max(int v[],int dim){ //CERCARE LA MASSIMA AUTONOMIA NEL PARCO AUTO
    int max = -1;
    for(int i=0; i<dim;i++) {
        if (v[i] >= max) {
            max = v[i];
        }
    }
    return max;
}

Stazione *creastazione(int dist, int numero, int v[]){

    // creo nuovo elemento
    Stazione *newNode = malloc(sizeof(Stazione));

    if(newNode!= NULL) {
        newNode->distanza = dist;
        newNode->cont = numero;
        newNode->id = -1; //fino a che non uso Dijkstra non mi occorre
        // salvo i valori delle autonomie
        for (int i = 0; i < numero; i++)
            newNode->parco[i] = v[i];

        if(newNode->cont==0) {
            newNode->max_autonomia = -1; //non c'è nessuna auto
        }
        else {
            int max = trova_max(v, numero);
            newNode->max_autonomia = max;
        }
        newNode->prev = NULL;
        newNode->next = NULL;
    }
    return newNode;
}

int aggiungiauto(Stazione *newcar, int new_autonomia){
    int i;
    i=newcar->cont;

    if(i>=512) //spazio nel parco non disponibile
        return -1;

    //aggiungo la nuova auto al parco
    newcar->parco[i]=new_autonomia;
    newcar->cont++; //aggiorno il numero di macchine

    //aggiorno massimo se necessario
    if(new_autonomia > newcar->max_autonomia){
        newcar->max_autonomia = new_autonomia;
        return 1;
    }

    return 1;
}

int rimuoviauto(Stazione *dem_car, int automobile) {

    //nel parco non sono presenti auto
    if(dem_car->cont == 0)
        return -1;

    int i = 0;
    int trovato = 0;

    //ricerca auto...
    while (i < dem_car->cont && trovato == 0) {

        if (dem_car->parco[i] == automobile)
            trovato = 1;
        else
            i++;
    }

    if (i >= dem_car->cont) {
        //autonomia non trovata
        //printf("\nauto non aggiunta poichè assente nella stazione");
        return -1;
    }

    if (automobile != dem_car->max_autonomia) { //non tocco max rimuovo elem e basta
        //sposto gli elementi indietro di 1
        int j;
        for (j = i; j < dem_car->cont; j++)
            dem_car->parco[j] = dem_car->parco[j + 1];

        dem_car->cont = dem_car->cont - 1;
        return 1;
    }
    else { // rimuovo max, devo ritrovarlo
        //sposto gli elementi indietro di 1
        int j;
        for (j = i; j < dem_car->cont; j++)
            dem_car->parco[j] = dem_car->parco[j + 1];

        dem_car->cont = dem_car->cont - 1;
        //ricalcolo il massimo
        int newmax = trova_max(dem_car->parco,dem_car->cont);
        dem_car->max_autonomia = newmax;
        return  1;
    }
}

void aggiungistazione_sorted(node_autostrada* L,vettore_distanza v[],Stazione *new) {

    //Caso 1: base in cui non uso binary search avendo n_staz == 0; Lista vuota
    // il nuovo nodo fara' sia da TESTA che da CODA
    if (L->n_staz == 0) {
        new->prev = NULL;
        L->Head = new;
        L->Tail = new;
        printf("aggiunta\n");
        v[0].pointer=new;
        v[0].distanza_km = new->distanza;
        L->n_staz = L->n_staz + 1;
        return;
    }
    // se esiste già almeno un elemento nella lista...

    int res;
    res = binary_search_distanze(v, 0, L->n_staz - 1, new->distanza);

    if (v[res].distanza_km == new->distanza) {
        //Stazione già presente, non devo aggiungere
        printf("non aggiunta\n");
        return;
    }
    else {
        //devo inserire prima di ogni elemento successivo al res per garantire ordine nel vettore
        //distinguo il caso in cui res sia 0 o diverso da 0
        if (res == 0 && v[res].distanza_km > new->distanza) {
            for (int i = L->n_staz - 1; i >= res; i--) {
                v[i + 1] = v[i];
            }
            //aggiungo elemento nel vettore e collego pointer
            v[res].distanza_km = new->distanza;
            v[res].pointer = new;
        } else {
            for (int i = L->n_staz - 1; i >= res; i--) {
                v[i + 1] = v[i];
            }
            //aggiungo elemento nel vettore e collego pointer
            v[res + 1].distanza_km = new->distanza;
            v[res + 1].pointer = new;
        }

        //collego elemento nuovo nella lista...
        // CASO 2: la lista contiene già almeno un elemento, devo però inserire new come NUOVA TESTA avendo una key che è il minimo
        if (new->distanza < (L->Head)->distanza) {
            new->next = L->Head;
            (L->Head)->prev = new;
            L->Head = new;
            printf("aggiunta\n");
            //Tail non viene aggiornata
        }
            //CASO 3:
            // aggancio il nuovo nodo in mezzo alla lista o in coda
        else {
            Stazione *tmp;
            tmp = v[res].pointer;

        //AGGANCIO IL NUOVO ELEMENTO

        //controllo se devo aggiornare TAIL
            if (tmp->next != NULL) {
                new->next = tmp->next;
                new->prev = tmp;
                (tmp->next)->prev = new;
            }
            else {
                // Caso in cui l'elemento viene inserito dopo l'ultimo elemento
                new->prev = L->Tail;
                (L->Tail)->next = new;
                L->Tail = new;
            }
            tmp->next = new;
            printf("aggiunta\n");
        }
        L->n_staz = L->n_staz + 1;//aggiorno col numero di stazioni
    }
}

//funzione per eliminare una stazione aggiornando il vettore
int vettore_elimina_distanza(node_autostrada* L, vettore_distanza v[],int dist){

    //gestisco caso base quando non ho stazioni nella lista
    if(L->n_staz == 0)
        return -1; //produce output "non rottamata"

    //elimino la stazione

    int m;
    m = binary_search_distanze(v,0,L->n_staz-1,dist);

    if(v[m].distanza_km != dist)
        return -1; //elemento non trovato nella binary search

    else {
        //Dealloco elemento dalla lista...

        Stazione *tocanc;
        tocanc = v[m].pointer;

        //codice per eliminare dalla stazione

        if (L->Head == tocanc) { //elimino testa
            L->Head = tocanc->next;
        }

        if (L->Tail == tocanc) { //elimino coda
            L->Tail = tocanc->prev;
        }

        // Aggiornamento del puntatore dell'elemento precedente
        if (tocanc->prev != NULL) {
            (tocanc->prev)->next = tocanc->next;
        }

        // Aggiornamento del puntatore dell'elemento successivo
        if (tocanc->next != NULL) {
            (tocanc->next)->prev = tocanc->prev;
        }

        free(tocanc); //dealloco la memoria dinamica

        // scorro in avanti tutti gli elementi per eliminare la cella dal vettore delle distanze
        for (int i = m; i < L->n_staz-1; i++) // dall'elemento trovato della binary search scorro in avanti elimando la cella del vettore
            v[i] = v[i+1];

        L->n_staz = L->n_staz - 1; // aggiorno nuovo numero di stazioni
        return 1;
    }
}

//Uso algoritmo di Dijkstra per calcolare il percorso ma con un'ottimizzazione, per ogni nodo corrente guardo solo gli adiacenti mai raggiunti dai correnti precedenti, il quanto il peso di ogni arco raggiungibile è un +1
void Dijkstra_avanti (Stazione* partenza, Stazione* arrivo) {

    int numero_nodi = 0;
    Stazione *tmp = partenza;

    int i;

    for (i = 1; tmp != arrivo && tmp!= NULL; tmp = tmp->next)
        i++;
    numero_nodi = i;  //numero nodi tra partenza e arrivo (inclusi estremi)

    //Inizializzo le id delle numero_nodi stazioni totali
    int j = 0;
    tmp = partenza;
    int occorre_per_stampare[numero_nodi];

    //assegno gli id ai nodi da 0 a numero nodi per memorizzare ogni stazione
    while (j < numero_nodi && tmp != NULL) {
        tmp->id = j;
        occorre_per_stampare[j]=tmp->distanza;
        j++;
        tmp = tmp->next;
    }

//Inizializzazione e definizione array e valori per Dijkstra
    int distance[numero_nodi];
    int stampa[numero_nodi];
    int predecessori[numero_nodi];
    int max_autonomia;
    int minDistance;
    Stazione *current = partenza;
    Stazione *last_visited=partenza;

    //Imposto Dijkstra avendo un DAG (lo creo virtualmente "collegando i nodi")

    //Inizializzazione nodi "grafo"

    for (int i = 0; i < numero_nodi; i++) {
        distance[i] = INT_MAX; //distanza infinita
        predecessori[i] = -1; //nessun predecessore
    }

    distance[partenza->id] = 0; //Inizializzo a zero il peso della sorgente che ha partenza->id == 0
    int fine = 0;

    //per ogni vertice u preso in ordine topologico, garantito dall'inserimento ordinato della funzione aggiungi_stazione_sorted
    while (current != NULL && current->distanza <= arrivo->distanza && fine == 0) {
        if (current != partenza && predecessori[current->id] == -1) { // se non ho trovato un collegamento con un corrente termino
            fine = 1;
            break;
        }
        if (current->cont > 0) { // se esiste almeno una macchina per spostarmi

            max_autonomia = current->max_autonomia;

            if (max_autonomia > 0) { //la stazione ha la macchina MAX con autonomia positiva

                /*una volta che ho raggiunto una stazione non serve controllare se riesco a raggiungerla con un'altra
                ;il numero di passi non potrà diminuire visto che il salto tra le raggiungibili costa +1, e da sx a dx non ho problemi di scegliere la piu vicina */

                Stazione *toVisit;
                toVisit = last_visited->next;  //CONTROLLO SOLO GLI ADIACENTI NON VISITATI DAI CURRENT IN PRECEDENZA

                //considero tutti i nodi v adiacenti a u, ovvero quelli raggiungibili
                while (toVisit != NULL && toVisit->distanza - current->distanza <= max_autonomia && toVisit->distanza <= arrivo->distanza) { //condizione di "raggiungibilità"

                    last_visited=toVisit; // Stazione raggiunta

                    //Relax step Dijkstra
                    minDistance = distance[current->id] +1; // peso equivalente ai salti necessari per arrivare a una stazione considero un + 1 ogni volta che prendo una nuova stazione
                    if (minDistance < distance[toVisit->id]) {
                        distance[toVisit->id] = minDistance;
                        predecessori[toVisit->id] = current->id;
                    }

                    toVisit = toVisit->next; // esploro tutti i possibili adiacenti del curr;
                }
            }
        }
        current = current->next;
    }

    if (predecessori[arrivo->id] == -1) {
        printf("nessun percorso\n");
        return;
    } else {
        // codice per stampare il percorso....
        stampa[0] = arrivo->distanza;
        //printf("\narrivo alla distanza %d", arrivo->distanza);
        int curr;
        curr = predecessori[arrivo->id];
        //Stazione* tmp;
        int j = 1;
        //printf("\n la penultima stazione ha id %d e distanza %d", curr, tmp->distanza);
        stampa[j] = occorre_per_stampare[curr];
        j++;

        while (curr > 0) {
            curr = predecessori[curr];
            //printf("\n poi la stazione ha id %d e distanza %d", curr, tmp->distanza);
            stampa[j] = occorre_per_stampare[curr];
            j++;
        }

        int cont = j-1;
        //printf("\n");
        while (cont > 0) {
            printf("%d ", stampa[cont]);
            cont--;
        }
        printf("%d",stampa[cont]);
        printf("\n");
    }
}

void Dijkstra_indietro(Stazione* partenza, Stazione* arrivo) {
    Stazione *tmp;
    tmp = partenza; //distanza maggiore, nella lista partenza è a dx di arrivo

    //seguo lo stesso ragionamento di Inizializzazione...
    int i;
    for (i = 1; tmp != arrivo && tmp != NULL; tmp = tmp->prev)
        i++;
    int numero_nodi;
    numero_nodi = i;
    //printf("\n nodi compresi tra partenza e arrivo %d",numero_nodi);

    //Inizializzo le id delle numero_nodi stazioni totali
    int j = 0;
    tmp = partenza;
    int occorre_per_stampare[numero_nodi];

    while (j < numero_nodi && tmp != NULL) {
        tmp->id = j;
        occorre_per_stampare[j]=tmp->distanza;
        j++;
        tmp = tmp->prev;
    }

    int distance[numero_nodi];
    int stampa[numero_nodi];
    int predecessori[numero_nodi];
    int max_autonomia;
    int minDistance;
    Stazione *current = partenza;

    //Imposto Dijkstra avendo un DAG

    //Inizializzazione "grafo"

    for (int i = 0; i < numero_nodi; i++) {
        distance[i] = INT_MAX; //distanza infinita
        predecessori[i] = -1; //nessun predecessore
    }

    distance[partenza->id] = 0; //Inizializzo a zero il peso della sorgente (partenza)

    int fine = 0;
    //Stazione* save = current;
    Stazione* toVisit;

    while (current != NULL && current->distanza >= arrivo->distanza && fine == 0) {

        if (current != partenza && predecessori[current->id] == -1) { // se non ho trovato un collegamento termino
            fine = 1;
            break;
        }
        if (current->cont > 0) { // se esiste almeno una macchina per spostarmi
            max_autonomia = current->max_autonomia;

            if (max_autonomia > 0 ) { //elimino caso limite in cui c'è una stazione con unica macchina ad autonomia nulla
                //considero tutti i nodi v adiacenti a u, ovvero quelli raggiungibili
                toVisit = current->prev;
                while (toVisit != NULL && current->distanza - toVisit->distanza <= max_autonomia && toVisit->distanza >= arrivo->distanza){
                    minDistance = distance[current->id] + 1; //numero passi
                    //Relax step
                    if (minDistance < distance[toVisit->id]) {
                        distance[toVisit->id] = minDistance;
                        predecessori[toVisit->id] = current->id;
                    }
                    //controllo sulla distanza minore per i pari-merito
                    if (minDistance == distance[toVisit->id] && current->id > predecessori[toVisit->id])
                        predecessori[toVisit->id] = current->id;

                    toVisit = toVisit->prev; // cerco tra tutti i nodi raggiungibili
                }
            }
        }
        current = current->prev;
    }
    // codice per stampare il percorso...
    if (predecessori[arrivo->id] == -1) {
        printf("nessun percorso\n");
        return;
    } else {
        // codice per stampare il percorso....
        stampa[0] = arrivo->distanza;
        //printf("\narrivo alla distanza %d", arrivo->distanza);
        int curr;
        curr = predecessori[arrivo->id];
        //Stazione *tmp;
        int j = 1;
        //printf("\n la penultima stazione ha id %d e distanza %d", curr, tmp->distanza);
        stampa[j] = occorre_per_stampare[curr];
        j++;

        while (curr > 0) {
            curr = predecessori[curr];
            //printf("\n poi la stazione ha id %d e distanza %d", curr, tmp->distanza);
            stampa[j] = occorre_per_stampare[curr];
            j++;
        }
        int cont = j-1;
        // printf("\n");
        while (cont > 0) {
            printf("%d ", stampa[cont]);
            cont--;
        }
        printf("%d",stampa[cont]);
        printf("\n");
    }
}


int main() {

    //printf("\nCostruisco la mia autostrada\n");
    node_autostrada autostrada;
    inizializzazione_autostrada(&autostrada);

    Stazione* last_dist = NULL;
    FILE *file =stdin/* fopen("archivio_test_aperti/open_11.txt","r")*/;
    char comando[50];

    int distanza, autonomia;
    vettore_distanza *v=malloc(MAX_DIM*sizeof (vettore_distanza));
    int distanza_partenza;
    int distanza_arrivo;
    int numero_auto;
    int autonomie[512];
    //int max_autonomia = 0; mi serve in futuro per max autonomia

//lettura da stdin per i dati
    while (1) {
        char c = getc_unlocked(file);

        if (c == EOF)
            break;

        int idx = 0;
        while (c != ' ' && c != '\n') {
            comando[idx++] = c;
            c = getc_unlocked(file);
        }
        comando[idx] = '\0';

        //aggiunta stazione...
        if (strcmp(comando, "aggiungi-stazione") == 0) {
            distanza = legginum(file);
            numero_auto = legginum(file);
            for (int j = 0; j < numero_auto; j++) {
                autonomie[j] = legginum(file);
            }
            Stazione* new = creastazione(distanza,numero_auto,autonomie);
            aggiungistazione_sorted(&autostrada,v,new);
        }

        //demolizione stazione...
        else if (strcmp(comando, "demolisci-stazione") == 0) {
            distanza = legginum(file);
            if (vettore_elimina_distanza(&autostrada,v,distanza)==1)
                printf("demolita\n");
            else
                printf("non demolita\n");
        }

        //aggiungere auto nel parco di una stazione, ottimizzata salvando l'ultima stazione in cui è stata aggiunta auto
        else if (strcmp(comando, "aggiungi-auto") == 0) {
            distanza = legginum(file);
            autonomia = legginum(file);
            if(last_dist != NULL && last_dist->distanza == distanza) {
                if (aggiungiauto(last_dist, autonomia) == 1)
                    //printf("\n auto aggiunta alla stazione %d con autonomia %d ", distanza, autonomia);
                    printf("aggiunta\n");
                else
                    printf("non aggiunta\n");
            }

            else {
                Stazione *stazione_auto;
                if (autostrada.n_staz == 0)
                    printf("non aggiunta\n");
                else {
                    stazione_auto = v[binary_search_distanze(v, 0, autostrada.n_staz - 1, distanza)].pointer;
                    last_dist = stazione_auto;
                    if (stazione_auto->distanza == distanza) {
                        if (aggiungiauto(stazione_auto, autonomia) == 1) {
                            //printf("\n auto aggiunta alla stazione %d con autonomia %d ", distanza, autonomia);
                            printf("aggiunta\n");
                        } else
                            printf("non aggiunta\n"); //limiti spazio parco
                    } else
                        printf("non aggiunta\n");
                }
            }
        }

        //rimuovere auto nel parco di una stazione
        else if (strcmp(comando, "rottama-auto") == 0) {
            distanza = legginum(file);
            autonomia = legginum(file);
            Stazione *stazione_auto_canc;
            if(autostrada.n_staz==0)
                printf("non rottamata\n");
            else {
                stazione_auto_canc = v[binary_search_distanze(v, 0, autostrada.n_staz - 1, distanza)].pointer;
                if (stazione_auto_canc->distanza == distanza) { //binary search trova effettivamente la stazione cercata
                    if (rimuoviauto(stazione_auto_canc, autonomia) == 1)
                        printf("rottamata\n");
                    else
                        printf("non rottamata\n");
                } else
                    printf("non rottamata\n");
            }
        }

        //calcolo del percorso minimo..
        else if (strcmp(comando, "pianifica-percorso") == 0) {

            distanza_partenza = legginum(file);
            distanza_arrivo = legginum(file);
            Stazione *par;
            Stazione *arr;
            par = v[binary_search_distanze(v, 0, autostrada.n_staz - 1, distanza_partenza)].pointer;
            arr = v[binary_search_distanze(v, 0, autostrada.n_staz - 1, distanza_arrivo)].pointer;

            if(arr != NULL && par != NULL){
                if (distanza_partenza <= distanza_arrivo)
                        Dijkstra_avanti(par, arr);
                else
                    Dijkstra_indietro(par, arr);
            }
        }
    }
    free(v);
    return 0;
}

