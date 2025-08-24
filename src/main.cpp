#include "librairies.h"
#include "utiles.h"


using namespace std;

int Objet::compteur_id = 0;

int main(int argc, char* argv[]){
     

    Uint32 frameStart;
    int frameTime;

    SDL_Rect mouseClick;
    int theme_value = DARK;

    bool erase_mode = false;



    Uint32 last_update = SDL_GetTicks();

    // Création de la fenêtre
    Window window(WIN_WIDTH, WIN_HEIGHT);

    // Écran de chargement
    Texture background_load(window, string(TEXTURE_BACKGROUND_LOAD), window.get_width(), window.get_height(), 0, 0);
    window.setTextureBackground(&background_load);
    window.render();


    Texture background_default(window, string(TEXTURE_BACKGROUND_DEFAULT), window.get_width(), window.get_height(), 0, 0);
    Texture background_light(window, string(TEXTURE_BACKGROUND_LIGHT), window.get_width(), window.get_height(), 0, 0);
    Texture background_dark(window, string(TEXTURE_BACKGROUND_DARK), window.get_width(), window.get_height(), 0, 0);

    window.addTextureBackground(&background_default);
    window.addTextureBackground(&background_light);
    window.addTextureBackground(&background_dark);

    window.setTextureBackground(&background_dark);


    Texture background_settings_0(window, string(TEXTURE_BACKGROUND_SETTINGS_0), window.get_width(), window.get_height(), 0, 0);
    window.addTextureSettings(&background_settings_0);



    window.chunks.insert({0,0});
    window.chunks.insert({1,0});
    window.chunks.insert({0,1});


    window.addTextZones(1, window.get_width()-80+10, (H4/2)+10); // nombre de frame écoulées
    window.updateText(0, "" + to_string(window.get_nb_frames()));
    window.showTextZones();

    window.showGrid();


    window.render();


    while(window.program_launched){

        frameStart = SDL_GetTicks(); // Temps au début de la frame


        while(SDL_PollEvent(&window.event)){

            window.handleCameraEvents();

            switch(window.event.type){
           
                case SDL_KEYDOWN: // gestion du clavier appui de la touche

                    switch (window.event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            window.program_launched = SDL_FALSE;
                            break;

                        case SDLK_TAB: 
                            window.settings_open = true;
                            window.swap_page_settings();
                            break;

                        case SDLK_SPACE:
                            window.toggleAnimations();
                            break;

                        case SDLK_c:
                            window.chunks.clear();
                            window.reset_frame();
                            break;

                        case SDLK_e:
                            erase_mode = !erase_mode;
                            break;

                        case SDLK_g:
                            window.toggleGrid();
                            break;

                        case SDLK_UP: 
                            window.speedupAnimations();
                            break;
                        
                        case SDLK_DOWN: 
                            window.lowdownAnimations();
                            break;

                        case SDLK_t:

                            theme_value = (theme_value < window.get_nb_theme()) ? theme_value+1 : 0;
                            window.change_theme(theme_value);
                            
                            break;
                

                        default:
                            continue;
                    }
                    
                    break;     
                    
                case SDL_KEYUP:
                    switch (window.event.key.keysym.sym) {
                        case SDLK_TAB: 
                            window.settings_open = false;
                            break;
                    }   
                    break;


                case SDL_MOUSEBUTTONDOWN:

                    switch(window.event.button.button) {
                        case SDL_BUTTON_LEFT:
                            window.mouseLeftDown = true;
                            
                            mouseClick = window.getMouseClick();

                            if(erase_mode && window.chunks.count({mouseClick.x,mouseClick.y})){
                                window.chunks.erase({mouseClick.x,mouseClick.y});
                            }
                            else{
                                if(!erase_mode)
                                    window.chunks.insert({mouseClick.x,mouseClick.y});
                            }
                            
                            break;
                    }
                    break;

                case SDL_MOUSEBUTTONUP:

                    switch(window.event.button.button) {
                        case SDL_BUTTON_LEFT:
                            window.mouseLeftDown = false;
                            break;
                    }
                    break;

                case SDL_MOUSEMOTION :
                    if(window.mouseLeftDown){
                        mouseClick = window.getMouseClick();

                        if(erase_mode && window.chunks.count({mouseClick.x,mouseClick.y})){
                            window.chunks.erase({mouseClick.x,mouseClick.y});
                        }
                        else{
                            if(!erase_mode)
                                window.chunks.insert({mouseClick.x,mouseClick.y});
                        }

                    }
                    
                    break;
              
                case SDL_QUIT: // clique sur la croix de la fenêtre
                    window.program_launched = SDL_FALSE; break;
                
                default:
                    break;
            }

        }//fin boucle event

        window.refresh_window();


        window.moveText(0, window.get_width()-80+10, (H4/2)+10);

        last_update = window.animate(last_update);

        window.updateText(0, "" + to_string(window.get_nb_frames()));

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