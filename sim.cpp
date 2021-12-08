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

Facility narocne("Narocne");
Facility normalni("Normalni");

class Zakaznik : public Process {
    void Behavior(){
        if (Uniform(0,100) <= 33){
            Seize(narocne);
            cout << "*** Zabiram narocne ***" << endl;
            Wait(Exponential(45));
            Release(narocne);
        } else {

        }
    }
};

// System input model 
class Generator : public Event {
    void Behavior()
    {
        (new Zakaznik)->Activate();
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