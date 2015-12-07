#include <cstdio>
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

	createScene();		// creamos la escena
	createOverlay();	// creamos el overlay

	// Creamos nuestra query de rayos
	_raySceneQuery = _sceneMgr->createRayQuery(Ogre::Ray());

	// inicializamos variables de estado
	memset(_CPUShotsGrid, AGUA, sizeof(_CPUShotsGrid));
	CambiarTurno(PLAYER);
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

#ifdef _DEBUG
	// movimiento de camara luego quitar
 	 Ogre::Vector3 vt(0,0,0);     Ogre::Real tSpeed = 20.0;
	  if(e.key == OIS::KC_UP)    vt+=Ogre::Vector3(0,0,-1);
	  if(e.key == OIS::KC_DOWN)  vt+=Ogre::Vector3(0,0,1);
	  if(e.key == OIS::KC_LEFT)  vt+=Ogre::Vector3(-1,0,0);
	  if(e.key == OIS::KC_RIGHT) vt+=Ogre::Vector3(1,0,0);
	  _camera->moveRelative(vt * 0.1 * tSpeed);
#endif

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
	int cellx = e.state.X.abs, posx = e.state.X.abs;
	int celly = e.state.Y.abs, posy = e.state.Y.abs;


	std::string s_CellName = "";
	Ogre::SceneNode *node = NULL;
	Ogre::Entity *pEnt = NULL;
	std::string s_Material = "";

	getSelectedNode(CPU_CELLS, cellx, celly, s_CellName);
