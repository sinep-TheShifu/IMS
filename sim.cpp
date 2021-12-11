#include <iostream>
#include <cstdlib>  // exit()
#include <cstring>
#include "simlib.h"

using namespace std;

// Globalne premenne
int pocet_zamestancov_cistenie = 2;
int pocet_zamestancov_dizajn = 2;
int pocet_zamestancov_lepenie = 7;
int pocet_parkovacich_miest = 8;
int pocet_pozicovanych_aut = 2;

// Facilities
Facility myciBox("MyciBox");

// Stores
Store zamestnanecNaCisteni("zamestanecNaCisteni", pocet_zamestancov_cistenie);
Store zamestnanecNaDesign("zamestnanecNaDesign", pocet_zamestancov_dizajn);
Store zamestnanecNaLepeni("zamestanecNaLepeni", pocet_zamestancov_lepenie);
Store miestaPreAuta("miestaPreAuta", pocet_parkovacich_miest);
Store autoNaPozicanie("autoNaPozicanie", pocet_pozicovanych_aut);

// Histogram
Histogram dokonceniePoziadavku("Celkova doba poziadavku v systeme", 0, 60, 20);
Histogram pozicanieAuta("...", 0, 60, 15);

void help()
{
    cout << "Help" << endl;
    exit(0);
}

void spracovanie_argumentov(int argc, char * argv[])
{
    cout << "*** Spracovanie argumentov" << endl;
    for (int i = 1; i < argc ; i++)
    {
        if ((strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) && argv[i + 1] != NULL)
        {
            help();
        }            
        else if (strcmp(argv[i], "-pzc") == 0 && argv[i + 1] != NULL)        
        {
            try
            {
                pocet_zamestancov_cistenie = stoi(argv[i + 1]);     
                cout << "Pocet zamestnancov na cistenie je nastaveny na: " << pocet_zamestancov_cistenie << endl;
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            i++;
        }            
        else if (strcmp(argv[i], "-pzd") == 0 && argv[i + 1] != NULL)
        {
            try
            {
                pocet_zamestancov_dizajn = stoi(argv[i + 1]);     
                cout << "Pocet zamestnancov na dizajn je nastaveny na: " << pocet_zamestancov_dizajn << endl;
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            i++;
        }            
        else if(strcmp(argv[i], "-pzl") == 0 && argv[i + 1] != NULL)
        {
            try
            {
                pocet_zamestancov_lepenie = stoi(argv[i + 1]);     
                cout << "Pocet zamestnancov na lepenie je nastaveny na: " << pocet_zamestancov_lepenie << endl;
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            i++;
        }            
        else if(strcmp(argv[i], "-ppm") == 0 && argv[i + 1] != NULL)
        {
            try
            {
                pocet_parkovacich_miest = stoi(argv[i + 1]);     
                cout << "Pocet parkovacich miest je nastaveny na: " << pocet_parkovacich_miest << endl;
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }    
            i++;
        }            
        else if(strcmp(argv[i], "-ppa") == 0 && argv[i + 1] != NULL)
        {
            try
            {
                pocet_pozicovanych_aut = stoi(argv[i + 1]);     
                cout << "Pocet aut na pozicanie je nastaveny na: " << pocet_pozicovanych_aut << endl;
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            i++;
        }            
        else
        {
            perror("Chyba argumentov programu.\n");
            exit(1);
        }
    }    
    cout << "*** Koniec spracovania agumentov"<< endl;
    cout << endl;
}

class PozicanieAuta : public Process 
{
    void Behavior()
    {
        //double prichod = Time;
        int pravdepodobnost = Uniform(0, 100);

        if (pravdepodobnost <= 15)
        {
            // Je volne auto na pozicanie
            if (autoNaPozicanie.Free() != 0)
            {
                Enter(autoNaPozicanie, 1);
                cout << "Pozicanie auta: Zakaznik si pozical 1 auto." << endl;    

                // Najkratsi mozny proces je cistenie vozidla : 2h + 6h = 8h
                // Najdlhzsi proces je lepenie designom : 2h + 12h + 25h = 39h
                // + Exp(4h) je priblizna doba vozidla stravena vo frontach
                Wait(Exponential(480) + Uniform(0, 2340) + Exponential(240));

                Leave(autoNaPozicanie, 1);
                cout << "Pozicanie auta: Zakaznik vratil pozicane auto." << endl;
            }
            // Nieje volne auto na pozicanie
            else
            {
                cout << "Pozicanie auta: Zakaznik si chcel pozicat auto, ale nebolo dostupne." << endl;
                return;
            }
        } 
        // Zakaznik si auto nepozicia s 85% pravdepodobnostou
        else
        {
            cout << "Pozicanie auta: Zakaznik si auto nechce pozicat." << endl;
            return;
        }       
    }
};

// Zakaznik
class Zakaznik : public Process 
{
    void Behavior()
    {
        //double prichod = Time;

        // Volne parkovacie miesta pre auta v dielni
        if(miestaPreAuta.Free() != 0)
        {

            // GOTO pre zakaznika ktory sa druhy krat vracia a je volne miesto
            obsluha: 
            int pravdepodobnost = Uniform(0, 100);

            Enter(miestaPreAuta, 1);
            cout << "Miesta pre auta: Zaberam 1 parkovacie miesto na dielni." << endl;

            // Vytvara sa proces pozicania auta  
            (new PozicanieAuta)->Activate();
                Activate(Time);

            // Cistenie
            if (pravdepodobnost <= 20) // 20% pravdepodobnost
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
            else  if(pravdepodobnost > 20 && pravdepodobnost <= 40 ) // 20% pravdepodobnost
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
            else if (pravdepodobnost > 40) // 60% pravdepodobnost
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

                Wait(Exponential(480));

                // Miesto pre auto na dielni je volne
                if (miestaPreAuta.Free() != 0)
                {
                    cout << "Pri druhom prichode do dielne bolo volne parkovacie miesto. Zakaznik ide do obsluhy." << endl;
                    // goto?
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

// Generator zakaznikov
class Generator_Zakaznik : public Event 
{
    void Behavior()
    {
        (new Zakaznik)->Activate();
            Activate(Time + Exponential(480));  
    }
};

// Modelovy cas
double Time;

// Main
int main(int argc, char * argv[])
{
    // Spracovanie argumentov
    spracovanie_argumentov(argc, argv);

    // Vystupne data simulacie
    SetOutput("polepy_out.dat");
    Print("Model firmy na polepy aut");

    cout << "*** Inicializacia ***" << endl;

    Init(0,14400); // 30 dni (po 8 hodinach)
    (new Generator_Zakaznik)->Activate();
    Run();

    // Statistiky
    zamestnanecNaCisteni.Output();
    zamestnanecNaDesign.Output();
    zamestnanecNaLepeni.Output();
    
    myciBox.Output();
    
    miestaPreAuta.Output();
    autoNaPozicanie.Output();

    pozicanieAuta.Output();
    dokonceniePoziadavku.Output();

    SIMLIB_statistics.Output();
}