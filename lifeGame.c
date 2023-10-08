#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef struct Cellule_type{
    int statut;
    int voisin;
    int x;
    int y;
}Cellule;

void afficherPlateau(Cellule** tab, int t, int tour);

void partie(Cellule** tab, int t);




int main(int argc, char* argv[]){

    FILE* fichier;
    Cellule** tableau;
    char* nomFichier = argv[1];
    char ch_temp[50];

    int plateauTaille = 10;
    int nTour = atoi(argv[2]);
    int xInit, yInit;


    //ouverture du fichier txt avec vérification
    if(!(fichier = fopen(nomFichier, "r"))){
        printf("Erreur ouverture fichier\n");
        return 1;
    }

    fscanf(fichier, "%s", ch_temp);
    fscanf(fichier, "%d", &plateauTaille);

    fscanf(fichier, "%s", ch_temp);
    if(strcmp(ch_temp, "DEBUT_TABLEAU")!=0){
        printf("Erreur de fichier\n");
        return 1;
    }
    

    //initialisation du plateau de jeu
    tableau = (Cellule**)malloc(plateauTaille*sizeof(Cellule*));

    for(int i=0; i<plateauTaille; i++){
        tableau[i] = (Cellule*)malloc(plateauTaille*sizeof(Cellule));
        for(int j=0; j<plateauTaille; j++){
            tableau[i][j].statut = 0;
            tableau[i][j].voisin = 0;
            tableau[i][j].x = i;
            tableau[i][j].y = j;
        }
    }


    //initialisation des cellules vivantes à partir d'un fichier txt
    do{
        xInit = 0;
        yInit = 0;
        fscanf(fichier, "%s", ch_temp);
        if(strcmp(ch_temp, "FIN_TABLEAU")!=0){

            if(atoi(ch_temp) < plateauTaille){
                xInit = atoi(ch_temp);
            }

            fscanf(fichier, "%s", ch_temp);
            if(atoi(ch_temp) < plateauTaille){
                yInit = atoi(ch_temp);
            }
            

            tableau[xInit][yInit].statut = 1;
        }

    }while(strcmp(ch_temp, "FIN_TABLEAU")!=0);



    //lancement des tours avec affichage du plateau
    for(int h=0; h<nTour; h++){
        afficherPlateau(tableau, plateauTaille, h);
        partie(tableau, plateauTaille);
        sleep(1);
    }

    
    //destruction du tableau
    for(int i=0; i<plateauTaille; i++){
        free(tableau[i]);
        tableau[i] = NULL;
    }

    free(tableau);
    tableau = NULL;

    fclose(fichier);

    printf("Fin du prog\n");

    return 0;
}//fin main




void afficherPlateau(Cellule** tab, int t, int tour){
    system("clear");//vide la console

    printf("Tour : %d\n", tour);

    for(int k=0; k<t; k++){
        for(int l=0; l<t; l++){
            if(tab[k][l].statut == 0){
                printf("_");
            }
            else{
                printf("#");
            }
        }
        printf("\n");
    }
}//fin afficherPlateau



void partie(Cellule** tab, int t){

    for(int h=0; h<2; h++){//h=0 compter les voisins, h=1 définir le statut de chaque cellule

        /*Avec une boucle for en plus (h) on évite de refaire 2 boucles for supplémentaires
        pour repasser dans le tableau*/

        for(int k=0; k<t; k++){
            for(int l=0; l<t; l++){

                if(h==0){
                    tab[k][l].voisin = 0;
                    //droite
                    if(l!=t-1){ //évite de sortir de la bordure de droite
                        if(k!=0 && tab[k-1][l+1].statut == 1){
                            tab[k][l].voisin ++;
                        }

                        if(tab[k][l+1].statut == 1){
                            tab[k][l].voisin ++;
                        }

                        if(k!=t-1 && tab[k+1][l+1].statut == 1){
                            tab[k][l].voisin ++;
                        }
                    }


                    //milieu
                    if(k!=0 && tab[k-1][l].statut == 1){
                        tab[k][l].voisin ++;
                    }

                    if(k!=t-1 && tab[k+1][l].statut == 1){
                        tab[k][l].voisin ++;
                    }


                    //gauche
                    if(l!=0){//évite de sortir de la bordure de gauche
                        if(k!=0 && tab[k-1][l-1].statut == 1){
                            tab[k][l].voisin ++;
                        }

                        if(tab[k][l-1].statut == 1){
                            tab[k][l].voisin ++;
                        }

                        if(k!=t-1 && tab[k+1][l-1].statut == 1){
                            tab[k][l].voisin ++;
                        }
                    }

                }
                else{
                    //règles du jeu
                    if(tab[k][l].voisin == 3){
                        tab[k][l].statut = 1;
                    }

                    if(tab[k][l].voisin < 2 || tab[k][l].voisin > 3){
                        tab[k][l].statut = 0;
                    }
                }

            }//for j
        }//for i

    }//for h

}//fin partie