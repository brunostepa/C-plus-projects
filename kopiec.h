#pragma once

struct Zadanie
{
	Zadanie(int id_val, int R_val, int P_val, int Q_val) { id = id_val; R = R_val; P = P_val; Q = Q_val; }
	void wyswietl();
	int id;
	int R, P, Q;
};

class Kopiec
{
public:
	Kopiec(int rozmiar_kopca, int typ) { zadania = new Zadanie * [rozmiar_kopca + 1]; ilosc = 0; tryb = typ; }
	~Kopiec() {};
	void dodaj(Zadanie* zad);
	void porownaj_z_rodzicem(int nr_elementu);
	void porownaj_z_dziecmi(int nr_elementu);
	bool porownaj_dwa_elementy(int elem1, int elem2);
	int ile_elem() { return ilosc; }
	void zamien_elementy(int nr_elementu);
	Zadanie* pobierz_z_kopca();
	int odczytaj_wartosc();
	void wyswietl();

private:
	bool tryb; // 0 - Najwiêkszy na gorze(Q)  || 1 - Najmniejszy na górze(R)
	int ilosc; //Ilosc elemntów znajduj¹cych siê w kopcu
	Zadanie** zadania;
}; 
