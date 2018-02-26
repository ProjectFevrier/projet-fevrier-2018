#include <SFML\Graphics.h>
#include <SFML\Audio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "lib.h"



int main()
{

	/*CSFML Vars*/
	sfVideoMode mode = { 1920, 1080, 32 };
	sfRenderWindow* window;
	sfEvent event;
	List* list = malloc(sizeof(List));
	list->firstElement = NULL;

	sfSprite* clouds = createSprite("resources/textures/nuage.png");
	sfVector2f cloudsPos = { 0 + X_OFFSET,0 };
	sfSprite_setPosition(clouds, cloudsPos);

	/*C Vars*/
	int randomMapNb = 0;
	int currentLevel = 1;
	float velocityOffset = 0;
	float startTimeBackground = (float)clock() / CLOCKS_PER_SEC;
	float timer_Start = (float)clock() / CLOCKS_PER_SEC;
	float timer_Since = 0;
	float timer_Current = 0;

	float gravity_Start = (float)clock() / CLOCKS_PER_SEC;;
	float gravity_Since = 0;
	float gravity_Current = 0;

	float startJauge = (float)clock() / CLOCKS_PER_SEC;
	///*Block The direction for Hitbox*/
	int BlockRight = 0;
	int BlockLeft = 0;

	int asStarted = 0;

	/*Structures*/
	t_paralax paralax;
	t_hud Hud;

	/*Ajout du 24/02/18*/
	t_maps maps;

#pragma region inits

	/*parallax init*/
	paralax.sprite = createSprite("resources/textures/fond.png");
	paralax.animRect.left = 0;
	paralax.animRect.top = 0;
	paralax.animRect.width = mode.width;
	paralax.animRect.height = mode.height;
	paralax.pos.x = 0 + X_OFFSET;
	paralax.pos.y = 0;
	sfSprite_setTextureRect(paralax.sprite, paralax.animRect);
	sfSprite_setPosition(paralax.sprite, paralax.pos);


	/*Player init*/
	initPlayer(&Player);

	/*Hud init*/
	initHud(&Hud, &Player);

	asStarted = 1;

	/*Load maps*/
	loadMaps(&maps, currentLevel, asStarted);
	nextMapYOffset(&maps, 0);

#pragma region // Position de la console
	HWND consoleWindow = GetConsoleWindow();
	SetWindowPos(consoleWindow, 0, -500, 500, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
#pragma endregion


#pragma endregion inits

	window = sfRenderWindow_create(mode, "SFML window", sfResize | sfClose, NULL);
	if (!window)
		return -1;

	sfRenderWindow_setFramerateLimit(window, 30);

	srand(time(NULL));

	/*Time*/

	while (sfRenderWindow_isOpen(window))
	{
		while (sfRenderWindow_pollEvent(window, &event))
		{
			if (event.type == sfEvtClosed)
				sfRenderWindow_close(window);
		}
		sfRenderWindow_clear(window, sfBlack);

		///////////////

		/*Calcul temps de boucle*/
		float currentTimeBackground = (float)clock() / CLOCKS_PER_SEC;
		float timeSinceBackground = currentTimeBackground - startTimeBackground;
		
		float currentJauge = (float)clock() / CLOCKS_PER_SEC;
		float sinceJauge = currentJauge - startJauge;
		velocityOffset = (BG_VELOCITY * timeSinceBackground);
		// Calcule de point !!!
		if (sinceJauge >= 1.5)
		{
			sinceJauge = 0;
			startJauge = currentJauge;
			Player.jaugePoint -= 0.2;
			printf("%.2f", Player.jaugePoint);
		}

		if (timeSinceBackground > 1.0f / FRAMERATE)
		{
			timeSinceBackground = 1.0f / FRAMERATE;
		}
		startTimeBackground = currentTimeBackground;

		// Timer Gravity Global
		gravity_Current = (float)clock() / CLOCKS_PER_SEC;
		gravity_Since = gravity_Current - gravity_Start;

		if (gravity_Since > 1.0f / FRAMERATE)
		{
			gravity_Since = 1.0f / FRAMERATE;
		}
		Player.shoot_Current = (float)clock() / CLOCKS_PER_SEC;


		/*Deplacements*/
		paralax.animRect.top += 5;
		if (paralax.animRect.top >= 8000)
		{
			paralax.animRect.top = 0;
		}
		moveMaps(&maps, velocityOffset);


		/*Set et draw parallax*/
		sfSprite_setTextureRect(paralax.sprite, paralax.animRect);
		sfRenderWindow_drawSprite(window, paralax.sprite, NULL);

		/*set et draw des elements*/
		displayMaps(&maps, window);



		managePoney(window, mode, maps.currentMap.ennemis, maps.nextMap.ennemis, timeSinceBackground);

		affMap(window, &Player);
		ReadBullet(window, mode, list, maps.currentMap.ennemis, maps.nextMap.ennemis, &Player);
		managePlayer(window, mode, &Player, list, &gravity_Since);
		manageHud(window, &Hud, &Player);
		Gravity(window, mode, &Player, &gravity_Since);
		checkColision(&Player, maps.currentMap.collisions, maps.nextMap.collisions);

		sfRenderWindow_drawSprite(window, clouds, NULL);
		sfRenderWindow_display(window);
	}
	sfRenderWindow_destroy(window);

	return 0;
}

sfSprite* createSprite(char *_source)
{
	sfTexture* texture = sfTexture_createFromFile(_source, NULL);
	if (texture == NULL)
		return NULL;


	sfSprite* sprite = sfSprite_create();

	sfSprite_setTexture(sprite, texture, sfTrue);

	return sprite;
}



void managePoney(sfRenderWindow *_window, sfVideoMode _mode, t_poney *_poney1, t_poney *_poney2,float _timeSinceBackground)
{
	for (int i = 0; i < _poney1[0].elementsNumber; i++)
	{

		//Mouvement Droite Gauche
		if (_poney1[i].Direction == 0)
		{
			if ((float)(_poney1[i].xStart + _poney1[i].distMax) < _poney1[i].pos.x)
			{
				_poney1[i].Direction = 1;
			}
			else
			{
				_poney1[i].pos.x += PONEY_VELOCITY * _timeSinceBackground;
			}
		}
		else
		{
			if ((float)(_poney1[i].xStart - _poney1[i].distMax) > _poney1[i].pos.x)
			{
				_poney1[i].Direction = 0;
			}
			else
			{
				_poney1[i].pos.x -= PONEY_VELOCITY * _timeSinceBackground;
			}
		}
	}
	for (int i = 0; i < _poney2[0].elementsNumber; i++)
	{

		//Mouvement Droite Gauche
		if (_poney2[i].Direction == 0)
		{
			if ((float)(_poney2[i].xStart + _poney2[i].distMax) < _poney2[i].pos.x)
			{
				_poney2[i].Direction = 1;
			}
			else
			{
				_poney2[i].pos.x += PONEY_VELOCITY * _timeSinceBackground;
			}
		}
		else
		{
			if ((float)(_poney2[i].xStart - _poney2[i].distMax) > _poney2[i].pos.x)
			{
				_poney2[i].Direction = 0;
			}
			else
			{
				_poney2[i].pos.x -= PONEY_VELOCITY * _timeSinceBackground;
			}
		}
	}
}

void initPlayer(t_player *Player)
{
	Player->sprite = createSprite("resources/textures/persoT.png");

	Player->pos.x = 500 + X_OFFSET;
	Player->pos.y = 500;

	Player->shootPoint.x = Player->pos.x;
	Player->shootPoint.y = Player->pos.y;

	Player->Shoot = sfFalse;

	Player->ShootAng = 0;

	Player->angCursor = 0;

	Player->cooldownShoot = 1.50;


	Player->rectAnim.left = 0;
	Player->rectAnim.top = 0;
	Player->rectAnim.width = WIDTH_PLAYER;
	Player->rectAnim.height = HEIGHT_PLAYER;

	sfSprite_setTextureRect(Player->sprite, Player->rectAnim);

	Player->intAnim = 0;

	Player->airOn = 1;

	Player->hitBox = sfSprite_getGlobalBounds(Player->sprite);

	Player->Origin.x = Player->hitBox.width / 2;
	Player->Origin.y = Player->hitBox.height / 2;

	sfSprite_setOrigin(Player->sprite, Player->Origin);

	Player->velocity.x = 0;
	Player->velocity.y = 0;

	Player->Recoil = 0;

	Player->Speed = 3.5;
	Player->speedAir = 1.5;

	// Anim Bras
	Player->Bras = createSprite("resources/textures/Bras.png");

	Player->rectBras.left = 0;
	Player->rectBras.top = 0;
	Player->rectBras.width = 85;
	Player->rectBras.height = 50;

	sfSprite_setTextureRect(Player->Bras, Player->rectBras);
	Player->sizeSpr = sfSprite_getGlobalBounds(Player->Bras);

	Player->origin_Bras.x = Player->sizeSpr.width / 2 + 33;
	Player->origin_Bras.y = Player->sizeSpr.height / 2 - 13;

	sfSprite_setOrigin(Player->Bras, Player->origin_Bras);

	Player->BlockLeft = 0;
	Player->BlockRight = 0;

	Player->jaugePoint = 135.5;

	return Player;
}

void affMap(sfRenderWindow *_window, t_player *Player)
{
	//sfSprite_setPosition(BackGround->sprite, BackGround->pos);
	//sfRenderWindow_drawSprite(_window, BackGround->sprite, NULL);

	sfSprite_setPosition(Player->sprite, Player->pos);
	sfRenderWindow_drawSprite(_window, Player->sprite, NULL);

	//sfRenderWindow_drawSprite(_window, Hud->sprite, NULL);
	// AnimBras
	sfSprite_setPosition(Player->Bras, Player->pos_Bras);
	sfRenderWindow_drawSprite(_window, Player->Bras, NULL);
}

void AddBullet(sfRenderWindow* _window, sfVideoMode _mode, List *_list, t_player *Player, sfVector2f Direction)
{
	ListElement *newElement = malloc(sizeof(ListElement));
	if (Player->intAnim == 0) // Gauche
	{
		newElement->Bullet.pos.x = Player->pos.x - 48;
		newElement->Bullet.pos.y = Player->pos.y - 12;
	}
	else if (Player->intAnim == 1) // Droite
	{
		newElement->Bullet.pos.x = Player->pos.x + 48;
		newElement->Bullet.pos.y = Player->pos.y - 12;
	}
	else if (Player->intAnim == 2) // Mid
	{
		newElement->Bullet.pos.x = Player->pos.x;
		newElement->Bullet.pos.y = Player->pos.y + 25;
	}


	newElement->Bullet.sprite = createSprite("resources/textures/Coeur.png");

	newElement->Bullet.intAnim = 0;

	newElement->Bullet.rect.width = WIDTH_COEUR;
	newElement->Bullet.rect.height = HEIGHT_COEUR;
	newElement->Bullet.rect.left = newElement->Bullet.intAnim * WIDTH_COEUR;
	newElement->Bullet.rect.top = 0;

	newElement->Bullet.anim_shoot_Current = 0;
	newElement->Bullet.anim_shoot_Since = 0;
	newElement->Bullet.anim_shoot_Start = (float)clock() / CLOCKS_PER_SEC;

	sfSprite_setTextureRect(newElement->Bullet.sprite, newElement->Bullet.rect);

	newElement->Bullet.hitBox = sfSprite_getGlobalBounds(newElement->Bullet.sprite);

	newElement->Bullet.Origin.x = newElement->Bullet.hitBox.width / 2;
	newElement->Bullet.Origin.y = newElement->Bullet.hitBox.height / 2;

	newElement->Bullet.Dir.x = Direction.x;
	newElement->Bullet.Dir.y = Direction.y;

	newElement->Bullet.speed = 20.0;

	newElement->Bullet.angle = RadToDeg(createAngle(Player->pos, _window));

	sfSprite_setOrigin(newElement->Bullet.sprite, newElement->Bullet.Origin);

	newElement->nextElement = _list->firstElement;
	_list->firstElement = newElement;
}

void ReadBullet(sfRenderWindow* _window, sfVideoMode _mode, List *_list, t_poney *_poney1, t_poney *_poney2, t_player *Player)
{
	ListElement *currentElement = _list->firstElement;

	int countElement = 0;

	while (currentElement != NULL)
	{
		currentElement->Bullet.pos.x += currentElement->Bullet.Dir.x * currentElement->Bullet.speed;
		currentElement->Bullet.pos.y += currentElement->Bullet.Dir.y * currentElement->Bullet.speed;
		currentElement->Bullet.hitBox = sfSprite_getGlobalBounds(currentElement->Bullet.sprite);

		sfSprite_setPosition(currentElement->Bullet.sprite, currentElement->Bullet.pos);

		// Bords de map X
		if (currentElement->Bullet.pos.x <= (0 + X_OFFSET) + WIDTH_COEUR || currentElement->Bullet.pos.x >= 1920 + WIDTH_COEUR)
		{
			printf("Delete");
			/*DeleteBulletToID(_list, countElement);
			break;*/
		}
		// Bords de map Y
		if (currentElement->Bullet.pos.y <= 0 || currentElement->Bullet.pos.y >= 1080 + HEIGHT_COEUR)
		{
			printf("Delete");
			/*DeleteBulletToID(_list, countElement);
			break;*/
		}

		//Animation
		currentElement->Bullet.anim_shoot_Current = (float)clock() / CLOCKS_PER_SEC;

		currentElement->Bullet.anim_shoot_Since = currentElement->Bullet.anim_shoot_Current - currentElement->Bullet.anim_shoot_Start;

		if (currentElement->Bullet.anim_shoot_Since >= CD_ANIM_COEUR)
		{
			currentElement->Bullet.anim_shoot_Since = 0;
			currentElement->Bullet.anim_shoot_Start = currentElement->Bullet.anim_shoot_Current;

			currentElement->Bullet.intAnim += 1;

			if (currentElement->Bullet.intAnim >= NB_ANIM_COEUR - 1)
			{
				currentElement->Bullet.intAnim = 0;
			}

			currentElement->Bullet.rect.left = currentElement->Bullet.intAnim * currentElement->Bullet.rect.width;

			sfSprite_setTextureRect(currentElement->Bullet.sprite, currentElement->Bullet.rect);
		}
		////////////////

		sfRenderWindow_drawSprite(_window, currentElement->Bullet.sprite, NULL);

		for (int i = 0; i < _poney1[0].elementsNumber; i++)
		{
			//_poney1[i].hitBox = sfSprite_getGlobalBounds(_poney1[i].sprite);
			if (sfFloatRect_intersects(&_poney1[i].hitBox, &currentElement->Bullet.hitBox, NULL))
			{
				_poney1[i].sprite = NULL;
				_poney1[i].hitBox.width = 0;
				_poney1[i].hitBox.height = 0;
				Player->jaugePoint += 10;
				DeleteBulletToID(_list, countElement);
				break;
			}
		}
		for (int i = 0; i < _poney2[0].elementsNumber; i++)
		{
			//_poney2[i].hitBox = sfSprite_getGlobalBounds(_poney2[i].sprite);
			if (sfFloatRect_intersects(&(_poney2[i].hitBox), &currentElement->Bullet.hitBox, NULL))
			{
				_poney2[i].sprite = NULL;
				_poney2[i].hitBox.width = 0;
				_poney2[i].hitBox.height = 0;
				Player->jaugePoint += 50.0;
				DeleteBulletToID(_list, countElement);
				break;
			}
		}

		currentElement = currentElement->nextElement;

		countElement++;
	}
}

void DeleteFirstBullet(List *_list)
{
	ListElement *deleteElement = _list->firstElement;
	_list->firstElement = _list->firstElement->nextElement;
	free(deleteElement);
}

void DeleteBulletToID(List *_list, int ID)
{
	ListElement *currentElement = _list->firstElement;

	int currentID = 0;

	if (currentID == ID)
	{
		DeleteFirstBullet(_list);
	}
	else
	{
		while (currentElement != NULL)
		{
			currentID++;
			if (currentID == ID)
			{
				ListElement *deletedElement = currentElement->nextElement;

				currentElement->nextElement = currentElement->nextElement->nextElement;
				free(deletedElement);
				break;
			}
			else
			{
				currentElement = currentElement->nextElement;
			}
		}
	}
}

sfVector2f vectorStart(sfRenderWindow* _window, t_player *Player, int noAngle)
{
	sfVector2i posCursor = sfMouse_getPosition(_window);

	sfVector2f Direction;

	if (noAngle == 1)
	{
		posCursor.x = Player->pos.x - 262;
		posCursor.y = Player->pos.y - 50;
	}
	else if (noAngle == 2)
	{
		posCursor.x = Player->pos.x + 262;
		posCursor.y = Player->pos.y - 50;
	}

	float BA = 0;
	float AC = 0;
	float Ang = 0;
	float Angle = 0;

	// Haut Droite
	if (posCursor.x > Player->pos.x && posCursor.y < Player->pos.y)
	{
		BA = Player->pos.x - (float)posCursor.x;
		AC = Player->pos.y - (float)posCursor.y;

		Ang = atan(AC / BA);
		Angle = Ang * (180 / PI);

		Direction.x = cos(Ang);
		Direction.y = sin(Ang);
	}
	// Bas Droite
	if (posCursor.x > Player->pos.x && posCursor.y > Player->pos.y)
	{
		BA = Player->pos.x - (float)posCursor.x;
		AC = Player->pos.y - (float)posCursor.y;

		Ang = atan(AC / BA);
		Angle = Ang * (180 / PI);

		Direction.x = cos(Ang);
		Direction.y = sin(Ang);
	}
	// Haut Gauche
	if (posCursor.x < Player->pos.x && posCursor.y > Player->pos.y)
	{
		BA = (float)posCursor.x - Player->pos.x;
		AC = (float)posCursor.y - Player->pos.y;

		Ang = atan(AC / BA);
		Angle = Ang * (180 / PI);

		Direction.x = cos(Ang);
		Direction.y = sin(Ang);

		Direction.x *= -1;
		Direction.y *= -1;
	}
	// Bas Gauche
	if (posCursor.x < Player->pos.x && posCursor.y < Player->pos.y)
	{
		BA = (float)posCursor.x - Player->pos.x;
		AC = (float)posCursor.y - Player->pos.y;

		Ang = atan(AC / BA);
		Angle = Ang * (180 / PI);

		Direction.x = cos(Ang);
		Direction.y = sin(Ang);

		Direction.x *= -1;
		Direction.y *= -1;
	}

	return Direction;
}

float RadToDeg(double _radians)
{
	return _radians * (180 / PI);
}

float createAngle(sfVector2f _pointA, sfRenderWindow *_window)
{
	sfVector2i posMouse = sfMouse_getPosition(_window);

	float angleRad = atan2((posMouse.y - _pointA.y), (posMouse.x - _pointA.x));

	return angleRad;
}

void managePlayer(sfRenderWindow* _window, sfVideoMode _mode, t_player *Player, List *_list, float *gravity_Since)
{
	// Calcule D'angle
	Player->angCursor = RadToDeg(createAngle(Player->pos, _window));

	if (Player->angCursor < 0) // Evite un angle n�gatif !
	{
		Player->angCursor += (2 * 180);
	}

	int noAngle = 0;

	if ((Player->angCursor > 0 && Player->angCursor < 42.50) || Player->angCursor >= 350)  // Droite
	{
		Player->ShootAng = 1;
		Player->intAnim = 1;

		sfSprite_setRotation(Player->Bras, Player->angCursor);
	}
	else if (Player->angCursor >= 42.5 && Player->angCursor < 137.50)    // Mid
	{
		Player->ShootAng = 1;
		Player->intAnim = 2;
	}
	else if (Player->angCursor >= 137.50 && Player->angCursor < 190)   // Gauche
	{
		Player->ShootAng = 1;
		Player->intAnim = 0;

		Player->angCursor += (2 * 90);
		sfSprite_setRotation(Player->Bras, Player->angCursor);
		Player->angCursor -= (2 * 90);
	}
	else
	{
		Player->ShootAng = 1;
		if (Player->angCursor > 190 && Player->angCursor < 270) // Gauche
		{
			noAngle = 1;
			Player->intAnim = 0;
		}
		else if (Player->angCursor > 270 && Player->angCursor < 350)
		{
			noAngle = 2;
			Player->intAnim = 1;
		}
	}
	sfVector2i Temporaire = sfMouse_getPosition(_window);
	/////////////////////////////////////////////

	// Tir
	if (Player->shoot_Since <= CD_SHOOT)
	{
		Player->shoot_Since = Player->shoot_Current - Player->shoot_Start;
	}

	if (sfMouse_isButtonPressed(sfMouseLeft) && Player->Shoot == sfFalse && Player->shoot_Since > CD_SHOOT && Player->ShootAng == 1)
	{
		Player->Recoil = 2;
		sfVector2f Direction = vectorStart(_window, Player, noAngle);
		AddBullet(_window, _mode, _list, Player, Direction);

		Player->shoot_Start = Player->shoot_Current;
		Player->Shoot = sfTrue;
	}
	else if (sfMouse_isButtonPressed(sfMouseLeft) == sfFalse)
	{
		Player->Shoot = sfFalse;
		Player->shoot_Since = 0;
	}
	/////////////////////////////////////////////

	// Recul du tir
	if (Player->Recoil == 1 || Player->Recoil == 2)
	{
		if (Player->Recoil == 2)
		{
			Player->recoil_Start = (float)clock() / CLOCKS_PER_SEC;
			Player->Recoil = 1;
		}

		Player->velocity.y -= (*gravity_Since * GRAVITY) * 110;

		if (Player->velocity.y < 0)
		{
			if (Player->velocity.y <= -150)
			{
				Player->velocity.y = -150;
			}
		}
		Player->pos.y += Player->velocity.y * (*gravity_Since);

		Player->recoil_Since = Player->shoot_Current - Player->recoil_Start;

		if (Player->recoil_Since >= CD_RECOIL)
		{
			Player->Recoil = 0;
			Player->recoil_Start = Player->shoot_Current;
		}
	}
	/////////////////////////////////////////////

	// Deplacement 

	if (sfKeyboard_isKeyPressed(sfKeyLeft) && Player->pos.x >= (0 + X_OFFSET) + WIDTH_PLAYER/2)
	{
		// Si Colisions sous les pieds
		if (Player->airOn == 0)
		{
			Player->pos.x -= Player->Speed * 5;
		}
		else
		{
			Player->pos.x -= Player->speedAir * 5;
		}
	}
	else if (sfKeyboard_isKeyPressed(sfKeyRight) && Player->pos.x <= 1920 - WIDTH_PLAYER/2)
	{
		if (Player->airOn == 0)
		{
			Player->pos.x += Player->Speed * 5;
		}
		else
		{
			Player->pos.x += Player->speedAir * 5;
		}
	}
	//////////////////////////////////////////////

	if (Player->pos.y < 0 || Player->pos.y > 1080)
	{
		//sfRenderWindow_close(_window);
	}

	manageAnimPlayer(Player);
}

void manageAnimPlayer(t_player *Player)
{

	Player->rectAnim.top = Player->intAnim * Player->rectAnim.height;
	sfSprite_setTextureRect(Player->sprite, Player->rectAnim);

	Player->rectBras.top = Player->intAnim * Player->rectBras.height;
	sfSprite_setTextureRect(Player->Bras, Player->rectBras);

	if (Player->airOn == 1 || Player->airOn == 2)
	{
		if (Player->intAnim == 0)
		{

			Player->pos_Bras.x = Player->pos.x + 10;
			Player->pos_Bras.y = Player->pos.y - 6;

			Player->origin_Bras.x = Player->sizeSpr.width / 2 + 33;
			Player->origin_Bras.y = Player->sizeSpr.height / 2 - 13;

			sfSprite_setOrigin(Player->Bras, Player->origin_Bras);
		}
		else if (Player->intAnim == 1)
		{

			Player->pos_Bras.x = Player->pos.x - 10;
			Player->pos_Bras.y = Player->pos.y - 6;

			Player->origin_Bras.x = Player->sizeSpr.width / 2 - 33;
			Player->origin_Bras.y = Player->sizeSpr.height / 2 - 13;

			sfSprite_setOrigin(Player->Bras, Player->origin_Bras);
		}
	}
	else if (Player->airOn == 3 || Player->airOn == 4)
	{

	}
}

void Gravity(sfRenderWindow* _window, sfVideoMode _mode, t_player *Player, float *gravity_Since)
{
	// Gravit�
	//if (Player->pos.y <= _mode.height - (Player->hitBox.height / 2))
	//{
	Player->velocity.y += (*gravity_Since * GRAVITY) * 25;
	Player->airOn = 1;

	Player->pos.x += Player->velocity.x * (*gravity_Since);
	Player->pos.y += Player->velocity.y * (*gravity_Since);
	//////////////////////////////////////////////
}

void checkColision(t_player *Player, t_rectangle *_rectangle1, t_rectangle *_rectangle2)
{
	
	for (int i = 0; i < _rectangle1[0].elementsNumber; i++)
	{

		Player->hitBox = sfSprite_getGlobalBounds(Player->sprite);
		_rectangle1[i].hitBox = sfRectangleShape_getGlobalBounds(_rectangle1[i].rectangle);
		sfFloatRect frctIntersection;
		if (sfFloatRect_intersects(&_rectangle1[i].hitBox, &Player->hitBox, &frctIntersection) == sfTrue)
		{
			/*Create Center Player and Rectangle*/


			/*Test Rantangle Collision (global)*/
			if (frctIntersection.height < _rectangle1[i].hitBox.height && frctIntersection.width > 1) {
				sfVector2f PlayerCenter, RectCenter;

				PlayerCenter.y = Player->hitBox.top + Player->hitBox.height / 2;
				RectCenter.y = _rectangle1[i].hitBox.top + _rectangle1[i].hitBox.height / 2;
				PlayerCenter.x = Player->hitBox.left + Player->hitBox.width / 2;
				RectCenter.x = _rectangle1[i].hitBox.left + _rectangle1[i].hitBox.width / 2;

				if (RectCenter.y - PlayerCenter.y > 0) {
					//printf("Up");
					Player->pos.y -= frctIntersection.height;
					Player->OnPlatform = 1;
					Player->OnJump = 0;
					Player->velocity.y = 0;

				}
				else if (RectCenter.y - PlayerCenter.y < 0) {
					//printf("Down");
					Player->pos.y += frctIntersection.height;
					Player->velocity.y = 0;
					Player->OnPlatform = 0;
					Player->OnJump = 1;

				}
			}

			if (frctIntersection.width < Player->hitBox.width && frctIntersection.height > 1) {
				sfVector2f PlayerCenter, RectCenter;
				PlayerCenter.x = Player->hitBox.left + Player->hitBox.width / 2;
				PlayerCenter.y = Player->hitBox.top + Player->hitBox.height / 2;
				RectCenter.y = _rectangle1[i].hitBox.top + _rectangle1[i].hitBox.height / 2;
				RectCenter.x = _rectangle1[i].hitBox.left + _rectangle1[i].hitBox.width / 2;
				/*Test Collision*/
				if (RectCenter.x - PlayerCenter.x > 0 && ((PlayerCenter.y - (PLAYER_SIZE_HEIGHT / 2)) - (RectCenter.y + (54 / 2)) <= -10)) {
					printf("%d\n", Player->OnPlatform);

					if (Player->OnPlatform == 0) {
						Player->pos.x -= frctIntersection.width;

					}
					printf("right\n");
					Player->BlockLeft = 1;

				}
				else if (RectCenter.x - PlayerCenter.x < 0 && ((PlayerCenter.y - (PLAYER_SIZE_HEIGHT / 2)) - (RectCenter.y + (54 / 2)) <= -10)) {
					printf("%d\n", Player->OnPlatform);

					if (Player->OnPlatform == 0) {
						Player->pos.x += frctIntersection.width;

					}
					printf("left\n");
					Player->BlockRight = 1;


				}

				printf("RectPoint : %.2f\n", ((PlayerCenter.y - (PLAYER_SIZE_HEIGHT / 2)) - (RectCenter.y + (54 / 2))));
			}
		}
	}
	for (int i = 0; i < _rectangle2[0].elementsNumber; i++)
	{

		Player->hitBox = sfSprite_getGlobalBounds(Player->sprite);
		_rectangle2[i].hitBox = sfRectangleShape_getGlobalBounds(_rectangle2[i].rectangle);
		sfFloatRect frctIntersection;
		if (sfFloatRect_intersects(&_rectangle2[i].hitBox, &Player->hitBox, &frctIntersection) == sfTrue)
		{
			/*Create Center Player and Rectangle*/


			/*Test Rantangle Collision (global)*/
			if (frctIntersection.height < _rectangle2[i].hitBox.height && frctIntersection.width > 1) {
				sfVector2f PlayerCenter, RectCenter;

				PlayerCenter.y = Player->hitBox.top + Player->hitBox.height / 2;
				RectCenter.y = _rectangle2[i].hitBox.top + _rectangle2[i].hitBox.height / 2;
				PlayerCenter.x = Player->hitBox.left + Player->hitBox.width / 2;
				RectCenter.x = _rectangle2[i].hitBox.left + _rectangle2[i].hitBox.width / 2;

				if (RectCenter.y - PlayerCenter.y > 0) {
					//printf("Up");
					Player->pos.y -= frctIntersection.height;
					Player->OnPlatform = 1;
					Player->OnJump = 0;
					Player->velocity.y = 0;

				}
				else if (RectCenter.y - PlayerCenter.y < 0) {
					//printf("Down");
					Player->pos.y += frctIntersection.height;
					Player->velocity.y = 0;
					Player->OnPlatform = 0;
					Player->OnJump = 1;

				}
			}

			if (frctIntersection.width < Player->hitBox.width && frctIntersection.height > 1) {
				sfVector2f PlayerCenter, RectCenter;
				PlayerCenter.x = Player->hitBox.left + Player->hitBox.width / 2;
				PlayerCenter.y = Player->hitBox.top + Player->hitBox.height / 2;
				RectCenter.y = _rectangle2[i].hitBox.top + _rectangle2[i].hitBox.height / 2;
				RectCenter.x = _rectangle2[i].hitBox.left + _rectangle2[i].hitBox.width / 2;
				/*Test Collision*/
				if (RectCenter.x - PlayerCenter.x > 0 && ((PlayerCenter.y - (PLAYER_SIZE_HEIGHT / 2)) - (RectCenter.y + (54 / 2)) <= -10)) {
					printf("%d\n", Player->OnPlatform);

					if (Player->OnPlatform == 0) {
						Player->pos.x -= frctIntersection.width;

					}
					printf("right\n");
					Player->BlockLeft = 1;

				}
				else if (RectCenter.x - PlayerCenter.x < 0 && ((PlayerCenter.y - (PLAYER_SIZE_HEIGHT / 2)) - (RectCenter.y + (54 / 2)) <= -10)) {
					printf("%d\n", Player->OnPlatform);

					if (Player->OnPlatform == 0) {
						Player->pos.x += frctIntersection.width;

					}
					printf("left\n");
					Player->BlockRight = 1;


				}

				printf("RectPoint : %.2f\n", ((PlayerCenter.y - (PLAYER_SIZE_HEIGHT / 2)) - (RectCenter.y + (54 / 2))));
			}
		}
	}
}

void initHud(t_hud *Hud, t_player *Player)
{
	Hud->sprite = createSprite("resources/textures/Jauge.png");
	Hud->sprite_grille = createSprite("resources/textures/Grille.png");

	Hud->pos.x = 0;
	Hud->pos.y = 0;


	Hud->jaugeRect = sfRectangleShape_create();
	Hud->jaugeScale.x = 1;
	Hud->jaugeScale.y = 1;

	Hud->jaugeSize.x = 38;
	Hud->jaugeSize.y = 0;

	Hud->jaugeSizeMax = 227;

	Hud->jaugePos.x = 221;
	Hud->jaugePos.y = 633;

	Hud->jaugeOrigin.x = 0;
	Hud->jaugeOrigin.y = Hud->jaugeSize.y;
}

void manageHud(sfRenderWindow *window, t_hud *Hud, t_player *Player)
{
	sfRenderWindow_drawSprite(window, Hud->sprite, NULL);

	sfRectangleShape_setPosition(Hud->jaugeRect, Hud->jaugePos);
	sfRectangleShape_setSize(Hud->jaugeRect, Hud->jaugeSize);
	sfRectangleShape_setFillColor(Hud->jaugeRect, red);
	sfRectangleShape_setScale(Hud->jaugeRect, Hud->jaugeScale);
	Hud->jaugeOrigin.x = 0;
	Hud->jaugeOrigin.y = Hud->jaugeSize.y;
	sfRectangleShape_setOrigin(Hud->jaugeRect, Hud->jaugeOrigin);
	Hud->jaugeSize.y = Player->jaugePoint;

	if (Hud->jaugeSize.y >= Hud->jaugeSizeMax)
	{
		Hud->jaugeSize.y = Hud->jaugeSizeMax;
		sfRenderWindow_close(window);
	}

	sfRenderWindow_drawRectangleShape(window, Hud->jaugeRect, NULL);

	//sfRenderWindow_drawSprite(window, Hud->sprite_grille, NULL);
}


void loadMaps(t_maps* _maps, int _currentLevel, int _asStarted)
{
	char* path = malloc(50);
	int elementsNumber = 0;
	FILE*file = NULL;

	sprintf_s(path, 50, "resources/maps/m%d-1.png", _currentLevel);
	_maps->saveMap1.background.sprite = createSprite(path);
	sprintf_s(path, 50, "resources/maps/m%d-2.png", _currentLevel);
	_maps->saveMap2.background.sprite = createSprite(path);
	sprintf_s(path, 50, "resources/maps/m%d-3.png", _currentLevel);
	_maps->saveMap3.background.sprite = createSprite(path);
	sprintf_s(path, 50, "resources/maps/m%d-4.png", _currentLevel);
	_maps->saveMap4.background.sprite = createSprite(path);


	///////////////////////////////////////////////////////////////////////////////////
	/*charger la map 1 du level en cours*/
	sprintf_s(path, 50, "resources/datas/m%d-1.txt", _currentLevel);
	fopen_s(&file, path, "r");
	if (file == NULL)
		printf_s("erreur ouvrture fichier plateformes");
	/*chargement des plateformes map 1*/
	fscanf_s(file, "%d\n", &elementsNumber);
	for (int i = 0; i < elementsNumber; i++)
	{
		_maps->saveMap1.collisions[i].rectangle = sfRectangleShape_create();
		fscanf_s(file, "pX=%f,pY=%f,sX=%f,sY=%f\n", &_maps->saveMap1.collisions[i].pos.x, &_maps->saveMap1.collisions[i].pos.y, &_maps->saveMap1.collisions[i].size.x, &_maps->saveMap1.collisions[i].size.y);
		printf_s ("colisions pX=%f,pY=%f,sX=%f,sY=%f\n", _maps->saveMap1.collisions[i].pos.x, _maps->saveMap1.collisions[i].pos.y, _maps->saveMap1.collisions[i].size.x, _maps->saveMap1.collisions[i].size.y);
		_maps->saveMap1.collisions[i].pos.x += X_OFFSET;
		
		sfRectangleShape_setSize(_maps->saveMap1.collisions[i].rectangle, _maps->saveMap1.collisions[i].size);
		sfRectangleShape_setPosition(_maps->saveMap1.collisions[i].rectangle,_maps->saveMap1.collisions[i].pos);
		_maps->saveMap1.collisions[i].hitBox = sfRectangleShape_getGlobalBounds(_maps->saveMap1.collisions[i].rectangle);
		_maps->saveMap1.collisions[i].elementsNumber = elementsNumber;
	}
	/*chargement des ennemis map 1*/
	fscanf_s(file, "%d\n", &elementsNumber);
	for (int i = 0; i < elementsNumber; i++)
	{
		_maps->saveMap1.ennemis[i].sprite = sfSprite_create();
		_maps->saveMap1.ennemis[i].sprite = createSprite("resources/sprites/m1-1.png");
		fscanf_s(file, "pXS=%d,pX=%f,pY=%f,d=%d,dM=%d\n", &_maps->saveMap1.ennemis[i].xStart, &_maps->saveMap1.ennemis[i].pos.x, &_maps->saveMap1.ennemis[i].pos.y, &_maps->saveMap1.ennemis[i].Direction, &_maps->saveMap1.ennemis[i].distMax);
		printf_s("poney : pXS=%d,pX=%f,pY=%f,d=%d,dM=%d\n", _maps->saveMap1.ennemis[i].xStart, _maps->saveMap1.ennemis[i].pos.x,_maps->saveMap1.ennemis[i].pos.y, _maps->saveMap1.ennemis[i].Direction, _maps->saveMap1.ennemis[i].distMax);

		_maps->saveMap1.ennemis[i].xStart += X_OFFSET;
		_maps->saveMap1.ennemis[i].pos.x += X_OFFSET;
		_maps->saveMap1.ennemis[i].hitBox = sfSprite_getGlobalBounds(_maps->saveMap1.ennemis[i].sprite);
		_maps->saveMap1.ennemis[i].Origin.x = _maps->saveMap1.ennemis[i].hitBox.width / 2;
		_maps->saveMap1.ennemis[i].Origin.y = _maps->saveMap1.ennemis[i].hitBox.height / 2;
		sfSprite_setOrigin(_maps->saveMap1.ennemis[i].sprite, _maps->saveMap1.ennemis[i].Origin);
		sfSprite_setPosition(_maps->saveMap1.ennemis[i].sprite, _maps->saveMap1.ennemis[i].pos);
		_maps->saveMap1.ennemis[i].elementsNumber = elementsNumber;
	}
	_maps->saveMap1.background.backgroundNumber = 1;
	_maps->saveMap1.background.pos.x = 0 + X_OFFSET;
	_maps->saveMap1.background.pos.y = 0;
	fclose(file);

	//////////////////////////////////////////////////////////////////////////////////
	/*charger la map 2 du level en cours*/
	sprintf_s(path, 50, "resources/datas/m%d-2.txt", _currentLevel);
	fopen_s(&file, path, "r");
	if (file == NULL)
		printf_s("erreur ouvrture fichier plateformes");
	/*chargement des plateformes map 2*/
	fscanf_s(file, "%d\n", &elementsNumber);
	for (int i = 0; i < elementsNumber; i++)
	{
		_maps->saveMap2.collisions[i].rectangle = sfRectangleShape_create();
		fscanf_s(file, "pX=%f,pY=%f,sX=%f,sY=%f\n", &_maps->saveMap2.collisions[i].pos.x, &_maps->saveMap2.collisions[i].pos.y, &_maps->saveMap2.collisions[i].size.x, &_maps->saveMap2.collisions[i].size.y);
		_maps->saveMap2.collisions[i].pos.x += X_OFFSET;

		sfRectangleShape_setSize(_maps->saveMap2.collisions[i].rectangle, _maps->saveMap2.collisions[i].size);
		sfRectangleShape_setPosition(_maps->saveMap2.collisions[i].rectangle, _maps->saveMap2.collisions[i].pos);
		_maps->saveMap2.collisions[i].hitBox = sfRectangleShape_getGlobalBounds(_maps->saveMap2.collisions[i].rectangle);
		_maps->saveMap2.collisions[i].elementsNumber = elementsNumber;
	}
	/*chargement des ennemis map 2*/
	fscanf_s(file, "%d\n", &elementsNumber);
	for (int i = 0; i < elementsNumber; i++)
	{
		_maps->saveMap2.ennemis[i].sprite = sfSprite_create();
		_maps->saveMap2.ennemis[i].sprite = createSprite("resources/sprites/m1-1.png");
		fscanf_s(file, "pXS=%d,pX=%f,pY=%f,d=%d,dM=%d\n", &_maps->saveMap2.ennemis[i].xStart, &_maps->saveMap2.ennemis[i].pos.x, &_maps->saveMap2.ennemis[i].pos.y, &_maps->saveMap2.ennemis[i].Direction, &_maps->saveMap2.ennemis[i].distMax);
		_maps->saveMap2.ennemis[i].xStart += X_OFFSET;
		_maps->saveMap2.ennemis[i].pos.x += X_OFFSET;
		_maps->saveMap2.ennemis[i].hitBox = sfSprite_getGlobalBounds(_maps->saveMap2.ennemis[i].sprite);
		_maps->saveMap2.ennemis[i].Origin.x = _maps->saveMap2.ennemis[i].hitBox.width / 2;
		_maps->saveMap2.ennemis[i].Origin.y = _maps->saveMap2.ennemis[i].hitBox.height / 2;
		sfSprite_setOrigin(_maps->saveMap2.ennemis[i].sprite, _maps->saveMap2.ennemis[i].Origin);
		sfSprite_setPosition(_maps->saveMap2.ennemis[i].sprite, _maps->saveMap2.ennemis[i].pos);
		_maps->saveMap2.ennemis[i].elementsNumber = elementsNumber;
	}
	_maps->saveMap2.background.backgroundNumber = 2;
	_maps->saveMap2.background.pos.x = 0 + X_OFFSET;
	_maps->saveMap2.background.pos.y = 0;
	fclose(file);

	//////////////////////////////////////////////////////////////////////////////////
	/*charger la map 3 du level en cours*/
	sprintf_s(path, 50, "resources/datas/m%d-3.txt", _currentLevel);
	fopen_s(&file, path, "r");
	if (file == NULL)
		printf_s("erreur ouvrture fichier plateformes");
	/*chargement des plateformes map 3*/
	fscanf_s(file, "%d\n", &elementsNumber);
	for (int i = 0; i < elementsNumber; i++)
	{
		_maps->saveMap3.collisions[i].rectangle = sfRectangleShape_create();
		fscanf_s(file, "pX=%f,pY=%f,sX=%f,sY=%f\n", &_maps->saveMap3.collisions[i].pos.x, &_maps->saveMap3.collisions[i].pos.y, &_maps->saveMap3.collisions[i].size.x, &_maps->saveMap3.collisions[i].size.y);
		_maps->saveMap3.collisions[i].pos.x += X_OFFSET;

		sfRectangleShape_setSize(_maps->saveMap3.collisions[i].rectangle, _maps->saveMap3.collisions[i].size);
		sfRectangleShape_setPosition(_maps->saveMap3.collisions[i].rectangle, _maps->saveMap3.collisions[i].pos);
		_maps->saveMap3.collisions[i].hitBox = sfRectangleShape_getGlobalBounds(_maps->saveMap3.collisions[i].rectangle);
		_maps->saveMap3.collisions[i].elementsNumber = elementsNumber;
	}
	/*chargement des ennemis map 3*/
	fscanf_s(file, "%d\n", &elementsNumber);
	for (int i = 0; i < elementsNumber; i++)
	{
		_maps->saveMap3.ennemis[i].sprite = sfSprite_create();
		_maps->saveMap3.ennemis[i].sprite = createSprite("resources/sprites/m1-1.png");
		fscanf_s(file, "pXS=%d,pX=%f,pY=%f,d=%d,dM=%d\n", &_maps->saveMap3.ennemis[i].xStart, &_maps->saveMap3.ennemis[i].pos.x, &_maps->saveMap3.ennemis[i].pos.y, &_maps->saveMap3.ennemis[i].Direction, &_maps->saveMap3.ennemis[i].distMax);
		_maps->saveMap3.ennemis[i].xStart += X_OFFSET;
		_maps->saveMap3.ennemis[i].pos.x += X_OFFSET;
		_maps->saveMap3.ennemis[i].hitBox = sfSprite_getGlobalBounds(_maps->saveMap3.ennemis[i].sprite);
		_maps->saveMap3.ennemis[i].Origin.x = _maps->saveMap3.ennemis[i].hitBox.width / 2;
		_maps->saveMap3.ennemis[i].Origin.y = _maps->saveMap3.ennemis[i].hitBox.height / 2;
		sfSprite_setOrigin(_maps->saveMap3.ennemis[i].sprite, _maps->saveMap3.ennemis[i].Origin);
		sfSprite_setPosition(_maps->saveMap3.ennemis[i].sprite, _maps->saveMap3.ennemis[i].pos);
		_maps->saveMap3.ennemis[i].elementsNumber = elementsNumber;
	}
	_maps->saveMap3.background.backgroundNumber = 3;
	_maps->saveMap3.background.pos.x = 0 + X_OFFSET;
	_maps->saveMap3.background.pos.y = 0;
	fclose(file);

	//////////////////////////////////////////////////////////////////////////////////
	/*charger la map 4 du level en cours*/
	sprintf_s(path, 50, "resources/datas/m%d-4.txt", _currentLevel);
	fopen_s(&file, path, "r");
	if (file == NULL)
		printf_s("erreur ouvrture fichier plateformes");
	/*chargement des plateformes map 4*/
	fscanf_s(file, "%d\n", &elementsNumber);
	for (int i = 0; i < elementsNumber; i++)
	{
		_maps->saveMap4.collisions[i].rectangle = sfRectangleShape_create();
		fscanf_s(file, "pX=%f,pY=%f,sX=%f,sY=%f\n", &_maps->saveMap4.collisions[i].pos.x, &_maps->saveMap4.collisions[i].pos.y, &_maps->saveMap4.collisions[i].size.x, &_maps->saveMap4.collisions[i].size.y);
		_maps->saveMap4.collisions[i].pos.x += X_OFFSET;

		sfRectangleShape_setSize(_maps->saveMap4.collisions[i].rectangle, _maps->saveMap4.collisions[i].size);
		sfRectangleShape_setPosition(_maps->saveMap4.collisions[i].rectangle, _maps->saveMap4.collisions[i].pos);
		_maps->saveMap4.collisions[i].hitBox = sfRectangleShape_getGlobalBounds(_maps->saveMap4.collisions[i].rectangle);
		_maps->saveMap4.collisions[i].elementsNumber = elementsNumber;
	}
	/*chargement des ennemis map 4*/
	fscanf_s(file, "%d\n", &elementsNumber);
	for (int i = 0; i < elementsNumber; i++)
	{
		_maps->saveMap4.ennemis[i].sprite = sfSprite_create();
		_maps->saveMap4.ennemis[i].sprite = createSprite("resources/sprites/m1-1.png");
		fscanf_s(file, "pXS=%d,pX=%f,pY=%f,d=%d,dM=%d\n", &_maps->saveMap4.ennemis[i].xStart, &_maps->saveMap4.ennemis[i].pos.x, &_maps->saveMap4.ennemis[i].pos.y, &_maps->saveMap4.ennemis[i].Direction, &_maps->saveMap4.ennemis[i].distMax);
		_maps->saveMap4.ennemis[i].xStart += X_OFFSET;
		_maps->saveMap4.ennemis[i].pos.x += X_OFFSET;
		_maps->saveMap4.ennemis[i].hitBox = sfSprite_getGlobalBounds(_maps->saveMap4.ennemis[i].sprite);
		_maps->saveMap4.ennemis[i].Origin.x = _maps->saveMap4.ennemis[i].hitBox.width / 2;
		_maps->saveMap4.ennemis[i].Origin.y = _maps->saveMap4.ennemis[i].hitBox.height / 2;
		sfSprite_setOrigin(_maps->saveMap4.ennemis[i].sprite, _maps->saveMap4.ennemis[i].Origin);
		sfSprite_setPosition(_maps->saveMap4.ennemis[i].sprite, _maps->saveMap4.ennemis[i].pos);
		_maps->saveMap4.ennemis[i].elementsNumber = elementsNumber;
	}
	_maps->saveMap4.background.backgroundNumber = 4;
	_maps->saveMap4.background.pos.x = 0 + X_OFFSET;
	_maps->saveMap4.background.pos.y = 0;
	fclose(file);

	_maps->currentMap = _maps->saveMap1;
	_maps->nextMap = _maps->saveMap2;
	
}

void nextMapYOffset(t_maps* _maps, float _velocityOffset)
{
	for (int i = 0; i < _maps->nextMap.collisions[0].elementsNumber; i++)
	{
		_maps->nextMap.collisions[i].pos.y += MAP_HEIGHT - _velocityOffset;
	}
	for (int i = 0; i < _maps->nextMap.ennemis[0].elementsNumber; i++)
	{
		_maps->nextMap.ennemis[i].pos.y += MAP_HEIGHT - _velocityOffset;
	}
	_maps->nextMap.background.pos.y += MAP_HEIGHT - _velocityOffset;
}

void moveMaps(t_maps* _maps, float _velocityOffset)
{
	/*Deplacements*/
	for (int i = 0; i < _maps->currentMap.collisions[0].elementsNumber; i++)
	{
		_maps->currentMap.collisions[i].pos.y -= _velocityOffset;
	}
	for (int i = 0; i < _maps->currentMap.ennemis[0].elementsNumber; i++)
	{
		_maps->currentMap.ennemis[i].pos.y -= _velocityOffset;
	}

	for (int i = 0; i < _maps->nextMap.collisions[0].elementsNumber; i++)
	{
		_maps->nextMap.collisions[i].pos.y -= _velocityOffset;
	}
	for (int i = 0; i < _maps->nextMap.ennemis[0].elementsNumber; i++)
	{
		_maps->nextMap.ennemis[i].pos.y -= _velocityOffset;
	}
	_maps->currentMap.background.pos.y -= _velocityOffset;
	_maps->nextMap.background.pos.y -= _velocityOffset;


	/*chargement de la map suivante*/
	if (_maps->currentMap.background.pos.y <= -MAP_HEIGHT)
	{
		_maps->currentMap = _maps->nextMap;
		int randomMapNb = rand() % (49 - 10) + 10;
		while ((randomMapNb / 10) == _maps->currentMap.background.backgroundNumber)
		{
			randomMapNb = rand() % (49 - 10) + 10;
		}
		randomMapNb = randomMapNb / 10;
		switch (randomMapNb)
		{
			case 1 :
				_maps->nextMap = _maps->saveMap1;
				break;
			case 2:
				_maps->nextMap = _maps->saveMap2;
				break;
			case 3:
				_maps->nextMap = _maps->saveMap3;
				break;
			case 4:
				_maps->nextMap = _maps->saveMap4;
				break;
		}
		printf_s("next map :%d\n",_maps->nextMap.background.backgroundNumber);
		nextMapYOffset(_maps, _velocityOffset);
	}

}

void displayMaps(t_maps* _maps, sfRenderWindow *window)
{
	sfSprite_setPosition(_maps->currentMap.background.sprite, _maps->currentMap.background.pos);
	sfRenderWindow_drawSprite(window, _maps->currentMap.background.sprite, NULL);
	sfSprite_setPosition(_maps->nextMap.background.sprite, _maps->nextMap.background.pos);
	sfRenderWindow_drawSprite(window, _maps->nextMap.background.sprite, NULL);

	for (int i = 0; i < _maps->currentMap.collisions[0].elementsNumber; i++)
	{
		//printf_s("rectangle pos y :%.2f\n", _maps->currentMap.collisions[i].pos.y);
		sfRectangleShape_setPosition(_maps->currentMap.collisions[i].rectangle, _maps->currentMap.collisions[i].pos);
		sfRenderWindow_drawRectangleShape(window, _maps->currentMap.collisions[i].rectangle, NULL);
	}
	for (int i = 0; i < _maps->currentMap.ennemis[0].elementsNumber; i++)
	{
		//printf_s("poney pos y :%.2f\n", _maps->currentMap.ennemis[i].pos.y);
		sfSprite_setPosition(_maps->currentMap.ennemis[i].sprite, _maps->currentMap.ennemis[i].pos);
		sfRenderWindow_drawSprite(window, _maps->currentMap.ennemis[i].sprite, NULL);
	}

	for (int i = 0; i < _maps->nextMap.collisions[0].elementsNumber; i++)
	{
		sfRectangleShape_setPosition(_maps->nextMap.collisions[i].rectangle, _maps->nextMap.collisions[i].pos);
		sfRenderWindow_drawRectangleShape(window, _maps->nextMap.collisions[i].rectangle, NULL);
	}
	for (int i = 0; i < _maps->nextMap.ennemis[0].elementsNumber; i++)
	{
		sfSprite_setPosition(_maps->nextMap.ennemis[i].sprite, _maps->nextMap.ennemis[i].pos);
		sfRenderWindow_drawSprite(window, _maps->nextMap.ennemis[i].sprite, NULL);
	}
}