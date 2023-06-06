#include <algorithm>
#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#define MAXPOWER 100
#define APPRENTICEPOWER MAXPOWER / 2;
using namespace std;

#pragma region Structures and Classes
struct Village;
struct Guardian;
struct TrainingHistory;
struct JourneyHistory;

struct Village{
    string name;
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
        void printGuardianInfo(Guardian* current);
        void printGuardiansList();
        vector<Guardian*>* getGuardiansList();
    private:
        vector<Guardian*> *guardiansList;
};
#pragma endregion

#pragma region Function Declarations
void loadData(VillagesControl vControl, GuardiansControl gControl);
void addGuardianNode(Guardian **list, Guardian *node);
void sortGuardiansInVillage(VillagesControl vControl, GuardiansControl gControl);
bool verifyData(VillagesControl vControl, GuardiansControl gControl);

void gameLoop(VillagesControl vControl, GuardiansControl gControl);
int startMenu();
Guardian* createGuardian(GuardiansControl gControl, VillagesControl vControl);
Guardian* selectGuardian(GuardiansControl gControl);

int intInput(string msg, int min, int max);
void clearConsole();
#pragma endregion

int main(){
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
    TrainingHistory *trainingHistory = new TrainingHistory;
    JourneyHistory *journeyHistory = new JourneyHistory;

    if(startMenu() == 1) player = createGuardian(gControl, vControl);
    else player = selectGuardian(gControl);
    journeyHistory->journey.push_back(player->village);
    clearConsole();
    gControl.printGuardianInfo(player);
    
    while(1){
        break;    
    }
}

int startMenu(){
    clearConsole();
    int opt;
    cout << "\n\n\tTHE GUARDIAN JOURNEY";
    cout << "\n\nPara iniciar tu viaje, debes seleccionar una de las opciones: ";
    cout << "\n\n  1. Crear un Guardian.";
    cout << "\n  2. Seleccionar un Guardian existente.";
    return intInput("\n\nIngresa el numero de tu opcion: ", 1, 2);
}

Guardian* createGuardian(GuardiansControl gControl, VillagesControl vControl){
    clearConsole();
    vector<Village*> &villages = *vControl.getVillagesList();
    vector<Guardian*> &guardians = *gControl.getGuardiansList();
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
        newGuardian->power = APPRENTICEPOWER;
        newGuardian->village = villages[selectedVillage - 1];
        guardians.push_back(newGuardian);
        return newGuardian;
    }
    else{
        current->power = APPRENTICEPOWER;
        return current;
    }
}

Guardian* selectGuardian(GuardiansControl gControl){
    clearConsole();
    vector<Guardian*> &list = *gControl.getGuardiansList();
    Guardian* current;
    cout << "Elige el Guardian que deseas usar.";
    gControl.printGuardiansList();
    int selectedGuardian = intInput("\n\nIngresa el numero de tu opcion: ", 1, list.size() - 1);
    for(int i = 0; i < list.size(); i++)
        if(i == selectedGuardian){
            current = gControl.findByName(list[i]->name);
            break;
        }
    current->power = APPRENTICEPOWER;
    return current;
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