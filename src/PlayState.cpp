#include <cstdio>
#include "MenuState.h"
#include "PlayState.h"
#include "PauseState.h"
#include "EndGameState.h"

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

	//_sceneMgr->addRenderQueueListener(new Ogre::OverlaySystem());	// consulta de rayos

	_camera->setPosition(Ogre::Vector3(0, 50, (MAX_ROWS_GRID*CELL_WIDTH) * 2.3));	// posicionamos...
	_camera->lookAt(Ogre::Vector3(0, 0, (MAX_ROWS_GRID*CELL_WIDTH) / 2));			// enfocamos a 0,0,0
	_camera->setNearClipDistance(5);		// establecemos plano cercano del frustum
	_camera->setFarClipDistance(300);		// establecemos plano lejano del frustum

	// Creamos el plano de imagen (lienzo) asociado a la camara
	_viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,0.0));	// color de fondo del viewport(negro)
	double width = _viewport->getActualWidth();		// recogemos ancho del viewport actual
	double height = _viewport->getActualHeight();	// recogemos alto del viewport actual
	_camera->setAspectRatio(width / height);		// calculamos ratio (4:3 = 1,333 16:9 1,777)

	// inicializamos variables de juego
	puntosPlayer	= 0;
	puntosCPU		= 0;
	PlayerGrid.setCasillasVida(5+4+3+3+2);
	CPUGrid.setCasillasVida(5+4+3+3+2);
	PlayerGrid.clearGrid();
	CPUGrid.clearGrid();

	createScene();		// creamos la escena
	createOverlay();	// creamos el overlay

	// Creamos nuestra query de rayos
	_raySceneQuery = _sceneMgr->createRayQuery(Ogre::Ray());

	updateInfoOverlay();
	CambiarTurno(PLAYER);
	_exitGame 		= false;
}

void PlayState::exit ()
{
	_sceneMgr->destroyQuery(_raySceneQuery);
	// si lo descomentamos se elimina la escena y las particulas del fuego se quedan paradas...
//	_sceneMgr->clearScene();
//	_root->getAutoCreatedWindow()->removeAllViewports();
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
	else if(e.key == OIS::KC_ESCAPE)
	{
		showExitMsgCegui();
	}


#ifdef _DEBUG
	// movimiento de camara luego quitar
	Ogre::Vector3 vt(0,0,0);	Ogre::Real tSpeed = 20.0;
	if(e.key == OIS::KC_UP)		vt+=Ogre::Vector3(0,0,-1);
	if(e.key == OIS::KC_DOWN)	vt+=Ogre::Vector3(0,0,1);
	if(e.key == OIS::KC_LEFT)	vt+=Ogre::Vector3(-1,0,0);
	if(e.key == OIS::KC_RIGHT)	vt+=Ogre::Vector3(1,0,0);
	_camera->moveRelative(vt * 0.1 * tSpeed);
#endif

}

void PlayState::keyReleased(const OIS::KeyEvent &e) {}

void PlayState::mouseMoved(const OIS::MouseEvent &e)
{
	// guardamos el nombre del ultimo nodo seleccionado, para devolverle al estado normal
	static std::string s_LastCell= "";
	static int lastcellx = 0;
	static int lastcelly = 0;

	// posiciones del puntero del raton en pixeles
	int cellx = e.state.X.abs, posx = e.state.X.abs;
	int celly = e.state.Y.abs, posy = e.state.Y.abs;

	std::string s_CellName = "";
	Ogre::SceneNode *node = NULL;
	Ogre::Entity *pEnt = NULL;

	getSelectedNode(CPU_CELLS, cellx, celly, s_CellName);

	if (s_CellName != "")
	{
		// si habia una celda seleccionada... y es distinta a la actual... y no hemos DISPARADO sobre ella....la dejamos con color NORMAL
		if(s_LastCell.size() != 0 && s_LastCell != s_CellName && CPUGrid(lastcellx,lastcelly) != DISPARADO)
		{
			node = _sceneMgr->getSceneNode(s_LastCell);
			pEnt = static_cast <Ogre::Entity *> (node->getAttachedObject(s_LastCell));

			// cambiamos la textura del objeto a NORMAL
			if (pEnt)
				pEnt->setMaterialName(MATERIAL_CELL);
		}

		node = _sceneMgr->getSceneNode(s_CellName);
		pEnt = static_cast <Ogre::Entity *> (node->getAttachedObject(s_CellName));
		// cambiamos la textura del objeto a SELECCIONADA
		pEnt->setMaterialName(MATERIAL_CELL_SELECTED);

		s_LastCell = s_CellName;
		lastcellx = cellx;
		lastcelly = celly;
	}
	else	// si sacamos el cursor de las celdas, dejamos la ultima con color NORMAL
	{
		if(s_LastCell.size())
		{
			node = _sceneMgr->getSceneNode(s_LastCell);
			pEnt = static_cast <Ogre::Entity *> (node->getAttachedObject(s_LastCell));
			// cambiamos la textura del objeto a oscura
			if (pEnt)
				pEnt->setMaterialName(MATERIAL_CELL);
		}
	}

	// Gestion del overlay (CURSOR)-----------------------------
	Ogre::OverlayElement *oe;
	oe = _overlayManager->getOverlayElement("cursor");
	oe->setLeft(posx); oe->setTop(posy);

	CEGUI::Vector2f mousePos = CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setPosition(CEGUI::Vector2f(e.state.X.abs,e.state.Y.abs));
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(mousePos.d_x/float(e.state.width), mousePos.d_y/float(e.state.height));

}

