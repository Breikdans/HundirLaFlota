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
		Ogre::RenderWindow* _renderWindow;
		Ogre::Viewport* _viewport;
		Ogre::Camera* _camera;
		Ogre::OverlayManager* _overlayManager;
		Ogre::RaySceneQuery *_raySceneQuery;

		void createScene();
		void createOverlay();

		Ogre::Ray setRayQuery(int posx, int posy, uint32 mask);
		void getSelectedNode(uint32 mask, int &x, int &y, std::string &nodeName);

		void CalculaDisparoCPU(int &posX, int &posY);
		void ActualizaTablero(usint16, std::string);
		bool CompruebaDisparo(Grid&, usint16 posx, usint16 posy);
		void CheckHundido(Grid& grid, usint16 posx, usint16 posy);
		void CambiarTurno(EN_TURNO turno);

		EN_TURNO _turno;
		bool _exitGame;
		int _CPUShotsGrid[MAX_ROWS_GRID][MAX_COLS_GRID];			// Registro de disparos que hace la CPU

		Grid CPUGrid;
		Grid PlayerGrid;

};

#endif
