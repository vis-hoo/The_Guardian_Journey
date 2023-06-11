#include <algorithm>
#include <bits/stdc++.h>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <vector>
using namespace std;
// Valores de poder Máximo y de Aprendiz.
#define MAX_POWER 100
#define APPRENTICE_POWER MAX_POWER / 2;

// Región que contiene las estructuras y clases necesarias para el manejo de la información.
#pragma region Structures and Classes

// Declaración de estructuras.
struct Village;
struct Guardian;
struct TrainingHistory;
struct JourneyHistory;

// Estructura de Aldea. Contiene nombre, si ya ha sido visitada,
// si el entrenamiento ha sido finalizado, y un vector de guardianes.
struct Village{
    string name;
    bool visited;
    bool trainingFinished;
    vector<Guardian*> *guardiansInVillage;
};

// Estructura de Guardián. Contiene nombre, poder, un puntero a su primer aprendiz,
// un puntero a sus compañeros aprendices de un mismo maestro, y la aldea a la que pertenece.
struct Guardian{
    string name;
    int power;
    Guardian *firstApprentice;
    Guardian *nextInList;
    Village *village;
};

// Estructura de Historial de Entrenamiento. Contiene una aldea, un guardián,
// si el jugador ganó o perdió, el puntaje obtenido y los puntos que tenía antes
// de aplicar la suma de puntos.
struct TrainingHistory{
    Village *village;
    Guardian *rival;
    bool winner;
    int score, pointsAtTheMoment;
};

// Estructra de Historial de Viaje. Contiene un vector de aldeas.
struct JourneyHistory{
    vector<Village*> journey;
};

// Clase Control de Aldeas. Contiene la lista de todas las aldeas y la matriz de
// adyacencia que las relaciona. Contiene las funciones necesarias para manipular
// y navegar entre estas estructuras.
class VillagesControl{
    public:
        VillagesControl();
        void initializeVillage(string name);
        Village* findByName(string name);
        void initializeEdge(string name, string connectedVillage);
        void printVillagesList();
        vector<Village*>* getVillagesList();
        vector<vector<int>>* getAdjacencyMatrix();
    private:
        vector<Village*> *villagesList;
        vector<vector<int>> *adjacencyMatrix;
};

// Clase Control de Guardianes. Contiene la lista de todos los guardianes, y las funciones
// necesarias para manipular y navegar en esta estructura.
class GuardiansControl{
    public:
        GuardiansControl();
        void initializeGuardian(string name, string power, string masterName, string village, VillagesControl vControl);
        Guardian* findByName(string name);
        Guardian* findMaster(Guardian* requested);
        int guardianIndex(Guardian* current);
        void printGuardianInfo(Guardian* current);
        void printGuardiansList();
        vector<Guardian*>* getGuardiansList();
    private:
        vector<Guardian*> *guardiansList;
};
#pragma endregion

// Región que contiene la declaración de funciones generales del juego.
#pragma region Functions Declaration

// Funciones que sirven para el cargado y verificación de datos.
void loadData(VillagesControl vControl, GuardiansControl gControl);
void addGuardianNode(Guardian **list, Guardian *node);
void sortGuardiansInVillage(VillagesControl vControl, GuardiansControl gControl);
bool verifyData(VillagesControl vControl, GuardiansControl gControl);

// Funciones generales del cuerpo del juego.
void gameLoop(VillagesControl vControl, GuardiansControl gControl);
int startMenu();
Guardian* createGuardian(VillagesControl vControl, GuardiansControl gControl);
Guardian* selectGuardian(VillagesControl vControl, GuardiansControl gControl);
Guardian* guardianToPlayer(Guardian* guardian, VillagesControl vControl, GuardiansControl gControl);
void removeFromTree(Guardian* master, Guardian* requested);
void transferApprentices(Guardian* guardian, GuardiansControl gControl);
int gameMenu(Guardian* player, Village* currentVillage, GuardiansControl gControl);
void train(Guardian** player, Village** currentVillage, vector<Guardian*>* currentFought, int* currentPoints, vector<TrainingHistory*>* trainingHistory);
bool diceResult(int min, int max, Guardian* currentRival);
void travel(Guardian** player, Village** currentVillage, vector<Guardian*>* currentFought, int* currentPoints, JourneyHistory** journey, VillagesControl vControl);
void alchemy(Guardian** player, Village* currentVillage, VillagesControl vControl);
bool allVillagesVisited(VillagesControl vControl);
void gameEnd(Guardian* player, JourneyHistory* journeyHistory, vector<TrainingHistory*>* trainingHistory, GuardiansControl gControl);
int gameEndMenu(Guardian* player, GuardiansControl gControl);
void printJourneyHistory(JourneyHistory* journeyHistory);
void printTrainingHistory(Guardian* player, vector<TrainingHistory*>* trainingHistory);

