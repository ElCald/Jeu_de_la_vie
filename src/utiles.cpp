#include "librairies.h"

#include "utiles.h"


/// Window

/**
 * Constructeur d'une fenêtre SDL2.
 * Initialisation vidéo uniquement.
 * Création de la fenêtre et du renderer.
 * 
 * @param _win_width Largeur de la fenêtre
 * @param _win_height Hauteur de la fenêtre
 */
Window::Window(const size_t _win_width, const size_t _win_height) : nb_frames(0), settings_open(false), win_width(_win_width), win_height(_win_height), 
                                                                    animationEnable(false), visibleTextZones(false), delay_animation(FRAME_DELAY_ANIMATION), 
                                                                    default_color(DARK), displayGrid(false), dragging(false), lastMouseX(0), lastMouseY(0),
                                                                    mouseLeftDown(false), mouseRightDown(false), page_settings(0) {

    #ifdef _DEBUG_
    SDL_version v;
    SDL_VERSION(&v);
    printf("SDL version : %d.%d.%d\n", v.major, v.minor, v.patch);
    #endif

    if( SDL_Init(SDL_INIT_VIDEO) != 0 )
        ExitWithError("Initialisation SDL");

    // if( SDL_CreateWindowAndRenderer(win_width, win_height, SDL_WINDOW_SHOWN, &fenetre, &renderer) != 0 )
    //     ExitWithError("Creation fenetre et rendu echouee");

    fenetre = SDL_CreateWindow(
        "Life Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        win_width,
        win_height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE /*| SDL_WINDOW_FULLSCREEN_DESKTOP*/
    );

    renderer = SDL_CreateRenderer(
        fenetre,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    program_launched = SDL_TRUE;

    // Police d'écriture
    if (TTF_Init() != 0) 
        ExitWithError("Erreur TTF_Init");

    font = TTF_OpenFont(POLICE_IMPACT, H4);

    if (!font) 
        ExitWithError("Erreur TTF_OpenFont");
    

    center_width = win_width/2;
    center_height = win_height/2;

    cam = new Camera(0, 0, 1.0f);

    text_color = {255,255,255,255};

    #ifdef _DEBUG_
    cout << "Fenêtre crée" << endl;
    #endif
}


Window::~Window(){

    for(Texture* t : texture_scene){
        SDL_DestroyTexture(t->texture);
    }

    #ifdef _DEBUG_
    cout << "Destruction des textures" << endl;
    #endif

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenetre);

    #ifdef _DEBUG_
    cout << "Destruction de la window" << endl;
    #endif
}


void Window::ExitWithError(string msg){
    SDL_Log("Erreur : %s > %s\n", msg.c_str(), SDL_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenetre);
    SDL_Quit();
    exit(EXIT_FAILURE);
}



/**
 * Rendu des images de la fenêtre
 */
void Window::render(){
    SDL_RenderClear(renderer);

    if(!settings_open){

        // Rendu du fond
        if( (SDL_RenderCopy(renderer, texture_background->texture, NULL, texture_background->position)) != 0 )
            ExitWithError("Erreur affichage image dans le rendu");


        #ifdef _DEBUG_
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 128);
            SDL_RenderDrawRect(renderer, t->position);
        #endif

        if(displayGrid && cam->zoom > 0.9f)
            drawGrid();


        // Rendu des éléments de scène
        for(Texture* t : texture_scene){
            if(t == nullptr)
                continue;

            if( (SDL_RenderCopy(renderer, t->texture, NULL, t->position)) != 0 )
                ExitWithError("Erreur affichage image dans le rendu");

            #ifdef _DEBUG_
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);
            SDL_RenderDrawRect(renderer, t->position);
            #endif

        }


        // rendu de mes cellules
        apply_color();
        
        for(const auto& c : chunks){

            SDL_Rect cell;
            cell.x = c.first*SIZE_CELL;
            cell.y = c.second*SIZE_CELL;
            cell.w = SIZE_CELL;
            cell.h = SIZE_CELL;

            SDL_Rect* new_cell = worldToScreen(&cell);


            SDL_RenderFillRect(renderer, new_cell);
        }

        
        #ifdef _DEBUG_
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        #endif
        
        renderTexts(); // affichage du texte  

    }
    else{

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        if(!textures_settings.empty()){
            if( (SDL_RenderCopy(renderer, texture_background->texture, NULL, texture_background->position)) != 0 )
                ExitWithError("Erreur affichage image dans le rendu");

            if( (SDL_RenderCopy(renderer, textures_settings.at(page_settings)->texture, NULL, textures_settings.at(page_settings)->position)) != 0 )
                ExitWithError("Erreur affichage image dans le rendu");
        }
            
    }


    SDL_RenderPresent(renderer);
}


