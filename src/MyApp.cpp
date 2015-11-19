#include "MyApp.h"

MyApp::MyApp()
{
	_sceneManager = NULL;
	_framelistener = NULL;
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

	// creamos camara virtual
	Ogre::Camera* cam = _sceneManager->createCamera("MainCamera");
	cam->setPosition(Ogre::Vector3(5,20,20));	// posicionamos...
	cam->lookAt(Ogre::Vector3(0,0,0));			// enfocamos a 0,0,0
	cam->setNearClipDistance(5);		// establecemos plano cercano del frustum
	cam->setFarClipDistance(10000);		// establecemos plano lejano del frustum


	// Creamos el plano de imagen (lienzo) asociado a la camara
	Ogre::Viewport* viewport = window->addViewport(cam);
	viewport->setBackgroundColour(Ogre::ColourValue(0.0,0.0,0.0));	// color de fondo del viewport(negro)
	double width = viewport->getActualWidth();		// recogemos ancho del viewport actual
	double height = viewport->getActualHeight();	// recogemos alto del viewport actual
	cam->setAspectRatio(width / height);			// calculamos ratio (4:3 = 1,333 16:9 1,777)

	loadResources();	// cargamos fichero de recursos
	createScene();		// creamos la escena

	// Recuperamos el nodo de escena "SinbadNode"
	//Ogre::SceneNode *node = _sceneManager->getSceneNode("SinbadNode");

	//_framelistener = new MyFrameListener(window, cam, node);
	//_root->addFrameListener(_framelistener);

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

	// creacion de tablas de punteros a entidades para cargar los .mesh de las celdas
	Ogre::Entity *ent_tablero_CPU[MAX_ROWS_GRID][MAX_COLS_GRID];
	Ogre::Entity *ent_tablero_Player[MAX_ROWS_GRID][MAX_COLS_GRID];

	// creacion de tablas de punteros a nodos para cargar las entidades
	Ogre::SceneNode *node_CPU[MAX_ROWS_GRID][MAX_COLS_GRID];
	Ogre::SceneNode *node_Player[MAX_ROWS_GRID][MAX_COLS_GRID];

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
			ent_tablero_CPU[i][j] = _sceneManager->createEntity("Celda.mesh");
			ent_tablero_Player[i][j] = _sceneManager->createEntity("Celda.mesh");

			std::stringstream s_node_player_aux;
			s_node_player_aux << "node_player_" << i << "_" << j;
			node_Player[i][j] = _sceneManager->createSceneNode(s_node_player_aux.str());
			node_Player[i][j]->attachObject(ent_tablero_Player[i][j]);
			main_node_tablero_Player->addChild(node_Player[i][j]);
			node_Player[i][j]->translate(j,0,i);

			std::stringstream s_node_cpu_aux;
			s_node_cpu_aux << "node_cpu_" << i << "_" << j;
			node_CPU[i][j] = _sceneManager->createSceneNode(s_node_cpu_aux.str());
			node_CPU[i][j]->attachObject(ent_tablero_CPU[i][j]);
			main_node_tablero_CPU->addChild(node_CPU[i][j]);
			node_CPU[i][j]->translate(j+MAX_COLS_GRID+2,0,i);


		}
	}

	// añadimos a Root un nodo hijo nodeCPU y nodePlayer
	_sceneManager->getRootSceneNode()->addChild(main_node_tablero_CPU);
	_sceneManager->getRootSceneNode()->addChild(main_node_tablero_Player);

	//Ogre::Plane plane1(Ogre::Vector3::UNIT_Y, -5);
	//Ogre::MeshManager::getSingleton().createPlane("plane1",
	//Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane1,
	//200,200,1,1,true,1,20,20,Ogre::Vector3::UNIT_Z);

	//Ogre::SceneNode* node2 = _sceneManager->createSceneNode("ground");
	//Ogre::Entity* groundEnt = _sceneManager->createEntity("planeEnt", "plane1");
//	groundEnt->setMaterialName("Ground");
//	node2->attachObject(groundEnt);

	//_sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	//Ogre::Light* light = _sceneManager->createLight("Light1");
	//light->setType(Ogre::Light::LT_DIRECTIONAL);
	//light->setDirection(Ogre::Vector3(1,-1,0));
	//node2->attachObject(light);

	//_sceneManager->getRootSceneNode()->addChild(node2);


}



