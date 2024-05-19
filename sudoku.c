#include <stdlib.h>
#include <stdio.h>

#define DIM 9

void RecupGrille(int **grille);
void AffichageGrille(int **grille);
int RechercheLigne(int **grille, int y, int val);
int RechercheColonne(int **grille, int x, int val);
int RechercheCarre(int **grille, int x, int y, int val);
void PossibiliteCase(int **grille, int x, int y, int *resultat);
int NombrePossibilite(int *tab);
int NoombreCaseVide(int **grille);
void PossibiliteGrille(int **grille, int **possibilies_cases);
int ChoixUnique(int *tab);
void RemplirCaseUniqueChoix(int **grille, int **possibilies_cases);
void ExclusionLigne(int **grille, int **possibilies_cases);
void ExclusionColonnes(int **grille, int **possibilies_cases);
void ExclusionCarre(int **grille, int **possibilies_cases);
int Solution(int **grille, int **possibilies_cases);
void Supposition(int **grille, int **possibilies_cases);
void CopieGrille(int **grille1, int **grille2);
int GrilleCoherante(int **grille); 

int **g = NULL, nb = 0;/*g = grille du sudoku finale et nb le nombre d'appels a la fonction "Supposition"*/

int main()
{
	int  **possibilies_cases = NULL, i;
	
	g = (int**)calloc(DIM, sizeof(int*));
	for(i = 0; i < DIM; i++)
		g[i] = (int*)calloc(DIM, sizeof(int));
	possibilies_cases = (int**)calloc(DIM * DIM, sizeof(int*));
	for(i = 0; i < (DIM * DIM); i++)
		possibilies_cases[i] = (int*)calloc(DIM, sizeof(int));

	RecupGrille(g);
	printf("Grille de départ");
	AffichageGrille(g);
	Supposition(g, possibilies_cases);
	/*printf("\nGrille résolue avec %d appels a suposition():", nb);*/
	printf("\nSolution:");
	AffichageGrille(g);
	
	for(i = 0; i < DIM; i++)
		free(g[i]);
	for(i = 0; i < (DIM * DIM); i++)
		free(possibilies_cases[i]);
	free(g);
	free(possibilies_cases);

	return 0;
}

/*charge la grille a partir du fichier grille.txt*/
void RecupGrille(int **grille)
{
	FILE *fichier = NULL;
	int i, j;
	char c;
	fichier = fopen("grille.txt","rb");
	if(fichier == NULL)
	{
		printf("ERROR: ouverture du fichier impossible, merci de mettre le fichier grille.txt dans le dossier du programme.\n");
		exit(EXIT_FAILURE);
	}
	for(j = 0; j < DIM; j++)
	{
		for(i = 0; i < DIM; i++)
		{
			c = fgetc(fichier);
			grille[j][i] = c - '0';/*converti les char en int '0' -> 0*/
		}
		c = fgetc(fichier);/* vire le \n */
	}

	fclose(fichier);
}

/*affiche la grille de sudoku*/
void AffichageGrille(int **grille)
{
	int i, j;

	for(j = 0; j < DIM; j++)
	{
		printf("\n");
		for(i = 0; i < DIM; i++)
		{
			printf("%d", grille[j][i]);
			if(i % 3 == 2)
				printf(" ");
		}
		if(j % 3 == 2)
			printf("\n");
	}

}

/*recherche une valeur dans une ligne*/
int RechercheLigne(int **grille, int y, int val)
{
	int i;
	
	for(i = 0; i < DIM; i++)
		if(grille[y][i] == val)
			return i;
	return -1;
}

/*recherche une valeur dans une colonne*/
int RechercheColonne(int **grille, int x, int val)
{
	int i;
	
	for(i = 0; i < DIM; i++)
		if(grille[i][x] == val)
			return i;

	return -1;
}

/*recherche une valeur dans un carré*/
int RechercheCarre(int **grille, int x, int y, int val)
{
	int i, j, size, startj, starti;

	size = DIM / 3;
	startj = ((y / size) * size);
	starti = ((x / size) * size);

	for(j = startj; j < (startj + size); j++)
		for(i = starti; i < (starti + size); i++)
			if(grille[j][i] == val)
				return i;

	return -1;
}