// Funciones auxiliares.
int intInput(string msg, int min, int max);
void clearConsole();
#pragma endregion

// Main. Se inicializa la semilla aleatoria. Se crean las instancias de las clases
// VillagesControl y GuardiansControl. Se cargan los datos. Se verifican y, en caso
// de que no haya errores, se inicia el juego.
int main(){
    srand(time(0));
    VillagesControl vControl;
    GuardiansControl gControl;
    loadData(vControl, gControl);
    if(verifyData(vControl, gControl)) gameLoop(vControl, gControl);
    return 0;
}

// Región que contiene la inicialización de las funciones de la clase VillagesControl.
#pragma region VillagesControl Functions

// Constructor general.
VillagesControl::VillagesControl(){
    villagesList = new vector<Village*>;
    adjacencyMatrix = new vector<vector<int>>;
}

// Inicializar aldea. Recibe el nombre de la aldea obtenido del documento
// y crea una nueva aldea.
void VillagesControl::initializeVillage(string name){
    if(findByName(name) == nullptr){
        Village *newVillage = new Village;
        newVillage->name = name;
        newVillage->visited = false;
        newVillage->trainingFinished = false;
        newVillage->guardiansInVillage = new vector<Guardian*>;
        villagesList->push_back(newVillage);

        vector<vector<int>> &matrix = *adjacencyMatrix;
        for(int i = 0; i < adjacencyMatrix->size(); i++) matrix[i].push_back(0);
        matrix.push_back(vector<int>(adjacencyMatrix->size() + 1, 0));
    }
}

// Encontrar aldea por el nombre. Recibe un nombre retorna la aldea en la lista.
Village* VillagesControl::findByName(string name){
    vector<Village*> &list = *villagesList;
    if(&list[0] != nullptr)
        for(int i = 0; i < list.size(); i++)
            if(list[i]->name == name) return list[i];
    return nullptr;
}

// Inicializar arista. Recibe el nombre de la aldea de origen y la aldea destino.
// Crea un camino en la matriz de adyacencia.
void VillagesControl::initializeEdge(string name, string connectedVillage){
    vector<Village*> &list = *villagesList;
    vector<vector<int>> &matrix = *adjacencyMatrix;
    Village *src = findByName(name);
    Village *dest = findByName(connectedVillage);

    if(src == nullptr || dest == nullptr){
        cout << "\n\n  !!ERROR EN EL ARCHIVO!!";
        cout << "\nDetalle: ";
        if (src == nullptr) cout << "Origen->" << name << "   ";
        if (dest == nullptr) cout << "Destino->" << connectedVillage;
        if (connectedVillage == "") cout << name << "->VACIO";
    }
    else{
        int i, j;
        for(i = 0; i < villagesList->size(); i++)
            if(list[i]->name == src->name) break;
        for(j = 0; j < villagesList->size(); j++)
            if(list[j]->name == dest->name) break;
        
        matrix[i][j] = 1;
        matrix[j][i] = 1;
    }
}

// Imprime la lista de aldeas.
void VillagesControl::printVillagesList(){
    vector<Village*> &list = *villagesList;
    cout << "\n\n\tAldeas\n";
    for(int i = 0; i < villagesList->size(); i++) cout << i + 1 << ". " << list[i]->name << endl;
}

