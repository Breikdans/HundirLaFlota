#include "EndGameState.h"
#include "PlayState.h"
#include "MenuState.h"

template<> EndGameState* Ogre::Singleton<EndGameState>::msSingleton = 0;

void EndGameState::enter ()
{
	_root = Ogre::Root::getSingletonPtr();

	// Se recupera el gestor de escena y la cámara.
	_sceneMgr 	= _root->getSceneManager("SceneManager");
	_camera 	= _sceneMgr->getCamera("IntroCamera");
	_viewport 	= _root->getAutoCreatedWindow()->getViewport(0);

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
	CEGUI::Window* _ceguiSheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","endgame");

	//Config Window
	CEGUI::Window* endMsgWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("endgame.layout");

	endMsgWin->getChild("Text2")->setText("[font='Batang-26']VENCEDOR:  [colour='FFFF0000'] SU PRIMA");
	endMsgWin->getChild("Text3")->setText("[font='DejaVuSans-12'][colour='FFFF0000']Puntos CPU: [colour='FF000000'] ");
	endMsgWin->getChild("Text4")->setText("[font='DejaVuSans-12'][colour='FF0000FF']Puntos PLAYER: [colour='FF000000'] ");

	// OK
	CEGUI::Window* okButton = endMsgWin->getChild("btn_ok");
	okButton->subscribeEvent( CEGUI::PushButton::EventClicked,
							  CEGUI::Event::Subscriber(&EndGameState::BotonOk, this));

	//Attaching buttons
	_ceguiSheet->addChild(endMsgWin);
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_ceguiSheet);

}

bool EndGameState::BotonOk(const CEGUI::EventArgs &e)
{
	std::cout << __func__ << "----OK----" << std::endl;
	changeState(MenuState::getSingletonPtr());
	return true;
}



