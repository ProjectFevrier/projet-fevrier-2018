#define PI 3.14159265359
#define FRAMERATE 120
#define BG_SPEED 5
#define BG_VELOCITY 200
#define PRLX_VELOCITY 30
#define MAP_HEIGHT 2000
#define PONEY_VELOCITY 500
#define X_OFFSET 500

#define CD_SHOOT 0.55
#define CD_RECOIL 0.25
#define GRAVITY 9.81
#define SPEED_PLAYER -100
#define CD_ANIM_WALK 0.02
#define NB_ANIM_WALK 8

#define WIDTH_PLAYER 87
#define HEIGHT_PLAYER 178

#define WIDTH_BRAS 85
#define HEIGHT_BRAS 51

// Coeur
#define WIDTH_COEUR 33
#define HEIGHT_COEUR 40

#define CD_ANIM_COEUR 0.05
#define NB_ANIM_COEUR 6

// -- | Hud | --

// Jauge
#define CD_ANIM_JAUGE 0.080
#define NB_ANIM_JAUGE 13
#define CD_TIME_JAUGE 1.20
#define JAUGE_START_SIZE 128
//

#define PLAYER_SIZE_WIDTH 46
#define PLAYER_SIZE_HEIGHT 98
#define SPEED_PLAYER 250
#define SPEED_PLAYER_FALL 20
#define GRAVITY 100

#define Color_Collid 255

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
	int intAnimX;
	int intAnim;
	int intAnimBras;
	int Shoot;
	int ShootAng;
	int Recoil;
	int airOn;
	int jaugePoint;
	int upShoot;

	int OnJump;
	int OnPlatform;

	int BlockRight;
	int BlockLeft;

	float Speed;
	float angCursor;
	float cooldownShoot;
	//vitesse en fct de la jauge
	float speedFactor;
	enum e_stateAnim
	{
		AIR = 0,
		SOL,

	}stateAir;

	float shoot_Current;
	float shoot_Since;
	float shoot_Start;

	float recoil_Current;
	float recoil_Start;
	float recoil_Since;

	float walk_Current;
	float walk_Start;
	float walk_Since;

	float timer_Current;
	float timer_Start;
	int timer_Since;
	
	sfVector2f velocity;

	// Anim
	sfSprite *Bras;
	sfVector2f pos_Bras;
	sfVector2f origin_Bras;
	sfFloatRect sizeSpr;
	sfIntRect rectBras;
	/*Collision Control*/
	int leftCollider;
	int rightCollider;
	int downCollider;

	float offSetCollision_Y ;
	float offSetCollision_X ;

	int blobk ;

};

t_player Player;



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

typedef struct s_background t_background;

struct s_background
{
	sfSprite* sprite;
	sfVector2f pos;
	sfImage *collid;
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
	sfIntRect animRect;
	int ennemiType;
	int animFrames;
	int currentAnimFrame;
	int currentAnimFrameTop;
	float speed;
	float angle;
	int distMax;
	int Direction;
	int elementsNumber;

	float enemy_Current;
	float enemy_Since;
	float enemy_Start;

};

#pragma region Aiguille et Rouages
typedef struct s_aiguille t_aiguille;

struct s_aiguille
{
	// Aiguilles
	sfSprite *sprite_barre;
	sfVector2f pos;
	sfVector2f Origin;
	sfFloatRect size_barre;

	int sens;
	float angMin;
	float angMax;
	float angle;
	float angStart;
	float speed;
};

typedef struct s_rouages t_rouages;

struct s_rouages
{
	// Rouages
	sfSprite *sprite;
	sfVector2f pos;
	sfVector2f Origin;
	sfFloatRect size;

	float angle;
};
#pragma endregion

typedef struct s_hud t_hud;

struct s_hud
{
	sfSprite* sprite;
	sfSprite* sprite_grille;
	sfSprite* sprite_jauge;
	sfVector2f pos;

	// Hud
	sfVector2f jaugePos;
	sfIntRect jaugeRect;
	sfVector2f jaugeOrigin;
	int intAnimX;

	float jauge_Start;
	float jauge_Current;
	float jauge_Since;

	// Progression
	float time_Start;
	float time_Current;
	float time_Since;

	//Menu
	sfSprite *sprite_buttMenu;
	sfVector2f pos_buttMenu;
	sfIntRect buttMenu_Rect;
	sfFloatRect buttMenu_hitBox;
	int intButt;

