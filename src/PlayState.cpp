#include "PlayState.h"
#include "PauseState.h"

template<> PlayState* Ogre::Singleton<PlayState>::msSingleton = 0;

void PlayState::enter ()
{
	_root = Ogre::Root::getSingletonPtr();

	// Se recupera el gestor de escena y la cámara.
	_sceneMgr 		= _root->getSceneManager("SceneManager");
	_camera 		= _sceneMgr->getCamera("IntroCamera");
	_renderWindow 	= _root->getAutoCreatedWindow();
	_viewport 		= _renderWindow->addViewport(_camera);

	// Metemos una luz ambiental, una luz que no tiene fuente de origen. Ilumina a todos los objetos
	_sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));

	_sceneMgr->addRenderQueueListener(new Ogre::OverlaySystem());	// consulta de rayos

	_camera->setPosition(Ogre::Vector3(0, 50, (MAX_ROWS_GRID*CELL_WIDTH) * 1.7));	// posicionamos...
	_camera->lookAt(Ogre::Vector3(0, 0, (MAX_ROWS_GRID*CELL_WIDTH) / 2));			// enfocamos a 0,0,0
	_camera->setNearClipDistance(5);		// establecemos plano cercano del frustum
	_camera->setFarClipDistance(300);		// establecemos plano lejano del frustum

	// Creamos el plano de imagen (lienzo) asociado a la camara
	_viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,0.0));	// color de fondo del viewport(negro)
	double width = _viewport->getActualWidth();		// recogemos ancho del viewport actual
	double height = _viewport->getActualHeight();	// recogemos alto del viewport actual
	_camera->setAspectRatio(width / height);		// calculamos ratio (4:3 = 1,333 16:9 1,777)

	loadResources();
	createScene();		// creamos la escena
	createOverlay();	// creamos el overlay

	// Creamos nuestra query de rayos
	_raySceneQuery = _sceneMgr->createRayQuery(Ogre::Ray());

	_exitGame = false;
}

void PlayState::exit ()
{
	_sceneMgr->destroyQuery(_raySceneQuery);
	_sceneMgr->clearScene();
	_root->getAutoCreatedWindow()->removeAllViewports();
}

void PlayState::pause() {}

void PlayState::resume()
{
	// Se restaura el background colour.
	_viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 1.0));
}

bool PlayState::frameStarted(const Ogre::FrameEvent& evt)
{
	return true;
}

bool PlayState::frameEnded(const Ogre::FrameEvent& evt)
{
	if (_exitGame)
		return false;

	return true;
}

void PlayState::keyPressed(const OIS::KeyEvent &e)
{
	// Tecla p --> PauseState.
	if (e.key == OIS::KC_P)
	{
		pushState(PauseState::getSingletonPtr());
	}
}

void PlayState::keyReleased(const OIS::KeyEvent &e)
{
	if (e.key == OIS::KC_ESCAPE)
	{
		_exitGame = true;
	}
}