/**
 * Animation des personnages
 */
Uint32 Window::animate(Uint32 last_update){

    Uint32 now = SDL_GetTicks();
    Uint32 new_last_update = last_update;
    
    // now - last_update >= FRAME_DELAY_ANIMATION

    if(animationEnable && (now - last_update) >= (Uint32)delay_animation){
        update_grid(chunks);
        nb_frames++;
        new_last_update = now;
    }


    return new_last_update;
}


/**
 * Adaptation des objets de la scène à la caméra
 */
SDL_Rect* Window::worldToScreen(SDL_Rect* obj) {
    SDL_Rect* r = new SDL_Rect;

    r->w = static_cast<int>(obj->w * cam->zoom);
    r->h = static_cast<int>(obj->h * cam->zoom);

    // Translation : on centre la caméra
    r->x = static_cast<int>((obj->x - cam->x) * cam->zoom + win_width / 2);
    r->y = static_cast<int>((obj->y - cam->y) * cam->zoom + win_height / 2);

    return r;
}



void Window::handleCameraEvents() {
    switch (event.type) {
        case SDL_MOUSEWHEEL: {
            // zoom in / out
            if (event.wheel.y > 0) {      // molette vers le haut
                cam->zoom *= 1.1f;
            } else if (event.wheel.y < 0) { // molette vers le bas
                cam->zoom /= 1.1f;
            }
            if (cam->zoom < 0.1f) cam->zoom = 0.1f; // éviter un zoom trop petit
            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            if (event.button.button == SDL_BUTTON_RIGHT) {
                dragging = true;
                lastMouseX = event.button.x;
                lastMouseY = event.button.y;
            }
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            if (event.button.button == SDL_BUTTON_RIGHT) {
                dragging = false;
            }
            break;
        }

        case SDL_MOUSEMOTION: {
            if (dragging) {
                int dx = event.motion.x - lastMouseX;
                int dy = event.motion.y - lastMouseY;

                // déplace la caméra (dx/dy doivent être divisés par le zoom
                // sinon ça bouge trop vite quand on est zoomé)
                cam->x -= dx / cam->zoom;
                cam->y -= dy / cam->zoom;

                lastMouseX = event.motion.x;
                lastMouseY = event.motion.y;
            }
            break;
        }
    }
}


/**
 * Permet d'obtenir les coordonnées de la souris selon la caméra et la taille des cases du quadriage
 * 
 * @return Position en X et Y
 */
SDL_Rect Window::getMouseClick(){
    
    int mx = 0;
    int my = 0;
    int cellX;
    int cellY;
    double worldX;
    double worldY;

    SDL_Rect pos;
    pos.x = 0;
    pos.y = 0;
    pos.w = 0;
    pos.h = 0;


    
    mx = event.button.x;
    my = event.button.y;

    worldX = (mx - center_width) / cam->zoom + cam->x;
    worldY = (my - center_height) / cam->zoom + cam->y;

    cellX = static_cast<int>(worldX / SIZE_CELL);
    cellY = static_cast<int>(worldY / SIZE_CELL);


    // Recalibrage dans les zones négatives
    if(worldX < 0) 
        cellX-=1;
    
    if(worldY < 0) 
        cellY-=1;

    pos.x = cellX;
    pos.y = cellY;
    


    return pos;
}


void Window::change_theme(int t){
    switch(t){
        case DEFAULT:
            setTextureBackground(textures_background.at(0));
            default_color = DEFAULT;
            break;

        case LIGHT:
            setTextureBackground(textures_background.at(1));
            default_color = LIGHT;
            break;

        case DARK:
            setTextureBackground(textures_background.at(2));
            default_color = DARK;
            break;

        default:
            setTextureBackground(textures_background.at(0));
            default_color = DEFAULT;
            break;
    }
}

