#ifndef PlayState_H
#define PlayState_H

#include <Ogre.h>
#include <OIS/OIS.h>
#include <OgreOverlaySystem.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>

#include "GameState.h"
#include "Grid.h"

class PlayState : public Ogre::Singleton<PlayState>, public GameState
{
	public:
		PlayState () {}

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

		// Heredados de Ogre::Singleton.
		static PlayState& getSingleton ();
		static PlayState* getSingletonPtr ();

	protected:
		Ogre::Root* _root;
		Ogre::SceneManager* _sceneMgr;
		Ogre::Viewport* _viewport;
		Ogre::Camera* _camera;
		Ogre::OverlaySystem* _overlaySystem;
		Ogre::OverlayManager* _overlayManager;

		void createScene();
		void createOverlay();
		void loadResources();

		void ActualizaTablero(Ogre::Node* node, usint16 F, usint16 C);

		bool _exitGame;

		Grid CPUGrid;
		Grid PlayerGrid;

};

#endif
