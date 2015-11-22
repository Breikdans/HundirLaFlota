#include "MyApp.h"

MyApp::MyApp()
{
	_sceneManager 	= NULL;
	_framelistener 	= NULL;
	_root			= NULL;
	_overlayManager	= NULL;
}

MyApp::~MyApp()
{
	delete _root;
	delete _framelistener;
}

int MyApp::start()
{
	_root = new Ogre::Root();		// Creamos el objeto root. Sin parametros busca ogre.cfg

	if(!_root->restoreConfig())		// Si no se ha podido restaurar la configuracion de ogre...
	{
		_root->showConfigDialog();	// mostramos dialogo de configuracion
		_root->saveConfig();		// Guardamos
	}

	// Creamos nuestra ventana. true = creacion automatica de la ventana.
	Ogre::RenderWindow* window = _root->initialise(true,"Hundir la Flota!");
	// Creamos Gestor de Escena, en modo ST_GENERIC (modo mínimo no optimizado)
	_sceneManager = _root->createSceneManager(Ogre::ST_GENERIC);
	// Metemos una luz ambiental, una luz que no tiene fuente de origen. Ilumina a todos los objetos
	_sceneManager->setAmbientLight(Ogre::ColourValue(1, 1, 1));
	_sceneManager->addRenderQueueListener(new Ogre::OverlaySystem());

	// creamos camara virtual
	Ogre::Camera* cam = _sceneManager->createCamera("MainCamera");
	cam->setPosition(Ogre::Vector3(0.5,20,30));	// posicionamos...
	cam->lookAt(Ogre::Vector3(0,0,0));			// enfocamos a 0,0,0
	cam->setNearClipDistance(5);		// establecemos plano cercano del frustum
	cam->setFarClipDistance(300);		// establecemos plano lejano del frustum


	// Creamos el plano de imagen (lienzo) asociado a la camara
	Ogre::Viewport* viewport = window->addViewport(cam);
	viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,0.0));	// color de fondo del viewport(negro)
	double width = viewport->getActualWidth();		// recogemos ancho del viewport actual
	double height = viewport->getActualHeight();	// recogemos alto del viewport actual
	cam->setAspectRatio(width / height);			// calculamos ratio (4:3 = 1,333 16:9 1,777)

	loadResources();	// cargamos fichero de recursos
	createScene();		// creamos la escena
	createOverlay();	// creamos el overlay

	_framelistener = new MyFrameListener(window, _sceneManager, _overlayManager, cam);
	_root->addFrameListener(_framelistener);

	_root->startRendering();
	return 0;
}

/**
 * Carga el fichero de recursos resources.cfg
 */
void MyApp::loadResources()
{
	Ogre::ConfigFile cf;
	cf.load("resources.cfg");	// cargamos el fichero de recursos

//	ejemplo de resources.cfg:
//
//	[General]
//	FileSystem=media

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

void MyApp::createScene()
{

	// creacion punteros a entidades para cargar los .mesh de las celdas
	Ogre::Entity *ent_CeldaCPU;
	Ogre::Entity *ent_CeldaPlayer;

	// creacion de nodos para cargar las entidades
	Ogre::SceneNode *node_CPU;
	Ogre::SceneNode *node_Player;

	// creamos nodos de escena para tablero de CPU y tablero de Player
	Ogre::SceneNode* main_node_tablero_CPU = _sceneManager->createSceneNode("tablero_CPU");
	Ogre::SceneNode* main_node_tablero_Player = _sceneManager->createSceneNode("tablero_Player");

	// creamos las entidades y las cargamos en las tablas de punteros
	// enlazamos a los nodos, los objetos de cada tipo
	for (int i = 0; i < MAX_ROWS_GRID ; i++ )
	{
		for (int j = 0; j < MAX_COLS_GRID ; j++ )
		{
			// crea entidades 3d
			ent_CeldaPlayer = _sceneManager->createEntity("Celda.mesh");
			ent_CeldaCPU = _sceneManager->createEntity("Celda.mesh");

			// Establecemos mascaras de busqueda para nuestras querys
			ent_CeldaPlayer->setQueryFlags(PLAYER_CELLS);
			ent_CeldaCPU->setQueryFlags(CPU_CELLS);

			// creamos nodos para el tablero de jugador y atachamos la entidad
			// colgamos de main_node_tablero_Player, todos los nodos del tablero
			std::stringstream s_node_player_aux;
			s_node_player_aux << "node_player_" << i << "_" << j;	// node_player_X_Y
			node_Player = _sceneManager->createSceneNode(s_node_player_aux.str());
			node_Player->attachObject(ent_CeldaPlayer);
			node_Player->translate((-1*j)-2,0,i);
			main_node_tablero_Player->addChild(node_Player);

			// creamos nodos para el tablero de CPU y atachamos la entidad
			// colgamos de main_node_tablero_CPU, todos los nodos del tablero
			std::stringstream s_node_cpu_aux;
			s_node_cpu_aux << "node_cpu_" << i << "_" << j;	// node_cpu_X_Y
			node_CPU = _sceneManager->createSceneNode(s_node_cpu_aux.str());
			node_CPU->attachObject(ent_CeldaCPU);
			node_CPU->translate(j+2,0,i);
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
												  200,200,					// ancho y alto del plano en coordenadas del mundo
												  1,1,						// numero de segmentos para definir el plano (1x1)
												  true,						// true indica que los vectores normales se calculan perpendicular al plano
												  1,						// conjunto de coordenadas de texturas (por defecto 1)
												  10,10,					// numero de replicacion de la textura en horizontal y vertical
												  Ogre::Vector3::UNIT_Z);	// indica la direccion del vector del plano (up)

	Ogre::SceneNode* node_water = _sceneManager->createSceneNode("node_water");	// creamos nodo de escena para el fondo de agua
	Ogre::Entity* entWater = _sceneManager->createEntity("fondo_plano_agua", "plano_agua");
	entWater->setMaterialName("Water");		// establecemos el material a usar
	node_water->attachObject(entWater);		// adjuntamos al nodo, la entidad

	// Creamos estructura de grafos.....
	// del root cuelga el nodo_water... y de ahi los tableros CPU y Player
	_sceneManager->getRootSceneNode()->addChild(node_water);
	node_water->addChild(main_node_tablero_CPU);
	node_water->addChild(main_node_tablero_Player);
}

void MyApp::createOverlay()
{
	_overlayManager = Ogre::OverlayManager::getSingletonPtr();
	Ogre::Overlay *overlay = _overlayManager->getByName("Info");
	overlay->show();
}



