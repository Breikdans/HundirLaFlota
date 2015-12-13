#include "EndGameState.h"
#include "PlayState.h"
#include "MenuState.h"
#include "IntroState.h"

template<> EndGameState* Ogre::Singleton<EndGameState>::msSingleton = 0;

void EndGameState::enter ()
{
	_root = Ogre::Root::getSingletonPtr();

	// Se recupera el gestor de escena y la cámara.
	_sceneMgr 	= _root->getSceneManager("SceneManager");
	_camera 	= _sceneMgr->getCamera("IntroCamera");
	_viewport 	= _root->getAutoCreatedWindow()->getViewport(0);

	_exitGame = false;

	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().show();
	showEndMsgCegui();
}

void EndGameState::exit ()
{
//	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().hide();
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->hide();
	_sceneMgr->clearScene();
	_root->getAutoCreatedWindow()->removeAllViewports();
}

void EndGameState::pause () {}

void EndGameState::resume () {}

bool EndGameState::frameStarted(const Ogre::FrameEvent& evt)
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectTimePulse(evt.timeSinceLastFrame);
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

void EndGameState::mouseMoved(const OIS::MouseEvent &e)
{
	// para igualar punteros de raton en posicion 0,0
	CEGUI::Vector2f mousePos = CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setPosition(CEGUI::Vector2f(e.state.X.abs,e.state.Y.abs));
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(mousePos.d_x/float(e.state.width), mousePos.d_y/float(e.state.height));
}

void EndGameState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
}

void EndGameState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
}

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
	int iPuntosCpu = PlayState::getSingleton().getPuntosCPU();
	int iPuntosPlayer = PlayState::getSingleton().getPuntosPlayer();

	std::stringstream sCadena;

	//Sheet
	CEGUI::Window* _ceguiSheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","endgame");

	//Config Window
	CEGUI::Window* endMsgWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("endgame.layout");

	sCadena << "[font='Batang-26']VENCEDOR: ";
	if(iPuntosCpu > iPuntosPlayer)
		sCadena << "[colour='FFFF0000']ENEMIGO";
	else
		sCadena << "[colour='FF0000FF']JUGADOR";
	endMsgWin->getChild("Text2")->setText(sCadena.str());

	sCadena.str("");
	sCadena << "[font='DejaVuSans-12'][colour='FFFF0000']Puntos ENEMIGO: [colour='FFFFFFFF'] " << iPuntosCpu;
	endMsgWin->getChild("Text3")->setText(sCadena.str());

	sCadena.str("");
	sCadena << "[font='DejaVuSans-12'][colour='FF0000FF']Puntos JUGADOR: [colour='FFFFFFFF'] " << iPuntosPlayer;
	endMsgWin->getChild("Text4")->setText(sCadena.str());

	// OK
	CEGUI::Window* okButton = endMsgWin->getChild("btn_ok");
	okButton->subscribeEvent( CEGUI::PushButton::EventClicked,
							  CEGUI::Event::Subscriber(&EndGameState::BotonOk, this));

	//Attaching buttons
	_ceguiSheet->addChild(endMsgWin);
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_ceguiSheet);
}

bool EndGameState::isNewRecord(unsigned int puntos)
{
	bool sw_result = false;

	rit = IntroState::getSingleton().gameRecords.rbegin();
	for (int i = 0; rit != IntroState::getSingleton().gameRecords.rend() && sw_result != true; rit++,i++)
	{
		if (puntos > (*rit).first && i < 10)
			sw_result = true;
	}

	return sw_result;
}

bool EndGameState::BotonOk(const CEGUI::EventArgs &e)
{
std::cout << __func__ << "----OK----" << std::endl;
	int iPuntosCpu = PlayState::getSingleton().getPuntosCPU();
	int iPuntosPlayer = PlayState::getSingleton().getPuntosPlayer();

	if(iPuntosPlayer > iPuntosCpu)
	{
		if(isNewRecord(iPuntosPlayer))
			showEnterRecordName();
	}
	else
	{
		changeState(MenuState::getSingletonPtr());
	}

	return true;
}

void EndGameState::showEnterRecordName()
{
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->hide();

	//Sheet
	CEGUI::Window* _ceguiSheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","newrecord");

	//Config Window
	CEGUI::Window* newRecordWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("ceguiNewRecord.layout");

	// OK
	CEGUI::Window* okButton = newRecordWin->getChild("btn_Aceptar");
	okButton->subscribeEvent( CEGUI::PushButton::EventClicked,
							  CEGUI::Event::Subscriber(&EndGameState::BotonAceptar, this));

	//Attaching buttons
	_ceguiSheet->addChild(newRecordWin);
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_ceguiSheet);

}

bool EndGameState::BotonAceptar(const CEGUI::EventArgs &e)
{
//	std::string sCadena;
//	int Puntos = 0;
//
//	CEGUI::Window* newRecordWin = CEGUI::Window* newRecordWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("ceguiNewRecord.layout");
//
//	sCadena = newRecordWin->getChild("txt_Name")->getText();
//
//	Puntos = PlayState::getSingleton().getPuntosPlayer();
//
//	gameRecords.insert(std::make_pair(Puntos, sCadena));
	changeState(MenuState::getSingletonPtr());
	return true;
}


CEGUI::MouseButton EndGameState::convertMouseButton(OIS::MouseButtonID id)
{
	CEGUI::MouseButton ceguiId;
	switch(id)
	{
		case OIS::MB_Left:
			ceguiId = CEGUI::LeftButton;
			break;
		case OIS::MB_Right:
			ceguiId = CEGUI::RightButton;
			break;
		case OIS::MB_Middle:
			ceguiId = CEGUI::MiddleButton;
			break;
		default:
			ceguiId = CEGUI::LeftButton;
	}
	return ceguiId;
}


