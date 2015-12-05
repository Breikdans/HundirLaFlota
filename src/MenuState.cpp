#include "MenuState.h"
#include "PlayState.h"

template<> MenuState* Ogre::Singleton<MenuState>::msSingleton = 0;

void MenuState::enter ()
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

	//loadResources();
	//createScene();		// creamos la escena
	//createOverlay();	// creamos el overlay

	// Creamos nuestra query de rayos
	//_raySceneQuery = _sceneMgr->createRayQuery(Ogre::Ray());

	_exitGame = false;
}

void MenuState::exit ()
{
	_sceneMgr->destroyQuery(_raySceneQuery);
	_sceneMgr->clearScene();
	_root->getAutoCreatedWindow()->removeAllViewports();
}

void MenuState::pause() {}

void MenuState::resume()
{
	// Se restaura el background colour.
	//_viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 1.0));
}

bool MenuState::frameStarted(const Ogre::FrameEvent& evt)
{
	return true;
}

bool MenuState::frameEnded(const Ogre::FrameEvent& evt)
{
	if (_exitGame)
		return false;

	return true;
}

void MenuState::keyPressed(const OIS::KeyEvent &e)
{
	// Tecla p --> PauseState.
	/*if (e.key == OIS::KC_P)
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
*/

}

void MenuState::keyReleased(const OIS::KeyEvent &e)
{
	if (e.key == OIS::KC_ESCAPE)
	{
		_exitGame = true;
	}
}

void MenuState::mouseMoved(const OIS::MouseEvent &e)
{

}

void MenuState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	std::cout << "SU PRIMA!!" << std::endl;
}

void MenuState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) {}

MenuState* MenuState::getSingletonPtr ()
{
	return msSingleton;
}

MenuState& MenuState::getSingleton ()
{
	assert(msSingleton);
	return *msSingleton;
}

void MenuState::createScene()
{
	}





