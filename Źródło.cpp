#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <chrono>
#include "kopiec.h"
#include <windows.h>
#define POKAZ_KOLEJNOSC 1

using namespace std;
void z_kopca_do_kopca(Kopiec& z, Kopiec& d, int M)
{
	while (z.odczytaj_wartosc() <= M)
	{
		d.dodaj(z.pobierz_z_kopca());
		if (z.ile_elem() == 0) break;
	}
}
void testuj_kopiec()
{
	int N = 10;
	Kopiec kopiecR(N, 1);
	Zadanie* nowe_zadanie;
	for (int i = 0; i < 10; i++)
	{
		nowe_zadanie = new Zadanie(i, 0, i * 2, 0);
		kopiecR.dodaj(nowe_zadanie);
		kopiecR.wyswietl();
		cout << endl << "========================================================" << endl;
	}
}
//Wykonanie operacji i policzenie cmax (na zajeciach) | z X[] -> cmax
int policz_cmax(int N, int* R, int* P, int* Q, int* X)
{
	// Pierwsze zajęcia SPD
	int m = 0, c = 0;
	for (int i = 0; i < N; i++)
	{
		int id = X[i];
		m = max(m, R[id]) + P[id];
		c = max(c, m + Q[id]);
	}
	return c;
}
int Schrage_tablica_przerywana(int N, int* R, int* P_org, int* Q)
{
	int* P = new int[N];
	int maxCzas = 12000; int pom = 0, c = 0;
	int X[100];  //Tablica przechowująca id kolejności wykonania operacji na maszynie
	int RX[100]; //Tablica przechowująca id wg. najmniejszego czasu przyjazdu
	int QX[100]; //Tablica przechowująca id wg. największego czasu odczekania
	//Ustalenie kolejności | z R[],Q[] -> RX[],QX[]
	// RX - Od najmniejszego czasu przyjazdu
	// QX - Od największego czasu odczekania
	for (int i = 0; i < N; i++)
	{
		P[i] = P_org[i];
		RX[i] = i;
		QX[i] = i;
		pom = i;
		while (pom > 0) //od najmniejszego czasu przyjazdu - RX
		{
			if (R[RX[pom]] < R[RX[pom - 1]]) swap(RX[pom], RX[pom - 1]);
			else pom = 0;
			pom--;
		}
		pom = i;
		while (pom > 0) //Od największego czasu odczekania - QX
		{
			if (Q[QX[pom]] > Q[QX[pom - 1]]) swap(QX[pom], QX[pom - 1]);
			else pom = 0;
			pom--;
		}
	}
	// Ustalnie kolojeności wykonania operajci | z RX[],QX[] -> X[]
	int M = 0, przesuniecie = 0, j = 0, najlepsze_czekajace = -1, wybrane_zadanie = 0;   //Moment na maszynie ¯\_(ツ)_/¯
	for (int i = 0; i < N; i++)
	{
		najlepsze_czekajace = -1;
		M = max(M, R[RX[0]]); //Jezeli nie mozna wykonac zadnej operacji przeskocz do miejsca kiedy bedzie mozna wykonac (tworzy dziury)
		for (j = 0; j < N; j++)
		{
			if (R[QX[j]] <= M)
			{
				wybrane_zadanie = QX[j];
				break;
			}
			if (najlepsze_czekajace == -1) najlepsze_czekajace = QX[j]; //Nie ma jeszcze wybranego najlepszego zadania czekającego na wykoniania
			else if (R[najlepsze_czekajace] > R[QX[j]]) najlepsze_czekajace = QX[j];
		}
		if (najlepsze_czekajace == -1) przesuniecie = P[wybrane_zadanie]; //Zadne zadanie nie jest lepsze, mozna wykonać całe wybrane zadanie
		else przesuniecie = min(P[wybrane_zadanie], R[najlepsze_czekajace] - M);
		//cout << wybrane_zadanie + 1 << " "; // Wyswietl id wykonywanej operacji 
		//cout << " wybiream zadanie " << wybrane_zadanie + 1 << " od " << M << " do " << M + przesuniecie << endl;
		M += przesuniecie;
		P[wybrane_zadanie] -= przesuniecie;
		if (P[wybrane_zadanie] == 0) //Zadanie wykonało się w pełni można usununąć/przesunąć z oczekujących
		{
			//cout << "Zadanie " << wybrane_zadanie + 1 << "w prełni wykonane" << endl;
			for (int j = 0; j < N - i; j++) // Przerzucenie id z 1 miejsca na koniec kolejki [R] i z miejsca X na koniec [Q]
			{
				if (QX[j] == wybrane_zadanie) swap(QX[j], QX[j + 1]);
				if (RX[j] == wybrane_zadanie) swap(RX[j], RX[j + 1]);
			}
		}
		else i--;
		c = max(c, M + Q[wybrane_zadanie]);
	}
	return c;
}
int Schrage_kopiec_przerywany(int N, int* R, int* P, int* Q)
{
	Kopiec niedostepne(N, 1); //Zadania posortowane wg. R
	Kopiec dostepne(N, 0); //Zadania posortowane wg. Q
	Zadanie* zad = nullptr;
	for (int i = 0; i < N; i++) //3x Tablica(R,P,Q) -> Zadanie -> Kopiec
	{
		zad = new Zadanie(i, R[i], P[i], Q[i]);
		niedostepne.dodaj(zad);
	}
	int M = 0, i = 0, przesuniecie = 0, c = 0;
	while (niedostepne.ile_elem() + dostepne.ile_elem() > 0)
	{
		if (dostepne.ile_elem() == 0) M = max(M, niedostepne.odczytaj_wartosc());
		if (niedostepne.ile_elem() > 0) z_kopca_do_kopca(niedostepne, dostepne, M); //Dostępne i niedostępne zaktualizowane do momemntu na maszynie
		zad = dostepne.pobierz_z_kopca();
		//Sprawdzenie czy są jakieś niedostępne zadania -> sprawdzenie kiedy mogą się wykonać
		if (niedostepne.ile_elem() > 0) przesuniecie = min(zad->P, niedostepne.odczytaj_wartosc() - M);
		else przesuniecie = zad->P;
		//Wykonanie zadania na maszynie
		M += przesuniecie; zad->P -= przesuniecie;
		if (zad->P == 0) c = max(c, M + zad->Q); //jezeli zadanie wykonało się w pełni dodajemy czas stygnięcia
		else dostepne.dodaj(zad);				 //nie wykonało się -> powrót do kopca (zadanie trafi na początek lub wyprzedzi je nowe zadanie(a))
	}
	return c;
}
int Schrage_kopiec(int N, int* R, int* P, int* Q, int* kolejka, bool kolejnosc = 0)
{
	Kopiec niedostepne(N, 1); //Zadania posortowane wg. R
	Kopiec dostepne(N, 0); //Zadania posortowane wg. Q
	//int kolejka[100];    //Sluzy do przechowywania kolejności zadań żeby obliczyc cmax
	Zadanie* zad = nullptr;
	for (int i = 0; i < N; i++)
	{
		zad = new Zadanie(i, R[i], P[i], Q[i]);
		niedostepne.dodaj(zad);
	}
	int M = 0, i = 0;
	while (niedostepne.ile_elem() + dostepne.ile_elem() > 0)
	{
		if (dostepne.ile_elem() == 0) M = max(M, niedostepne.odczytaj_wartosc());
		if (niedostepne.ile_elem() > 0) z_kopca_do_kopca(niedostepne, dostepne, M); //Dostępne i niedostępne zaktualizowane do momemntu na maszynie
		zad = dostepne.pobierz_z_kopca();
		kolejka[i] = zad->id; // <------ zad->id + 1
		if (kolejnosc == POKAZ_KOLEJNOSC) cout << zad->id + 1 << " ";
		M += zad->P;
		i++;
	}
	return policz_cmax(N, R, P, Q, kolejka);
}
int ALG(int N, int* R, int* P, int* Q, bool kolejnosc = 0) //<---Schrage na tablicy
{
	int maxCzas = 12000; int pom = 0;
	int X[100];  //Tablica przechowująca id kolejności wykonania operacji na maszynie
	int RX[100]; //Tablica przechowująca id wg. najmniejszego czasu przyjazdu
	int QX[100]; //Tablica przechowująca id wg. największego czasu odczekania
	//Ustalenie kolejności | z R[],Q[] -> RX[],QX[]
	// RX - Od najmniejszego czasu przyjazdu
	// QX - Od największego czasu odczekania
	for (int i = 0; i < N; i++)
	{
		RX[i] = i;
		QX[i] = i;
		pom = i;
		while (pom > 0) //od najmniejszego czasu przyjazdu - RX
		{
			if (R[RX[pom]] < R[RX[pom - 1]]) swap(RX[pom], RX[pom - 1]);
			else pom = 0;
			pom--;
		}
		pom = i;
		while (pom > 0) //Od największego czasu odczekania - QX
		{
			if (Q[QX[pom]] > Q[QX[pom - 1]]) swap(QX[pom], QX[pom - 1]);
			else pom = 0;
			pom--;
		}
	}
	// Ustalnie kolojeności wykonania operajci | z RX[],QX[] -> X[]
	int M = 0;   //Moment na maszynie ¯\_(ツ)_/¯
	for (int i = 0; i < N; i++)
	{
		M = max(M, R[RX[0]]); //Jezeli nie mozna wykonac zadnej operacji przeskocz do miejsca kiedy bedzie mozna wykonac (tworzy dziury)
		for (int j = 0; j < N; j++)
			if (R[QX[j]] <= M) { pom = QX[j]; break; };
		if (kolejnosc == POKAZ_KOLEJNOSC)cout << pom + 1 << " "; // Wyswietl id wykonywanej operacji 
		M += P[pom];
		X[i] = pom;
		for (int j = 0; j < N - i; j++) // Przerzucenie id z 1 miejsca na koniec kolejki
		{
			if (QX[j] == pom) swap(QX[j], QX[j + 1]);
			if (RX[j] == pom) swap(RX[j], RX[j + 1]);
		}
	}
	return policz_cmax(N, R, P, Q, X);
}
int wezly = 0;
int cMax = 99999;
void carlier(int N, int* R, int* P, int* Q, int* X, int &UB) {
	wezly++;
	int cTemp, LB;
	int kolej[50];
	cTemp = Schrage_kopiec(N, R, P, Q, kolej);
	LB = Schrage_kopiec_przerywany(N, R, P, Q);
	//if (cTemp < UB) UB = cTemp;
	if (cTemp < cMax) {
		cMax = cTemp;
		for (int i = 0; i < N; i++) X[i] = kolej[i];
	}
	if (LB >= cMax) return;
	int maxQ, maxQindeks, minQ, criticalIndeks, minRblock, minQblock, blockLength;
	int flagaWyjscia = 0;
	maxQ = -1;
	minRblock = 9999;
	minQblock = 9999;
	//criticalIndeks = -1;
	blockLength = 0;
	//maxQindeks = -1;
	int* kol = new int[N];
	for (int i = 0; i < N; i++) kol[i] = X[i];
	int m = 0;
	for (int i = 0; i < N; i++)
	{
		int id = kol[i];
		m = max(m, R[id]) + P[id];
		if ((m + Q[id]) > maxQ) {
			maxQ = m + Q[id];
			maxQindeks = i;
		}
	}
	//if (maxQindeks == -1) return;
	minQ = Q[kol[maxQindeks]];
	int mBef = 0;
	int p = maxQindeks;
		while (flagaWyjscia == 0 && p > -1) {
			mBef = 0;
			for (int j = 0; j < p; j++)
			{
				int id = kol[j];
				mBef = max(mBef, R[id]) + P[id];
			}
			if (R[kol[p]] > mBef) {
				flagaWyjscia = 1;
			}
			if (Q[kol[p]] < minQ) {
				minQ = Q[kol[p]];
				criticalIndeks = p;
				flagaWyjscia = 1;
			}
			p--;
		}
	if (minQ == Q[kol[maxQindeks]]) return;
	//if (criticalIndeks == maxQindeks) return;
	//if (criticalIndeks == -1) return;
	//if (minQ == -1) return;
	int h = kol[criticalIndeks];
	for (int i = criticalIndeks+1; i<=maxQindeks; i++)	blockLength = blockLength + P[kol[i]];
	for (int i = criticalIndeks + 1; i <= maxQindeks; i++) {
		minRblock = min(minRblock, R[kol[i]]);
		minQblock = min(minQblock, Q[kol[i]]);
	}
	int rTemp = R[h];
	int qTemp = Q[h];
	R[h] = minRblock + blockLength;
	//int HR = min(minRblock, R[h]) + blockLength + P[h] + min(minQblock, Q[h]);
	//if (HR < cMax) 
	carlier(N, R, P, Q, X, cMax);
	R[h] = rTemp;
	Q[h] = minQblock + blockLength;
	//int HQ = min(minRblock, R[h]) + blockLength + P[h] + min(minQblock, Q[h]);
	//if (HQ < cMax)
	carlier(N, R, P, Q, X, cMax);
	Q[kol[h]] = qTemp;
}
int main()
{
	//testuj_kopiec();
	//int N, X[7];
	//N = 7;
	//int R[7] = { 10, 13, 11, 20, 30, 0, 30 };
	//int P[7] = { 5, 6, 7, 4, 3, 6, 2 };
	//int Q[7] = { 7, 26, 24, 21, 8, 17, 0 };
	//int UBinit = 99999;
	//carlier(N, R, P, Q, X, UBinit);
	//cout << cMax << endl;
	//cout << wezly << endl;
	int N, R[50], P[50], Q[50], X[50];
	string dane[] = { "data.000:","data.001:","data.002:","data.003:","data.004:","data.005:", "data.006:","data.007:","data.008:"};
	ifstream data("C:/Users/bruno/Desktop/data.txt");
	std::string tmp;
	int UBinit = 99999;
	for (int j = 0; j < 9; j++)
	{
		//Czytanie danych z pliku (z zajęć)
		while (tmp != dane[j]) data >> tmp;
		data >> N;
		for (int i = 0; i < N; i++)
			data >> R[i] >> P[i] >> Q[i];

		carlier(N, R, P, Q, X, UBinit);
		cout << cMax << endl;
		cMax = 99999;
		for (int i = 0; i < N; i++) cout << X[i] << " ";
		cout << endl;
		cout << wezly << endl;
		wezly = 0;
	}
	data.close();
}
