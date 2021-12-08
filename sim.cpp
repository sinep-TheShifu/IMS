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

// pozadavek od zakaznika na polep, design nebo cisteni auta
class Pozadavek : public Process {
    void Behavior(){
        int rozdeleni = Uniform(0,100);
        if (rozdeleni <= 20 ){
            Enter(zamestnanecNaDesign, 1);
            cout << "Info: Zabiram 1 zamestance na design" << endl;
            Seize(myciBox);
            cout << "Info: Zabiram myci box pred designem auta" << endl;
            Wait(Normal(120)); // myti auta
            Release(myciBox);
            cout << "Info: Uvolnuji myci box a jdu na tvorbu designu" << endl;
            Wait(Normal(7200)); // tvorba designu

            Leave(zamestnanecNaDesign, 1)
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
        Activate(Time+Exponential(20));  // TODO
    }
};

// Time of model
double Time;

int main()
{

    cout << "*** Inicializacia ***" << endl;

    Init(0,1000);
    (new Generator)->Activate();
    Run();

    //Time = 0;

    //cout << "Time = " << Time << endl;

    //schedule(event1, Time);     // Prva aktivacia

    //while (!calendar.empty())
    //{

    //}

}