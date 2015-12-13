#ifndef IntroState_H
#define IntroState_H

#include <Ogre.h>
#include <OIS/OIS.h>
#include <iterator>
#include <map>

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

	protected:
		Ogre::Root* 			_root;
		Ogre::SceneManager* 	_sceneMgr;
		Ogre::Viewport* 		_viewport;
		Ogre::Camera* 			_camera;
		Ogre::OverlayManager* 	_overlayManager;

		void createOverlay();
		void loadResources();
		void createCegui();

		void loadRecordsFile();
		void fillRecordsFile();
		void showRecordsFile();

		bool _exitGame;
};

#endif
