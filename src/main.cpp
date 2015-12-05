#define UNUSED_VARIABLE(x) (void)x

#include "GameManager.h"
#include "IntroState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "PauseState.h"

#include <iostream>

using namespace std;

int main ()
{
	GameManager* game 		= new GameManager();
	IntroState* introState 	= new IntroState();
	MenuState* menuState 	= new MenuState();
	PlayState* playState	= new PlayState();
	PauseState* pauseState 	= new PauseState();

	UNUSED_VARIABLE(introState);
	UNUSED_VARIABLE(menuState);
	UNUSED_VARIABLE(playState);
	UNUSED_VARIABLE(pauseState);

	try
	{
		// Inicializa el juego y transición al primer estado.
		game->start(IntroState::getSingletonPtr());
	}
	catch (Ogre::Exception& e)
	{
		std::cerr << "Excepción detectada: " << e.getFullDescription();
	}

	delete game;

	return 0;
}
