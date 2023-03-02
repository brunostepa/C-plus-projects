#include "kopiec.h"
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;

// ============== ZADANIE (KLASA) ===============
void Zadanie::wyswietl()
{
	printf("ID: %d R: %d P: %d Q: %d\n", id, R, P, Q);
}



// ============== KOPIEC (KLASA) ================
void Kopiec::dodaj(Zadanie* zad)
{
	ilosc++;
	zadania[ilosc] = zad;
	//cout << "Pomyslnie dodano Zadanie(" << zad << ") do kopca - " << endl;
	//zad->wyswietl();
	porownaj_z_rodzicem(ilosc); //Dodany elemnt do ogona -> 
								//Trzeba zobaczyc czy mozna przesunac wyzej
}

void Kopiec::porownaj_z_rodzicem(int dziecko)
{
	if (dziecko == 1) return;//Pierwszy element na Kopcu -> nie ma co porównywaæ
	int rodzic = dziecko / 2;
	if (porownaj_dwa_elementy(dziecko, rodzic))
	{
		zamien_elementy(dziecko);
		porownaj_z_rodzicem(rodzic); // <-- jest juz po zamianie 
	}
}

void Kopiec::porownaj_z_dziecmi(int rodzic)
{
	int dziecko = rodzic * 2;
	if (ilosc < dziecko) return; //za ma³o elemntow zeby rodzic móg³ wgl posiadaæ dziecko
	if (ilosc == dziecko) //dany rodzic ma tylko 1 dziecko
	{
		if (porownaj_dwa_elementy(dziecko, rodzic))
		{
			zamien_elementy(dziecko);
			porownaj_z_dziecmi(dziecko);// <- jest ju¿ po zamianie
		}
		return;
	}
	if (ilosc > dziecko)
	{
		//Porównanie dzieci -> Wybranie lepszego dziecka
		if (porownaj_dwa_elementy(dziecko + 1, dziecko)) //Czy prawe dziecko jest lepszym kandydatem?
			dziecko += 1; //Dziecko prawe jest lepsze do zamiany np.(w kopcu R jest wiêksze od lewego)
		//Porównanie dziecka z rodzicem
		if (porownaj_dwa_elementy(dziecko, rodzic))
		{
			zamien_elementy(dziecko);
			porownaj_z_dziecmi(dziecko);
		}
	}
}

/*
 * Porównuje dwa elementy:
 * Jezeli jest to kopiecR zwraca True jezeli elem1 < elem2
 * Jezeli jest to kopiecQ zwraca True jezeli elem1 > elem2
 * W przeciwnym zwraca False
 * Funkcja odpowiada na pytanie - "Czy elem1 jest lepszym kandydatem od elem2"
 */
bool Kopiec::porownaj_dwa_elementy(int elem1, int elem2)
{
	if ((elem1 > ilosc) || (elem2 > ilosc)) cout << "OJOJ" << endl;
	if (tryb == 1) //Kopiec R
		if (zadania[elem1]->R < zadania[elem2]->R) return true;
	if (tryb == 0) //Kopiec Q
		if (zadania[elem1]->Q > zadania[elem2]->Q) return true;
	return false;
}

void Kopiec::zamien_elementy(int dziecko)
{
	int rodzic = dziecko / 2;
	Zadanie* pom = zadania[dziecko];
	zadania[dziecko] = zadania[rodzic];
	zadania[rodzic] = pom;
}

Zadanie* Kopiec::pobierz_z_kopca()
{
	Zadanie* pom = zadania[1];
	zadania[1] = zadania[ilosc];
	ilosc--;
	porownaj_z_dziecmi(1);
	return pom;
}

int Kopiec::odczytaj_wartosc()
{
	return zadania[1]->R;
}

void Kopiec::wyswietl()
{
	for (int i = 1; i <= ilosc; i++)
	{
		if (log2(i) == floor(log2(i))) cout << endl;
		for (int j = 0; j < 20 - 4 * i; j++) cout << " ";
		cout << zadania[i]->R << "[" << zadania[i]->id << "]" << "  ";

	}
}