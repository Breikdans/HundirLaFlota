#include "EndGameState.h"

template<> EndGameState* Ogre::Singleton<EndGameState>::msSingleton = 0;

void EndGameState::enter ()
{
	_root = Ogre::Root::getSingletonPtr();

	// Se recupera el gestor de escena y la cámara.
	_sceneMgr 	= _root->getSceneManager("SceneManager");
	_camera 	= _sceneMgr->getCamera("IntroCamera");
	_viewport 	= _root->getAutoCreatedWindow()->getViewport(0);
	// Nuevo background colour.
	_viewport->setBackgroundColour(Ogre::ColourValue(0.0, 1.0, 0.0));

	_exitGame = false;
	showEndMsgCegui();
}

void EndGameState::exit () {}

void EndGameState::pause () {}

void EndGameState::resume () {}

bool EndGameState::frameStarted(const Ogre::FrameEvent& evt)
{
	return true;
}

bool EndGameState::frameEnded(const Ogre::FrameEvent& evt)
{
	if (_exitGame)
		return false;

	return true;
}

void EndGameState::keyPressed(const OIS::KeyEvent &e)
{
	// Tecla p --> Estado anterior.
	if (e.key == OIS::KC_P)
	{
		popState();
	}
}

void EndGameState::keyReleased(const OIS::KeyEvent &e) {}

void EndGameState::mouseMoved(const OIS::MouseEvent &e) {}

void EndGameState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id) {}

void EndGameState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) {}

EndGameState* EndGameState::getSingletonPtr ()
{
	return msSingleton;
}

EndGameState& EndGameState::getSingleton ()
{ 
	assert(msSingleton);
	return *msSingleton;
}

void EndGameState::showEndMsgCegui()
{
	//Sheet
	CEGUI::Window* _ceguiSheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","menu_principal");


	//Config Window
	CEGUI::Window* menuWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("menu_principal.layout");

menuWin->getChild("Text1")->setText("Este color es [colour='FFFF0000'] AZUL, mientras que [colour='FF00FF00'] este es ROJO [colour='FF0000FF'] y este VERDE!");

	// NEW GAME
	CEGUI::Window* newGameButton = menuWin->getChild("btn_new_game");
	newGameButton->subscribeEvent( CEGUI::PushButton::EventClicked,
							   	   CEGUI::Event::Subscriber(&MenuState::newGame, this));
	// RECORDS
	CEGUI::Window* recordsButton = menuWin->getChild("btn_records");
	recordsButton->subscribeEvent( CEGUI::PushButton::EventClicked,
							   	   CEGUI::Event::Subscriber(&MenuState::records, this));
	// CREDITS
	CEGUI::Window* creditsButton = menuWin->getChild("btn_credits");
	creditsButton->subscribeEvent( CEGUI::PushButton::EventClicked,
							   	   CEGUI::Event::Subscriber(&MenuState::credits, this));
	// QUIT
	CEGUI::Window* exitButton = menuWin->getChild("btn_quit");
	exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
							   CEGUI::Event::Subscriber(&MenuState::quit, this));

	//Attaching buttons
	_ceguiSheet->addChild(menuWin);
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_ceguiSheet);


}
