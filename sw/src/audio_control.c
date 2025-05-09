/**
 * @file audio_control.c
 * @brief Implements the audio control functions for the game.
 *
 * This file provides the implementation for initializing the audio system
 * and triggering sound effects using the hardware interface.
 */

#include "../include/audio_control.h"
#include "../include/hw_interface.h" // Needs access to play_audio()
#include <stdio.h>

/**
 * @brief Initializes the audio control module (Placeholder).
 * Prints an initialization message.
 *
 * @return Always returns 0 (success) in this placeholder version.
 */
int audio_control_init()
{
    printf("Initializing Audio Control...\n");
    // Placeholder: Add any necessary audio hardware/software initialization here.
    return 0;
}

/**
 * @brief Cleans up the audio control module (Placeholder).
 * Prints a cleanup message.
 */
void audio_control_cleanup()
{
    printf("Cleaning up Audio Control...\n");
    // Placeholder: Add any necessary audio hardware/software cleanup here.
}

/**
 * @brief Triggers the playback of a specific sound effect (SFX) via the hardware.
 * Checks if the provided clip ID is valid before calling the hardware interface function.
 *
 * @param clip_id The audio_clip_id_t of the sound effect to play.
 */
void play_sfx(audio_clip_id_t clip_id)
{
    // Validate the clip ID against the maximum defined value.
    if (clip_id >= AUDIO_MAX_CLIPS)
    {
        fprintf(stderr, "Error: Invalid audio clip ID requested: %d\n", clip_id);
        return;
    }

    printf("Triggering SFX: %d\n", clip_id); // Debug message

    // Call the hardware interface function to trigger the audio playback.
    // This function handles writing to the control register during VBlank.
    play_audio((uint8_t)clip_id);
}
