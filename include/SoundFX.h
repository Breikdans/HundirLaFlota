#ifndef __SOUNDFXH__
#define __SOUNDFXH__

#include <OGRE/Ogre.h>
#include <OGRE/OgreResourceManager.h>
#include <OGRE/OgreSharedPtr.h>
#include <SDL2/SDL_mixer.h>

// Clase para mezclar efectos de sonido.
// Delega en SDL mixer.
class SoundFX: public Ogre::Resource
{
	public:
		// Constructor (ver Ogre::Resource).
		SoundFX(Ogre::ResourceManager* creator,
				const Ogre::String& resource_name,
				Ogre::ResourceHandle handle,
				const Ogre::String& resource_group,
				bool isManual = false,
				Ogre::ManualResourceLoader* loader = 0);
		~SoundFX();

		int play(int loop = 0);

	protected:
		void loadImpl();
		void unloadImpl();
		size_t calculateSize() const;

	private:
		Mix_Chunk* _pSound; // Info sobre el efecto de sonido.
		Ogre::String _path; // Ruta completa al efecto de sonido.
		size_t _size;       // Tamaño del efecto (bytes).
};

// Puntero inteligente a SoundFX.
typedef Ogre::SharedPtr<SoundFX> SoundFXPtr;

#endif
