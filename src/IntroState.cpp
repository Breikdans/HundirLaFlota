#include "IntroState.h"
#include "MenuState.h"

template<> IntroState* Ogre::Singleton<IntroState>::msSingleton = 0;

void IntroState::enter()
{
	_root 		= Ogre::Root::getSingletonPtr();
	_sceneMgr 	= _root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
	_camera 	= _sceneMgr->createCamera("IntroCamera");
	_viewport 	= _root->getAutoCreatedWindow()->addViewport(_camera);
	_viewport->setBackgroundColour(Ogre::ColourValue(0,0,0));

	_sceneMgr->addRenderQueueListener(new Ogre::OverlaySystem());	// consulta de rayos

	loadResources();

	_overlayManager = Ogre::OverlayManager::getSingletonPtr();
	createOverlay();	// creamos el overlay
	createCegui();
	loadRecordsFile();

	_exitGame 	= false;
}


void IntroState::createCegui()
{
	CEGUI::OgreRenderer::bootstrapSystem();
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");

	CEGUI::FontManager::getSingleton().createAll("*.font", "Fonts");
	CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
	CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-12");
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");

	// igualamos posiciones de ratones
	CEGUI::Vector2f mousePos = CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(-mousePos.d_x,-mousePos.d_y);
}


void IntroState::createOverlay()
{
	unsigned int width, height, depth;
	int left, top;

	Ogre::Overlay *overlay = _overlayManager->getByName("Intro");
	_root->getAutoCreatedWindow()->getMetrics(width, height, depth, left, top);

	overlay->setScale(((float(width) / 100) / 1024) * 100, ((float(height) / 100) / 768) * 100);
	overlay->show();
}


void IntroState::exit()
{
	Ogre::Overlay *overlay = _overlayManager->getByName("Intro");
	overlay->clear();

	_sceneMgr->clearScene();
	_root->getAutoCreatedWindow()->removeAllViewports();
}


void IntroState::pause() {}

void IntroState::resume() {}

bool IntroState::frameStarted(const Ogre::FrameEvent& evt)
{
	return true;
}

bool IntroState::frameEnded(const Ogre::FrameEvent& evt)
{
	if (_exitGame)
		return false;

	return true;
}

void IntroState::keyPressed(const OIS::KeyEvent &e)
{
	// Transición al siguiente estado.
	// Espacio --> PlayState
	//if (e.key == OIS::KC_SPACE)
	//{

	changeState(MenuState::getSingletonPtr());
	//}
}

void IntroState::keyReleased(const OIS::KeyEvent &e )
{
	if (e.key == OIS::KC_ESCAPE)
	{
		_exitGame = true;
	}
}

void IntroState::mouseMoved(const OIS::MouseEvent &e) {

	// Gestion del overlay (CURSOR)-----------------------------
		// posiciones del puntero del raton en pixeles
		int posx = e.state.X.abs;
		int posy = e.state.Y.abs;

			Ogre::OverlayElement *oe;
			oe = _overlayManager->getOverlayElement("cursor_intro");
			oe->setLeft(posx); oe->setTop(posy);

}

void IntroState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	changeState(MenuState::getSingletonPtr());
}

void IntroState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) {}

IntroState* IntroState::getSingletonPtr()
{
	return msSingleton;
}

IntroState& IntroState::getSingleton ()
{
	assert(msSingleton);
	return *msSingleton;
}

void IntroState::loadResources()
{
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

void IntroState::fillRecordsFile()
{
	STR_Record str_record;
	FILE *file = NULL;
	size_t lenEscrito = 0;

	memset(&str_record, 0x00, sizeof(str_record));

	file = fopen("records.txt","w");
	for(int i = 0; i < MAX_PLAYER_RECORDS; i++)
	{
		lenEscrito = fwrite(&str_record, sizeof(str_record), 1, file);
		if(lenEscrito!=0)
			gameRecords.insert(std::make_pair(str_record.iPuntos, str_record.sJugador));

std::cout << "iPuntos: " << str_record.iPuntos << " Jugador: " << str_record.sJugador << std::endl;
std::cout << "gameRecords.size(): " << gameRecords.size() << " Escrito: " << lenEscrito << std::endl;
	}

	fclose(file);
}

void IntroState::loadRecordsFile()
{
	STR_Record str_record;
	FILE *file = NULL;
	size_t lenLeido = 0;

	file = fopen("records.txt","rb");
	if(file)	// Existe y se ha podido abrir el fichero? lo leemos!
	{
		while(!feof(file) && gameRecords.size() <= MAX_PLAYER_RECORDS)
		{
			memset(&str_record, 0x00, sizeof(str_record));

			lenLeido = fread(&str_record, sizeof(str_record), 1, file);
			if(lenLeido!=0)
				gameRecords.insert(std::make_pair(str_record.iPuntos, str_record.sJugador));
std::cout << "iPuntos: " << str_record.iPuntos << " Jugador: " << str_record.sJugador << std::endl;
std::cout << "gameRecords.size(): " << gameRecords.size() << std::endl;
		}
		fclose(file);
	}
	else
		fillRecordsFile();
}

void IntroState::showRecordsFile()
{
	for (std::multimap<unsigned int, std::string>::iterator it = gameRecords.begin(); it != gameRecords.end(); it++)
	{
		//std::cout << " iPuntos: " << it->first() << " Jugador: " << it->second() << std::endl;
	}
	std::cout << "gameRecords.size(): " << gameRecords.size() << std::endl;

}