/*marque (dans un tableau) toutes les possibilités prise par une case*/
void PossibiliteCase(int **grille, int x, int y, int *resultat)
{
	int  i;
	/*test les 9 valeurs en ligne/colonne/carré*/
	for(i = 0; i < DIM; i++)
	{
		if((RechercheLigne(grille, y, i + 1) == -1) && (RechercheCarre(grille, x, y, i + 1) == -1) && (RechercheColonne(grille, x, i + 1) == -1))
			resultat[i] = 1;
		else
			resultat[i] = 0;
	}
}

/*donne le nombre de possibilité dans une case*/
int NombrePossibilite(int *tab)
{
	int i, somme = 0;
	for(i = 0; i < DIM; i++)
		somme += tab[i];

	return somme;
}

/*compte le nombre de cases vides*/
int NoombreCaseVide(int **grille)
{
	int i, j, somme = 0;
	for(j = 0; j < DIM; j++)/*parcourt les lignes*/
		for(i = 0; i < DIM; i++)/*parcourt les colonnes*/
			if(grille[j][i] == 0)/*si la case est vide on incrémente*/
				somme++;

	return somme;
}

/*remplie toutes les possibilitées de la grille*/
void PossibiliteGrille(int **grille, int **possibilies_cases)
{
	int i, j;
	
	for(j = 0; j < DIM; j++)
		for(i = 0; i < DIM; i++)
			if(grille[j][i] == 0)
				PossibiliteCase(grille, i, j, possibilies_cases[i + (j * DIM)]);
}

/*si il y a une seule possibilités dans une case, on la remplie*/
void RemplirCaseUniqueChoix(int **grille, int **possibilies_cases)
{
	int i, j;
	for(j = 0; j < DIM; j++)
		for(i = 0; i < DIM; i++)
			if((NombrePossibilite(possibilies_cases[i + (j * DIM)])) == 1)
				grille[j][i] = ChoixUnique(possibilies_cases[i + (j * DIM)]);
}

/*renvoie la valeur du choix unique*/
int ChoixUnique(int *tab)
{
	int i;
	for(i = 0; i < DIM; i++)
		if(tab[i] == 1)
			return i + 1;
	return -1;
}

/*exclusion par lignes*/
void ExclusionLigne(int **grille, int **possibilies_cases)
{
	int i, j, k, somme;

	for(j = 0; j < DIM; j++)/*par ligne*/
	{
		for(i = 0; i < DIM; i++)/*chaque valeur*/
		{
			somme = 0;
			for(k = 0; k < DIM; k++)/*chaque case de la ligne*/
				somme += possibilies_cases[(j * DIM) + k][i];

			if(somme == 1)
			for(k = 0; k < DIM; k++)
				if(possibilies_cases[(j * DIM) + k][i] == 1)	
					grille[j][k] = i + 1;
		}
	}
}

/*exclusion par colonnes*/
void ExclusionColonnes(int **grille, int **possibilies_cases)
{
	int i, j, k, somme;

	for(j = 0; j < DIM; j++)/*par Colonne*/
		for(i = 0; i < DIM; i++)/*chaque valeur*/
		{
			somme = 0;
			for(k = 0; k < DIM; k++)/*chaque case de la ligne*/
				somme += possibilies_cases[(k * DIM) + j][i];

			if(somme == 1)
			for(k = 0; k < DIM; k++)
				if(possibilies_cases[(k * DIM) + j][i] == 1)	
					grille[k][j] = i + 1;
		}
}

/*exclusion par carrés*/
void ExclusionCarre(int **grille, int **possibilies_cases)
{
	int i, j, l, m, somme[DIM], tmp;

	for(m = 0; m < DIM; m++)
	{
		for(i = 0; i < DIM; i++)
			somme[i] = 0;
		tmp = (m % 3) * 3 + ((m / 3) * (3 * DIM));
		for(j = 0; j < (3 * DIM); j += DIM)/*saut 9 pour changer de ligne*/
			for(l = 0; l < 3; l++)/*se promene de 3 case pour parcourir le carré*/
				for(i = 0; i < DIM; i++)/*chaque valeur*/
						somme[i] += possibilies_cases[j + l + tmp][i];	

		for(i = 0; i < DIM; i++)
			if(somme[i] == 1)
				for(j = 0; j < (3 * DIM); j += DIM)/*saut 9 pour changer de ligne*/
					for(l = 0; l < 3; l++)/*se promene de 3 case pour parcourir le carré*/
						if(possibilies_cases[j + l + tmp][i] == 1)	
							grille[(j + l + tmp) / DIM][(j + l + tmp) % DIM] = i + 1;
	}
}

