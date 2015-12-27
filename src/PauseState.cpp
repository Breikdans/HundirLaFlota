#include "IntroState.h"
#include "PauseState.h"

template<> PauseState* Ogre::Singleton<PauseState>::msSingleton = 0;

void PauseState::enter ()
{
	_root = Ogre::Root::getSingletonPtr();

	// Se recupera el gestor de escena y la cámara.
	_sceneMgr = _root->getSceneManager("SceneManager");
	_camera = _sceneMgr->getCamera("IntroCamera");
	_viewport = _root->getAutoCreatedWindow()->getViewport(0);

	// musica del menu
	IntroState::getSingleton().getMenuTrackPtr()->play();

	_overlayManager = Ogre::OverlayManager::getSingletonPtr();
	Ogre::Overlay *overlay_player = _overlayManager->getByName("panel_pause");
	overlay_player->show();


	_exitGame = false;
}

void PauseState::exit ()
{
	// paramos musica del menu
	IntroState::getSingleton().getMenuTrackPtr()->stop();

	Ogre::Overlay *overlay_player = _overlayManager->getByName("panel_pause");
	overlay_player->hide();
}

void PauseState::pause () {}

void PauseState::resume () {}

bool PauseState::frameStarted(const Ogre::FrameEvent& evt)
{
	return true;
}

bool PauseState::frameEnded(const Ogre::FrameEvent& evt)
{
	if (_exitGame)
		return false;

	return true;
}

void PauseState::keyPressed(const OIS::KeyEvent &e)
{
	// Tecla p --> Estado anterior.
	if (e.key == OIS::KC_P)
	{
		popState();
	}
}

void PauseState::keyReleased(const OIS::KeyEvent &e) {}

void PauseState::mouseMoved(const OIS::MouseEvent &e)
{
	// Gestion del overlay (CURSOR)-----------------------------
	// posiciones del puntero del raton en pixeles
	int posx = e.state.X.abs;
	int posy = e.state.Y.abs;

	Ogre::OverlayElement *oe;
	oe = _overlayManager->getOverlayElement("cursor");
	oe->setLeft(posx); oe->setTop(posy);
}

void PauseState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id) {}

void PauseState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) {}

PauseState* PauseState::getSingletonPtr ()
{
	return msSingleton;
}

PauseState& PauseState::getSingleton ()
{ 
	assert(msSingleton);
	return *msSingleton;
}

