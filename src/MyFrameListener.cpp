#include <iostream>
#include "MyFrameListener.h"

MyFrameListener::MyFrameListener(Ogre::RenderWindow *win,	// necesario para obtener el handle de ventana dl S.O.
								 Ogre::SceneManager *sm,	// para nuestro OverlayManager
								 Ogre::OverlayManager *om,	// para nuestro SceneManager
								 Ogre::Camera *cam			// para nuestro SceneManager
								 ) : _window(win), _sceneManager(sm), _overlayManager(om), _camera(cam)
{
	OIS::ParamList param;			// Lista de parametros necesaria para OIS
	size_t windowHandle;			// Para recoger el handle de ventana del S.O.
	std::ostringstream wHandleStr;	// Para pasar el handle de ventana a formato STRING. Asi lo necesita OIS

	_window = win;
	_sceneManager = sm;
	_camera = cam;
	_window->getCustomAttribute("WINDOW", &windowHandle);			// Recogemos del S.O. el handle de la ventana de renderizado...
	wHandleStr << windowHandle;									// Lo pasamos a cadena...
	param.insert(std::make_pair("WINDOW", wHandleStr.str()));	// Insertamos en parametros de OIS, la dupla: "WINDOW","HandleVentana"

	_inputManager = OIS::InputManager::createInputSystem(param);// Creamos InpuntManager con los parametros recogidos
	_keyboard = static_cast<OIS::Keyboard*>	(_inputManager->createInputObject(OIS::OISKeyboard, false)); // Creamos Objeto Input de tipo Teclado
	_mouse = static_cast<OIS::Mouse*>	(_inputManager->createInputObject(OIS::OISMouse, false)); // Creamos Objeto Input de tipo Mouse

	// Creamos nuestra query de rayos
	_raySceneQuery = _sceneManager->createRayQuery(Ogre::Ray());

	// establecemos las dimensiones maximas por donde se movera el raton
	_mouse->getMouseState().width = _window->getWidth();
	_mouse->getMouseState().height = _window->getHeight();
}

MyFrameListener::~MyFrameListener()
{
	_inputManager->destroyInputObject(_keyboard);
	_inputManager->destroyInputObject(_mouse);
	_sceneManager->destroyQuery(_raySceneQuery);
	OIS::InputManager::destroyInputSystem(_inputManager);
}

bool MyFrameListener::frameStarted(const Ogre::FrameEvent& evt)
{
	// Capturamos eventos
	_keyboard->capture();
	_mouse->capture();

	// posiciones del puntero del raton en pixeles
	int posx = _mouse->getMouseState().X.abs;
	int posy = _mouse->getMouseState().Y.abs;

	if(_keyboard->isKeyDown(OIS::KC_ESCAPE)) return false;	// Si es ESC.. devolviendo false salimos

	// Botones del raton pulsados?
	bool mbleft = _mouse->getMouseState().buttonDown(OIS::MB_Left);
//	bool mbmiddle = _mouse->getMouseState().buttonDown(OIS::MB_Middle);
//	bool mbright = _mouse->getMouseState().buttonDown(OIS::MB_Right);

	if(mbleft)
	{
		std::cout << "holaaa izquierdooo: X: " << posx << " Y: " << posy << std::endl;
		uint32 mask = PLAYER_CELLS | CPU_CELLS;
		Ogre::Ray r = setRayQuery(posx, posy, mask);
	}

	// Gestion del overlay -----------------------------
	Ogre::OverlayElement *oe;
	oe = _overlayManager->getOverlayElement("cursor");
	oe->setLeft(posx); oe->setTop(posy);

	std::ostringstream s_posMouse;
	s_posMouse << posx;
	oe = _overlayManager->getOverlayElement("PosXMouse");
	oe->setCaption(s_posMouse.str());

	s_posMouse.str("");
	s_posMouse << posy;
	oe = _overlayManager->getOverlayElement("PosYMouse");
	oe->setCaption(s_posMouse.str());

	return true;
}

Ogre::Ray MyFrameListener::setRayQuery(int posx, int posy, uint32 mask)
{
	Ogre::Ray rayMouse = _camera->getCameraToViewportRay(posx/float(_window->getWidth()),
												   	     posy/float(_window->getHeight()));
	_raySceneQuery->setRay(rayMouse);
	_raySceneQuery->setSortByDistance(true);
	_raySceneQuery->setQueryMask(mask);
	return (rayMouse);


}
