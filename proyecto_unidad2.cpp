#include <algorithm>
#include <bits/stdc++.h>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <vector>
#define MAX_POWER 100
#define APPRENTICE_POWER MAX_POWER / 2;
using namespace std;

#pragma region Structures and Classes
struct Village;
struct Guardian;
struct TrainingHistory;
struct JourneyHistory;

struct Village{
    string name;
    bool trainingFinished;
    vector<Guardian*> *guardiansInVillage;
};

struct Guardian{
    string name;
    int power;
    Guardian *firstApprentice;
    Guardian *nextInList;
    Village *village;
};

struct TrainingHistory{
    Village *village;
    Guardian *rival;
    bool winner;
};

struct JourneyHistory{
    vector<Village*> journey;
};

class VillagesControl{
    public:
        VillagesControl();
        void initializeVillage(string name);
        Village* findByName(string name);
        void initializeEdge(string name, string connectedVillage);
        void printVillagesList();
        void printAdjacencyMatrix();
        vector<Village*>* getVillagesList();
        vector<vector<int>>* getAdjacencyMatrix();
    private:
        vector<Village*> *villagesList;
        vector<vector<int>> *adjacencyMatrix;
};

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

#pragma region Functions Declaration
void loadData(VillagesControl vControl, GuardiansControl gControl);
void addGuardianNode(Guardian **list, Guardian *node);
void sortGuardiansInVillage(VillagesControl vControl, GuardiansControl gControl);
bool verifyData(VillagesControl vControl, GuardiansControl gControl);

void gameLoop(VillagesControl vControl, GuardiansControl gControl);
int startMenu();
Guardian* createGuardian(VillagesControl vControl, GuardiansControl gControl);
Guardian* selectGuardian(VillagesControl vControl, GuardiansControl gControl);
Guardian* guardianToPlayer(Guardian* guardian, VillagesControl vControl, GuardiansControl gControl);
void removeFromTree(Guardian* master, Guardian* requested);
void transferApprentices(Guardian* guardian, GuardiansControl gControl);
int gameMenu(Village* currentVillage);
void train(Guardian** player, Village** currentVillage);
void travel(Guardian** player, Village** currentVillage, JourneyHistory** journey, VillagesControl vControl);
void alchemy(Guardian** player, Village* currentVillage, VillagesControl vControl);
void gameEnd(Guardian* player, JourneyHistory* journeyHistory, GuardiansControl gControl);

int intInput(string msg, int min, int max);
void clearConsole();
#pragma endregion

int main(){
    srand(time(0));
    VillagesControl vControl;
    GuardiansControl gControl;
    loadData(vControl, gControl);
    if(verifyData(vControl, gControl)) gameLoop(vControl, gControl);
    return 0;
}

#pragma region VillagesControl Functions
VillagesControl::VillagesControl(){
    villagesList = new vector<Village*>;
    adjacencyMatrix = new vector<vector<int>>;
}

void VillagesControl::initializeVillage(string name){
    if(findByName(name) == nullptr){
        Village *newVillage = new Village;
        newVillage->name = name;
        newVillage->trainingFinished = false;
        newVillage->guardiansInVillage = new vector<Guardian*>;
        villagesList->push_back(newVillage);

        vector<vector<int>> &matrix = *adjacencyMatrix;
        for(int i = 0; i < adjacencyMatrix->size(); i++) matrix[i].push_back(0);
        matrix.push_back(vector<int>(adjacencyMatrix->size() + 1, 0));
    }
}

Village* VillagesControl::findByName(string name){
    vector<Village*> &list = *villagesList;
    if(&list[0] != nullptr)
        for(int i = 0; i < list.size(); i++)
            if(list[i]->name == name) return list[i];
    return nullptr;
}

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

void VillagesControl::printVillagesList(){
    vector<Village*> &list = *villagesList;
    cout << "\n\n\tAldeas\n";
    for(int i = 0; i < villagesList->size(); i++) cout << i + 1 << ". " << list[i]->name << endl;
}

void VillagesControl::printAdjacencyMatrix(){
    vector<vector<int>> &matrix = *adjacencyMatrix;
    cout << "\n\n\tMatriz\n";
    int i = 0;
    for(const auto& row : matrix){
        for(int value : row){
            if(value == 0) cout << "- ";
            else cout << "1 ";
        }
        cout << "\n";
        i++;
    }
}

vector<Village*>* VillagesControl::getVillagesList(){
    return villagesList;
}

vector<vector<int>>* VillagesControl::getAdjacencyMatrix(){
    return adjacencyMatrix;
}
#pragma endregion

#pragma region GuardiansControl Functions
GuardiansControl::GuardiansControl(){
    guardiansList = new vector<Guardian*>;
}

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

Guardian* GuardiansControl::findByName(string name){
    vector<Guardian*> &list = *guardiansList;
    if(&list[0] != nullptr)
        for(int i = 0; i < list.size(); i++)
            if(list[i]->name == name) return list[i];
    return nullptr;
}

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