void PlayState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if (id == OIS::MB_Left && _turno == PLAYER)
	{
		// posiciones del puntero del raton en pixeles
		int posx = e.state.X.abs;
		int posy = e.state.Y.abs;

		std::string s_CellName 	= "";

		getSelectedNode(CPU_CELLS, posx, posy, s_CellName);
		if (s_CellName != "")
		{
DEBUG_TRZ(std::cout << "CLICK NODE: " << s_CellName<< " X: " << posx << " Y: " << posy << std::endl;)

			// Todos los disparos producen un cambio: AGUA -> TOCADO ó DISPARADO, menos cuando disparan sobre algo ya disparado (DISPARADO, PROA_H_T, PROA_H_Q,...)
			if (CompruebaDisparo(CPUGrid, posx, posy))	// Si ha habido algun cambio con este disparo...
			{
				ActualizaTablero(posx, posy);			// Actualizamos tablero gráfico, según contenido de posicion del grid ya actualizado.
				CheckHundido(CPUGrid, posx, posy);
				updateInfoOverlay();
DEBUG_TRZ(std::cout << "CPU GRID: ";)
DEBUG_TRZ(CPUGrid.DebugGrid();)
				if(CPUGrid.getCasillasVida() == 0)
				{
					puntosPlayer += (PlayerGrid.getCasillasVida() * 5);
					// FIN DE JUEGO, GANA EL PLAYER
DEBUG_TRZ(std::cout << "PLAYER WIN!!!!!!" << " Puntos: " << puntosPlayer << std::endl;)
					hideOverlay();
					pushState(EndGameState::getSingletonPtr());
					_exitGame = true;
				}
				else
					CambiarTurno(CPU);
			}
			else
			{
				//sonido de error y que tire otra vez
			}

		} else {
			// hay pulsado el mouse fuera del area permitida, tablero usuario, o agua
		}
	}
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
}

void PlayState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
}