void PlayState::mouseMoved(const OIS::MouseEvent &e)
{
	// guardamos el nombre del ultimo nodo seleccionado, para devolverle al estado normal
	static std::string s_LastCell= "";

	// posiciones del puntero del raton en pixeles
	int posx = e.state.X.abs;
	int posy = e.state.Y.abs;

	// Botones del raton pulsados?
//	bool mbleft = _mouse->getMouseState().buttonDown(OIS::MB_Left);
//	bool mbmiddle = _mouse->getMouseState().buttonDown(OIS::MB_Middle);
//	bool mbright = _mouse->getMouseState().buttonDown(OIS::MB_Right);

	std::string s_CellName;
	Ogre::SceneNode *node = NULL;
	Ogre::Entity *pEnt = NULL;
	std::string s_Material = "";

	uint32 mask = PLAYER_CELLS | CPU_CELLS;			// mascara para la query...
	setRayQuery(posx, posy, mask);	// establecemos query...
	Ogre::RaySceneQueryResult &result = _raySceneQuery->execute();
	Ogre::RaySceneQueryResult::iterator it;
	it = result.begin();
	if (it != result.end())
	{
		s_CellName = it->movable->getParentSceneNode()->getName();	// cogemos el nombre del nodo seleccionado con el rayo

		// si habia una celda seleccionada... y es distinta a la actual... la dejamos con color NORMAL
		if(s_LastCell.size() != 0 && s_LastCell != s_CellName)
		{
			node = _sceneMgr->getSceneNode(s_LastCell);
			pEnt = static_cast <Ogre::Entity *> (node->getAttachedObject(s_LastCell));

			s_Material = "celda";
			// cambiamos la textura del objeto a NORMAL
			if (pEnt)
				pEnt->setMaterialName(s_Material);
		}

		node = _sceneMgr->getSceneNode(s_CellName);
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
			node = _sceneMgr->getSceneNode(s_LastCell);
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
}

void PlayState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	std::cout << "SU PRIMA!!" << std::endl;
}

void PlayState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) {}

PlayState* PlayState::getSingletonPtr ()
{
	return msSingleton;
}

PlayState& PlayState::getSingleton ()
{ 
	assert(msSingleton);
	return *msSingleton;
}