//std::cout << "NODE: " << s_CellName<< " X: " << cellx << " Y: " << celly << std::endl;
	if (s_CellName != "")
	{
		// si habia una celda seleccionada... y es distinta a la actual... la dejamos con color NORMAL
		if(s_LastCell.size() != 0 && s_LastCell != s_CellName)
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
	if (id == OIS::MB_Left && _turno == PLAYER)
	{
		// posiciones del puntero del raton en pixeles
		int posx = e.state.X.abs;
		int posy = e.state.Y.abs;

		std::string s_CellName 	= "";

		getSelectedNode(CPU_CELLS, posx, posy, s_CellName);
		if (s_CellName != "")
		{
std::cout << "CLICK NODE: " << s_CellName<< " X: " << posx << " Y: " << posy << std::endl;

			// Todos los disparos producen un cambio: AGUA -> TOCADO ó DISPARADO, menos cuando disparan sobre algo ya disparado (DISPARADO, PROA_H_T, PROA_H_Q,...)
			if (CompruebaDisparo(CPUGrid, posx, posy))				// Si ha habido algun cambio con este disparo...
			{

				ActualizaTablero(CPUGrid(posx, posy), s_CellName);	// Actualizamos tablero gráfico, según contenido de posicion del grid ya actualizado.
				CheckHundido(CPUGrid, posx, posy);

				if(CPUGrid.getCasillasVivas() == 0)
				{
					// FIN DE JUEGO, GANA EL PLAYER
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
}

void PlayState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) {}

void PlayState::CambiarTurno(EN_TURNO turno)
{
	_turno = turno;
	int x = 0, y = 0;

	if(_turno == CPU)
	{
//		sleep(3);
		CalculaDisparoCPU(x, y);

		if(CompruebaDisparo(PlayerGrid, x, y))
		{
			std::stringstream s_node_player;
			s_node_player << "node_player_" << x << "_" << y;	// node_player_X_Y

			ActualizaTablero(PlayerGrid(x, y), s_node_player.str());	// Actualizamos tablero gráfico, según contenido de posicion del grid ya actualizado.
			CheckHundido(PlayerGrid, x, y);

			if(PlayerGrid.getCasillasVivas() == 0)
			{
				// FIN DE JUEGO, GANA LA CPU
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
std::cout << std::endl << "CPU:";
	CPUGrid.IniciaBarcosAleatorio();
std::cout << std::endl << "PLAYER:";
	PlayerGrid.IniciaBarcosAleatorio();


	for(int y = 0; y < MAX_ROWS_GRID; y++)
	{
		for(int x = 0; x < MAX_COLS_GRID; x++)
		{
			std::stringstream nodeNamePlayer;
			std::stringstream nodeNameCPU;
			nodeNamePlayer << "node_player_" << x << "_" << y;	// node_player_X_Y;
			nodeNameCPU << "node_cpu_" << x << "_" << y;	// node_cpu_X_Y;

			ActualizaTablero(PlayerGrid(x, y), nodeNamePlayer.str());
//			ActualizaTablero(CPUGrid(x, y), nodeNameCPU.str());
		}
	}
}

void PlayState::ActualizaTablero(usint16 valor, std::string nodeName)
{
	Ogre::Entity* entidad=NULL;
	Ogre::SceneNode* shipNode=NULL;
	Ogre::SceneNode* TableroNode = _sceneMgr->getSceneNode(nodeName);
	std::stringstream shipNodeName;
	std::string pieza;
	bool pintarBarco = true;
	bool esHorizontal = false;

	switch(valor)
	{
		case AGUA: pintarBarco=false; break;
		case DISPARADO: pintarBarco=false; break;

		case PROA_H: pieza="proa.mesh";  esHorizontal=true; shipNodeName << "ship_" << nodeName; break;
		case CUERPO1_H: pieza="cuerpo1.mesh";  esHorizontal=true; shipNodeName << "ship_" << nodeName;break;
		case CUERPO2_H: pieza="cuerpo2.mesh";  esHorizontal=true; shipNodeName << "ship_" << nodeName;break;
		case POPA_H: pieza="popa.mesh";  esHorizontal=true; shipNodeName << "ship_" << nodeName;break;

		case PROA_V: pieza="proa.mesh"; shipNodeName << "ship_" << nodeName; break;
		case CUERPO1_V: pieza="cuerpo1.mesh"; shipNodeName << "ship_" << nodeName; break;
		case CUERPO2_V: pieza="cuerpo2.mesh"; shipNodeName << "ship_" << nodeName;break;
		case POPA_V :pieza="popa.mesh";  shipNodeName << "ship_" << nodeName;break;

		case PROA_H_T :pieza="proa_t.mesh"; esHorizontal=true; shipNodeName << "shipTocado_" << nodeName; break;
		case CUERPO1_H_T :pieza="cuerpo1_t.mesh"; esHorizontal=true; shipNodeName << "shipTocado_" << nodeName; break;
		case CUERPO2_H_T :pieza="cuerpo2_t.mesh"; esHorizontal=true; shipNodeName << "shipTocado_" << nodeName; break;
		case POPA_H_T :pieza="popa_t.mesh"; esHorizontal=true; shipNodeName << "shipTocado_" << nodeName; break;

		case PROA_V_T :pieza="proa_t.mesh"; shipNodeName << "shipTocado_" << nodeName; break;
		case CUERPO1_V_T :pieza="cuerpo1_t.mesh"; shipNodeName << "shipTocado_" << nodeName;break;
		case CUERPO2_V_T :pieza="cuerpo2_t.mesh"; shipNodeName << "shipTocado_" << nodeName;break;
		case POPA_V_T :pieza="popa_t.mesh"; shipNodeName << "shipTocado_" << nodeName; break;

		/*case PROA_H_Q :pieza="proa_ardiendo.mesh"; esHorizontal=true; break;
		case CUERPO1_H_Q :pieza="cuerpo1_q.mesh"; esHorizontal=true; break;
		case CUERPO2_H_Q :pieza="cuerpo2_q.mesh"; esHorizontal=true; break;
		case POPA_H_Q :pieza="popa_q.mesh"; esHorizontal=true; break;

		case PROA_V_Q :pieza="proa_ardiendo.mesh"; break;
		case CUERPO1_V_Q :pieza="cuerpo1_q.mesh"; break;
		case CUERPO2_V_Q :pieza="cuerpo2_q.mesh"; break;
		case POPA_V_Q :pieza="popa_q.mesh"; break;*/
	}

	 if (pintarBarco) {

		// if (TableroNode->numChildren()>0)
		// { 	// si tiene algun hijo entonces es que ya tenia barco sin tocar, y hay que cambiar el modelo por el roto
			// 	 TableroNode->
		// }

		entidad = _sceneMgr->createEntity(shipNodeName.str(), pieza);
		entidad->setQueryFlags(SHIP_CELL);
		shipNode = _sceneMgr->createSceneNode(shipNodeName.str());
		shipNode->attachObject(entidad);
		if (esHorizontal) {
			shipNode->yaw(Ogre::Degree(90));
		}
		TableroNode->addChild(shipNode);

		 // meter lo que sea
	 }
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

//TODO: Comprobar si esta hundido el barco y en ese caso cambiar el estado de TOCADO a HUNDIDO de sus casillas y las de alrededor ponerlas a DISPARADO

	// Actualizamos grid de disparos de la CPU
	if (_turno == CPU)
		_CPUShotsGrid[posx][posy] = grid(posx, posy);

	return sw_casillaCambiada;
}

void PlayState::CheckHundido(Grid& grid, usint16 posx, usint16 posy)
{
}

void PlayState::CalculaDisparoCPU(int &posX, int &posY)
{
	int x = 0, y = 0;
	bool sw_disparoCalculado = false;

	// Recorremos la matriz de disparos realizados por la CPU
	for(int i = 0; i < MAX_COLS_GRID && sw_disparoCalculado == false; i++)
	{
		for(int j = 0; j < MAX_ROWS_GRID && sw_disparoCalculado == false; j++)
		{
			// si hay alguna casilla "TOCADA" intentamos seguir disparando por ahi...
			if (_CPUShotsGrid[i][j] >= PROA_H_T || _CPUShotsGrid[i][j] <= POPA_V_T)
			{
				// comprobamos si podemos continuar disparando en horizontal...
				int incr = 1;
				while( i+incr < MAX_COLS_GRID &&
					   (_CPUShotsGrid[i+incr][j] >= PROA_H_T || _CPUShotsGrid[i+incr][j] <= POPA_V_T) )
				{
					sw_disparoCalculado = true;
					x = i+incr;
					y = j;

					incr++;
				}

				// si no podemos seguir disparando en horizontar... probamos en vertical
				if(sw_disparoCalculado == false)
				{
					incr = 1;
					while( j+incr < MAX_ROWS_GRID &&
						   (_CPUShotsGrid[i][j+incr] >= PROA_H_T || _CPUShotsGrid[i][j+incr] <= POPA_V_T) )
					{
						sw_disparoCalculado = true;
						x = i;
						y = j+incr;

						incr++;
					}
				}
			}
		}
	}

	// Si no habia ninguna casilla "TOCADA", creamos un disparo aleatorio a casilla aun no disparada (AGUA)
	if(sw_disparoCalculado == false)
	{
		do{
			x = rangeRandomNumber(0, MAX_COLS_GRID);
			y = rangeRandomNumber(0, MAX_ROWS_GRID);
		}while(_CPUShotsGrid[x][y] != AGUA);
	}

	posX = x;
	posY = y;
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
//			std::cout << "X: " << x << " Y: " << y << std::endl;
		}
	}
}

void PlayState::createOverlay()
{
	_overlayManager = Ogre::OverlayManager::getSingletonPtr();
	Ogre::Overlay *overlay = _overlayManager->getByName("Info");
	overlay->show();
}