// Retorna la lista de aldeas.
vector<Village*>* VillagesControl::getVillagesList(){
    return villagesList;
}

// Retorna la matriz de adyacencia.
vector<vector<int>>* VillagesControl::getAdjacencyMatrix(){
    return adjacencyMatrix;
}
#pragma endregion

// Región que contiene la inicialización de las funciones de la clase GuardiansControl.
#pragma region GuardiansControl Functions

// Constructor general.
GuardiansControl::GuardiansControl(){
    guardiansList = new vector<Guardian*>;
}

// Inicializar guardián. Recibe el nombre, poder, nombre del maestro y nombre de la aldea.
// Crea un nuevo guardián, lo añade a la lista de su aldea y a la lista de aprendices
// de su maestro.
void GuardiansControl::initializeGuardian(string name, string power, string masterName, string village, VillagesControl vControl){
    if(findByName(name) == nullptr){
        Guardian *newGuardian = new Guardian;
        newGuardian->name = name;
        newGuardian->power = stoi(power);
        newGuardian->firstApprentice = nullptr;
        newGuardian->nextInList = nullptr;

        Village *auxVillage = vControl.findByName(village);
        if(auxVillage != nullptr) {
            newGuardian->village = auxVillage;
            auxVillage->guardiansInVillage->push_back(newGuardian);
        }

        Guardian *auxMaster = findByName(masterName);
        if(auxMaster != nullptr) addGuardianNode(&auxMaster->firstApprentice, newGuardian);

        guardiansList->push_back(newGuardian);
    }
}

// Encontrar guardián por nombre. Recibe el nombre y retorna el guardián en la lista.
Guardian* GuardiansControl::findByName(string name){
    vector<Guardian*> &list = *guardiansList;
    if(&list[0] != nullptr)
        for(int i = 0; i < list.size(); i++)
            if(list[i]->name == name) return list[i];
    return nullptr;
}

// Encontrar guardián maestro. Recibe el nombre de un guardián y encuentra a
// su maestro en la lista de guardianes.
Guardian* GuardiansControl::findMaster(Guardian* requested){
    vector<Guardian*> &list = *guardiansList;
    queue<Guardian*> q;
    
    q.push(list[0]);
    while(!q.empty()){
        Guardian* master = q.front();
        q.pop();
        if(master->firstApprentice != nullptr){
            Guardian* aux = master->firstApprentice;
            while(1){
                if(aux == requested) return master;
                q.push(aux);
                if(aux->nextInList == nullptr) break;
                aux = aux->nextInList;
            }
        }
    }
    return nullptr;
}

// Índice del guardián. Recibe un guardián y encuentra su índice en la lista.
int GuardiansControl::guardianIndex(Guardian* current){
    vector<Guardian*> &list = *guardiansList;
    for(int i = 0; i < guardiansList->size(); i++)
        if(current = list[i]) return i;
    return -1;
}

// Imprimir información de Guardián. Recibe un guardián e imprime sus datos.
void GuardiansControl::printGuardianInfo(Guardian* current){
    if(current != nullptr){
        cout << current->name;
        cout << "\n  Poder: " << current->power;
        cout << "\n  Aldea: " << current->village->name;
        cout << endl;
    }
}

// Imprimir lista de Guardianes.
void GuardiansControl::printGuardiansList(){
    vector<Guardian*> &list = *guardiansList;
    cout << "\n\n\tGuardianes\n";
    for(int i = 0; i < guardiansList->size(); i++){
        if(i != 0){
            cout << i << ". ";
            printGuardianInfo(list[i]);
            cout << endl;
        }
    }
}

// Retorna la lista de guardianes.
vector<Guardian*>* GuardiansControl::getGuardiansList(){
    return guardiansList;
}
#pragma endregion

// Región que contiene la inicialización de las funciones de cargado y verificación de datos.
#pragma region Loading Data

