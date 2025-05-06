#ifndef AUDIO_CONTROL_H
#define AUDIO_CONTROL_H

#include <stdint.h>

/**
 * @file audio_control.h
 * @brief Defines the interface for controlling audio playback.
 * 
 * This file declares functions and data types for triggering sound effects (SFX)
 * via the hardware interface.
 */

/**
 * @brief Enumeration defining IDs for different audio clips (Sound Effects).
 * The specific mapping of IDs to sounds depends on the hardware implementation.
 */
typedef enum {
    AUDIO_JUMP = 0,         ///< Sound effect for character jump.
    AUDIO_GEM_COLLECT,    ///< Sound effect for collecting a gem.
    AUDIO_DEATH,          ///< Sound effect for character death.
    AUDIO_LEVEL_COMPLETE, ///< Sound effect for completing the level.
    // --- Add more audio clip IDs as defined by the hardware --- 
    AUDIO_BUTTON_PRESS,   ///< Example: Sound for pressing a button.
    AUDIO_DOOR_OPEN,      ///< Example: Sound for a door opening.
    
    AUDIO_MAX_CLIPS       ///< Sentinel value to count the number of defined clips.
} audio_clip_id_t;

/**
 * @brief Initializes the audio control module.
 * Placeholder function; might be needed for more complex audio systems.
 * 
 * @return 0 on success, -1 on failure.
 */
int audio_control_init();

/**
 * @brief Cleans up resources used by the audio control module.
 * Placeholder function.
 */
void audio_control_cleanup();

/**
 * @brief Triggers the playback of a specific sound effect (SFX) via the hardware.
 * Uses the hardware interface (play_audio function) to send the command.
 * 
 * @param clip_id The audio_clip_id_t of the sound effect to play.
 */
void play_sfx(audio_clip_id_t clip_id);

#endif // AUDIO_CONTROL_H

