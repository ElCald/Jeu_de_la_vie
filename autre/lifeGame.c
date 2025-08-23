#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ncurses.h>  

#include "functions.h"

typedef struct Cellule_type{
    int statut;
    int voisin;
    int x;
    int y;
}Cellule;

void afficherPlateau(Cellule** tab, int t, int tour);

void partie(Cellule** tab, int t);

void palette();


#define X_plateau 20
#define Y_plateau 2

int main(int argc, char* argv[]){

    //vérification des arguments entrés
    if(argc != 4){
        fprintf(stderr, "Nombre d\'arguments incorrect : [nom fichier], [nombre de tours], [pause=1, dynam=0]\n");
        exit(EXIT_FAILURE);
    }

    FILE* fichier;
    Cellule** tableau;
    char* nomFichier = argv[1];
    char ch_temp[50];

    int plateauTaille = 10;
    int nTour = atoi(argv[2]);
    int xInit, yInit;
    int ch;


    errno = 0;
    int rFSCANF = 0;

    //ouverture du fichier txt avec vérification
    if(!(fichier = fopen(nomFichier, "r"))){
        fprintf(stderr,"Erreur ouverture fichier\n");
        exit(EXIT_FAILURE);
    }

    rFSCANF = fscanf(fichier, "%s", ch_temp);

    rFSCANF = fscanf(fichier, "%d", &plateauTaille);

    rFSCANF = fscanf(fichier, "%s", ch_temp);

   
    if(strcmp(ch_temp, "DEBUT_TABLEAU")!=0){
        fprintf(stderr,"Erreur debut tableau\n");
        exit(EXIT_FAILURE);
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
        rFSCANF = fscanf(fichier, "%s", ch_temp);

        if(strcmp(ch_temp, "FIN_TABLEAU")!=0){

            if(atoi(ch_temp) < plateauTaille){
                xInit = atoi(ch_temp);
            }

            rFSCANF = fscanf(fichier, "%s", ch_temp);
            if(atoi(ch_temp) < plateauTaille){
                yInit = atoi(ch_temp);
            }
            

            tableau[xInit][yInit].statut = 1;
        }

    }while(strcmp(ch_temp, "FIN_TABLEAU")!=0);



    //Gestion des exceptions des fscanf
    if(rFSCANF != 1){
        fprintf(stderr,"Erreur lecture dans le fichier\n");
        exit(EXIT_FAILURE);
    }
    else if(errno != 0){
        perror("fscanf");
    }
    else{
        fprintf(stderr,"Je ne sais pas à quoi sert cette ligne.\n");
    }


    //Partie affichage et jeu après récupération du fichier

    ncurses_init();
    ncurses_colors();
    ncurses_init_mouse();
    palette();



    //lancement des tours avec affichage du plateau
    for(int h=0; h<nTour; h++){

        afficherPlateau(tableau, plateauTaille, h);
        partie(tableau, plateauTaille);

        if(atoi(argv[3])==1){
            if((ch = getch()) == 'q'){
                exit(EXIT_SUCCESS);
            }
        }
        else{
            usleep(150000); //pause en microsecondes : 1 sec == 1000000 usec
        }
        
    }

    

    //fin du prog 


    //destruction du tableau
    for(int i=0; i<plateauTaille; i++){
        free(tableau[i]);
        tableau[i] = NULL;
    }

    free(tableau);
    tableau = NULL;

    fclose(fichier);

    ncurses_stop();

    printf("Fin du prog\n");

    return EXIT_SUCCESS;
}//fin main




void afficherPlateau(Cellule** tab, int t, int tour){

    mvprintw(0,0,"Tour : %d\n", tour);

    for(int k=0; k<t; k++){
        for(int l=0; l<t; l++){
            if(tab[k][l].statut == 0){
                //espace vide
                attron(COLOR_PAIR(0));
                mvaddch(Y_plateau+l, X_plateau+k, '_');
                attroff(COLOR_PAIR(0));
            }
            else{
                //espace plein
                attron(COLOR_PAIR(1));
                mvaddch(Y_plateau+l, X_plateau+k, ' ');
                attroff(COLOR_PAIR(1));
            }
        }
    }

    refresh();

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


void palette(){
    init_pair(0, COLOR_GREEN, COLOR_BLACK);
    init_pair(1, COLOR_RED, COLOR_RED);
}