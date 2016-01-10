#ifndef IntroState_H
#define IntroState_H

#include <Ogre.h>
#include <OIS/OIS.h>
#include <iterator>
#include <map>
#include "TrackManager.h"
#include "SoundFXManager.h"
#include "Fader.h"

#include "GameState.h"

typedef struct
{
	unsigned int iPuntos;
	std::string sJugador;
}STR_Record;

#define MAX_PLAYER_RECORDS		10

class IntroState : public Ogre::Singleton<IntroState>, public GameState
{
	public:
		IntroState() {}

		void enter ();
		void exit ();
		void pause ();
		void resume ();

		void keyPressed (const OIS::KeyEvent &e);
		void keyReleased (const OIS::KeyEvent &e);

		void mouseMoved (const OIS::MouseEvent &e);
		void mousePressed (const OIS::MouseEvent &e, OIS::MouseButtonID id);
		void mouseReleased (const OIS::MouseEvent &e, OIS::MouseButtonID id);

		bool frameStarted (const Ogre::FrameEvent& evt);
		bool frameEnded (const Ogre::FrameEvent& evt);

		std::multimap<unsigned int, std::string> gameRecords;

		// Heredados de Ogre::Singleton.
		static IntroState& getSingleton ();
		static IntroState* getSingletonPtr ();

		TrackPtr 	getMenuTrackPtr () { return _mainMenuTrack; }
		TrackPtr 	getMainThemeTrackPtr () { return _gameThemeLoop; }
		SoundFXPtr 	getFireBangFXPtr () { return _fireBangEffect; }
		SoundFXPtr 	getCannonFXPtr () { return _cannonEffect; }
		SoundFXPtr 	getWaterFXPtr () { return _waterExplosionEffect; }
		SoundFXPtr 	getErrorFXPtr () { return _errorEffect; }
		SoundFXPtr 	getFinalExplosionFXPtr () { return _finalExplosionEffect; }

	protected:
		Ogre::Root* 			_root;
		Ogre::SceneManager* 	_sceneMgr;
		Ogre::Viewport* 		_viewport;
		Ogre::Camera* 			_camera;
		Ogre::OverlayManager* 	_overlayManager;

		// Manejadores del sonido.
		TrackManager* 			_TrackManager;
		SoundFXManager* 		_SoundFXManager;
		TrackPtr 				_mainMenuTrack;			// puntero inteligente
		TrackPtr 				_gameThemeLoop;			// puntero inteligente
		SoundFXPtr 				_fireBangEffect;		// puntero inteligente
		SoundFXPtr 				_cannonEffect;			// puntero inteligente
		SoundFXPtr 				_waterExplosionEffect;	// puntero inteligente
		SoundFXPtr				_errorEffect;			// puntero inteligente
		SoundFXPtr				_finalExplosionEffect;	// puntero inteligente

		Fader* 					_fader;

		void createOverlay();
		void loadResources();
		void createCegui();
		void initSDL();

		void loadRecordsFile();
		void fillRecordsFile();
		void showRecordsFile();

		bool _exitGame;
};

#endif