void PlayState::createScene()
{
	const int ESPACIO_ENTRE_TABLEROS = 2;
	// creacion punteros a entidades para cargar los .mesh de las celdas
	Ogre::Entity *ent_CeldaCPU;
	Ogre::Entity *ent_CeldaPlayer;

	// creacion de nodos para cargar las entidades
	Ogre::SceneNode *node_CPU;
	Ogre::SceneNode *node_Player;

	// creamos nodos de escena para tablero de CPU y tablero de Player
	Ogre::SceneNode* main_node_tablero_CPU = _sceneMgr->createSceneNode("tablero_CPU");
	Ogre::SceneNode* main_node_tablero_Player = _sceneMgr->createSceneNode("tablero_Player");

	// creamos las entidades y las cargamos en las tablas de punteros
	// enlazamos a los nodos, los objetos de cada tipo
	for (int i = 0; i < MAX_ROWS_GRID ; i++ )
	{
		for (int j = 0; j < MAX_COLS_GRID ; j++ )
		{
			// creamos nodos para el tablero de jugador y atachamos la entidad
			// colgamos de main_node_tablero_Player, todos los nodos del tablero
			std::stringstream s_node_player_aux;
			s_node_player_aux << "node_player_" << i << "_" << j;	// node_player_X_Y

			// creamos nodos para el tablero de CPU y atachamos la entidad
			// colgamos de main_node_tablero_CPU, todos los nodos del tablero
			std::stringstream s_node_cpu_aux;
			s_node_cpu_aux << "node_cpu_" << i << "_" << j;	// node_cpu_X_Y

			// crea entidades 3d
			ent_CeldaPlayer = _sceneMgr->createEntity(s_node_player_aux.str(), "celda.mesh");
			ent_CeldaCPU = _sceneMgr->createEntity(s_node_cpu_aux.str(),"celda.mesh");

			// Establecemos mascaras de busqueda para nuestras querys
			ent_CeldaPlayer->setQueryFlags(PLAYER_CELLS);
			ent_CeldaCPU->setQueryFlags(CPU_CELLS);

			node_Player = _sceneMgr->createSceneNode(s_node_player_aux.str());
			node_Player->attachObject(ent_CeldaPlayer);
			node_Player->translate(j*CELL_WIDTH - (MAX_COLS_GRID * CELL_WIDTH), 0, i*CELL_WIDTH);
			main_node_tablero_Player->addChild(node_Player);
			node_CPU = _sceneMgr->createSceneNode(s_node_cpu_aux.str());
			node_CPU->attachObject(ent_CeldaCPU);
			node_CPU->translate(j*CELL_WIDTH + ESPACIO_ENTRE_TABLEROS, 0, i*CELL_WIDTH);
			main_node_tablero_CPU->addChild(node_CPU);
		}
	}

	// definimos un plano
	Ogre::Plane planoAgua(Ogre::Vector3::UNIT_Y,	// Vector normal del plano (el eje perpendicular)
						  -5);						// estará situado a -5 unidades respecto del vector normal.
					 								// Esta definición se corresponde con un plano infinito (descripción matemática abstracta)
	// creamos el plano:
	Ogre::MeshManager::getSingleton().createPlane("plano_agua",				// nombre malla resultante
												  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, // grupo de mallas
												  planoAgua,				// objeto donde se ha definido el plano
												  300,300,					// ancho y alto del plano en coordenadas del mundo
												  1,1,						// numero de segmentos para definir el plano (1x1)
												  true,						// true indica que los vectores normales se calculan perpendicular al plano
												  1,						// conjunto de coordenadas de texturas (por defecto 1)
												  70,70,					// numero de replicacion de la textura en horizontal y vertical
												  Ogre::Vector3::UNIT_Z);	// indica la direccion del vector del plano (up)

	Ogre::SceneNode* node_water = _sceneMgr->createSceneNode("node_water");	// creamos nodo de escena para el fondo de agua
	Ogre::Entity* entWater = _sceneMgr->createEntity("fondo_plano_agua", "plano_agua");
	entWater->setMaterialName("Water");			// establecemos el material a usar
	entWater->setQueryFlags(SEA_BACKGROUND);	// Lo identificamos para las Queries...
	node_water->attachObject(entWater);			// adjuntamos al nodo, la entidad

	// Creamos estructura de grafos.....
	// del root cuelga el nodo_water... y de ahi los tableros CPU y Player
	_sceneMgr->getRootSceneNode()->addChild(node_water);
	node_water->addChild(main_node_tablero_CPU);
	node_water->addChild(main_node_tablero_Player);

	CPUGrid.IniciaBarcosAleatorio();
	PlayerGrid.IniciaBarcosAleatorio();


	for(int x = 0; x < MAX_COLS_GRID; x++)
	{
		for(int y = 0; y < MAX_ROWS_GRID; y++)
		{
			std::stringstream nodeNamePlayer;
			std::stringstream nodeNameCPU;
			nodeNamePlayer << "node_player_" << x << "_" << y;	// node_player_X_Y;
			nodeNameCPU << "node_cpu_" << x << "_" << y;	// node_cpu_X_Y;

			ActualizaTablero(node_Player, PlayerGrid(x, y), nodeNamePlayer.str());
			ActualizaTablero(node_CPU, CPUGrid(x, y), nodeNameCPU.str());
		}
	}
}

