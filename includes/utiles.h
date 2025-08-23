#ifndef UTILES_H
#define UTILES_H

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Répertoires
#define REP_MISC "misc/"
#define REP_IMAGES REP_MISC "images/"
#define REP_BACKGROUND REP_IMAGES "background/"
#define REP_UI REP_IMAGES "ui/"


// Textures
#define TEXTURE_BACKGROUND REP_BACKGROUND "bg_bleu.bmp"
#define TEXTURE_BACKGROUND_LOAD REP_BACKGROUND "bg_bleu_chargement.bmp"
#define TEXTURE_BACKGROUND_SETTINGS REP_BACKGROUND "bg_settings.bmp"
#define TEXTURE_CELL REP_IMAGES "cellule_vide.bmp"



// Polices d'écriture
#define REP_POLICES REP_MISC "polices/"
#define POLICE_ARIAL REP_POLICES "arial.ttf"
#define POLICE_IMPACT REP_POLICES "impact.ttf"

enum Police_size {
    H0 = 40,
    H1 = 36,
    H2 = 30,
    H3 = 26,
    H4 = 24,
    H5 = 20,
    H6 = 16
};


#define MOVE_VALUE 10

#define FRAME_DELAY_ANIMATION 300
#define FPS 60
#define FRAME_DELAY 1000 / FPS  // délai entre frames (ici 16 ms)

#define MOVE_CAM 10


#define SIZE_CELL 16


enum type_objet {DEFAULT_OBJET};


using namespace std;

class Window; 
class Camera;
class TextElement; 
class Texture; 
class Objet;


        
class Window {

    public:
        Window(const size_t _win_width, const size_t _win_height);
        ~Window();

        SDL_Window* fenetre;
        SDL_Renderer* renderer;

        Camera* cam;

        SDL_bool program_launched; // True si la fenêtre est ouverte
        SDL_Event event; // Evènements de la fenêtre
        TTF_Font* font;

        int nb_frames;
        bool settings_open;

        

        vector<Objet*> objets_scene; // Emplacement des objets de la scène

        vector<TextElement*> texts;

        set<pair<int,int>> chunks;


    private:
        int win_width;
        int win_height;

        int center_win_width;
        int center_win_height;
        bool animationEnable;
        bool visibleTextZones;

        int delay_animation;
        
        

        Texture* texture_background; // Images de la scène

        vector<Texture*> texture_scene; // Images de la scène

        

        void addText(const string& text, int x, int y, SDL_Color color);   
        void renderTexts();
        SDL_Rect* worldToScreen(SDL_Rect* obj);



    public:
        void ExitWithError(string msg);
        void render(); // Actualise à l'écran
        Uint32 animate(Uint32 last_update);
        

        void addTextureScene(Texture* texture);
        void setTextureBackground(Texture* texture);
        void addObjet(Objet* obj);

        void cleanTextureScene();

        int get_center_win_width();
        int get_center_win_height();
 
        void updateText(size_t index, const string& newText);

        void addTextZones(int num, const int x, const int y);

        void showTextZones();
        void hideTextZones();

        void enableAnimations();
        void disableAnimations();
        void toggleAnimations();

        void speedupAnimations();
        void lowdownAnimations();

        void handleCameraEvents(bool& dragging, int& lastMouseX, int& lastMouseY);
        
};



class Camera {

    public:
        Camera(double _x, double _y, double _zoom);
        ~Camera();
        
        double x, y;
        double zoom;

};


class TextElement {
    public:
        TextElement(SDL_Renderer* renderer, TTF_Font* font, const string& text, int x, int y, SDL_Color color);
        ~TextElement();

        void setText(const string& newText);
        void render(SDL_Renderer* renderer);

        void addX(int _x);
        void addY(int _y);

        bool visible;

    private:
        SDL_Texture* texture;
        SDL_Rect rect;
        TTF_Font* font;
        string currentText;
        SDL_Color color;
        
        

        void updateTexture(SDL_Renderer* renderer);
};


class Texture {

    public:
        Texture(Window& window, string img, const size_t _width, const size_t _height, const int _x, const int _y);
        Texture(Window& window, string img, SDL_Rect* pos);
        ~Texture();

        SDL_Texture* texture;
        SDL_Rect* position;

    private:
        

    public:
        void setPlace(int _x, int _y);
        void setSize(int _width, int _height);

        void setX(int _x);
        void setY(int _y);

        void addX(int _x);
        void addY(int _y);
};


class Objet {
    public:
        Objet(Window& window, string texture_path, const size_t width, const size_t height, const int _x, const int _y, const type_objet _type);
        ~Objet();

        SDL_Rect* position;

    private:
        Texture* texture; // Texture qui va être affichée
        static int compteur_id;
        int id;

        int type;
};



void update_grid(set<pair<int,int>>& chunks);


#endif