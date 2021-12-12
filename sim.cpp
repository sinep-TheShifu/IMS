#include <iostream>
#include <cstdlib>  
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
Store miestaPrevozidla("miestaPrevozidla", pocet_parkovacich_miest);
Store vozidloNaPozicanie("vozidloNaPozicanie", pocet_pozicovanych_aut);

void help()
{
    cout << "usage: ./sim [<options> ...] \n\n"
    "<options> is one of\n"
    "-pzc <count>\n"
    "\tnumber of employees working in cleaning\n"
    "-pzd <count>\n"
    "\tnumber of employees working on design\n"
    "-pzl <count>\n"
    "\tnumber of employees working on glueing\n"
    "-ppm <count>\n"
    "\tnumber of parking spots inside the building\n"
    "-ppa <count>\n"
    "\tnumber of rental vehicles\n" << endl;
    exit(0);
}

void spracovanie_argumentov(int argc, char * argv[])
{
    cout << "*** Spracovanie argumentov" << endl;
    for (int i = 1; i < argc ; i++)
    {
        if ((strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0))
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

// Proces pozicania vozidla
class PozicanieVozidla : public Process 
{
    void Behavior()
    {
        //double prichod = Time;
        int pravdepodobnost = Uniform(0, 100);

        if (pravdepodobnost <= 15)
        {
            // Je volne vozidlo na pozicanie
            if (vozidloNaPozicanie.Free() != 0)
            {
                Enter(vozidloNaPozicanie, 1);
                cout << "Pozicanie vozidla: Zakaznik si pozical 1 vozidlo." << endl;    

                // Najkratsi mozny proces je cistenie vozidla : 2h + 6h = 8h
                // Najdlhzsi proces je lepenie designom : 2h + 12h + 25h = 39h
                // + Exp(4h) je priblizna doba vozidla stravena vo frontach
                Wait(Uniform(480, 2580));

                Leave(vozidloNaPozicanie, 1);
                cout << "Pozicanie vozidla: Zakaznik vratil pozicane vozidlo." << endl;
            }
            // Nieje volne vozidlo na pozicanie
            else
            {
                cout << "Pozicanie vozidla: Zakaznik si chcel pozicat vozidlo, ale nebolo dostupne." << endl;
                return;
            }
        } 
        // Zakaznik si vozidlo nepozicia s 85% pravdepodobnostou
        else
        {
            cout << "Pozicanie vozidla: Zakaznik si vozidlo nechce pozicat." << endl;
            return;
        }       
    }
};

// Proces zakaznik
class Zakaznik : public Process 
{
    void Behavior()
    {
        // Volne parkovacie miesta pre vozidla v dielni
        if(miestaPrevozidla.Free() != 0)
        {

            // GOTO pre zakaznika ktory sa druhy krat vracia a je volne miesto
            obsluha: 
            int pravdepodobnost = Uniform(0, 100);

            Enter(miestaPrevozidla, 1);
            cout << "Miesta pre vozidla: Zaberam 1 parkovacie miesto na dielni." << endl;

            // Vytvara sa proces pozicania vozidla  
            (new PozicanieVozidla)->Activate();
                Activate(Time);

            // Cistenie
            if (pravdepodobnost <= 20) // 20% pravdepodobnost
            {    
                Enter(zamestnanecNaCisteni, 1);
                cout << "Cistenie info: Zaberam 1 zamestnanca na cistenie vozidla" << endl;

                Seize(myciBox);
                cout << "Cistenie info: Zaberam mici box na cistenie vozidla" << endl;

                Wait(Exponential(120));

                Release(myciBox);
                cout << "Cistenie info: Uvolnujem myci box" << endl;

                Wait(Exponential(360));
                cout << "Cistenie info: vozidlo je vycistene a pripravene na predaj zakaznikovy" << endl;

                Leave(zamestnanecNaCisteni, 1);
                cout << "Cistenie info: Uvolnujem 1 zamestnanca na cistenie" << endl;

                Leave(miestaPrevozidla, 1);
                cout << "Miesta pre vozidla: Uvolnujem 1 parkovacie miesto na dielni." << endl;
            }
            // Polep dizajnom
            else  if(pravdepodobnost > 20 && pravdepodobnost <= 40 ) // 20% pravdepodobnost
            {   
                Enter(zamestnanecNaDesign, 1);
                cout << "Dizajn info: Zabiram 1 zamestance na design" << endl;

                Seize(myciBox);
                cout << "Dizajn info: Zabiram myci box pred designem vozidla" << endl;

                Wait(Exponential(120)); // myti vozidla

                Release(myciBox);
                cout << "Dizajn info: Uvolnuji myci box a jdu na tvorbu designu" << endl;

                Wait(Exponential(7200)); // tvorba designu

                Leave(zamestnanecNaDesign, 1);
                cout << "Dizajn info: Uvolnuji 1 zamestance na design a jdu na tisk designu" << endl;

                Wait(360); // tisk designu
                cout << "Dizajn info: Tisk dokoncen, vozidlo je pripravene na nalepeni designu" << endl;

                Enter(zamestnanecNaLepeni, 1);
                cout << "Dizajn info: Zabiram 1 zamestance na nalepeni designu" << endl;

                Wait(Exponential(1500)); // lepeni designu
                cout << "Dizajn info: vozidlo je dokoncene a pripravene na predaj zakaznikovy" << endl;

                Leave(zamestnanecNaLepeni, 1);
                cout << "Dizajn info: Uvolnuji 1 zamestance na nalepeni designu" << endl;

                Leave(miestaPrevozidla, 1);
                cout << "Miesta pre vozidla: Uvolnujem 1 parkovacie miesto na dielni." << endl;   
            }
            // Polep standardni
            else if (pravdepodobnost > 40) // 60% pravdepodobnost
            {            
                Enter(zamestnanecNaLepeni, 1);
                cout << "Lepenie info: Zabiram 1 zamestance na polep" << endl;

                Seize(myciBox);
                cout << "Lepenie info: Zabiram myci box pred polepem vozidla" << endl;

                Wait(Exponential(120)); // myti vozidla

                Release(myciBox);
                cout << "Lepenie info: Uvolnuji myci box a jdu na polep" << endl;

                Wait(Exponential(1200)); // polep vozidla
                cout << "Lepenie info: vozidlo je dokoncene a pripravene na predaj zakaznikovy" << endl;

                Leave(zamestnanecNaLepeni, 1);
                cout << "Lepenie info: Uvolnuji 1 zamestance na lepeni" << endl;

                Leave(miestaPrevozidla, 1);
                cout << "Miesta pre vozidla: Uvolnujem 1 parkovacie miesto na dielni." << endl;
            }
        }
        // Nieje volne parkovacie miesto pre vozidlo v dielni
        else
        {
            cout << "Nieje volne parkovacie miesto na dielni." << endl;

            // Zakaznik sa vracia - 60% pravdepodobnost
            if (Uniform(0, 100) <= 60) 
            {
                cout << "Nebolo volne parkovacie miesto. Zakaznik sa vracia o Exp(8h)." << endl;

                Wait(Exponential(480));

                // Miesto pre vozidlo na dielni je volne
                if (miestaPrevozidla.Free() != 0)
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
    SetOutput("data_out.dat");
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
    
    miestaPrevozidla.Output();
    vozidloNaPozicanie.Output();

    SIMLIB_statistics.Output();
}