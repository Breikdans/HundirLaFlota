#ifndef MenuState_H
#define MenuState_H

#include <Ogre.h>
#include <OIS/OIS.h>
#include <OgreOverlaySystem.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>

#include "GameState.h"

class MenuState : public Ogre::Singleton<MenuState>, public GameState
{
	public:
		MenuState () {}

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
		static MenuState& getSingleton ();
		static MenuState* getSingletonPtr ();

	protected:
		Ogre::Root* _root;
		Ogre::SceneManager* _sceneMgr;
		Ogre::RenderWindow* _renderWindow;
		Ogre::Viewport* _viewport;
		Ogre::Camera* _camera;
		Ogre::OverlayManager* _overlayManager;
		Ogre::RaySceneQuery *_raySceneQuery;

		void createScene();
		//void createOverlay();
		//void loadResources();

		//void ActualizaTablero(Ogre::SceneNode*, usint16, std::string);
		//Ogre::Ray setRayQuery(int posx, int posy, uint32 mask);

		bool _exitGame;

		//Grid CPUGrid;
		//Grid PlayerGrid;

};

#endif