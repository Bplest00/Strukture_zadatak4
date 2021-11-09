#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#define MAXPATH 260
#define MAXLINE 1024


// 1.Pitanje: KBT je napravila polynomial.h i polynomial.c --> .h su samo deklaracije funkcija --> main.c includa samo .h file. Jel se poly.c i main.c skupa kompajlaju?

struct _polinom;
typedef struct _polinom * position;
typedef struct _polinom {
	int koef;
	int exp;
	position next;
}polinom;

FILE* OtvoriDatoteku(char* path, FILE* in) {
	in = fopen(path, "r");
	if (in == NULL) 
		{ 
		printf("Neuspjesno otvaranje datoteke");
		}
	
	return in;
}                               

					//Prije citanja potreban mi je sortirani unos --> za sortirani unos bilo bi dobro napravit par funkcija za iskoristit

position Stvori(int exp, int koef) {
	position novi_elem = NULL;
	novi_elem = (position)malloc(sizeof(polinom));
	if (!novi_elem) { return NULL; }
	novi_elem->exp = exp;
	novi_elem->koef = koef;
	novi_elem->next = NULL;
	return novi_elem;
}

int UnesiNakon(position misto, position novi_elem) {
	novi_elem->next = misto->next;
	misto->next = novi_elem;
	return 0;
}
int StvoriUnesiNakon(position misto,int koef, int exp) {
	position novi_elem = Stvori(koef, exp);
	if (!novi_elem) {
		return EXIT_FAILURE;
	}
	UnesiNakon(misto, novi_elem); 
	                                  // 2.PITANJE --> KBT JE ODE IMALA POSITION=POSITION->NEXT? CINI MI SE REDUDANTNO S OBZIROM DA U INSERT AFTER SE IZVRSI ZAMJENA POKAZIVACA?
	return 0;
}

int BrisiNakon(position prethodnik) {
	position za_brisat = NULL;
	za_brisat = prethodnik->next;
	prethodnik->next = za_brisat->next;
	free(za_brisat);
	return 0;
}


int BrisiListu(position head) {
	position temp = head;
	while (temp->next) {
		BrisiNakon(temp);
	}
	return 0;
}

int SpojiNakon(position misto, position novi_elem) {
	if (misto->next == NULL || misto->next->exp != novi_elem->exp) {
		// -->samo provjeravamo jeli kraj liste ili je razlicit eksponent, sortirani unos se pobrinia da su poredani po velicini--> Ocito iz "SpojiNAKON"
		UnesiNakon(misto, novi_elem);
	}
	else { // --> SADA PROVJERAVAMO KOEFICIJENTE
		int rezultat = misto->next->koef + novi_elem->koef;
		if (rezultat == 0) { BrisiNakon(misto); }
		else {
			misto->next->koef = rezultat;
			free(novi_elem);
		}
	}
	return 0;
}

int SortirajUnesi(position head, position novi_elem) 
{
	position temp = head;
	while ((temp->next != NULL) || (temp->next->exp > novi_elem->exp))
	{
		temp = temp->next;
	}
	SpojiNakon(temp,novi_elem); //NETRIBA FREE novi_elem, funckija spoji nakon to ucini

	return 0; 
}

int Pretvori(char* buffer, position head) {
	position novi_elem = NULL;
	int numbyte = 0;
	int status = 0;  //broj procitanih/unesenih
	int koef = 0;
	int exp = 0;
	char* c_buffer = buffer; //U pocetnom trenutku puni line
	while (strlen(c_buffer) > 0) {
		status = sscanf(c_buffer, "%d %d %n", &koef, &exp, &numbyte);
		if (status != 2) 
		{
			printf("Fajl nije dobar");
			return -1;
		}
		novi_elem = Stvori(exp, koef);
		if (!novi_elem) {
			return -1;
		}
		SortirajUnesi(head, novi_elem);
		c_buffer = c_buffer + numbyte; // Pomicemo pointer duz line-a
		
	}

	return 0;
}



int CitajDatoteku(FILE* in, position head1, position head2) {
	int status = 0;
	char buffer[MAXLINE] = { 0 };
	fgets(buffer, MAXLINE, in);
	status = Pretvori(buffer, head1);
	if (status != 0) {
		return -1;
	}
	fgets(buffer, MAXLINE, in);
	status = Pretvori(buffer, head2);
	if (status != 0) { return -1; }
	
	fclose(in);
	return 0;
}

int pomnozi(position produkthead, position head1, position head2) {
	position j = NULL;
	position i = NULL;
	for (i = head1->next;i != NULL;i = i->next) {
		for (j = head2->next;j != NULL;j = j->next) {
			position novi_elem = Stvori(i->exp + j->exp, i->koef * j->koef);
			if (!novi_elem) {
				return -1;
			}
			SortirajUnesi(produkthead, novi_elem);
		}
	}
	return 0;
}
int zbroji(position zbrojhead, position head1, position head2) {
	position i = head1->next;
	position j = head2->next;
	position k = zbrojhead;
	position temp = NULL;
	//PRVA PETLJA DOK NE DODEMO DO KRAJA POLINOMA NAKON TOGA PROVJERA DO CIJEG KRAJA SMO DOSLI
	while (i != NULL && j != NULL) {
		if (i->exp == j->exp) {
			StvoriUnesiNakon(zbrojhead, i->koef + j->koef, i->exp);
			i = i->next;
			j = j->next;
		}
		else if (i->exp > j->exp) {
			StvoriUnesiNakon(zbrojhead, i->koef, i->exp);

		}
		else {
			StvoriUnesiNakon(zbrojhead, j->koef, j->exp);
			j = j->next;
		}

	}
	if (i == NULL) { temp = j; }
	if (j == NULL) { temp = i; }
	while (temp != NULL) {
		StvoriUnesiNakon(zbrojhead, temp->koef, temp->exp);
		temp = temp->next;
	}

	return 0;
}


int main() {
	FILE *in=NULL;
	char path[MAXPATH];
	polinom head1 = { .koef = 0, .exp = 0, .next = NULL };
	polinom head2 = { .koef = 0, .exp = 0, .next = NULL }; 
	polinom produkt = { .koef = 0, .exp = 0, .next = NULL };
	polinom zbroj = { .koef = 0, .exp = 0, .next = NULL };
	                          // ---> Petlja za otvaranje datoteke
	while (in == NULL) {
		scanf("%s", path);
		in = OtvoriDatoteku(path, in);
	}
	CitajDatoteku(in, &head1, &head2);
	zbroji(&zbroj, &head1, &head2);
	pomnozi(&produkt, &head1, &head2);
	BrisiListu(&head1);
	BrisiListu(&head2);
	BrisiListu(&produkt);
	BrisiListu(&zbroj);
	return 0;
	

}