void PlayState::CambiarTurno(EN_TURNO turno)
{
	_turno = turno;
	int x = 0, y = 0;

	if(_turno == CPU)
	{
		CalculaDisparoCPU(x, y);
DEBUG_TRZ(std::cout << "CALCULA DISPARO: " << " X: " << x << " Y: " << y << std::endl;)
		if(CompruebaDisparo(PlayerGrid, x, y))
		{
			std::stringstream s_node_player;
			s_node_player << STRING_NODE_PLAYER_ << x << "_" << y;	// node_player_X_Y

			ActualizaTablero(x, y);			// Actualizamos tablero gráfico, según contenido de posicion del grid ya actualizado.
			CheckHundido(PlayerGrid, x, y);
			updateInfoOverlay();
DEBUG_TRZ(std::cout << "PLAYER GRID: ";)
DEBUG_TRZ(PlayerGrid.DebugGrid();)

			if(PlayerGrid.getCasillasVida() == 0)
			{
				puntosCPU += (CPUGrid.getCasillasVida() * 5);
				// FIN DE JUEGO, GANA LA CPU
DEBUG_TRZ(std::cout << "CPU WIN!!!!!!" << " Puntos: " << puntosCPU << std::endl;)
				hideOverlay();
				changeState(EndGameState::getSingletonPtr());
DEBUG_TRZ(_exitGame = true;)
			}
			else
				CambiarTurno(PLAYER);
		}
	}
}

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
	for (int j = 0; j < MAX_ROWS_GRID ; j++ )
	{
		for (int i = 0; i < MAX_COLS_GRID ; i++ )
		{
			// creamos nodos para el tablero de jugador y atachamos la entidad
			// colgamos de main_node_tablero_Player, todos los nodos del tablero
			std::stringstream s_node_player_aux;
			s_node_player_aux << STRING_NODE_PLAYER_ << i << "_" << j;	// node_player_X_Y

			// creamos nodos para el tablero de CPU y atachamos la entidad
			// colgamos de main_node_tablero_CPU, todos los nodos del tablero
			std::stringstream s_node_cpu_aux;
			s_node_cpu_aux << STRING_NODE_CPU_ << i << "_" << j;	// node_cpu_X_Y

			// crea entidades 3d
			ent_CeldaPlayer = _sceneMgr->createEntity(s_node_player_aux.str(), "celda.mesh");
			ent_CeldaCPU = _sceneMgr->createEntity(s_node_cpu_aux.str(),"celda.mesh");

			// Establecemos mascaras de busqueda para nuestras querys
			ent_CeldaPlayer->setQueryFlags(PLAYER_CELLS);
			ent_CeldaCPU->setQueryFlags(CPU_CELLS);

			node_Player = _sceneMgr->createSceneNode(s_node_player_aux.str());
			node_Player->attachObject(ent_CeldaPlayer);
			node_Player->translate(i*CELL_WIDTH - (MAX_COLS_GRID * CELL_WIDTH), 0, j*CELL_WIDTH);
			main_node_tablero_Player->addChild(node_Player);

			node_CPU = _sceneMgr->createSceneNode(s_node_cpu_aux.str());
			node_CPU->attachObject(ent_CeldaCPU);
			node_CPU->translate(i*CELL_WIDTH + ESPACIO_ENTRE_TABLEROS, 0, j*CELL_WIDTH);
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
												  350,200,					// ancho y alto del plano en coordenadas del mundo
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
DEBUG_TRZ(std::cout << std::endl << "CPU:";)
	CPUGrid.IniciaBarcosAleatorio();
DEBUG_TRZ(std::cout << std::endl << "PLAYER:";)
	PlayerGrid.IniciaBarcosAleatorio();


	for(int y = 0; y < MAX_ROWS_GRID; y++)
	{
		for(int x = 0; x < MAX_COLS_GRID; x++)
		{
			std::stringstream nodeNamePlayer;
			std::stringstream nodeNameCPU;
			nodeNamePlayer << STRING_NODE_PLAYER_ << x << "_" << y;	// node_player_X_Y;
			nodeNameCPU << STRING_NODE_CPU_ << x << "_" << y;	// node_cpu_X_Y;


			// Actualizamos tablero PLAYER (se actualiza el contrario al TURNO)
			_turno = CPU; ActualizaTablero(x, y);
// Actualizamos tablero CPU (se actualiza el contrario al TURNO)
//DEBUG_TRZ(_turno = PLAYER; ActualizaTablero(CPUGrid(x, y), nodeNameCPU.str());)
		}
	}
}

void PlayState::ActualizaTablero(usint16 x, usint16 y)
{
	Ogre::Entity* 		entidad	= NULL;
	Ogre::SceneNode* 	shipNode = NULL;
	Ogre::SceneNode* 	TableroNode = NULL;
	std::stringstream 	shipNodeName;
	std::string 		pieza;
	bool 				pintarBarco = true;
	bool 				esHorizontal = false;
	Ogre::SceneNode *	node = NULL;
	Ogre::Entity *		pEnt = NULL;
	bool 				pintarFuego = false;

	usint16 			valor;
	std::string 		nodeName;

	if(_turno == PLAYER)	// tablero CPU
	{
		std::stringstream s_node_stream;
		s_node_stream << STRING_NODE_CPU_ << x << "_" << y;

		valor = CPUGrid(x, y);
		nodeName = s_node_stream.str();
	}
	else if(_turno == CPU)	// tablero PLAYER
	{
		std::stringstream s_node_stream;
		s_node_stream << STRING_NODE_PLAYER_ << x << "_" << y;

		valor = PlayerGrid(x, y);
		nodeName = s_node_stream.str();
	}

	TableroNode = _sceneMgr->getSceneNode(nodeName);

	switch(valor)
	{
		case AGUA: pintarBarco=false; break;
		case DISPARADO:
		{
			node = _sceneMgr->getSceneNode(nodeName);
			pEnt = static_cast <Ogre::Entity *> (node->getAttachedObject(nodeName));
			// cambiamos la textura del objeto a SELECCIONADA
			pEnt->setMaterialName(MATERIAL_CELL_SELECTED);

			pintarBarco=false;
			break;
		}
		case PROA_H: pieza="proa.mesh";  esHorizontal=true; shipNodeName << "ship_" << nodeName; break;
		case CUERPO1_H: pieza="cuerpo1.mesh";  esHorizontal=true; shipNodeName << "ship_" << nodeName;break;
		case CUERPO2_H: pieza="cuerpo2.mesh";  esHorizontal=true; shipNodeName << "ship_" << nodeName;break;
		case POPA_H: pieza="popa.mesh";  esHorizontal=true; shipNodeName << "ship_" << nodeName;break;

		case PROA_V: pieza="proa.mesh"; shipNodeName << "ship_" << nodeName; break;
		case CUERPO1_V: pieza="cuerpo1.mesh"; shipNodeName << "ship_" << nodeName; break;
		case CUERPO2_V: pieza="cuerpo2.mesh"; shipNodeName << "ship_" << nodeName;break;
		case POPA_V :pieza="popa.mesh";  shipNodeName << "ship_" << nodeName;break;

		case PROA_H_T :pieza="proa_t.mesh";  pintarFuego=true; esHorizontal=true; shipNodeName << "shipTocado_" << nodeName; break;
		case CUERPO1_H_T :pieza="cuerpo1_t.mesh";  pintarFuego=true; esHorizontal=true; shipNodeName << "shipTocado_" << nodeName; break;
		case CUERPO2_H_T :pieza="cuerpo2_t.mesh";   pintarFuego=true; esHorizontal=true;shipNodeName << "shipTocado_" << nodeName; break;
		case POPA_H_T :pieza="popa_t.mesh";   pintarFuego=true; esHorizontal=true;shipNodeName << "shipTocado_" << nodeName; break;

		case PROA_V_T :pieza="proa_t.mesh"; pintarFuego=true; shipNodeName << "shipTocado_" << nodeName; break;
		case CUERPO1_V_T :pieza="cuerpo1_t.mesh";  pintarFuego=true; shipNodeName << "shipTocado_" << nodeName;break;
		case CUERPO2_V_T :pieza="cuerpo2_t.mesh";  pintarFuego=true; shipNodeName << "shipTocado_" << nodeName;break;
		case POPA_V_T :pieza="popa_t.mesh";  pintarFuego=true; shipNodeName << "shipTocado_" << nodeName; break;
/*
		case PROA_H_Q :pieza="proa_ardiendo.mesh"; esHorizontal=true; break;
		case CUERPO1_H_Q :pieza="cuerpo1_q.mesh"; esHorizontal=true; break;
		case CUERPO2_H_Q :pieza="cuerpo2_q.mesh"; esHorizontal=true; break;
		case POPA_H_Q :pieza="popa_q.mesh"; esHorizontal=true; break;

		case PROA_V_Q :pieza="proa_ardiendo.mesh"; break;
		case CUERPO1_V_Q :pieza="cuerpo1_q.mesh"; break;
		case CUERPO2_V_Q :pieza="cuerpo2_q.mesh"; break;
		case POPA_V_Q :pieza="popa_q.mesh"; break;
*/
		default:
			pintarBarco = false;
	}

	if (pintarBarco) {

		// si tiene algun hijo entonces es que ya tenia barco sin tocar, y hay que cambiar el modelo por el roto
		if (TableroNode->numChildren()>0)
		{
			Ogre::SceneNode* antiguo = static_cast<Ogre::SceneNode*>(TableroNode->getChild(0));
			antiguo->setVisible(false);
			antiguo->removeAndDestroyAllChildren();
			_sceneMgr->destroySceneNode(antiguo);
		}

		entidad = _sceneMgr->createEntity(shipNodeName.str(), pieza);
		entidad->setQueryFlags(SHIP_CELL);
		shipNode = _sceneMgr->createSceneNode(shipNodeName.str());
		shipNode->attachObject(entidad);
		if (esHorizontal) {
			shipNode->yaw(Ogre::Degree(90));
		}

		if (pintarFuego) {
			shipNodeName << "_Smoke";
			Ogre::ParticleSystem* partSystem = _sceneMgr->createParticleSystem(shipNodeName.str(),"Smoke");
			shipNode->attachObject(partSystem);
		}

		TableroNode->addChild(shipNode);

		 // meter lo que sea
	}
}

void PlayState::sumaPuntos()
{
	if(_turno == CPU)
		puntosCPU+=5;
	else if(_turno == PLAYER)
		puntosPlayer+=5;
}

/// Comprueba que hay en la casilla del disparo, cambia su estado si es necesario, decrementa casillas de vida, cambia indicador de turno
bool PlayState::CompruebaDisparo(Grid& grid, usint16 posx, usint16 posy)
{
	bool sw_casillaCambiada = false;
	usint16 celda = grid(posx, posy);

	switch(celda)
	{
		case AGUA:
			// SACAR SONIDO DE AGUA
			grid(posx, posy) = DISPARADO; sw_casillaCambiada=true;
			break;
		case DISPARADO:
		case PROA_H_T:
		case CUERPO1_H_T:
		case CUERPO2_H_T:
		case POPA_H_T:
		case PROA_V_T:
		case CUERPO1_V_T:
		case CUERPO2_V_T:
		case POPA_V_T:
			// cuando el barco esté totalmente quemado, se hundirá y se convertirán en casillas disparadas
		case PROA_H_Q:
		case CUERPO1_H_Q:
		case CUERPO2_H_Q:
		case POPA_H_Q:
		case PROA_V_Q:
		case CUERPO1_V_Q:
		case CUERPO2_V_Q:
		case POPA_V_Q:
		  	// SACAR SONIDO DE ERROR
			break;

		// el barco esta "intacto"
		case PROA_H:
			grid(posx, posy) = PROA_H_T; sw_casillaCambiada=true; grid.restaCasillas();
			break;
		case CUERPO1_H:
			grid(posx, posy) = CUERPO1_H_T; sw_casillaCambiada=true; grid.restaCasillas();
			break;
		case CUERPO2_H:
			grid(posx, posy) = CUERPO2_H_T; sw_casillaCambiada=true; grid.restaCasillas();
			break;
		case POPA_H:
			grid(posx, posy) = POPA_H_T; sw_casillaCambiada=true; grid.restaCasillas();
			break;

		case PROA_V:
			grid(posx, posy) = PROA_V_T; sw_casillaCambiada=true; grid.restaCasillas();
			break;
		case CUERPO1_V:
			grid(posx, posy) = CUERPO1_V_T; sw_casillaCambiada=true; grid.restaCasillas();
			break;
		case CUERPO2_V:
			grid(posx, posy) = CUERPO2_V_T; sw_casillaCambiada=true; grid.restaCasillas();
			break;
		case POPA_V:
			grid(posx, posy) = POPA_V_T; sw_casillaCambiada=true; grid.restaCasillas();
			break;
	}

	// si han tocado alguna pieza.... (el AGUA no lo queremos)
	if(sw_casillaCambiada == true && celda != AGUA)
		sumaPuntos();

	return sw_casillaCambiada;
}

bool PlayState::obtenerPopaH(Grid& grid, int posXProa, int posYProa, int &posXPopa, int &posYPopa) const
{
	bool sw_result = false;

	for(int x = posXProa; x < MAX_COLS_GRID && sw_result == false; x++)
	{
		if( grid(x, posYProa) == POPA_H_T)
		{
			posXPopa = x;
			posYPopa = posYProa;
			sw_result = true;
		}
	}

DEBUG_TRZ(std::cout << __func__ << " result: " << sw_result << " PopaX: " << posXPopa << " PopaY: " << posYPopa << std::endl;)
	return sw_result;
}

bool PlayState::obtenerProaH(Grid& grid, int posXPopa, int posYPopa, int &posXProa, int &posYProa) const
{
	bool sw_result = false;

	for(int x = posXPopa; x >= 0 && sw_result == false; x--)
	{
		if( grid(x, posYPopa) == PROA_H_T)
		{
			posXProa = x;
			posYProa = posYPopa;
			sw_result = true;
		}
	}

DEBUG_TRZ(std::cout << __func__ << " result: " << sw_result << " PopaX: " << posXPopa << " PopaY: " << posYPopa << std::endl;)
	return sw_result;
}

bool PlayState::obtenerPopaV(Grid& grid, int posXProa, int posYProa, int &posXPopa, int &posYPopa) const
{
	bool sw_result = false;

	for(int y = posYProa; y < MAX_ROWS_GRID && sw_result == false; y++)
	{
		if( grid(posXProa, y) == POPA_V_T)
		{
			posXPopa = posXProa;
			posYPopa = y;
			sw_result = true;
		}
	}

DEBUG_TRZ(std::cout << __func__ << " result: " << sw_result << " PopaX: " << posXPopa << " PopaY: " << posYPopa << std::endl;)
	return sw_result;
}

bool PlayState::obtenerProaV(Grid& grid, int posXPopa, int posYPopa, int &posXProa, int &posYProa) const
{
	bool sw_result = false;

	for(int y = posYPopa; y >= 0 && sw_result == false; y--)
	{
		if( grid(posXPopa, y) == PROA_V_T)
		{
			posXProa = posXPopa;
			posYProa = y;
			sw_result = true;
		}
	}

DEBUG_TRZ(std::cout << __func__ << " result: " << sw_result << " PopaX: " << posXPopa << " PopaY: " << posYPopa << std::endl;)
	return sw_result;
}



bool PlayState::checkHundidoHorizontal(Grid& grid, int posXProa, int posYProa, int posXPopa, int posYPopa) const
{
	bool sw_result = true;

	for(int x = posXProa; x <= posXPopa && sw_result == true; x++)
	{
		if(!esCasillaTocada(grid,x,posYPopa))
			sw_result = false;
	}

DEBUG_TRZ(std::cout << __func__ << " result: " << sw_result << std::endl;)
	return sw_result;
}

bool PlayState::checkHundidoVertical(Grid& grid, int posXProa, int posYProa, int posXPopa, int posYPopa) const
{
	bool sw_result = true;

	for(int y = posYProa; y <= posYPopa && sw_result == true; y++)
	{
		if(!esCasillaTocada(grid,posXProa,y))
			sw_result = false;
	}

DEBUG_TRZ(std::cout << __func__ << " result: " << sw_result << std::endl;)
	return sw_result;
}

void PlayState::rodearDisparadoHorizontal(Grid& grid, int posXProa, int posYProa, int posXPopa, int posYPopa)
{
	// comprobamos si hay que pintar el borde izquierdo
	if(posXProa > 0)
	{
		// centro
		grid(posXProa-1, posYProa) = DISPARADO;
		ActualizaTablero(posXProa-1, posYProa);

		// Hay espacio arriba?
		if(posYProa > 0)
		{
			grid(posXProa-1, posYProa-1) = DISPARADO;
			ActualizaTablero(posXProa-1, posYProa-1);
		}

		// Hay espacio abajo?
		if(posYProa+1 < MAX_ROWS_GRID)
		{
			grid(posXProa-1, posYProa+1) = DISPARADO;
			ActualizaTablero(posXProa-1, posYProa+1);
		}
	}

	// comprobamos si hay que pintar el borde derecho
	if(posXPopa+1 < MAX_COLS_GRID)
	{
		// centro
		grid(posXPopa+1, posYPopa) = DISPARADO;
		ActualizaTablero(posXPopa+1, posYPopa);

		// Hay espacio arriba?
		if(posYPopa > 0)
		{
			grid(posXPopa+1, posYPopa-1) = DISPARADO;
			ActualizaTablero(posXPopa+1, posYPopa-1);
		}

		// Hay espacio abajo?
		if(posYPopa+1 < MAX_ROWS_GRID)
		{
			grid(posXPopa+1, posYPopa+1) = DISPARADO;
			ActualizaTablero(posXPopa+1, posYPopa+1);
		}
	}

	for(int x = posXProa; x <= posXPopa; x++)
	{
		// Hay espacio por arriba?
		if(posYProa > 0)
		{
			grid(x,posYProa-1) = DISPARADO;
			ActualizaTablero(x,posYProa-1);
		}
		// Hay espacio por abajo?
		if(posYProa+1 < MAX_ROWS_GRID)
		{
			grid(x,posYProa+1) = DISPARADO;
			ActualizaTablero(x,posYProa+1);
		}
	}
}

void PlayState::rodearDisparadoVertical(Grid& grid, int posXProa, int posYProa, int posXPopa, int posYPopa)
{
	// comprobamos si hay que pintar el borde superior
	if(posYProa > 0)
	{
		// centro
		grid(posXProa, posYProa-1) = DISPARADO;
		ActualizaTablero(posXProa, posYProa-1);

		// Hay espacio a la izquierda?
		if(posXProa > 0)
		{
			grid(posXProa-1, posYProa-1) = DISPARADO;
			ActualizaTablero(posXProa-1, posYProa-1);
		}

		// Hay espacio a la derecha?
		if(posXProa+1 < MAX_COLS_GRID)
		{
			grid(posXProa+1, posYProa-1) = DISPARADO;
			ActualizaTablero(posXProa+1, posYProa-1);
		}
	}

	// comprobamos si hay que pintar el borde inferior
	if(posYPopa+1 < MAX_ROWS_GRID)
	{
		// centro
		grid(posXPopa, posYPopa+1) = DISPARADO;
		ActualizaTablero(posXPopa, posYPopa+1);

		// Hay espacio a la izquierda?
		if(posXPopa > 0)
		{
			grid(posXPopa-1, posYPopa+1) = DISPARADO;
			ActualizaTablero(posXPopa-1, posYPopa+1);
		}

		// Hay espacio a la derecha?
		if(posXPopa+1 < MAX_COLS_GRID)
		{
			grid(posXPopa+1, posYPopa+1) = DISPARADO;
			ActualizaTablero(posXPopa+1, posYPopa+1);
		}
	}

	for(int y = posYProa; y <= posYPopa; y++)
	{
		// Hay espacio por la izquierda?
		if(posXProa > 0)
		{
			grid(posXProa-1,y) = DISPARADO;
			ActualizaTablero(posXProa-1,y);
		}
		// Hay espacio por abajo?
		if(posXProa+1 < MAX_COLS_GRID)
		{
			grid(posXProa+1,y) = DISPARADO;
			ActualizaTablero(posXProa+1,y);
		}
	}
}

void PlayState::marcarHundidoHorizontal(Grid& grid, int posXProa, int posYProa, int posXPopa, int posYPopa)
{
	std::stringstream s_node;

	// recorrer las columnas, y cambiar estados
	for(int x = posXProa; x <= posXPopa; x++)
	{
		if (grid(x,posYProa) == PROA_H_T) 		grid(x,posYProa) = PROA_H_Q;
		if (grid(x,posYProa) == CUERPO1_H_T) 	grid(x,posYProa) = CUERPO1_H_Q;
		if (grid(x,posYProa) == CUERPO2_H_T) 	grid(x,posYProa) = CUERPO2_H_Q;
		if (grid(x,posYProa) == POPA_H_T) 		grid(x,posYProa) = POPA_H_Q;

		ActualizaTablero(x,posYProa);
	}

	rodearDisparadoHorizontal(grid, posXProa, posYProa, posXPopa, posYPopa);
}

void PlayState::marcarHundidoVertical(Grid& grid, int posXProa, int posYProa, int posXPopa, int posYPopa)
{
	std::stringstream s_node;

	// recorrer las filas, y cambiar estados
	for(int y = posYProa; y <= posYPopa; y++)
	{
		if (grid(posXProa,y) == PROA_V_T) 		grid(posXProa,y) = PROA_V_Q;
		if (grid(posXProa,y) == CUERPO1_V_T) 	grid(posXProa,y) = CUERPO1_V_Q;
		if (grid(posXProa,y) == CUERPO2_V_T) 	grid(posXProa,y) = CUERPO2_V_Q;
		if (grid(posXProa,y) == POPA_V_T) 		grid(posXProa,y) = POPA_V_Q;

		ActualizaTablero(posXProa,y);

	}

	rodearDisparadoVertical(grid, posXProa, posYProa, posXPopa, posYPopa);
}

bool PlayState::CheckHundido(Grid& grid, usint16 posX, usint16 posY)
{
	bool sw_hundido = false;

	int posXProa = 0;
	int posYProa = 0;

	int posXPopa = 0;
	int posYPopa = 0;

	if (grid(posX, posY) == PROA_H_T)
	{
		posXProa = posX; posYProa = posY;
		if(obtenerPopaH(grid, posXProa, posYProa, posXPopa, posYPopa))
		{
			if(checkHundidoHorizontal(grid, posXProa, posYProa, posXPopa, posYPopa))
			{
				marcarHundidoHorizontal(grid, posXProa, posYProa, posXPopa, posYPopa);
				sw_hundido = true;
			}
		}
	}

	else if (grid(posX, posY) == POPA_H_T)
	{
		posXPopa = posX; posYPopa = posY;
		if(obtenerProaH(grid, posXPopa, posYPopa, posXProa, posYProa))
		{
			if(checkHundidoHorizontal(grid, posXProa, posYProa, posXPopa, posYPopa))
			{
				marcarHundidoHorizontal(grid, posXProa, posYProa, posXPopa, posYPopa);
				sw_hundido = true;
			}
		}
	}

	else if (grid(posX, posY) == CUERPO1_H_T || grid(posX, posY) == CUERPO2_H_T)
	{
		if(obtenerProaH(grid, posX, posY, posXProa, posYProa))
		{
			if(obtenerPopaH(grid, posXProa, posYProa, posXPopa, posYPopa))
			{
				if(checkHundidoHorizontal(grid, posXProa, posYProa, posXPopa, posYPopa))
				{
					marcarHundidoHorizontal(grid, posXProa, posYProa, posXPopa, posYPopa);
					sw_hundido = true;
				}
			}
		}
	}

	else if (grid(posX, posY) == PROA_V_T)
	{
		posXProa = posX; posYProa = posY;
		if(obtenerPopaV(grid, posXProa, posYProa, posXPopa, posYPopa))
		{
			if(checkHundidoVertical(grid, posXProa, posYProa, posXPopa, posYPopa))
			{
				marcarHundidoVertical(grid, posXProa, posYProa, posXPopa, posYPopa);
				sw_hundido = true;
			}
		}
	}

	else if (grid(posX, posY) == POPA_V_T)
	{
		posXPopa = posX; posYPopa = posY;
		if(obtenerProaV(grid, posXPopa, posYPopa, posXProa, posYProa))
		{
			if(checkHundidoVertical(grid, posXProa, posYProa, posXPopa, posYPopa))
			{
				marcarHundidoVertical(grid, posXProa, posYProa, posXPopa, posYPopa);
				sw_hundido = true;
			}
		}
	}

	else if (grid(posX, posY) == CUERPO1_V_T || grid(posX, posY) == CUERPO2_V_T)
	{
		if(obtenerProaV(grid, posX, posY, posXProa, posYProa))
		{
			if(obtenerPopaV(grid, posXProa, posYProa, posXPopa, posYPopa))
			{
				if(checkHundidoVertical(grid, posXProa, posYProa, posXPopa, posYPopa))
				{
					marcarHundidoVertical(grid, posXProa, posYProa, posXPopa, posYPopa);
					sw_hundido = true;
				}
			}
		}
	}
	return sw_hundido;
}


///< Devuelve TRUE si hay casilla tocada y las posiciones, devuelve FALSE en caso contrario
bool PlayState::hayCasillaTocada(const Grid& grid, int &posX, int &posY) const
{
	bool sw_result = false;

	for(int x = 0; x < MAX_COLS_GRID && sw_result == false; x++)
	{
		for(int y = 0; y < MAX_ROWS_GRID && sw_result == false; y++)
		{
			if(esCasillaTocada(grid,x,y))
			{
				posX = x; posY = y;
				sw_result = true;
			}
		}
	}

DEBUG_TRZ(std::cout << __func__ << " result: " << sw_result << " X: " << posX << " Y: " << posY << std::endl;)
	return sw_result;
}

bool PlayState::esCasillaTocada(const Grid& grid, int posX, int posY) const
{
	bool sw_result = false;

	if (grid(posX, posY) >= PROA_H_T && grid(posX, posY) <= POPA_V_T)
		sw_result = true;

DEBUG_TRZ(if(sw_result))
DEBUG_TRZ(std::cout << __func__ << " result: " << sw_result << " X: " << posX << " Y: " << posY << std::endl;)
	return sw_result;
}

bool PlayState::BuscarCasillaLibreDerecha(int &posX, int &posY) const
{
	int y = posY;
	bool sw_result = false;

	for(int x = posX; x < MAX_COLS_GRID && PlayerGrid(x, y) != POPA_H_T && sw_result == false; x++)
	{
		if (!esCasillaTocada(PlayerGrid,x,y))
		{
			posX = x; posY = y;
			sw_result = true;
		}
	}
DEBUG_TRZ(std::cout << __func__ << " result: " << sw_result << " X: " << posX << " Y: " << posY << std::endl;)

	return sw_result;
}

bool PlayState::BuscarCasillaLibreIzquierda(int &posX, int &posY) const
{
	int y = posY;
	bool sw_result = false;

	for(int x = posX; x >= 0 && PlayerGrid(x, y) != PROA_H_T && sw_result == false; x--)
	{
		if (!esCasillaTocada(PlayerGrid,x,y))
		{
			posX = x; posY = y;
			sw_result = true;
		}
	}
DEBUG_TRZ(std::cout << __func__ << " result: " << sw_result << " X: " << posX << " Y: " << posY << std::endl;)

	return sw_result;
}

bool PlayState::BuscarCasillaLibreArriba(int &posX, int &posY) const
{
	int x = posX;
	bool sw_result = false;

	for(int y = posY; y >= 0 && PlayerGrid(x, y) != PROA_V_T && sw_result == false; y--)
	{
		if (!esCasillaTocada(PlayerGrid,x,y))
		{
			posX = x; posY = y;
			sw_result = true;
		}
	}
DEBUG_TRZ(std::cout << __func__ << " result: " << sw_result << " X: " << posX << " Y: " << posY << std::endl;)

	return sw_result;
}

bool PlayState::BuscarCasillaLibreAbajo(int &posX, int &posY) const
{
	int x = posX;
	bool sw_result = false;

	for(int y = posY; y < MAX_ROWS_GRID && PlayerGrid(x, y) != POPA_V_T && sw_result == false; y++)
	{
		if (!esCasillaTocada(PlayerGrid,x,y))
		{
			posX = x; posY = y;
			sw_result = true;
		}
	}
DEBUG_TRZ(std::cout << __func__ << " result: " << sw_result << " X: " << posX << " Y: " << posY << std::endl;)

	return sw_result;
}

///< ENTRADA/SALIDA:
void PlayState::ObtenerSiguienteCasilla(int &posX, int &posY) const
{
	if(PlayerGrid(posX, posY) == PROA_H_T)
		BuscarCasillaLibreDerecha(posX, posY);

	else if(PlayerGrid(posX, posY) == POPA_H_T)
		BuscarCasillaLibreIzquierda(posX, posY);

	else if(PlayerGrid(posX, posY) == POPA_V_T)
		BuscarCasillaLibreArriba(posX, posY);

	else if(PlayerGrid(posX, posY) == PROA_V_T)
		BuscarCasillaLibreAbajo(posX, posY);

	else if(PlayerGrid(posX, posY) == CUERPO1_H_T || PlayerGrid(posX, posY) == CUERPO2_H_T)
	{
		if (!BuscarCasillaLibreDerecha(posX, posY))
			BuscarCasillaLibreIzquierda(posX, posY);
	}

	else if(PlayerGrid(posX, posY) == CUERPO1_V_T || PlayerGrid(posX, posY) == CUERPO2_V_T)
	{
		if (!BuscarCasillaLibreArriba(posX, posY))
			BuscarCasillaLibreAbajo(posX,posY);
	}

}

void PlayState::ObtenerCasillaAleatoria(int &posX, int &posY) const
{
	bool sw_valido = false;
	while( sw_valido == false )
	{
		posX = rangeRandomNumber(0, MAX_COLS_GRID-1);
		posY = rangeRandomNumber(0, MAX_ROWS_GRID-1);

		if (PlayerGrid(posX,posY) == AGUA)
		{
			sw_valido = true;
		}

		if (PlayerGrid(posX,posY) >= PROA_H && PlayerGrid(posX,posY) <= POPA_V)
		{
			sw_valido = true;
		}
	}

DEBUG_TRZ(std::cout << __func__ << " CASILLA: " << PlayerGrid(posX,posY) << "       X: " << posX << " Y: " << posY << std::endl;)
}

///< Calculara un disparo y lo retornara en los parametros (SALIDA)
void PlayState::CalculaDisparoCPU(int &posX, int &posY)
{
	int x = 0, y = 0;

	if (hayCasillaTocada(PlayerGrid,x,y))
	{
		ObtenerSiguienteCasilla(x,y);
	}
	else
	{
		ObtenerCasillaAleatoria(x,y);
	}

	posX = x; posY = y;
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

void PlayState::getSelectedNode(uint32 mask,			///< ENTRADA. Mascara de objetos a enviar a la query
								int &x,					///< ENTRADA/SALIDA. E: Pixels en X del raton para el rayo. S: X del nodo en coordenadas grid
								int &y,					///< ENTRADA/SALIDA. E: Pixels en Y del raton para el rayo. S: X del nodo en coordenadas grid
								std::string &nodeName	///< SALIDA. Nombre del nodo seleccionado
								)
{
	setRayQuery(x, y, mask);			// establecemos query...
	Ogre::RaySceneQueryResult &result = _raySceneQuery->execute();
	Ogre::RaySceneQueryResult::iterator it;
	it = result.begin();
	if (it != result.end())	// recogemos la primera ocurrencia de la query
	{
		int xtemp=0,ytemp=0,i_st=-1;

		nodeName = it->movable->getParentSceneNode()->getName();	// cogemos el nombre del nodo seleccionado con el rayo
		i_st = std::sscanf(nodeName.c_str(),"%*[^0-9]%d%*[^0-9]%d",&xtemp, &ytemp);
		if(i_st == 2)
		{
			x=xtemp, y=ytemp;
		}
	}
}

void PlayState::createOverlay()
{
	_overlayManager = Ogre::OverlayManager::getSingletonPtr();
	Ogre::Overlay *overlay_cpu = _overlayManager->getByName("panel_cpu");
	overlay_cpu->show();
	Ogre::Overlay *overlay_player = _overlayManager->getByName("panel_player");
	overlay_player->show();
}

void PlayState::hideOverlay()
{
	_overlayManager = Ogre::OverlayManager::getSingletonPtr();
	Ogre::Overlay *overlay_cpu = _overlayManager->getByName("panel_cpu");
	overlay_cpu->hide();
	Ogre::Overlay *overlay_player = _overlayManager->getByName("panel_player");
	overlay_player->hide();
}

void PlayState::updateInfoOverlay()
{
	Ogre::OverlayElement *oe;
	oe = _overlayManager->getOverlayElement("player_puntos");
	oe->setCaption(Ogre::StringConverter::toString(puntosPlayer));

	oe = _overlayManager->getOverlayElement("player_efectivos");
	oe->setCaption(Ogre::StringConverter::toString(PlayerGrid.getCasillasVida()));

	oe = _overlayManager->getOverlayElement("cpu_puntos");
	oe->setCaption(Ogre::StringConverter::toString(puntosCPU));

	oe = _overlayManager->getOverlayElement("cpu_efectivos");
	oe->setCaption(Ogre::StringConverter::toString(CPUGrid.getCasillasVida()));

}

void PlayState::showExitMsgCegui()
{
	//Sheet
	CEGUI::Window* _ceguiSheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow","salir");

	//Config Window
	CEGUI::Window* exitMsg = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("salir.layout");

	exitMsg->getChild("lbl_salir")->setText("[font='major_shift-18'] Quieres Salir?");

	// OK
	CEGUI::Window* siButton = exitMsg->getChild("btn_si");
	siButton->subscribeEvent( CEGUI::PushButton::EventClicked,
							  CEGUI::Event::Subscriber(&PlayState::BotonSi, this));

	CEGUI::Window* noButton = exitMsg->getChild("btn_no");
	noButton->subscribeEvent( CEGUI::PushButton::EventClicked,
							  CEGUI::Event::Subscriber(&PlayState::BotonNo, this));

	//Attaching buttons
	_ceguiSheet->addChild(exitMsg);
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(_ceguiSheet);

	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().show();
}

bool PlayState::BotonSi(const CEGUI::EventArgs &e)
{
	hideOverlay();
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->hide();
	_sceneMgr->clearScene();
	_root->getAutoCreatedWindow()->removeAllViewports();

	changeState(MenuState::getSingletonPtr());
	return true;
}

bool PlayState::BotonNo(const CEGUI::EventArgs &e)
{
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->hide();
//	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().show();

	return true;
}

CEGUI::MouseButton PlayState::convertMouseButton(OIS::MouseButtonID id)
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