/*rempli la grille tant que possible*/
int Solution(int **grille, int **possibilies_cases)
{
	int nbcase = 0, nbcaseold = 1;
	while((nbcase != nbcaseold) && (NoombreCaseVide(grille) > 0))/*sarrete des que l'on ne remplie plus rien ou que la grille est complete*/
	{
		nbcaseold = NoombreCaseVide(grille);
		PossibiliteGrille(grille, possibilies_cases);/*calcul les possibilités de chaques case*/
		ExclusionLigne(grille, possibilies_cases);/*méthode d'exclusion par ligne*/
		ExclusionColonnes(grille, possibilies_cases);/*méthode d'exclusion par colonnes*/
		ExclusionCarre(grille, possibilies_cases);/*méthode d'exclusion par carré*/
		RemplirCaseUniqueChoix(grille, possibilies_cases);/*méthode d'inclusion*/
		nbcase = NoombreCaseVide(grille);
	}
	if(NoombreCaseVide(grille) > 0)/*return 0 si la grille est complete 0 sinon*/
		return -1;
	else
		return 0;
}

/*supposition*/
void Supposition(int **grille, int **possibilies_cases)
{
	int i, val[DIM], idxmin, **copy, j;
	nb++;

	Solution(grille, possibilies_cases);
	if(GrilleCoherante(grille) != 0)/*si la grille n'est pas cohérente on quitte*/
		return;

	if(NoombreCaseVide(grille) == 0)/*si la grille est complete, on copie celle-ci et on quitte*/
	{
		CopieGrille(grille, g);
		return;
	}
	copy = (int**)malloc(DIM * sizeof(int*));/*alloue une grille de copie*/
	for(i = 0; i < DIM; i++)
		copy[i] = (int*)malloc(DIM * sizeof(int));

	for(i = 0; i < DIM; i++)/*initialise le tableau de valeurs à 0*/
		val[i] = 0;
	for(i = 0; i < (DIM * DIM); i++)/*cherche une case vide pour faire une supposition*/
		if(grille[i / DIM][i % DIM] == 0)
			break;

	idxmin = i;
	for(i = 0; i < DIM; i++)/*copie toutes les possibilités de la case dans le tableau de val*/
		val[i] = possibilies_cases[idxmin][i];

	for(i = 0; i < DIM; i++)
		if(val[i] == 1)
		{
			CopieGrille(grille, copy);/*copie la grille dans la copie*/
			copy[idxmin / DIM][idxmin % DIM] = i + 1;/*fait la suposition(la premiere valeurs/la suivante possible dans la case)*/
			/*printf("valeur essayé %d case %d %d\n", i+1,idxmin / DIM,idxmin % DIM);*/
			Supposition(copy, possibilies_cases);/*appel récursif de la fonction*/
		}
	for(j = 0; j < DIM; j++)/*libere la copie*/
			free(copy[j]);
	free(copy);
}

/*copie la grille 1 dans la grille 2*/
void CopieGrille(int **grille1, int **grille2)
{
	int i, j;
	for(j = 0; j < DIM; j++)/*parcourt les colonnes*/
		for(i = 0; i < DIM; i++)/*parcourt les lignes*/
			grille2[j][i] = grille1[j][i];/*copie grille1 dans grille2*/
}

/*test si la grille reste coérrante*/
int GrilleCoherante(int **grille)
{
	int i, j, k;
	
	for(j = 0; j < DIM; j++)/*chaque ligne*/
		for(i = 0; i < DIM; i++)/*parcourt les lignes*/
			if(grille[j][i] > 0)/*si la case est remplie*/
				for(k = i + 1; k < DIM; k++)/*cherche sur le reste de la ligne la meme valeur*/
					if(grille[j][i] == grille[j][k])/*si y'a la meme valeur sur la ligne(grille non cohérente sur la ligne return 1*/
						return 1;
			

	for(j = 0; j < DIM; j++)
		for(i = 0; i < DIM; i++)
			if(grille[i][j] > 0)
				for(k = i + 1; k < DIM; k++)
					if(grille[i][j] == grille[k][j])
						return -1;/*return -1 si la grille n'est pas cohérente sur une colonne*/
	return 0;
}