void PlayState::ActualizaTablero(Ogre::SceneNode* node, usint16 valor, std::string nodeName)
{
	Ogre::Entity* entidad=NULL;
	Ogre::SceneNode* shipNode=NULL;
	Ogre::SceneNode* TableroNode = _sceneMgr->getSceneNode(nodeName);
	std::stringstream shipNodeName;
	shipNodeName << "ship_" << nodeName;
	std::string pieza;
	bool pintarBarco = true;
	bool esHorizontal = false;

	switch(valor)
	{
		case AGUA: pintarBarco=false; break;
		case DISPARADO: pintarBarco=false; break;
		case PROA_H: pieza="proa.mesh";  esHorizontal=true; break;
		case POPA_H: pieza="popa.mesh";  esHorizontal=true; break;
		case PROA_V: pieza="proa.mesh"; break;
		case CUERPO1_V: pieza="cuerpo1.mesh"; break;
		case CUERPO2_V: pieza="cuerpo2.mesh"; break;
		case CUERPO1_H: pieza="cuerpo1.mesh";  esHorizontal=true; break;
		case CUERPO2_H: pieza="cuerpo2.mesh";  esHorizontal=true; break;
		case POPA_V :pieza="popa.mesh";  break;
		case PROA_H_T :pieza="proa.mesh"; esHorizontal=true; break;
		case CUERPO1_H_T :pieza="cuerpo1.mesh"; esHorizontal=true; break;
		case CUERPO2_H_T :pieza="cuerpo2.mesh"; esHorizontal=true; break;
		case POPA_H_T :pieza="popa.mesh"; esHorizontal=true; break;
		case PROA_V_T :pieza="proa.mesh"; break;
		case CUERPO1_V_T :pieza="cuerpo1.mesh"; break;
		case CUERPO2_V_T :pieza="cuerpo2.mesh"; break;
		case POPA_V_T :pieza="popa.mesh"; break;
		case PROA_H_Q :pieza="proa.mesh"; esHorizontal=true; break;
		case CUERPO1_H_Q :pieza="cuerpo1.mesh"; esHorizontal=true; break;
		case CUERPO2_H_Q :pieza="cuerpo2.mesh"; esHorizontal=true; break;
		case POPA_H_Q :pieza="popa.mesh"; esHorizontal=true; break;
		case PROA_V_Q :pieza="proa.mesh"; break;
		case CUERPO1_V_Q :pieza="cuerpo1.mesh"; break;
		case CUERPO2_V_Q :pieza="cuerpo2.mesh"; break;
		case POPA_V_Q :pieza="popa.mesh"; break;
	}

	 if (pintarBarco) {
		entidad = _sceneMgr->createEntity(shipNodeName.str(), pieza);
		shipNode = _sceneMgr->createSceneNode(shipNodeName.str());
		shipNode->attachObject(entidad);
		if (!esHorizontal) {
			shipNode->yaw(Ogre::Degree(90));
		}
		TableroNode->addChild(shipNode);
	 } else {
		 // meter lo que sea
	 }


}

Ogre::Ray PlayState::setRayQuery(int posx, int posy, uint32 mask)
{
	Ogre::Ray rayMouse = _camera->getCameraToViewportRay(posx/float(_renderWindow->getWidth()),
												   	     posy/float(_renderWindow->getHeight()));
	_raySceneQuery->setRay(rayMouse);
	_raySceneQuery->setSortByDistance(true);
	_raySceneQuery->setQueryMask(mask);
	return (rayMouse);
}

void PlayState::createOverlay()
{
	_overlayManager = Ogre::OverlayManager::getSingletonPtr();
	Ogre::Overlay *overlay = _overlayManager->getByName("Info");
	overlay->show();
}

void PlayState::loadResources() {
Ogre::ConfigFile cf;
	cf.load("resources.cfg");	// cargamos el fichero de recursos

//	ejemplo de resources.cfg:
//
//	[General]
//	FileSystem=media
//  Zip=media/celda.zip

	Ogre::ConfigFile::SectionIterator sI = cf.getSectionIterator();	// Iterador de SECCIONES []
	Ogre::String sectionstr;	// Guardara toda la cadena a parsear
	Ogre::String typestr;		// Guardara la Clave
	Ogre::String datastr;		// Guardara el Valor

	while (sI.hasMoreElements())// mientras haya secciones...
	{
		sectionstr = sI.peekNextKey();		// Cogemos nombre de la SECCION, sin avanzar ([General])
		Ogre::ConfigFile::SettingsMultiMap *settings = sI.getNext();// Lee linea para parsear
		Ogre::ConfigFile::SettingsMultiMap::iterator i;				// Iterador para Clave-Valor
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			// cogemos Clave en typestr....Valor en datastr
			typestr = i->first;    datastr = i->second;
			// añadimos la localizacion del recurso, al gestor de recursos...
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation( datastr,		// Clave
																			typestr,		// Valor
																			sectionstr);	// Seccion
		}
	}

	// Decimos al gestor de recursos, que inicialice todos
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

}


