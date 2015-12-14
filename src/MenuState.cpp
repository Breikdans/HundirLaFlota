#include "MenuState.h"
#include "PlayState.h"
#include "CreditsState.h"
#include "RecordsState.h"

template<> MenuState* Ogre::Singleton<MenuState>::msSingleton = 0;

void MenuState::enter ()
{
	_root = Ogre::Root::getSingletonPtr();

	// Se recupera el gestor de escena y la cámara.
	_sceneMgr 		= _root->getSceneManager("SceneManager");
	_camera 		= _sceneMgr->getCamera("IntroCamera");
	_renderWindow 	= _root->getAutoCreatedWindow();
	_viewport 		= _renderWindow->addViewport(_camera);

	// Metemos una luz ambiental, una luz que no tiene fuente de origen. Ilumina a todos los objetos
	_sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));

	_camera->setPosition(Ogre::Vector3(0, 50, (MAX_ROWS_GRID*CELL_WIDTH) * 2.3));	// posicionamos...
	_camera->lookAt(Ogre::Vector3(0, 0, (MAX_ROWS_GRID*CELL_WIDTH) / 2));			// enfocamos a 0,0,0
	_camera->setNearClipDistance(5);		// establecemos plano cercano del frustum
	_camera->setFarClipDistance(300);		// establecemos plano lejano del frustum

	// Creamos el plano de imagen (lienzo) asociado a la camara
	_viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,0.0));	// color de fondo del viewport(negro)
	double width = _viewport->getActualWidth();		// recogemos ancho del viewport actual
	double height = _viewport->getActualHeight();	// recogemos alto del viewport actual
	_camera->setAspectRatio(width / height);		// calculamos ratio (4:3 = 1,333 16:9 1,777)

	_overlayManager = Ogre::OverlayManager::getSingletonPtr();
	createOverlay();
	showMenuCegui();

	_exitGame = false;
}


void MenuState::createOverlay() {

		unsigned int width, height, depth;
		int left, top;

		Ogre::Overlay *overlay = _overlayManager->getByName("Menu");
		_root->getAutoCreatedWindow()->getMetrics(width, height, depth, left, top);

		overlay->setScale(((float(width) / 100) / 1024) * 100, ((float(height) / 100) / 768) * 100);
		overlay->show();

}

void MenuState::exit ()
{
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().hide();
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->hide();
	Ogre::Overlay *overlay = _overlayManager->getByName("Menu");
	overlay->hide();
	_sceneMgr->clearScene();
	_root->getAutoCreatedWindow()->removeAllViewports();
}

void MenuState::pause() {}

void MenuState::resume()
{
	showMenuCegui();
}

bool MenuState::frameStarted(const Ogre::FrameEvent& evt)
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectTimePulse(evt.timeSinceLastFrame);

	return true;
}

bool MenuState::frameEnded(const Ogre::FrameEvent& evt)
{
	if (_exitGame)
	{
		exit();
		return false;
	}

	return true;
}

void MenuState::keyPressed(const OIS::KeyEvent &e)
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(e.key));
	CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text);
}

void MenuState::keyReleased(const OIS::KeyEvent &e)
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(e.key));

	if (e.key == OIS::KC_ESCAPE)
	{
		_exitGame = true;
	}

}

void MenuState::mouseMoved(const OIS::MouseEvent &e)
{
//	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(e.state.X.rel, e.state.Y.rel);

	// para igualar punteros de raton en posicion 0,0
	CEGUI::Vector2f mousePos = CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setPosition(CEGUI::Vector2f(e.state.X.abs,e.state.Y.abs));
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(mousePos.d_x/float(e.state.width), mousePos.d_y/float(e.state.height));
}

void MenuState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	std::cout << __FILE__ << ": " << __func__ << ": " "SU PRIMA!! Boton: " << id << std::endl;
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
}

void MenuState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
}

MenuState* MenuState::getSingletonPtr ()
{
	return msSingleton;
}

MenuState& MenuState::getSingleton ()
{
	assert(msSingleton);
	return *msSingleton;
}

CEGUI::MouseButton MenuState::convertMouseButton(OIS::MouseButtonID id)
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

void MenuState::showMenuCegui()
{
	//Sheet
	CEGUI::Window* _ceguiSheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","menu_principal");


	//Config Window
	CEGUI::Window* menuWin = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("menu_principal.layout");

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

bool MenuState::newGame(const CEGUI::EventArgs &e)
{
	std::cout << "NEW GAME" << std::endl;
	changeState(PlayState::getSingletonPtr());
	return true;
}

bool MenuState::records(const CEGUI::EventArgs &e)
{
	std::cout << "RECORDS" << std::endl;
	pushState(RecordsState::getSingletonPtr());
	return true;
}

bool MenuState::credits(const CEGUI::EventArgs &e)
{
	std::cout << "CREDITS" << std::endl;
	pushState(CreditsState::getSingletonPtr());
	return true;
}

bool MenuState::quit(const CEGUI::EventArgs &e)
{
	std::cout << "QUIT" << std::endl;
	_exitGame = true;
	return true;
}