// Cargado de datos. Recibe las clases que controlan los datos de las aldeas
// y los guardianes y carga los datos desde los archivos de texto.
void loadData(VillagesControl vControl, GuardiansControl gControl){
    ifstream file;
    string name, power, masterName, connectedVillage;
    file.open("villages.txt");
    while(getline(file, name, ',')){
        getline(file, connectedVillage, '\n');
        vControl.initializeVillage(name);
    }
    file.close();
    file.open("villages.txt");
    while(getline(file, name, ',')){
        getline(file, connectedVillage, '\n');
        vControl.initializeEdge(name, connectedVillage);
    }
    file.close();
    file.open("guardians.txt");
    while(getline(file, name, ',')){
        getline(file, power, ',');
        getline(file, masterName, ',');
        getline(file, connectedVillage, '\n');
        gControl.initializeGuardian(name, power, masterName, connectedVillage, vControl);
    }
    file.close();
    sortGuardiansInVillage(vControl, gControl);
}

// Agregar guardián. Recibe la lista de aprendices de un guardián y añade
// al nuevo guardián a esa lista.
void addGuardianNode(Guardian **list, Guardian *node){
    if(*list == nullptr) *list = node;
    else{
        Guardian *aux = *list;
        while(aux->nextInList != nullptr) aux = aux->nextInList;
        aux->nextInList = node;
    }
}

// Ordena los guardianes en la aldea, según sus puntos de poder.
void sortGuardiansInVillage(VillagesControl vControl, GuardiansControl gControl){
    vector<Village*> &villages = *vControl.getVillagesList();
    for(int i = 0; i < villages.size(); i++){
        vector<Guardian*> &guardians = *villages[i]->guardiansInVillage;
        sort(guardians.begin(), guardians.end(), [](const Guardian *left, const Guardian *right) { return (left->power < right->power); });
    }
}

// Comprueba que las aldeas tengan suficientes guardianes.
bool verifyData(VillagesControl vControl, GuardiansControl gControl){
    vector<Village*> &villages = *vControl.getVillagesList();
    for(int i = 0; i < villages.size(); i++)
        if(villages[i]->guardiansInVillage->size() < 2){
            cout << "No hay suficientes guardianes en la aldea " << villages[i]->name;
            return false;
        }
    return true;
}
#pragma endregion

// Región que contiene la inicialización de las funciones del cuerpo del juego.
#pragma region Game Body

// Contiene el Loop principal del juego. Aquí ocurre la selección/creación del personaje,
// el menú principal del juego, y el menú final del juego.
void gameLoop(VillagesControl vControl, GuardiansControl gControl){
    Guardian *player;
    Village *currentVillage;
    vector<Guardian*> *currentFought = new vector<Guardian*>;
    int currentPoints = 0;
    vector<TrainingHistory*> *trainingHistory = new vector<TrainingHistory*>;
    JourneyHistory *journeyHistory = new JourneyHistory;

    if(startMenu() == 1) player = createGuardian(vControl, gControl);
    else player = selectGuardian(vControl, gControl);
    currentVillage = player->village;
    currentVillage->visited = true;
    journeyHistory->journey.push_back(currentVillage);

    while(player->power < 90 && !allVillagesVisited(vControl)){
        int opt = gameMenu(player, currentVillage, gControl);
        if (opt == 1) train(&player, &currentVillage, currentFought, &currentPoints, trainingHistory);
        else if (opt == 2) travel(&player, &currentVillage, currentFought, &currentPoints, &journeyHistory, vControl);
        else alchemy(&player, currentVillage, vControl);    
    }
    gameEnd(player, journeyHistory, trainingHistory, gControl);
}

// Muestra el menú de selección/creación de personaje y retorna la opción elegida.
int startMenu(){
    clearConsole();
    int opt;
    cout << "\n\n\tTHE GUARDIAN JOURNEY";
    cout << "\n\nPara iniciar tu viaje, debes seleccionar una de las opciones:";
    cout << "\n\n  1. Crear un Guardian.";
    cout << "\n  2. Seleccionar un Guardian existente.";
    return intInput("\n\nIngresa el numero de tu opcion: ", 1, 2);
}

