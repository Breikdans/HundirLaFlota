#include <Ogre.h>

#include "GameManager.h"
#include "GameState.h"

template<> GameManager* Ogre::Singleton<GameManager>::msSingleton = 0;

GameManager::GameManager ()
{
	_root = 0;
}

GameManager::~GameManager ()
{
	while (!_states.empty())
	{
		_states.top()->exit();
		_states.pop();
	}

	if (_root)
		delete _root;
}

void GameManager::start(GameState* state)
{
	_root = new Ogre::Root();		// Creamos el objeto root. Sin parametros busca ogre.cfg


	if (!configure())
		return;

	_inputMgr = new InputManager;
	_inputMgr->initialise(_renderWindow);

	// Registro como key y mouse listener...
	_inputMgr->addKeyListener(this, "GameManager");
	_inputMgr->addMouseListener(this, "GameManager");

	// El GameManager es un FrameListener.
	_root->addFrameListener(this);

	// Transición al estado inicial.
	changeState(state);

	// Bucle de rendering.
	_root->startRendering();
}

void GameManager::changeState(GameState* state)
{
	// Limpieza del estado actual.
	if (!_states.empty())
	{
		// exit() sobre el último estado.
		_states.top()->exit();
		// Elimina el último estado.
		_states.pop();
	}

	// Transición al nuevo estado.
	_states.push(state);
	// enter() sobre el nuevo estado.
	_states.top()->enter();
}

void GameManager::pushState(GameState* state)
{
	// Pausa del estado actual.
	if (!_states.empty())
		_states.top()->pause();

	// Transición al nuevo estado.
	_states.push(state);
	// enter() sobre el nuevo estado.
	_states.top()->enter();
}

void GameManager::popState ()
{
	// Limpieza del estado actual.
	if (!_states.empty())
	{
		_states.top()->exit();
		_states.pop();
	}

	// Vuelta al estado anterior.
	if (!_states.empty())
		_states.top()->resume();
}

/**
 * Carga el fichero de recursos resources.cfg
 */
void GameManager::loadResources()
{

}

bool GameManager::configure ()
{
	if(!_root->restoreConfig())			// Si no se ha podido restaurar la configuracion de ogre...
	{
		if (!_root->showConfigDialog())	// mostramos dialogo de configuracion
			return false;

		_root->saveConfig();			// Guardamos
	}

	_renderWindow = _root->initialise(true, "Hundir la Flota!");

	return true;
}

GameManager* GameManager::getSingletonPtr ()
{
	return msSingleton;
}

GameManager& GameManager::getSingleton ()
{  
	assert(msSingleton);
	return *msSingleton;
}

// Las siguientes funciones miembro delegan
// el evento en el estado actual.
bool GameManager::frameStarted(const Ogre::FrameEvent& evt)
{
	_inputMgr->capture();
	return _states.top()->frameStarted(evt);
}

bool GameManager::frameEnded(const Ogre::FrameEvent& evt)
{
	return _states.top()->frameEnded(evt);
}

bool GameManager::keyPressed(const OIS::KeyEvent &e)
{
std::cout << __FILE__ << " " << __func__ << " KEY PRESSED: " << e.key << std::endl;
	_states.top()->keyPressed(e);
	return true;
}

bool GameManager::keyReleased(const OIS::KeyEvent &e)
{
std::cout << __FILE__ << " " << __func__ << " KEY RELEASED: " << e.key << std::endl;
	_states.top()->keyReleased(e);
	return true;
}

//bool GameManager::isKeyDown(OIS::KeyCode key) const
//{
//	_states.top()->isKeyDown(key);
//	return true;
//}

bool GameManager::mouseMoved(const OIS::MouseEvent &e)
{
	_states.top()->mouseMoved(e);
	return true;
}

bool GameManager::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	_states.top()->mousePressed(e, id);
	return true;
}

bool GameManager::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	_states.top()->mouseReleased(e, id);
	return true;
}