	// Aiguilles
	enum e_stateBarre
	{
		START = 0,
		MID,
		END,
		KILL,

	}stateBarre, previousState;

	int countAfterKill;

	t_aiguille Aiguille[3];

	t_rouages Rouages[5];
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
	int speedFactor;
};

typedef struct s_txtBox t_txtBox;

struct s_txtBox
{
	int txtOutline;
	unsigned int fontSize;
	sfText* txt;
	sfFont* font;
	sfVector2f txtOrigin;
	sfVector2f txtPos;
	sfFloatRect txtBoundingBox;
};


typedef struct s_menu t_menu;

struct s_menu
{
	t_txtBox Jouer;
	t_txtBox Highscore;
	t_txtBox Quitter;
};

typedef struct s_scores t_scores;

struct s_scores
{
	char* name;
	int score;
	t_txtBox txtB;
	t_txtBox txtS;
};

typedef struct s_scoreTable t_scoreTable;

struct s_scoreTable
{
	t_scores playerScore;
	t_scores score1;
	t_scores score2;
	t_scores score3;
	t_txtBox backBtn;
	int isPressed;
	int lastKeyPressed;
	int nameIdex;
	int isPressedBack;
};
///////////////////////////////////////
#pragma endregion
// Liste Chainé
typedef struct t_listElement ListElement;

#pragma region Liste Chaine
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

typedef enum e_gameState t_gameState;

enum e_gameState
{
	MENU = 0 ,
	HIGHSCORE,
	GAME,
	END_GAME,

};
#pragma endregion


#pragma region Fonctions
// Fonctions
sfSprite* createSprite(char *_source);
void txtCreate(char* _src, t_txtBox* _text);
void initMenu(t_menu* _menuTxts);
void loadHighscores(t_scoreTable* _scoreTable);
void highScoreEnterInit(t_scoreTable* _scoreTable, t_player *Player);
void highScoreEnter(t_scoreTable* _scoreTable, t_player *Player, t_gameState* _gameState);

/*fct player*/
void initPlayer(t_player *Player);
sfVector2f vectorStart(sfRenderWindow* _window, t_player *Player, int noAngle, int _index);
float createAngle(sfVector2f _pointA, sfRenderWindow *_window);
void managePlayer(sfRenderWindow* _window, sfVideoMode _mode, t_player *Player, List *_list, float *gravity_Since, float _velocityOffset);
void Gravity(sfRenderWindow* _window, sfVideoMode _mode, t_player *Player, float *gravity_Since);
void Gravity2(sfRenderWindow* _window, sfVideoMode _mode, t_player *Player, float *gravity_Since);
void manageAnimPlayer(t_player *PLAYER, sfRenderWindow* _window);
void checkColision(t_player *Player, t_rectangle *_rectangle1, t_rectangle *_rectangle2);
void collidPlayer(t_player *Player, t_maps* _maps);

/*fcts plateformes et enemis*/
void loadMaps(t_maps* _maps, int _currentLevel, int _asStarted);
void nextMapYOffset(t_maps* _maps, float _velocityOffset,t_player *Player);
void moveMaps(t_maps* _maps, float _velocityOffset, t_player *Player);
void displayMaps(t_maps* _maps, sfRenderWindow *window);
void managePoney(sfRenderWindow *_window, sfVideoMode _mode, t_poney *_poney1, t_poney *_poney2, float _timeSinceBackground);

// Liste
void AddBullet(sfRenderWindow* _window, sfVideoMode _mode, List *_list, t_player *Player, sfVector2f Direction);
void ReadBullet(sfRenderWindow* _window, sfVideoMode _mode, List *_list, t_maps* _maps, t_player *Player, t_hud *Hud);
void DeleteFirstBullet(List *_list);
void DeleteBulletToID(List *_list, int ID);
//

// Hud
void initHud(t_hud *Hud, t_player *Player);
void manageHud(sfRenderWindow *window, t_hud *Hud, t_player *Player);
int IsOver(sfRenderWindow *_window, sfFloatRect boundingBox);
void normalAngle(t_hud *Hud);
int randomBarre(int type, int _index, t_hud *Hud);
int randBarre_Speed(t_hud *Hud);
//


/*New fonctions*/
void loadMaps(t_maps* _maps, int _currentLevel, int _asStarted);
///////////////////////////////////////
#pragma endregion
