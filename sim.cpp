#include <iostream>
#include <queue>    // priority_queue
#include <cstdlib>  // exit()
#include "simlib.h"

using namespace std;

// Error message and quit program
void error(const char *msg)
{
    cerr << "ERROR: " << msg << endl;
    exit(1);
}

Facility myciBox("MyciBox");
Store zamestnanecNaDesign("zamestnanecNaDesign", 2);
Store zamestnanecNaLepeni("zamestanecNaLepeni", 7);
Store zamestnanecNaCisteni("zamestanecNaCisteni", 2);

// pozadavek od zakaznika na polep, design nebo cisteni auta
class Pozadavek : public Process {
    void Behavior(){
        int rozdeleni = Uniform(0,100);
        if (rozdeleni <= 20 ){
            // polep designem
            Enter(zamestnanecNaDesign, 1);
            cout << "Info: Zabiram 1 zamestance na design" << endl;
            Seize(myciBox);
            cout << "Info: Zabiram myci box pred designem auta" << endl;
            Wait(Exponential(120)); // myti auta
            Release(myciBox);
            cout << "Info: Uvolnuji myci box a jdu na tvorbu designu" << endl;
            Wait(Exponential(7200)); // tvorba designu
            Leave(zamestnanecNaDesign, 1);
            cout << "Info: Uvolnuji zamestance na design a jdu na tisk designu" << endl;
            Wait(360); // tisk designu
            cout << "Info: Tisk dokoncen, auto je pripravene na nalepeni designu" << endl;
            Enter(zamestnanecNaLepeni, 1);
            cout << "Info: Zabiram 1 zamestance na nalepeni designu" << endl;
            Wait(Exponential(1500)); // lepeni designu
            Leave(zamestnanecNaLepeni, 1);
            cout << "Info: Uvolnuji zamestance na nalepeni designu, auto je pripravene na predani zakaznikovi" << endl;

        } else  if(rozdeleni > 20 && rozdeleni <= 40){
            Enter(zamestnanecNaCisteni, 1);
            cout << "Info proces cisteni: Zaberam 1 zamestnanca na cistenie auta" << endl;
            Seize(myciBox);
            cout << "Info proces cisteni: Zaberam mici box na cistenie auta" << endl;
            Wait(Exponential(120));
            Release(myciBox);
            cout << "Info proces cisteni: Uvolnujem myci box" << endl;
            cout << "Info proces cisteni: Zacina proces cistenia auta." << endl;
            Wait(Exponential(360));
            cout << "Info proces cisteni: Auto je vycistene a pripravene na predaj zakaznikovy" << endl;
            Leave(zamestnanecNaCisteni, 1);
            cout << "Info proces cisteni: Uvolnujem 1 zamestnanca na cistenie" << endl;
        } else if ( rozdeleni > 40){
            // polep standardni
            Enter(zamestnanecNaLepeni, 1);
            cout << "Info: Zabiram 1 zamestance na polep" << endl;
            Seize(myciBox);
            cout << "Info: Zabiram myci box pred polepem auta" << endl;
            Wait(Exponential(120)); // myti auta
            Release(myciBox);
            cout << "Info: Uvolnuji myci box a jdu na polep" << endl;
            Wait(Exponential(1200)); // polep auta
            Leave(zamestnanecNaLepeni, 1);
            cout << "Info: Uvolnuji zamestance na lepeni, auto je pripravene na predani zakaznikovi" << endl;

        }
    }
};

// System input model 
class Generator : public Event {
    void Behavior()
    {
        (new Pozadavek)->Activate();
        Activate(Time+Exponential(480));  // TODO
    }
};

// Time of model
double Time;

int main(){

    SetOutput("polepy-out.dat");
    Print("Model firmy na polepy aut");


    cout << "*** Inicializacia ***" << endl;

    Init(0,14400); // 30 dni (po 8 hodinach)
    (new Generator)->Activate();
    Run();

    SIMLIB_statistics.Output();
}