// Crea un nuevo guardián. En caso de que el nombre del guardián ya esté presente
// en la lista, tansforma el guardián en jugador.
Guardian* createGuardian(VillagesControl vControl, GuardiansControl gControl){
    clearConsole();
    vector<Village*> &villages = *vControl.getVillagesList();
    string name;
    cout << "\n\nIngresa un nombre para tu Guardian: ";
    cin >> name;
    Guardian* current = gControl.findByName(name);

    if(current == nullptr){
        clearConsole();
        cout << "Elige la aldea a la que pertenece tu Guardian.";
        vControl.printVillagesList();
        int selectedVillage = intInput("\n\nIngresa el numero de tu opcion: ", 1, villages.size());
        Guardian *newGuardian = new Guardian;
        newGuardian->name = name;
        newGuardian->power = APPRENTICE_POWER;
        newGuardian->firstApprentice = nullptr;
        newGuardian->nextInList = nullptr;
        newGuardian->village = villages[selectedVillage - 1];
        return newGuardian;
    }
    else return guardianToPlayer(current, vControl, gControl);
}

// Selecciona un guardián existente.
Guardian* selectGuardian(VillagesControl vControl, GuardiansControl gControl){
    clearConsole();
    vector<Guardian*> &list = *gControl.getGuardiansList();
    Guardian* current;
    cout << "Elige el Guardian que deseas usar.";
    gControl.printGuardiansList();
    int selectedGuardian = intInput("\n\nIngresa el numero de tu opcion: ", 1, list.size() - 1);
    for(int i = 0; i < list.size(); i++)
        if(i == selectedGuardian){
            current = list[i];
            break;
        }
    return guardianToPlayer(current, vControl, gControl);
}

// Transforma los datos del guardián elegido para poder usarlo como jugador.
Guardian* guardianToPlayer(Guardian* guardian, VillagesControl vControl, GuardiansControl gControl){
    Guardian *master = gControl.findMaster(guardian);

    removeFromTree(master, guardian);
    if(guardian->firstApprentice != nullptr) transferApprentices(guardian, gControl);

    guardian->power = APPRENTICE_POWER;
    guardian->firstApprentice = nullptr;
    guardian->nextInList = nullptr;

    vector<Guardian*> &guardiansInVillage = *guardian->village->guardiansInVillage;
    for(int i = 0; i < guardiansInVillage.size(); i++)
        if(guardiansInVillage[i] == guardian){
            guardiansInVillage.erase(guardiansInVillage.begin() + i);
            break;
        }

    if(guardiansInVillage.size() < 2){
        vector<Village*> &villages = *vControl.getVillagesList();
        for(int i = 0; i < villages.size(); i++){
            if(villages[i]->guardiansInVillage->size() > 2){
                vector<Guardian*> &auxVillage = *villages[i]->guardiansInVillage;
                    Guardian* aux = auxVillage[0];
                    auxVillage.erase(auxVillage.begin());
                    aux->village = guardian->village;
                    guardiansInVillage.push_back(aux);
            }
        }
    }
    return guardian;
}

// Retira el guardián elegido de la lista de aprendices.
void removeFromTree(Guardian* master, Guardian* requested){
    Guardian *current = master->firstApprentice;
    while(1){
        if(current == requested){
            if(current == master->firstApprentice) master->firstApprentice = current->nextInList;
            else{
                Guardian *aux = master->firstApprentice;
                while(aux->nextInList != current) aux = aux->nextInList;
                aux->nextInList = current->nextInList;               
            }
            break;
        }
        else current = current->nextInList;
    }
}

// En caso de que el guardián elegido tenga aprendices, estos se transfieren
// al primer guardián sin aprendices que se encuentre.
void transferApprentices(Guardian* guardian, GuardiansControl gControl){
    vector<Guardian*> &list = *gControl.getGuardiansList();
    queue<Guardian*> q;
    q.push(list[0]);
    while(!q.empty()){
        Guardian* current = q.front();
        q.pop();
        if(current->firstApprentice != nullptr){
            Guardian *aux = current->firstApprentice;
            while(1){
                q.push(aux);
                if(aux->nextInList == nullptr) break;
                aux = aux->nextInList;
            }
        }
        else{
            current->firstApprentice = guardian->firstApprentice;
            break;
        }
    }
}

