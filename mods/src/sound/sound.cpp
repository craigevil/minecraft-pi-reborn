#include <string>

#include <libreborn/libreborn.h>
#include <symbols/minecraft.h>
#include <media-layer/audio.h>

#include "sound.h"
#include "../feature/feature.h"
#include "../override/override.h"
#include "../init/init.h"

// Resolve Source File Path
#define SOURCE_FILE_BASE "data/libminecraftpe.so"
static std::string get_source_file() {
    static bool source_loaded = false;
    static std::string source;

    // Check
    if (source_loaded) {
        // Already Resolved
        return source;
    } else {
        // Resolve

        // Get Binary Directory
        char *binary_directory = get_binary_directory();

        // Get Full Path
        char *full_path = NULL;
        safe_asprintf(&full_path, "%s/" SOURCE_FILE_BASE, binary_directory);

        // Free Binary Directory
        free(binary_directory);

        // Handle Overrides
        char *overridden_full_path = override_get_path(full_path);
        if (overridden_full_path != NULL) {
            free(full_path);
            full_path = overridden_full_path;
        }

        // Check If Sound Exists
        if (access(full_path, F_OK) == -1) {
            // Fail
            WARN("%s", "Audio Source File Doesn't Exists: " SOURCE_FILE_BASE);
            source.assign("");
        } else {
            // Set
            source.assign(full_path);
        }

        // Free
        free(full_path);

        // Mark As Loaded
        source_loaded = true;

        // Return
        return get_source_file();
    }
}
// Resolve On Startup
__attribute__((constructor)) static void resolve_source_file() {
    get_source_file();
}

// Play Sound
// The pitch value is unsued because it causes glitchy sounds, it is seemingly unused in MCPE as well.
static void SoundEngine_playUI_injection(__attribute__((unused)) unsigned char *sound_engine, std::string const& name, __attribute__((unused)) float pitch, float volume) {
    std::string source = get_source_file();
    if (source.size() > 0) {
        media_audio_play(source.c_str(), _sound_pick(name).c_str(), 0.0f, 0.0f, 0.0f, 1.0f, volume, 1);
    }
}
static void SoundEngine_play_injection(__attribute__((unused)) unsigned char *sound_engine, std::string const& name, float x, float y, float z, __attribute__((unused)) float pitch, float volume) {
    std::string source = get_source_file();
    if (source.size() > 0) {
        media_audio_play(source.c_str(), _sound_pick(name).c_str(), x, y, z, 1.0f, volume, 0);
    }
}

// Refresh Data
static void SoundEngine_update_injection(unsigned char *sound_engine, unsigned char *listener_mob, __attribute__((unused)) float listener_angle) {
    // Variables
    static float volume = 0;
    static float x = 0;
    static float y = 0;
    static float z = 0;
    static float yaw = 0;

    // SoundEngine Properties
    unsigned char *options = *(unsigned char **) (sound_engine + SoundEngine_options_property_offset);

    // Volume
    int32_t sound_enabled = *(int32_t *) (options + Options_sound_property_offset);
    volume = sound_enabled ? 1 : 0;

    // Position And Rotation
    if (listener_mob != NULL) {
        // Values
        x = *(float *) (listener_mob + Entity_x_property_offset);
        y = *(float *) (listener_mob + Entity_y_property_offset);
        z = *(float *) (listener_mob + Entity_z_property_offset);
        yaw = *(float *) (listener_mob + Entity_yaw_property_offset);
    }

    // Log
    media_audio_update(volume, x, y, z, yaw);
}

// Init
void init_sound() {
    // Implement Sound Engine
    if (feature_has("Implement Sound Engine", 0)) {
        overwrite_calls((void *) SoundEngine_playUI, (void *) SoundEngine_playUI_injection);
        overwrite_calls((void *) SoundEngine_play, (void *) SoundEngine_play_injection);
        overwrite_calls((void *) SoundEngine_update, (void *) SoundEngine_update_injection);
    }
}
