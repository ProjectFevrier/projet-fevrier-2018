#pragma once

#define PI 3.14159265359
#define FRAMERATE 60
#define BG_VELOCITY 200
#define PRLX_VELOCITY 30
#define MAP_HEIGHT 2000
#define PONEY_VELOCITY 500
#define X_OFFSET 500

#define CD_SHOOT 0.35
#define GRAVITY 9.81
#define SPEED_PLAYER -100
#define CD_RECOIL 0.25

#define WIDTH_PLAYER 124
#define HEIGHT_PLAYER 186

#define WIDTH_BRAS 85
#define HEIGHT_BRAS 50

// Coeur
#define WIDTH_COEUR 33
#define HEIGHT_COEUR 40

#define CD_ANIM_COEUR 0.05
#define NB_ANIM_COEUR 7

#define PLAYER_SIZE_WIDTH 46
#define PLAYER_SIZE_HEIGHT 98
#define SPEED_PLAYER 250
#define SPEED_PLAYER_FALL 20
#define GRAVITY 100

#pragma region Variables
// Variables

sfColor alpha = { 255,255,255,0 };
sfColor red = { 255,0,0,255 };
////////////////////////////////////////
#pragma endregion


#pragma region Struct
// Struct
typedef struct s_player t_player;

struct s_player
{
	sfSprite *sprite;
	sfVector2f pos;
	sfVector2f Origin;
	sfFloatRect hitBox;
	sfIntRect rectAnim;
	sfVector2f shootPoint;
	int intAnim;
	int Shoot;
	int ShootAng;
	int Recoil;
	int airOn;
	int jaugePoint;

	int OnJump;
	int OnPlatform;

	int BlockRight;
	int BlockLeft;

	float Speed;
	float speedAir;
	float angCursor;
	float cooldownShoot;

	float shoot_Current;
	float shoot_Since;
	float shoot_Start;

	float recoil_Current;
	float recoil_Start;
	float recoil_Since;

	sfVector2f velocity;

	// Anim
	sfSprite *Bras;
	sfVector2f pos_Bras;
	sfVector2f origin_Bras;
	sfFloatRect sizeSpr;
	sfIntRect rectBras;

};

t_player Player;

typedef struct s_background t_background;

typedef struct s_bullet t_bullet;

struct s_bullet
{
	sfVector2f pos;
	sfSprite* sprite;
	sfVector2f Origin;
	sfVector2f Dir;
	sfFloatRect hitBox;
	float speed;
	float angle;

	sfIntRect rect;
	int intAnim;
	float anim_shoot_Current;
	float anim_shoot_Since;
	float anim_shoot_Start;
};

struct s_background
{
	sfSprite* sprite;
	sfVector2f pos;
	int backgroundNumber;
};

typedef struct s_rectangle t_rectangle;

struct s_rectangle
{
	sfRectangleShape* rectangle;
	sfVector2f pos;
	sfVector2f size;
	sfVector2f Origin;
	sfFloatRect hitBox;
	int elementsNumber;
};

typedef struct s_paralax t_paralax;

struct s_paralax
{
	sfSprite* sprite;
	sfVector2f pos;
	sfIntRect animRect;
};

typedef struct s_poney t_poney;

struct s_poney
{
	sfVector2f pos;
	int xStart;
	sfSprite* sprite;
	sfVector2f Origin;
	sfFloatRect hitBox;
	float speed;
	float angle;
	int distMax;
	int Direction;
	int elementsNumber;

};

typedef struct s_hud t_hud;

struct s_hud
{
	sfSprite* sprite;
	sfSprite* sprite_grille;
	sfVector2f pos;

	sfRectangleShape *jaugeRect;
	sfVector2f jaugeScale;
	sfVector2f jaugeSize;
	sfVector2f jaugePos;
	sfVector2f jaugeOrigin;
	float jaugeSizeMax;
	sfColor alpha;
	sfColor red;

};

typedef struct s_mapSlot t_mapSlot;

struct s_mapSlot
{
	t_background background;
	t_rectangle collisions[10];
	t_poney ennemis[10];
};

typedef struct s_maps t_maps;

struct s_maps
{
	t_mapSlot saveMap1;
	t_mapSlot saveMap2;
	t_mapSlot saveMap3;
	t_mapSlot saveMap4;
	t_mapSlot currentMap;
	t_mapSlot nextMap;
};


///////////////////////////////////////
#pragma endregion
// Liste Chainé
typedef struct t_listElement ListElement;

#pragma region Liste Chainé
struct t_listElement
{
	t_bullet Bullet;
	ListElement *nextElement;
};

typedef struct t_list List;

struct t_list
{
	ListElement *firstElement;
};
#pragma endregion Liste Chainé


#pragma region Fonctions
// Fonctions
sfSprite* createSprite(char *_source);

/*fct player*/
void initPlayer(t_player *Player);
void affMap(sfRenderWindow *_window, t_player *Player);
sfVector2f vectorStart(sfRenderWindow* _window, t_player *Player, int noAngle);
float createAngle(sfVector2f _pointA, sfRenderWindow *_window);
void managePlayer(sfRenderWindow* _window, sfVideoMode _mode, t_player *Player, List *_list, float *gravity_Since);
void Gravity(sfRenderWindow* _window, sfVideoMode _mode, t_player *Player, float *gravity_Since);
void manageAnimPlayer(t_player *PLAYER);
void checkColision(t_player *Player, t_rectangle *_rectangle1, t_rectangle *_rectangle2);


/*fcts plateformes et enemis*/
void loadMaps(t_maps* _maps, int _currentLevel, int _asStarted);
void nextMapYOffset(t_maps* _maps, float _velocityOffset);
void moveMaps(t_maps* _maps, float _velocityOffset);
void displayMaps(t_maps* _maps, sfRenderWindow *window);
void managePoney(sfRenderWindow *_window, sfVideoMode _mode, t_poney *_poney1, t_poney *_poney2, float _timeSinceBackground);

// Liste
void AddBullet(sfRenderWindow* _window, sfVideoMode _mode, List *_list, t_player *Player, sfVector2f Direction);
void ReadBullet(sfRenderWindow* _window, sfVideoMode _mode, List *_list, t_poney *_poney1, t_poney *_poney2, t_player *Player);
void DeleteFirstBullet(List *_list);
void DeleteBulletToID(List *_list, int ID);
//

// Hud
void initHud(t_hud *Hud, t_player *Player);
void manageHud(sfRenderWindow *window, t_hud *Hud, t_player *Player);
//


/*New fonctions*/
void loadMaps(t_maps* _maps, int _currentLevel, int _asStarted);
///////////////////////////////////////
#pragma endregion