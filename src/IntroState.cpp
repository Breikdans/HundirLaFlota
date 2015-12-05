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
	createCegui();
	//createScene();	// creamos la escena

	_overlayManager = Ogre::OverlayManager::getSingletonPtr();
	createOverlay();	// creamos el overlay
    createCegui();
	_exitGame 	= false;
}

void IntroState::createCegui()
{
	  _cegui_renderer = &CEGUI::OgreRenderer::bootstrapSystem();
	  CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	  CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
	  CEGUI::Font::setDefaultResourceGroup("Fonts");
	  CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	  CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");

	  CEGUI::FontManager::getSingleton().createAll("*.font", "Fonts");
	  CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
	  CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-12");
	  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
}


void IntroState::createOverlay()
{
	Ogre::Overlay *overlay = _overlayManager->getByName("Intro");
	overlay->show();
}

void IntroState::exit()
{
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

	Ogre::Overlay *overlay = _overlayManager->getByName("Intro");
	overlay->hide();

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

void IntroState::mouseMoved(const OIS::MouseEvent &e) {}

void IntroState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id) {}

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

void IntroState::createCegui()
{
	  _cegui_renderer = &CEGUI::OgreRenderer::bootstrapSystem();
	  CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	  CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
	  CEGUI::Font::setDefaultResourceGroup("Fonts");
	  CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	  CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");

	  CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
	  CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-12");
	  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
}

