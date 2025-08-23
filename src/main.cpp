#include "librairies.h"
#include "utiles.h"


using namespace std;

int Objet::compteur_id = 0;

int main(int argc, char* argv[]){

     

    Uint32 frameStart;
    int frameTime;

    int mx = 0;
    int my = 0;
    int cellX;
    int cellY;
    double worldX;
    double worldY;


    bool dragging = false;
    int lastMouseX = 0, lastMouseY = 0;



    Uint32 last_update = SDL_GetTicks();

    // Création de la fenêtre
    Window window(WIN_WIDTH, WIN_HEIGHT);

    // Écran de chargement
    Texture background_load(window, string(TEXTURE_BACKGROUND_LOAD), WIN_WIDTH, WIN_HEIGHT, 0, 0);
    window.setTextureBackground(&background_load);
    window.render();


    Texture background(window, string(TEXTURE_BACKGROUND), WIN_WIDTH, WIN_HEIGHT, 0, 0);
    window.setTextureBackground(&background);


    Texture background_settings(window, string(TEXTURE_BACKGROUND_SETTINGS), WIN_WIDTH, WIN_HEIGHT, 0, 0); // chargement de l'écran des controles


    window.chunks.insert({0,0});
    window.chunks.insert({1,0});
    window.chunks.insert({0,1});


    window.addTextZones(1, WIN_WIDTH-80+10, (H4/2)+10); // nombre de frame écoulées
    window.updateText(0, "" + to_string(window.nb_frames));
    window.showTextZones();


    window.render();





    while(window.program_launched){

        frameStart = SDL_GetTicks(); // Temps au début de la frame

        while(SDL_PollEvent(&window.event)){

            window.handleCameraEvents(dragging, lastMouseX, lastMouseY);

            switch(window.event.type){
           
                case SDL_KEYDOWN: // gestion du clavier appui de la touche

                    switch (window.event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            window.program_launched = SDL_FALSE;
                            break;

                        case SDLK_TAB: 
                            window.settings_open = true;
                            window.setTextureBackground(&background_settings);
                            break;

                        case SDLK_SPACE:
                            window.toggleAnimations();
                            break;

                        case SDLK_c:
                            window.chunks.clear();
                            window.nb_frames = 0;
                            break;

                        case SDLK_UP: 
                            window.speedupAnimations();
                            break;
                        
                        case SDLK_DOWN: 
                            window.lowdownAnimations();
                            break;
                

                        default:
                            continue;
                    }
                    
                    break;

                case SDL_KEYUP:

                    switch (window.event.key.keysym.sym) {
                        case SDLK_TAB: 
                            window.settings_open = false;
                            window.setTextureBackground(&background);
                            break;
                    }
                    break;
                    


                case SDL_MOUSEBUTTONDOWN:
                    if (window.event.button.button == SDL_BUTTON_LEFT) {
                        mx = window.event.button.x;
                        my = window.event.button.y;

                        worldX = (mx - WIN_WIDTH / 2) / window.cam->zoom + window.cam->x;
                        worldY = (my - WIN_HEIGHT / 2) / window.cam->zoom + window.cam->y;

                        cellX = static_cast<int>(worldX / SIZE_CELL);
                        cellY = static_cast<int>(worldY / SIZE_CELL);

             
                        // Recalibrage dans les zones négatives
                        if(worldX < 0) 
                            cellX-=1;
                        
                        if(worldY < 0) 
                            cellY-=1;


                        if(window.chunks.count({cellX,cellY})){
                            window.chunks.erase({cellX,cellY});
                        }
                        else{
                            window.chunks.insert({cellX,cellY});
                        }
                    }

                    
                    break;

              
                case SDL_QUIT: // clique sur la croix de la fenêtre
                    window.program_launched = SDL_FALSE; break;
                
                default:
                    break;
            }

        }//fin boucle event



        last_update = window.animate(last_update);

        window.updateText(0, "" + to_string(window.nb_frames));

        window.render();



        // --- Contrôle du temps de frame ---
        frameTime = SDL_GetTicks() - frameStart;

        if (FRAME_DELAY > frameTime) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }



    SDL_Quit(); 

    #ifdef _DEBUG_
    cout << "Fin de prog" << endl;
    #endif


    cin.get();
    return EXIT_SUCCESS;
}