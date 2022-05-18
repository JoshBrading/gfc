#ifndef __GFC_INPUT_H__
#define __GFC_INPUT_H__

#include <SDL.h>
#include "gfc_text.h"
#include "gfc_list.h"

typedef enum
{
    EMK_None = 0,
    EMK_Shift = 1000,
    EMK_Alt,
    EMK_Ctrl,
    EMK_Super
}InputModKey;

typedef enum
{
    IET_Idle    = 0,
    IET_Press   = 1,
    IET_Hold    = 2,
    IET_Release = 3
}InputEventType;

/**
 * @brief Inputs abstract user input collection.  They can be setup to trigger callbacks and/or polled for current state
 */
typedef struct
{
    TextLine command;
    List *keyCodes;                      /**<list of keys that must be pressed together to count as a single input*/
    int downCount;
    Uint32 pressTime;                   /**<clock ticks when button was pressed*/
    InputEventType state;               /**<updated each frame*/
    void (*onPress)(void *data);        /**<callback for press event*/
    void (*onHold)(void *data);         /**<callback for hold event*/
    void (*onRelease)(void *data);      /**<callback for release event*/
    void *data;                         /**<pointer to be passed to callbacks*/
}Input;

/**
 * @brief initializes the config system
 * @param configFile if not NULL it will load the file for user input configuration
 */
void gfc_input_init(char *configFile);

/**
 * @brief called as often as you want your input data update.  Every Frame, or faster if you like
 * @note must be called or input will be stale
 */
void gfc_input_update();

/**
 * @brief load user configuration from file.
 * @note this operation appends to existing commands
 */
void gfc_input_commands_load(char *configFile);

/**
 * @brief clears all user input configuration
 */
void gfc_input_commands_purge();

/**
 * @brief check if a command was pressed this frame
 * @param command the name of the command to check
 * @returns true if pressed, false otherwise
 */
Uint8 gfc_input_command_pressed(const char *command);
Uint8 gfc_input_command_held(const char *command);
Uint8 gfc_input_command_released(const char *command);
Uint8 gfc_input_command_down(const char *command);

InputEventType gfc_input_command_get_state(const char *command);

/**
 * @brief report if the key provided has been pressed this frame
 * @param key the name of the key to check
 * @returns true if pressed, false otherwise
 * 
 */
Uint8 gfc_input_key_pressed(const char *key);
Uint8 gfc_input_key_released(const char *key);
Uint8 gfc_input_key_held(const char *key);
Uint8 gfc_input_key_down(const char *key);

/**
 * @brief check if the mouse wheel is moving in the indicated direction
 * @return 0 if it is not, 1 if it is
 */
Uint8 gfc_input_mouse_wheel_up();
Uint8 gfc_input_mouse_wheel_down();
Uint8 gfc_input_mouse_wheel_left();
Uint8 gfc_input_mouse_wheel_right();

/**
 * @brief configure callbacks for an input
 * @param command the name of the input to configure
 * @param onPress the function to call when the input is pressed
 * @param onHold the function to call when the input is held
 * @param onRelease the function to call when the input is released
 * @param data pointer to any custom data you want associated with the input
 */
void gfc_input_set_callbacks(
    char *command,
    void (*onPress)(void *data),
    void (*onHold)(void *data),
    void (*onRelease)(void *data),
    void *data
);


#endif
