#include "MyFrameListener.h"

MyFrameListener::MyFrameListener(Ogre::RenderWindow* win)	// necesario para obtener el handle de ventana dl S.O.
{
	OIS::ParamList param;			// Lista de parametros necesaria para OIS
	size_t windowHandle;			// Para recoger el handle de ventana del S.O.
	std::ostringstream wHandleStr;	// Para pasar el handle de ventana a formato STRING. Asi lo necesita OIS

	win->getCustomAttribute("WINDOW", &windowHandle);			// Recogemos del S.O. el handle de la ventana de renderizado...
	wHandleStr << windowHandle;									// Lo pasamos a cadena...
	param.insert(std::make_pair("WINDOW", wHandleStr.str()));	// Insertamos en parametros de OIS, la dupla: "WINDOW","HandleVentana"

	_inputManager = OIS::InputManager::createInputSystem(param);// Creamos InpuntManager con los parametros recogidos
	_keyboard = static_cast<OIS::Keyboard*>	(_inputManager->createInputObject(OIS::OISKeyboard, false)); // Creamos Objeto Input de tipo Teclado
	_mouse = static_cast<OIS::Mouse*>	(_inputManager->createInputObject(OIS::OISMouse, false)); // Creamos Objeto Input de tipo Mouse
}

MyFrameListener::~MyFrameListener()
{
	_inputManager->destroyInputObject(_keyboard);
	_inputManager->destroyInputObject(_mouse);
	OIS::InputManager::destroyInputSystem(_inputManager);
}

bool MyFrameListener::frameStarted(const Ogre::FrameEvent& evt)
{
	_keyboard->capture();	// Capturamos tecla

	if(_keyboard->isKeyDown(OIS::KC_ESCAPE)) return false;	// Si es ESC.. devolviendo false salimos
	return true;
}
