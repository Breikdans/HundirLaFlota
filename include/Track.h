#ifndef __TRACKH__
#define __TRACKH__

#include <OGRE/Ogre.h>
#include <OGRE/OgreResourceManager.h>
#include <OGRE/OgreSharedPtr.h>
#include <SDL2/SDL_mixer.h>

class Track : public Ogre::Resource
{
	public:
		// Constructor (ver Ogre::Resource).
		Track ( Ogre::ResourceManager* pManager,
				const Ogre::String& resource_name,
				Ogre::ResourceHandle handle,
				const Ogre::String& resource_group,
				bool manual_load = false,
				Ogre::ManualResourceLoader* pLoader = 0);
		~Track ();

		// Manejo básico del track.
		void play (int loop = -1);
		void pause ();
		void stop ();

		void fadeIn (int ms, int loop);
		void fadeOut (int ms);
		static bool isPlaying ();

	private:
		// Funcionalidad de Ogre::Resource.
		void loadImpl ();
		void unloadImpl ();
		size_t calculateSize () const;
		Ogre::String _resource_group;

		// Variables miembro.
		Mix_Music* _pTrack; // SDL
		Ogre::String _path; // Ruta al track.
		size_t _size;       // Tamaño.
};

// Puntero inteligente a Track.
typedef Ogre::SharedPtr<Track> TrackPtr;

#endif
