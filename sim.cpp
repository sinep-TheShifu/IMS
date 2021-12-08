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

// System input model 
class Generator : public Event
{
    void Behaviour()
    {
        (new Customer)->Activate();
        Activate(Time+Exponential(1));  // TODO 
    }
}

        Facility Box("Box");
Store

// Time of model
double Time;

int main()
{

    cout << "*** Inicializacia" << endl;

    Time = 0;

    cout << "Time = " << Time << endl;

    schedule(event1, Time);     // Prva aktivacia

    while (!calendar.empty())
    {

    }

}