int GuardiansControl::guardianIndex(Guardian* current){
    vector<Guardian*> &list = *guardiansList;
    for(int i = 0; i < guardiansList->size(); i++)
        if(current = list[i]) return i;
    return -1;
}

void GuardiansControl::printGuardianInfo(Guardian* current){
    if(current != nullptr){
        cout << current->name;
        cout << "\n  Poder: " << current->power;
        cout << "\n  Aldea: " << current->village->name;
        cout << endl;
    }
}

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

vector<Guardian*>* GuardiansControl::getGuardiansList(){
    return guardiansList;
}
#pragma endregion

#pragma region Loading Data
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

void addGuardianNode(Guardian **list, Guardian *node){
    if(*list == nullptr) *list = node;
    else{
        Guardian *aux = *list;
        while(aux->nextInList != nullptr) aux = aux->nextInList;
        aux->nextInList = node;
    }
}

void sortGuardiansInVillage(VillagesControl vControl, GuardiansControl gControl){
    vector<Village*> &villages = *vControl.getVillagesList();
    for(int i = 0; i < villages.size(); i++){
        vector<Guardian*> &guardians = *villages[i]->guardiansInVillage;
        sort(guardians.begin(), guardians.end(), [](const Guardian *left, const Guardian *right) { return (left->power < right->power); });
    }
}

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

#pragma region Game Body
void gameLoop(VillagesControl vControl, GuardiansControl gControl){
    Guardian *player;
    Village *currentVillage;
    TrainingHistory *trainingHistory = new TrainingHistory;
    JourneyHistory *journeyHistory = new JourneyHistory;

    if(startMenu() == 1) player = createGuardian(vControl, gControl);
    else player = selectGuardian(vControl, gControl);
    currentVillage = player->village;
    journeyHistory->journey.push_back(currentVillage);

    clearConsole();
    gControl.printGuardianInfo(player);
    
    while(1){
        if(player->power >= 90){
            gameEnd(player, journeyHistory, gControl);
            break;
        }
        else{
            int opt = gameMenu(currentVillage);
            if (opt == 1) train(&player, &currentVillage);
            else if (opt == 2) travel(&player, &currentVillage, &journeyHistory, vControl);
            else if (opt == 3) alchemy(&player, currentVillage, vControl);
            else{
                gameEnd(player, journeyHistory, gControl);
                break;
            }
        }
    }
}

int startMenu(){
    clearConsole();
    int opt;
    cout << "\n\n\tTHE GUARDIAN JOURNEY";
    cout << "\n\nPara iniciar tu viaje, debes seleccionar una de las opciones:";
    cout << "\n\n  1. Crear un Guardian.";
    cout << "\n  2. Seleccionar un Guardian existente.";
    return intInput("\n\nIngresa el numero de tu opcion: ", 1, 2);
}

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

int gameMenu(Village* currentVillage){
    clearConsole();
    int opt;
    cout << "\n\n\tTHE GUARDIAN JOURNEY";
    cout << "\n\nEn este momento te encuentras en la aldea " << currentVillage->name << ".";
    cout << "\nSelecciona la accion que deseas realizar:";
    cout << "\n\n  1. Entrenar.";
    cout << "\n  2. Viajar.";
    cout << "\n  3. Alquimia.";
    cout << "\n  4. Salir del juego."; // OPCIÓN DE PRUEBA - Para comprobar datos durante la ejecución.
    return intInput("\n\nIngresa el numero de tu opcion: ", 1, 4);
}

void train(Guardian** player, Village** currentVillage){
    clearConsole();
}

void travel(Guardian** player, Village** currentVillage, JourneyHistory** journey, VillagesControl vControl){
    vector<Village*> &list = *vControl.getVillagesList();
    vector<vector<int>> &matrix = *vControl.getAdjacencyMatrix();
    vector<Village*> *temp = new vector<Village*>;
    vector<Village*> &auxTemp = *temp;
    Guardian* &auxPlayer = *player;
    Village* &auxVillage = *currentVillage;
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
    auxPlayer->power++;
    auxJourney->journey.push_back(auxVillage);
    free(temp);
}

// (rand() % (max - min + 1)) + min
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
    free(temp);
}

void gameEnd(Guardian* player, JourneyHistory* journeyHistory, GuardiansControl gControl){
    clearConsole();
    gControl.printGuardianInfo(player);
    cout << "\n\nHistorial de Viaje:";
    for(int i = 0; i < journeyHistory->journey.size(); i ++){
        cout << "\n" << i + 1 << ". " << journeyHistory->journey[i]->name;
        if(i == 0) cout << " - ORIGEN";
    }
}
#pragma endregion

#pragma region Auxiliar Functions
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

void clearConsole(){
    cout << "\n\n";
    system("pause");
    system("cls");
}
#pragma endregion