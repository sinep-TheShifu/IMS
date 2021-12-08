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

// pozadavek od zakaznika na polep, design nebo cisteni auta
class Pozadavek : public Process {
    void Behavior(){
        int rozdeleni = Uniform(0,100);
        if (rozdeleni <= 20 ){
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
            // cisteni auta
        } else if ( rozdeleni > 40){
            // polep designem
        }
    }
};

// System input model 
class Generator : public Event {
    void Behavior()
    {
        (new Pozadavek)->Activate();
        Activate(Time+Exponential(8));  // TODO
    }
};

// Time of model
double Time;

int main()
{

    cout << "*** Inicializacia ***" << endl;

    Init(0,10000);
    (new Generator)->Activate();
    Run();

    //Time = 0;

    //cout << "Time = " << Time << endl;

    //schedule(event1, Time);     // Prva aktivacia

    //while (!calendar.empty())
    //{

    //}

}