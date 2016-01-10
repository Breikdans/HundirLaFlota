#include "EndGameState.h"
#include "PlayState.h"
#include "MenuState.h"
#include "RecordsState.h"
#include "IntroState.h"

template<> RecordsState* Ogre::Singleton<RecordsState>::msSingleton = 0;

void RecordsState::enter ()
{
	_root = Ogre::Root::getSingletonPtr();

	// Se recupera el gestor de escena y la cámara.
	_sceneMgr 	= _root->getSceneManager("SceneManager");
	_camera 	= _sceneMgr->getCamera("IntroCamera");
	_viewport 	= _root->getAutoCreatedWindow()->getViewport(0);

	_exitGame = false;

	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().show();
	showRecordsMsgCegui();
}

void RecordsState::exit () {}

void RecordsState::pause () {}

void RecordsState::resume () {}

bool RecordsState::frameStarted(const Ogre::FrameEvent& evt)
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectTimePulse(evt.timeSinceLastFrame);
	return true;
}

bool RecordsState::frameEnded(const Ogre::FrameEvent& evt)
{
	if (_exitGame)
		return false;

	return true;
}

void RecordsState::keyPressed(const OIS::KeyEvent &e)
{
	// Tecla p --> Estado anterior.
	if (e.key == OIS::KC_P)
	{
		popState();
	}
}

void RecordsState::keyReleased(const OIS::KeyEvent &e) {}

//void RecordsState::isKeyDown(OIS::KeyCode key) const {}

void RecordsState::mouseMoved(const OIS::MouseEvent &e)
{
	// para igualar punteros de raton en posicion 0,0
	CEGUI::Vector2f mousePos = CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setPosition(CEGUI::Vector2f(e.state.X.abs,e.state.Y.abs));
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(mousePos.d_x/float(e.state.width), mousePos.d_y/float(e.state.height));
}

void RecordsState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
}

void RecordsState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
}

RecordsState* RecordsState::getSingletonPtr ()
{
	return msSingleton;
}

RecordsState& RecordsState::getSingleton ()
{ 
	assert(msSingleton);
	return *msSingleton;
}

void RecordsState::fillRecordsTable(CEGUI::Window* win)
{
	std::stringstream sLabelName;
	std::stringstream sText;

	std::multimap<unsigned int, std::string>::reverse_iterator rit;

	rit = IntroState::getSingleton().gameRecords.rbegin();
	for (int i = 0; rit != IntroState::getSingleton().gameRecords.rend() && i < MAX_PLAYER_RECORDS; rit++,i++)
	{
		// POSICION
		sLabelName.str(""); sText.str("");
		sLabelName << "lbl_Pos" << i+1;
		sText << "[font='major_shift-18']" << i+1;
		win->getChild(sLabelName.str())->setText(sText.str());

		// NOMBRE
		sLabelName.str(""); sText.str("");
		sLabelName << "lbl_Name" << i+1;
		sText << "[font='major_shift-18']" << (*rit).second;
		win->getChild(sLabelName.str())->setText(sText.str());

		// PUNTOS
		sLabelName.str(""); sText.str("");
		sLabelName << "lbl_Puntos" << i+1;
		sText << "[font='major_shift-18']" << (*rit).first;
		win->getChild(sLabelName.str())->setText(sText.str());
	}
}

void RecordsState::showRecordsMsgCegui()
{
	//Sheet
	CEGUI::Window* _ceguiSheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","records");

	//Config Window
	CEGUI::Window* recordsMsg = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("ceguiRecords.layout");

	fillRecordsTable(recordsMsg);

	// OK
	CEGUI::Window* okButton = recordsMsg->getChild("btn_Volver");
	okButton->subscribeEvent( CEGUI::PushButton::EventClicked,
							  CEGUI::Event::Subscriber(&RecordsState::BotonVolver, this));

	//Attaching buttons
	_ceguiSheet->addChild(recordsMsg);
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_ceguiSheet);

}

bool RecordsState::BotonVolver(const CEGUI::EventArgs &e)
{
	popState();
	return true;
}

CEGUI::MouseButton RecordsState::convertMouseButton(OIS::MouseButtonID id)
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