// Menú principal del juego. Muestra las opciones de Entrenar, Viajar y Alquimia,
// y retorna la opción elegida.
int gameMenu(Guardian* player, Village* currentVillage, GuardiansControl gControl){
    clearConsole();
    int opt;
    cout << "\n\n\tTHE GUARDIAN JOURNEY\n\n";
    gControl.printGuardianInfo(player);
    cout << "\n\nEn este momento te encuentras en la aldea " << currentVillage->name << ".";
    cout << "\nSelecciona la accion que deseas realizar:";
    cout << "\n\n  1. Entrenar.";
    cout << "\n  2. Viajar.";
    cout << "\n  3. Alquimia.";
    return intInput("\n\nIngresa el numero de tu opcion: ", 1, 3);
}

// Entrenamiento. Muestra la lista de guardianes con los que se puede enfrentar,
// indica cuál es el maestro y el guardián recomendado.
// Esta función se asegura de que el jugador solo pueda alcanzar el puntaje máximo
// por aldea una sola vez.
// Añade el combate realizado al historial de entrenamiento.
void train(Guardian** player, Village** currentVillage, vector<Guardian*>* currentFought, int* currentPoints, vector<TrainingHistory*>* trainingHistory){
    Guardian* &auxPlayer = *player;
    Village* &auxVillage = *currentVillage;
    vector<Guardian*> &auxFought = *currentFought;
    vector<TrainingHistory*> &auxHistory = *trainingHistory;
    vector<Guardian*> &list = *auxVillage->guardiansInVillage;
    Guardian* currentRival;
    bool available = true;
    Guardian* master = list[0];
    Guardian* auxRecommended = list[0];
    for(int i = 0; i < list.size(); i++){
        if(auxRecommended->power > list[i]->power) auxRecommended = list[i];
        if(master->power < list[i]->power) master = list[i];
    }

    clearConsole();
    cout << "\n\n\tTHE GUARDIAN JOURNEY";
    if(!auxVillage->trainingFinished){
        cout << "\n\nElige el Guardian al que deseas enfrentar:\n";
        for(int i = 0; i < list.size(); i++){
            cout << "\n" << i + 1 << ". " << list[i]->name;
            if(master == list[i]) cout << " - Maestro";
            if(auxRecommended == list[i]) cout << " - Recomendado para luchar";
            for(int j = 0; j < currentFought->size(); j++)
                if(list[i]->name == auxFought[j]->name) cout << " - NO DISPONIBLE";
            cout << "\n     Poder: " << list[i]->power;
        }
        int opt = intInput("\n\nIngresa el numero de tu opcion: ", 1, list.size());
        currentRival = list[opt - 1];
        for(int j = 0; j < currentFought->size(); j++)
            if(currentRival->name == auxFought[j]->name){
                cout << "\nDebes elegir a otro Guardian.";
                available = false;
                break;
            }

        if(available){
            TrainingHistory *tempTraining = new TrainingHistory;
            tempTraining->pointsAtTheMoment = auxPlayer->power;
            if(diceResult(1, 6, currentRival)){
                cout << "\nHas ganado. Puntos de poder: " << auxPlayer->power;
                tempTraining->winner = true;
                int score;
                if(currentRival == master) score = 2;
                else score = 1;
                tempTraining->score = score;
                *currentPoints += score;
                auxPlayer->power += score;
                cout << " ---> " << auxPlayer->power;
            }
            else{
                tempTraining->winner = false;
                tempTraining->score = 0;
                cout << "\nHas perdido.";
            }
            auxFought.push_back(currentRival);

            tempTraining->village = auxVillage;
            tempTraining->rival = currentRival;
            auxHistory.push_back(tempTraining);
            if(*currentPoints >= 3) auxVillage->trainingFinished = true;
        }
    }
    else cout << "\n\nNo puedes entrenar en esta aldea. Ya has alcanzado el puntaje maximo.";
}

