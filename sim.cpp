#include <iostream>
#include <cstdlib>  // exit()
#include "simlib.h"

using namespace std;

// Error message and quit program
void error(const char *msg)
{
    cerr << "ERROR: " << msg << endl;
    exit(1);
}

// Facilities
Facility myciBox("MyciBox");

// Stores
Store zamestnanecNaCisteni("zamestanecNaCisteni", 2);
Store zamestnanecNaDesign("zamestnanecNaDesign", 2);
Store zamestnanecNaLepeni("zamestanecNaLepeni", 7);
Store miestaPreAuta("miestaPreAuta", 8);

// Histogram
Histogram celk("Celkova doba v systeme", 0, 5, 20);

// pozadavek od zakaznika na polep standardny, dizajnom alebo cisteni auta
class Zakaznik : public Process 
{
    // Chovanie procesu
    void Behavior()
    {
        // Volne parkovacie miesta pre auta v dielni
        if(miestaPreAuta.Free() != 0)
        {

        obsluha: 
            int rozdeleni = Uniform(0, 100);

            Enter(miestaPreAuta, 1);
            cout << "Miesta pre auta: Zaberam 1 parkovacie miesto na dielni." << endl;

            // Cistenie
            if (rozdeleni <= 20) // 20% pravdepodobnost
            {    
                Enter(zamestnanecNaCisteni, 1);
                cout << "Cistenie info: Zaberam 1 zamestnanca na cistenie auta" << endl;

                Seize(myciBox);
                cout << "Cistenie info: Zaberam mici box na cistenie auta" << endl;

                Wait(Exponential(120));

                Release(myciBox);
                cout << "Cistenie info: Uvolnujem myci box" << endl;

                Wait(Exponential(360));
                cout << "Cistenie info: Auto je vycistene a pripravene na predaj zakaznikovy" << endl;

                Leave(zamestnanecNaCisteni, 1);
                cout << "Cistenie info: Uvolnujem 1 zamestnanca na cistenie" << endl;

                Leave(miestaPreAuta, 1);
                cout << "Miesta pre auta: Uvolnujem 1 parkovacie miesto na dielni." << endl;
            }
            // Polep dizajnom
            else  if(rozdeleni > 20 && rozdeleni <= 40 ) // 20% pravdepodobnost
            {   
                Enter(zamestnanecNaDesign, 1);
                cout << "Dizajn info: Zabiram 1 zamestance na design" << endl;

                Seize(myciBox);
                cout << "Dizajn info: Zabiram myci box pred designem auta" << endl;

                Wait(Exponential(120)); // myti auta

                Release(myciBox);
                cout << "Dizajn info: Uvolnuji myci box a jdu na tvorbu designu" << endl;

                Wait(Exponential(7200)); // tvorba designu

                Leave(zamestnanecNaDesign, 1);
                cout << "Dizajn info: Uvolnuji 1 zamestance na design a jdu na tisk designu" << endl;

                Wait(360); // tisk designu
                cout << "Dizajn info: Tisk dokoncen, auto je pripravene na nalepeni designu" << endl;

                Enter(zamestnanecNaLepeni, 1);
                cout << "Dizajn info: Zabiram 1 zamestance na nalepeni designu" << endl;

                Wait(Exponential(1500)); // lepeni designu
                cout << "Dizajn info: Auto je dokoncene a pripravene na predaj zakaznikovy" << endl;

                Leave(zamestnanecNaLepeni, 1);
                cout << "Dizajn info: Uvolnuji 1 zamestance na nalepeni designu" << endl;

                Leave(miestaPreAuta, 1);
                cout << "Miesta pre auta: Uvolnujem 1 parkovacie miesto na dielni." << endl;                
            }
            // Polep standardni
            else if (rozdeleni > 40) // 60% pravdepodobnost
            {            
                Enter(zamestnanecNaLepeni, 1);
                cout << "Lepenie info: Zabiram 1 zamestance na polep" << endl;

                Seize(myciBox);
                cout << "Lepenie info: Zabiram myci box pred polepem auta" << endl;

                Wait(Exponential(120)); // myti auta

                Release(myciBox);
                cout << "Lepenie info: Uvolnuji myci box a jdu na polep" << endl;

                Wait(Exponential(1200)); // polep auta
                cout << "Lepenie info: Auto je dokoncene a pripravene na predaj zakaznikovy" << endl;

                Leave(zamestnanecNaLepeni, 1);
                cout << "Lepenie info: Uvolnuji 1 zamestance na lepeni" << endl;

                Leave(miestaPreAuta, 1);
                cout << "Miesta pre auta: Uvolnujem 1 parkovacie miesto na dielni." << endl;
            }
        }
        // Nieje volne parkovacie miesto pre auto v dielni
        else
        {
            cout << "Nieje volne parkovacie miesto na dielni." << endl;

            // Zakaznik sa vracia - 60% pravdepodobnost
            if (Uniform(0, 100) <= 60) 
            {
                cout << "Nebolo volne parkovacie miesto. Zakaznik sa vracia o Exp(8h)." << endl;

                // Vracia sa po 8h
                Wait(Exponential(480));

                // Miesto pre auto na dielni je volne
                if (miestaPreAuta.Free() != 0)
                {
                    cout << "Pri druhom prichode do dielne bolo volne parkovacie miesto. Zakaznik ide do obsluhy." << endl;
                    // TODO - goto?
                    goto obsluha;
                }
                // Ziadne miesto nieje volne -> opusta system
                else
                {
                    cout << "Zakaznik opusta system, pretoze nebolo volne parkovacie miesto ani v druhom prichode." << endl;
                    return;
                }                
            }
            // Zakaznik opusta system - 40% pravdepodobnost
            else
            {
                cout << "Zakaznik opusta system, pretoze nebolo volne parkovacie miesto." << endl;
                return;
            }            
        }        
    }
};

// System input model 
class Generator : public Event 
{
    void Behavior()
    {
        (new Zakaznik)->Activate();
            Activate(Time+Exponential(480));  
    }
};

// Model time
double Time;

// Main
int main(){

    SetOutput("polepy_out.dat");
    Print("Model firmy na polepy aut");

    cout << "*** Inicializacia ***" << endl;

    Init(0,14400); // 30 dni (po 8 hodinach)
    (new Generator)->Activate();
    Run();

    // Statistiky
    celk.Output();
    zamestnanecNaCisteni.Output();
    zamestnanecNaDesign.Output();
    zamestnanecNaLepeni.Output();
    miestaPreAuta.Output();
    //SIMLIB_statistics.Output();
}

// Poznamky:
// - prerusenie cakanie vo fronte pre zakaznikov kazdych 8h?