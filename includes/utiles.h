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
#define TEXTURE_BACKGROUND_DEFAULT REP_BACKGROUND "bg_bleu.bmp"
#define TEXTURE_BACKGROUND_DARK REP_BACKGROUND "bg_sombre.bmp"
#define TEXTURE_BACKGROUND_LIGHT REP_BACKGROUND "bg_clair.bmp"
#define TEXTURE_BACKGROUND_LOAD REP_BACKGROUND "bg_bleu_chargement.bmp"

#define TEXTURE_BACKGROUND_SETTINGS_0 REP_BACKGROUND "bg_settings.bmp"

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
enum theme {DEFAULT, DARK, LIGHT};


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

        bool mouseLeftDown;
        bool mouseRightDown;

        

        SDL_bool program_launched; // True si la fenêtre est ouverte
        SDL_Event event; // Evènements de la fenêtre
        TTF_Font* font;

        bool settings_open;


        vector<Objet*> objets_scene; // Emplacement des objets de la scène

        vector<TextElement*> texts;

        set<pair<int,int>> chunks;


    private:

        Camera* cam;

        int nb_frames;

        int win_width;
        int win_height;
        int center_width;
        int center_height;  

        bool animationEnable;
        bool visibleTextZones;

        int delay_animation;

        theme default_color;

        SDL_Color text_color;

        bool displayGrid;

        bool dragging;
        int lastMouseX;
        int lastMouseY;

        int page_settings;


        

        Texture* texture_background; // Images de la scène

        vector<Texture*> texture_scene; // Images de la scène
        vector<Texture*> textures_background; // Images de fond

        vector<Texture*> textures_settings; // Images de fond

        

        void addText(const string& text, int x, int y, SDL_Color color);   
        void renderTexts();
        SDL_Rect* worldToScreen(SDL_Rect* obj);
        void apply_color();
        void drawGrid();



    public:
        void ExitWithError(string msg);
        void render(); // Actualise à l'écran
        Uint32 animate(Uint32 last_update);
        

        void addTextureScene(Texture* texture);
        void addTextureSettings(Texture* texture);

        void addTextureBackground(Texture* texture);
        void setTextureBackground(Texture* texture);
        void addObjet(Objet* obj);

        void cleanTextureScene();

        int get_width();
        int get_height();

        void refresh_window();

        int get_center_width();
        int get_center_height();
 
        void updateText(size_t index, const string& newText);
        void moveText(int num, const int x, const int y);
        void addTextZones(int num, const int x, const int y);


        void showTextZones();
        void hideTextZones();

        void enableAnimations();
        void disableAnimations();
        void toggleAnimations();

        void showGrid();
        void hideGrid();
        void toggleGrid();

        void speedupAnimations();
        void lowdownAnimations();

        void handleCameraEvents();
        SDL_Rect getMouseClick();

        void change_theme(int t);

        int get_nb_frames();
        void reset_frame();

        int get_nb_theme();

        void swap_page_settings();
        
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

        void setX(int _x);
        void setY(int _y);

        void setColor(SDL_Color c);

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