// Resultado de los dados. Multiplica el poder del rival por un resultado
// aleatorio entre 1 y 6. En caso de que el resultado sea menor que 300
// el jugador gana el enfrentamiento.
// De esta forma, a mayor puntaje, menos probable es ganar.
bool diceResult(int min, int max, Guardian* currentRival){
    int result = currentRival->power * ((rand() % (max - min + 1)) + min);
    if(result < 300) return true;
    else return false;
}

// Función de viaje. El jugador selecciona la aldea a la que quiere ir
// y recibe puntaje por hacerlo. Resetea los guardianes enfrentados y la cantidad
// actual de puntaje obtenido. Añade la aldea al historial de viaje.
void travel(Guardian** player, Village** currentVillage, vector<Guardian*>* currentFought, int* currentPoints, JourneyHistory** journey, VillagesControl vControl){
    vector<Village*> &list = *vControl.getVillagesList();
    vector<vector<int>> &matrix = *vControl.getAdjacencyMatrix();
    vector<Village*> *temp = new vector<Village*>;
    vector<Village*> &auxTemp = *temp;
    Guardian* &auxPlayer = *player;
    Village* &auxVillage = *currentVillage;
    vector<Guardian*> &auxFought = *currentFought;
    JourneyHistory* &auxJourney = *journey;

    clearConsole();
    cout << "\n\n\tTHE GUARDIAN JOURNEY";
    cout << "\n\nElige la aldea a la que deseas viajar:\n";
    int opt, counter = 0;
    for(int i = 0; i < list.size(); i++)
        if(list[i] == *currentVillage)
            for(int j = 0; j < matrix[i].size(); j++)
                if(matrix[i][j] == 1) {
                    counter++;
                    cout << "\n\t" << counter <<". " << list[j]->name;
                    temp->push_back(list[j]);
                }
    opt = intInput("\n\nIngresa el numero de tu opcion: ", 1, counter);

    auxVillage = auxTemp[opt - 1];
    auxVillage->visited = true;
    auxPlayer->power++;
    vector<Guardian*> *tempFougt = new vector<Guardian*>;
    auxFought = *tempFougt;
    *currentPoints = 0;
    auxJourney->journey.push_back(auxVillage);
}

// Función de alquimia. El jugador selecciona la aldea a la que quiere construir
// el camino, y manipula la matriz de adyacencia para guardar los cambios.
// El camino tiene un costo aleatorio de 2, 3 o 4 puntos de poder.
void alchemy(Guardian** player, Village* currentVillage, VillagesControl vControl){
    vector<Village*> &list = *vControl.getVillagesList();
    vector<vector<int>> &matrix = *vControl.getAdjacencyMatrix();
    vector<Village*> *temp = new vector<Village*>;
    vector<Village*> &auxTemp = *temp;
    Guardian* &auxPlayer = *player;
    int min = 2, max = 4, counter = 0, opt;

    clearConsole();
    cout << "\n\n\tTHE GUARDIAN JOURNEY";
    cout << "\n\nElige la aldea a la que deseas agregar un camino:\n";
    for(int i = 0; i < list.size(); i++)
        if(list[i] == currentVillage)
            for(int j = 0; j < matrix[i].size(); j++)
                if(matrix[i][j] == 0  && list[j] != currentVillage) {
                    counter++;
                    cout << "\n\t" << counter <<". " << list[j]->name;
                    temp->push_back(list[j]);
                }
    if(counter != 0){
        opt = intInput("\n\nIngresa el numero de tu opcion: ", 1, counter);
        int cost = (rand() % (max - min + 1)) + min;
        if(auxPlayer->power < cost) cout << "\n\nEl costo del camino es de " << cost << " puntos.\nNo tienes suficientes puntos.";
        else{
            for(int i = 0; i < list.size(); i++)
                if(list[i] == currentVillage)
                    for(int j = 0; j < matrix[i].size(); j++)
                        if(list[j] == auxTemp[opt - 1]){
                            matrix[i][j] = 1;
                            matrix[j][i] = 1;
                        }
            auxPlayer->power -= cost;
            cout << "\n\nCamino creado.\nEl costo del camino es de " << cost << " puntos.\nTu nuevo puntaje es " << auxPlayer->power << " puntos.";
        }
    }
}

