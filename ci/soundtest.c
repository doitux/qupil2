/* Checks that SDL_mixer can open audio and load Qupil 1.4.1's sound files
 * (Ogg Vorbis and WAV). Run with SDL_AUDIODRIVER=dummy on CI. */
#include <SDL.h>
#include <SDL_mixer.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int i, bad = 0;
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
        return 2;
    }
    for (i = 1; i < argc; i++) {
        Mix_Chunk *c = Mix_LoadWAV(argv[i]);
        printf("%s: %s\n", argv[i], c ? "ok" : Mix_GetError());
        if (c)
            Mix_FreeChunk(c);
        else
            bad = 1;
    }
    Mix_CloseAudio();
    SDL_Quit();
    return bad;
}