void Window::apply_color(){
    switch(default_color){
        case DEFAULT:
            SDL_SetRenderDrawColor(renderer, 255, 58, 48, 255);  // rouge
            text_color = {255, 58, 48, 255};
            break;

        case LIGHT:
            SDL_SetRenderDrawColor(renderer, 48, 48, 48, 255);  // Noir
            text_color = {48, 48, 48, 255};
            break;

        case DARK:
            SDL_SetRenderDrawColor(renderer, 254, 254, 226, 255);  // Blanc
            text_color = {254, 254, 226, 255};
            break;
    }
}


void Window::showGrid(){
    displayGrid = true;
}

void Window::hideGrid(){
    displayGrid = false;
}

void Window::toggleGrid(){
    displayGrid = !displayGrid;
}


int Window::get_nb_theme(){
    return textures_background.size();
}


void Window::addTextureBackground(Texture* texture){
    textures_background.push_back(texture);
}

/**
 * Ajout d'une texture à la scène
 */
void Window::addTextureScene(Texture* texture){
    texture_scene.push_back(texture);
}


/**
 * Ajout d'une texture à la scène
 */
void Window::addTextureSettings(Texture* texture){
    textures_settings.push_back(texture);
}





void Window::cleanTextureScene(){
    texture_scene.clear();
}




/**
 * Ajout d'une texture au background mais retire la précédente
 */
void Window::setTextureBackground(Texture* texture){
    texture_background = texture;
}


void Window::addText(const string& text, int x, int y, SDL_Color color) {
    texts.push_back(new TextElement(renderer, font, text, x, y, color));
}


/**
 * 
 * @param index Index dans la liste de textw
 */
void Window::updateText(size_t index, const string& newText) {
    if (index < texts.size())
        texts[index]->setText(newText);
}



void Window::moveText(int num, const int x, const int y){
     if (num < texts.size())
        texts[num]->setX(x);
        texts[num]->setY(y);
}


void Window::renderTexts() {
    
    for(size_t i=0; i<texts.size(); i++){
        texts[i]->setColor(text_color);

        if(i==0 && visibleTextZones)
            texts[i]->render(renderer);
        else if(i!=0)
            texts[i]->render(renderer);
    }
        
}


void Window::swap_page_settings(){
    page_settings = (page_settings+1 >= textures_settings.size()) ? 0 : page_settings+1;
}



/**
 * Crée le nombre de zones de textes souhaité 
 * @param num Nombre de zones de textes
 * @param x
 * @param y
 */
void Window::addTextZones(int num, const int x, const int y){

    for(int i=0; i<num; i++){
        addText("Test", x, y+TTF_FontHeight(font)*i, {255,100,100,255});
    }

}



void Window::showTextZones(){
    visibleTextZones = true;
}


void Window::hideTextZones(){
    visibleTextZones = false;
}


void Window::enableAnimations(){
    animationEnable = true;
}

void Window::disableAnimations(){
    animationEnable = false;
}


void Window::toggleAnimations(){
    animationEnable = !animationEnable;
}


void Window::speedupAnimations(){
    if(delay_animation > 0 )
        delay_animation -= 25;
}

void Window::lowdownAnimations(){
    delay_animation += 25;
}




void Window::addObjet(Objet* obj){
    objets_scene.push_back(obj);
}


int Window::get_width(){
    return win_width;
}

int Window::get_height(){
    return win_height;
}

int Window::get_center_width(){
    return center_width;
}

int Window::get_center_height(){
    return center_height;
}

/**
 * Met à jours les données concernant les dimensions de la fenêtre
 */
void Window::refresh_window(){
    SDL_GetWindowSize(fenetre, &win_width, &win_height);
    center_width = win_width/2;
    center_height = win_height/2;

    for(auto& t : textures_background){
        t->setSize(win_width, win_height);
    }

    for(auto& t : textures_settings){
        t->setPlace(win_width/2-WIN_WIDTH/2, win_height/2-WIN_HEIGHT/2);
    }
}


