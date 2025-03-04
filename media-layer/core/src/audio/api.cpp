#include <vector>
#include <cmath>

#include <AL/al.h>

#include <media-layer/audio.h>
#include <libreborn/libreborn.h>

#include "file.h"
#include "engine.h"

// Store Audio Sources
static std::vector<ALuint> &get_sources() {
    static std::vector<ALuint> sources;
    return sources;
}

#define AL_ERROR_CHECK() \
    { \
        ALenum err = alGetError(); \
        if (err != AL_NO_ERROR) { \
            ERR("OpenAL Error: %s", alGetString(err)); \
        } \
    }

// Update Listener
void media_audio_update(float volume, float x, float y, float z, float yaw) {
    // Update Listener Volume
    alListenerf(AL_GAIN, volume);
    AL_ERROR_CHECK();

    // Update Listener Position
    alListener3f(AL_POSITION, x, y, z);
    AL_ERROR_CHECK();

    // Update Listener Orientation
    float radian_yaw = yaw * (M_PI / 180);
    ALfloat orientation[] = {-sinf(radian_yaw), 0.0f, cosf(radian_yaw), 0.0f, 1.0f, 0.0f};
    alListenerfv(AL_ORIENTATION, orientation);
    AL_ERROR_CHECK();

    // Clear Finished Sources
    std::vector<ALuint>::iterator it = get_sources().begin();
    while (it != get_sources().end()) {
        ALuint source = *it;
        // Check
        ALint source_state;
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
        AL_ERROR_CHECK();
        if (source_state != AL_PLAYING) {
            // Finished
            it = get_sources().erase(it);
            alDeleteSources(1, &source);
            AL_ERROR_CHECK();
        } else {
            // Still Playing
            ++it;
        }
    }
}

void media_audio_play(const char *source, const char *name, float x, float y, float z, float pitch, float volume, int is_ui) {
    // Load Sound
    ALuint buffer = _media_audio_get_buffer(source, name);
    if (buffer) {
        // Create Source
        ALuint al_source;
        alGenSources(1, &al_source);
        AL_ERROR_CHECK();

        // Set Properties
        alSourcef(al_source, AL_PITCH, pitch);
        AL_ERROR_CHECK();
        alSourcef(al_source, AL_GAIN, volume);
        AL_ERROR_CHECK();
        alSource3f(al_source, AL_POSITION, x, y, z);
        AL_ERROR_CHECK();
        alSource3f(al_source, AL_VELOCITY, 0, 0, 0);
        AL_ERROR_CHECK();
        alSourcei(al_source, AL_LOOPING, AL_FALSE);
        AL_ERROR_CHECK();
        alSourcei(al_source, AL_SOURCE_RELATIVE, is_ui ? AL_TRUE : AL_FALSE);
        AL_ERROR_CHECK();

        // Set Attenuation
        alSourcei(al_source, AL_DISTANCE_MODEL, AL_LINEAR_DISTANCE);
        AL_ERROR_CHECK();
        alSourcef(al_source, AL_MAX_DISTANCE, 16.0f);
        AL_ERROR_CHECK();
        alSourcef(al_source, AL_ROLLOFF_FACTOR, 1.0f);
        AL_ERROR_CHECK();
        alSourcef(al_source, AL_REFERENCE_DISTANCE, 0.0f);
        AL_ERROR_CHECK();

        // Set Buffer
        alSourcei(al_source, AL_BUFFER, buffer);
        AL_ERROR_CHECK();

        // Play
        alSourcePlay(al_source);
        AL_ERROR_CHECK();
        get_sources().push_back(al_source);
    }
}
