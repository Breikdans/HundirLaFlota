#include <iostream>
#include "MyFrameListener.h"
asdasdad
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
	_window->getCustomAttribute("WINDOW", &windowHandle);		// Recogemos del S.O. el handle de la ventana de renderizado...
	wHandleStr << windowHandle;									// Lo pasamos a cadena...
	param.insert(std::make_pair("WINDOW", wHandleStr.str()));	// Insertamos en parametros de OIS, la dupla: "WINDOW","HandleVentana"

#if defined _DEBUG
	// insert the following lines right before calling mInputSystem = OIS::InputManager::createInputSystem( paramList );
	// Usando esta lista de parámetros además de la obligatoria que le indica a OIS la ventana donde trabajar evitaremos que OIS se haga dueño y señor del teclado y el ratón.
	#if defined OIS_WIN32_PLATFORM
		param.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
		param.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		param.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
		param.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
	#elif defined OIS_LINUX_PLATFORM
		param.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
		param.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
		param.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
		param.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
	#endif
#endif

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
	// guardamos el nombre del ultimo nodo seleccionado, para devolverle al estado normal
	static std::string s_LastCell= "";

	// Capturamos eventos
	_keyboard->capture();
	_mouse->capture();

	// posiciones del puntero del raton en pixeles
	int posx = _mouse->getMouseState().X.abs;
	int posy = _mouse->getMouseState().Y.abs;

	if(_keyboard->isKeyDown(OIS::KC_ESCAPE)) return false;	// Si es ESC.. devolviendo false salimos

	// Botones del raton pulsados?
//	bool mbleft = _mouse->getMouseState().buttonDown(OIS::MB_Left);
//	bool mbmiddle = _mouse->getMouseState().buttonDown(OIS::MB_Middle);
//	bool mbright = _mouse->getMouseState().buttonDown(OIS::MB_Right);

	std::string s_CellName;
	Ogre::SceneNode *node = NULL;
	Ogre::Entity *pEnt = NULL;
	std::string s_Material = "";

	uint32 mask = PLAYER_CELLS | CPU_CELLS;			// mascara para la query...
	Ogre::Ray r = setRayQuery(posx, posy, mask);	// establecemos query...
	Ogre::RaySceneQueryResult &result = _raySceneQuery->execute();
	Ogre::RaySceneQueryResult::iterator it;
	it = result.begin();
	if (it != result.end())
	{
		s_CellName = it->movable->getParentSceneNode()->getName();	// cogemos el nombre del nodo seleccionado con el rayo

		// si habia una celda seleccionada... y es distinta a la actual... la dejamos con color NORMAL
		if(s_LastCell.size() != 0 && s_LastCell != s_CellName)
		{
			node = _sceneManager->getSceneNode(s_LastCell);
			pEnt = static_cast <Ogre::Entity *> (node->getAttachedObject(s_LastCell));

			s_Material = "celda";
			// cambiamos la textura del objeto a NORMAL
			if (pEnt)
				pEnt->setMaterialName(s_Material);
		}

		node = _sceneManager->getSceneNode(s_CellName);
		pEnt = static_cast <Ogre::Entity *> (node->getAttachedObject(s_CellName));
		s_Material = "celda_light";
		// cambiamos la textura del objeto a SELECCIONADA
		pEnt->setMaterialName(s_Material);
		s_LastCell = s_CellName;

	}
	else	// si sacamos el cursor de las celdas, dejamos la ultima con color NORMAL
	{
		if(s_LastCell.size())
		{
			node = _sceneManager->getSceneNode(s_LastCell);
			pEnt = static_cast <Ogre::Entity *> (node->getAttachedObject(s_LastCell));
			s_Material = "celda";
			// cambiamos la textura del objeto a oscura
			if (pEnt)
				pEnt->setMaterialName(s_Material);
		}
	}

	// Gestion del overlay -----------------------------
	Ogre::OverlayElement *oe;
	oe = _overlayManager->getOverlayElement("CPUSeleccion");
	oe->setCaption(s_CellName);

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