int Window::get_nb_frames(){
    return nb_frames;
}


void Window::reset_frame(){
    nb_frames = 0;
}

/// Camera
Camera::Camera(double _x, double _y, double _zoom) : x(_x), y(_y), zoom(_zoom) {}

Camera::~Camera(){}









/// TextElement

TextElement::TextElement(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) : texture(nullptr), font(font), currentText(text), color(color) {
    rect.x = x;
    rect.y = y;
    updateTexture(renderer);
}

TextElement::~TextElement() {
    if (texture)
        SDL_DestroyTexture(texture);
}

void TextElement::setText(const string& newText) {
    if (newText != currentText) {
        currentText = newText;
        if (texture)
            SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

void TextElement::updateTexture(SDL_Renderer* renderer) {

    SDL_Surface* surface = TTF_RenderText_Blended(font, currentText.c_str(), color);
    if (!surface) return;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect.w = surface->w;
    rect.h = surface->h;
    SDL_FreeSurface(surface);
}

void TextElement::render(SDL_Renderer* renderer) {

    updateTexture(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}


void TextElement::setColor(SDL_Color c){
    color = c;
}

void TextElement::addX(int _x){
    rect.x += _x;
    
}

void TextElement::addY(int _y){
    rect.y += _y;
}


void TextElement::setX(int _x){
    rect.x = _x;
}


void TextElement::setY(int _y){
    rect.y = _y;
}



/// Texture
/**
 * @param img Image bmp
 * @param _width Largeur de l'image
 * @param _height Hauteur de l'image
 * @param _x Position x
 * @param _y Position y
 */
Texture::Texture(Window& window, string img, const size_t _width, const size_t _height, const int _x, const int _y) {

    #ifdef _DEBUG_
    cout << "Chargement texture : " << img << endl;
    #endif

    SDL_Surface* image;

    position = new SDL_Rect;


    // Chargelent de l'image
    if( (image = SDL_LoadBMP(img.c_str())) == NULL )
        window.ExitWithError("Erreur du chargement de l'image : " + img);


    // Chargement de l'image dans une texture pour pouvoir l'afficher
    if( (texture = SDL_CreateTextureFromSurface(window.renderer, image)) == NULL ){
        SDL_FreeSurface(image);
        window.ExitWithError("Erreur du texturing : " + img);
    }


    SDL_FreeSurface(image); // Libération de la mémoire utilisé par l'image car on ne l'utilise plus


    // Emplacement de l'image dans notre fenêtre
    if( (SDL_QueryTexture(texture, NULL, NULL, &position->w, &position->h)) != 0 )
        window.ExitWithError("Erreur query texture : " + img);


    position->w = _width;
    position->h = _height;
    position->x = _x;
    position->y = _y;



    // Chargement de la texture contenant l'image dans le rendu pour l'affichage
    if( (SDL_RenderCopy(window.renderer , texture, NULL, position)) != 0 )
        window.ExitWithError("Erreur affichage image : " + img);

}



/**
 * @param img Image bmp
 * @param _width Largeur de l'image
 * @param _height Hauteur de l'image
 * @param _x Position x
 * @param _y Position y
 */
Texture::Texture(Window& window, string img, SDL_Rect* pos) {

    #ifdef _DEBUG_
    cout << "Chargement texture : " << img << endl;
    #endif

    SDL_Surface* image;

    position = new SDL_Rect;

    // Chargelent de l'image
    if( (image = SDL_LoadBMP(img.c_str())) == NULL )
        window.ExitWithError("Erreur du chargement de l'image : " + img);

    // Chargement de l'image dans une texture pour pouvoir l'afficher
    if( (texture = SDL_CreateTextureFromSurface(window.renderer, image)) == NULL ){
        SDL_FreeSurface(image);
        window.ExitWithError("Erreur du texturing : " + img);
    }

    SDL_FreeSurface(image); // Libération de la mémoire utilisé par l'image car on ne l'utilise plus

    // Emplacement de l'image dans notre fenêtre
    if( (SDL_QueryTexture(texture, NULL, NULL, &position->w, &position->h)) != 0 )
        window.ExitWithError("Erreur query texture : " + img);


    position = pos;
    


    // Chargement de la texture contenant l'image dans le rendu pour l'affichage
    if( (SDL_RenderCopy(window.renderer , texture, NULL, position)) != 0 )
        window.ExitWithError("Erreur affichage image : " + img);

}


Texture::~Texture(){}


void Texture::setPlace(int _x, int _y){
    position->x = _x;
    position->y = _y;
}

void Texture::setSize(int _width, int _height){
    position->w = _width;
    position->h = _height;
}



void Texture::setX(int _x){
    position->x = _x;
}

void Texture::setY(int _y){
    position->y = _y;
}

void Texture::addX(int _x){
    position->x += _x;
}

void Texture::addY(int _y){
    position->y += _y;
}






/// Objet

Objet::Objet(Window& window, string texture_path, const size_t width, const size_t height, const int _x, const int _y, const type_objet _type){
    id = compteur_id;
    compteur_id++;

    #ifdef _DEBUG_
    cout << "Création de l'objet id: " << id << endl;
    #endif

    type = _type;

    position = new SDL_Rect();

    position->w = width;
    position->h = height;
    position->x = _x;
    position->y = _y;

    texture = new Texture(window, texture_path, position);

    window.addTextureScene(texture);
}


Objet::~Objet(){}



void update_grid(set<pair<int,int>>& chunks){
    int nb_voisins = 0;
    int i, j;

    set<pair<int,int>> new_chunks;

    for(auto& c : chunks){ // On parcours tous les chunks chargés

        i = c.first;
        j = c.second;


        for(int x=-1; x<=1; x++){
            for(int y=-1; y<=1; y++){

                nb_voisins = 0;

                // cellules traitées par le chunk
                for(int a=-1; a<=1; a++){
                    for(int b=-1; b<=1; b++){
                        if(a==0 && b==0) continue;

                        if (chunks.count({i+x+a,j+y+b})) { // on vérifie s'il existe une voisine vivante autour
                            nb_voisins++; 
                            // cout << "voisin pour" << "{" << i+x << "," << j+y << "}" << "en : {" << i+x+a << "," << j+y+b << "}" << endl;
                        }

                        
                    }
                }
                
                //   Application des règles
                if(nb_voisins == 3){ // une cellule morte avec 3 voisines => vie
                    // cout << "bah alors faut mettre un cube en " << "{" << i+x << "," << j+y << "}" << endl;
                    new_chunks.insert({i+x,j+y});
                }

                if( chunks.count({i+x,j+y}) && (nb_voisins == 2 || nb_voisins == 3) ){ // une cellule vivante avec 2 ou 3 voisines => vie
                    new_chunks.insert({i+x,j+y});
                }

                // Le reste meurt

            }
        }

    } // fin chunks

    chunks = new_chunks;
}



void Window::drawGrid() {
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);

    // Monde visible
    float worldLeft   = cam->x - (win_width / 2.0f) / cam->zoom;
    float worldRight  = cam->x + (win_width / 2.0f) / cam->zoom;
    float worldTop    = cam->y - (win_height / 2.0f) / cam->zoom;
    float worldBottom = cam->y + (win_height / 2.0f) / cam->zoom;

    // Alignement sur la grille
    int startX = (int)floor(worldLeft / SIZE_CELL) * SIZE_CELL;
    int endX   = (int)ceil(worldRight / SIZE_CELL) * SIZE_CELL;
    int startY = (int)floor(worldTop / SIZE_CELL) * SIZE_CELL;
    int endY   = (int)ceil(worldBottom / SIZE_CELL) * SIZE_CELL;

    // Lignes verticales
    for (int x = startX; x <= endX; x += SIZE_CELL) {
        int screenX = (x - cam->x) * cam->zoom + win_width / 2;
        SDL_RenderDrawLine(renderer, screenX, 0, screenX, win_height);
    }

    // Lignes horizontales
    for (int y = startY; y <= endY; y += SIZE_CELL) {
        int screenY = (y - cam->y) * cam->zoom + win_height / 2;
        SDL_RenderDrawLine(renderer, 0, screenY, win_width, screenY);
    }
}