// Comprueba si todas las aldeas de la lista han sido visitadas.
bool allVillagesVisited(VillagesControl vControl){
    vector<Village*> &list = *vControl.getVillagesList();
    for(int i = 0; i < list.size(); i++)
        if(list[i]->visited == false) return false;
    return true;
}

// Al terminar el juego, se dan las opciones de ver los datos del personaje,
// ver el historial de viaje, ver el historial de entrenamiento, o salir.
void gameEnd(Guardian* player, JourneyHistory* journeyHistory, vector<TrainingHistory*>* trainingHistory, GuardiansControl gControl){
    while(1){
        int opt = gameEndMenu(player, gControl);
        if(opt == 1){
            clearConsole();
            cout << "\n\n\tTHE GUARDIAN JOURNEY\n\n";
            gControl.printGuardianInfo(player);
        }
        else if (opt == 2) printJourneyHistory(journeyHistory);
        else if (opt == 3) printTrainingHistory(player, trainingHistory);
        else break;    
    }
}

// Menú final del juego. Retorna la opción seleccionada.
int gameEndMenu(Guardian* player, GuardiansControl gControl){
    clearConsole();
    cout << "\n\n\tTHE GUARDIAN JOURNEY";
    cout << "\n\n" << player->name << "(" << player->power << ") vs Stormheart(100)";
    int opt;
    cout << "\n\nHas finalizado el juego.";
    cout << "\nSelecciona la accion que deseas realizar:";
    cout << "\n\n  1. Ver Informacion personaje.";
    cout << "\n  2. Ver Historial de Viaje.";
    cout << "\n  3. Ver Historial de Entrenamiento.";
    cout << "\n  4. Salir del juego.";
    return intInput("\n\nIngresa el numero de tu opcion: ", 1, 4);
}

// Imprime el historial de viaje. Muestra cuál es la aldea de origen.
void printJourneyHistory(JourneyHistory* journeyHistory){
    clearConsole();
    cout << "\n\n\tTHE GUARDIAN JOURNEY";
    cout << "\n\nHistorial de Viaje:";
    for(int i = 0; i < journeyHistory->journey.size(); i ++){
        cout << "\n" << i + 1 << ". " << journeyHistory->journey[i]->name;
        if(i == 0) cout << " - ORIGEN";
    }
}

// Imprime el historial de entrenamiento. Muestra el rival, si ganó o perdió, y
// en caso de haber ganado muestra el puntaje obtenido, el puntaje anterior y el puntaje final.
void printTrainingHistory(Guardian* player, vector<TrainingHistory*>* trainingHistory){
    clearConsole();
    vector<TrainingHistory*> &history = *trainingHistory;
    cout << "\n\n\tTHE GUARDIAN JOURNEY";
    cout << "\n\nHistorial de Entrenamiento:";
    if(history.size() == 0) cout << "\nNo hubo entrenamientos.";
    else
        for(int i = 0; i < history.size(); i++){
            cout << "\n" << i + 1 << ". " << history[i]->village->name;
            cout << "\n\t" << player->name << " vs " << history[i]->rival->name;
            if(history[i]->winner)
                cout << "\n\tVictoria - Puntos: " << history[i]->pointsAtTheMoment << " + "
                << history[i]->score << " ---> " << history[i]->pointsAtTheMoment + history[i]->score;
            else cout << "\n\tDerrota";
        }
}
#pragma endregion

// Región que contiene la inicialización de las funciones auxiliares.
#pragma region Auxiliar Functions

// Función que se asegura que el input del usuario sea un entero,
// y que este se encuentre dentro del rango solicitado.
int intInput(string msg, int min, int max){
    int opt;
    do{
        cout << msg;
        cin >> opt;
        if (cin.good() && opt >= min && opt <= max) return opt;
        else{
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            cout << "*Ingresa un valor dentro del rango.";
        }
    }while(1);
}

// Función estética que limpia la pantalla.
void clearConsole(){
    cout << "\n\n";
    system("pause");
    system("cls");
}
#pragma endregion