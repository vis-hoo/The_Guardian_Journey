#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#define MAXPOWER 100
#define APPRENTICEPOWER MAXPOWER / 2;
using namespace std;

struct Village;
struct Guardian;

struct Village{
    string name;
    bool visited;
    vector<Guardian*> *guardiansList;
};

struct Guardian{
    string name;
    int power;
    bool isPlayer;
    Guardian *master;
    Village *village;
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
        void initializeGuardian(string name, string power, string masterName, string village, VillagesControl vC);
        Guardian* findByName(string name);
        void printGuardianInfo(Guardian* current);
        void printGuardiansList();
        vector<Guardian*>* getGuardiansList();
    private:
        vector<Guardian*> *guardiansList;
};

void loadData(VillagesControl vControl, GuardiansControl gControl);
void gameLoop(VillagesControl vControl, GuardiansControl gControl);
int startMenu();
Guardian* createGuardian(GuardiansControl gControl, VillagesControl vControl);
Guardian* selectGuardian(GuardiansControl gControl);
int intInput(string msg, int min, int max);
void clearConsole();

int main(){
    VillagesControl vControl;
    GuardiansControl gControl;
    loadData(vControl, gControl);
    gameLoop(vControl, gControl);
    return 0;
}

VillagesControl::VillagesControl(){
    villagesList = new vector<Village*>;
    adjacencyMatrix = new vector<vector<int>>;
}

void VillagesControl::initializeVillage(string name){
    if(findByName(name) == nullptr){
        Village *newVillage = new Village;
        newVillage->name = name;
        newVillage->visited = false;
        newVillage->guardiansList = new vector<Guardian*>;
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

GuardiansControl::GuardiansControl(){
    guardiansList = new vector<Guardian*>;
}

void GuardiansControl::initializeGuardian(string name, string power, string masterName, string village, VillagesControl vC){
    if(findByName(name) == nullptr){
        Guardian *newGuardian = new Guardian;
        newGuardian->name = name;
        newGuardian->power = stoi(power);
        newGuardian->isPlayer = false;
        newGuardian->master = findByName(masterName);

        Village *auxVillage = vC.findByName(village);
        newGuardian->village = auxVillage;
        if(auxVillage != nullptr) auxVillage->guardiansList->push_back(newGuardian);

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
        if(current->isPlayer) cout << " --------------> JUGADOR";
        cout << "\n  Poder: " << current->power;
        if(current->master != nullptr) cout << "\n  Maestro: " << current->master->name;
        if(current->village == nullptr) cout << "\n  Aldea: Tesla";
        else cout << "\n  Aldea: " << current->village->name;
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
}

void gameLoop(VillagesControl vControl, GuardiansControl gControl){
    Guardian *player;
    if(startMenu() == 1) player = createGuardian(gControl, vControl);
    else player = selectGuardian(gControl);
    clearConsole();
    gControl.printGuardianInfo(player);

    //-----------------------------------
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
        newGuardian->isPlayer = true;
        newGuardian->master = nullptr;
        newGuardian->village = villages[selectedVillage - 1];
        guardians.push_back(newGuardian);
        return newGuardian;
    }
    else{
        current->power = APPRENTICEPOWER;
        current->isPlayer = true;
        current->master = nullptr;
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
    current->isPlayer = true;
    current->master = nullptr;
    return current;
}

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