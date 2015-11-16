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
	Ogre::SceneNode *node = _sceneManager->getSceneNode("SinbadNode");

	_framelistener = new MyFrameListener(window, cam, node);
	_root->addFrameListener(_framelistener);

	_root->startRendering();
	return 0;
}

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
	Ogre::Entity* ent1 = _sceneManager->createEntity("Sinbad.mesh");
	Ogre::SceneNode* node1 = _sceneManager->createSceneNode("SinbadNode");
	node1->attachObject(ent1);
	_sceneManager->getRootSceneNode()->addChild(node1);

	Ogre::Plane plane1(Ogre::Vector3::UNIT_Y, -5);
	Ogre::MeshManager::getSingleton().createPlane("plane1",
	Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane1,
	200,200,1,1,true,1,20,20,Ogre::Vector3::UNIT_Z);

	Ogre::SceneNode* node2 = _sceneManager->createSceneNode("ground");
	Ogre::Entity* groundEnt = _sceneManager->createEntity("planeEnt", "plane1");
	groundEnt->setMaterialName("Ground");
	node2->attachObject(groundEnt);

	_sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	Ogre::Light* light = _sceneManager->createLight("Light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(Ogre::Vector3(1,-1,0));
	node2->attachObject(light);

	_sceneManager->getRootSceneNode()->addChild(node2);
}

