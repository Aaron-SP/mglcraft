/* Copyright [2013-2018] [Aaron Springstroh, Minimal Graphics Library]

This file is part of the Fractex.

Fractex is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Fractex is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Fractex.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __CONTROLS__
#define __CONTROLS__

#include <functional>
#include <game/sound.h>
#include <game/state.h>
#include <game/text.h>
#include <game/ui_overlay.h>
#include <game/world.h>
#include <iostream>
#include <min/camera.h>
#include <min/ray.h>
#include <min/window.h>
#include <stdexcept>

namespace game
{

class controls
{
  private:
    static constexpr float _block_cost = 10.0;
    static constexpr float _beam_cost = 10.0;
    static constexpr float _beam_charge_cost = 20.0;
    static constexpr float _missile_cost = 20.0;
    static constexpr float _grapple_cost = 10.0;
    static constexpr float _health_regen = 5.0;
    static constexpr float _energy_regen = 10.0;
    static constexpr float _project_dist = 3.0;
    min::window *_window;
    min::camera<float> *_camera;
    character *_character;
    state *_state;
    ui_overlay *_ui;
    world *_world;
    sound *_sound;

  public:
    controls(min::window &window, min::camera<float> &camera,
             character &ch, state &state,
             ui_overlay &ui, world &world, sound &sound)
        : _window(&window), _camera(&camera), _character(&ch),
          _state(&state), _ui(&ui), _world(&world), _sound(&sound) {}

    min::camera<float> *get_camera()
    {
        return _camera;
    }
    character *get_character()
    {
        return _character;
    }
    sound *get_sound()
    {
        return _sound;
    }
    state *get_state()
    {
        return _state;
    }
    ui_overlay *get_ui()
    {
        return _ui;
    }
    world *get_world()
    {
        return _world;
    }
    min::window *get_window()
    {
        return _window;
    }
    void register_control_callbacks()
    {
        // Get the player skill beam string
        skills &skill = _world->get_player().get_skills();
        const std::string &beam_str = skill.get_beam_string();

        // Enable the console and set default message
        _ui->enable_console();
        _ui->set_console_string(beam_str);

        // Get access to the keyboard
        auto &keyboard = _window->get_keyboard();

        // Clear any keys mapped to keyboard
        keyboard.clear();

        // Register click callback function for placing path
        _window->register_data((void *)this);

        // Register left click events
        _window->register_lclick_down(controls::left_click_down);
        _window->register_lclick_up(controls::left_click_up);
        _window->register_rclick_down(controls::right_click_down);
        _window->register_rclick_up(controls::right_click_up);
        _window->register_update(controls::on_resize);

        // Add FPS(WADS) keys to watch
        keyboard.add(min::window::key_code::F1);
        keyboard.add(min::window::key_code::F2);
        keyboard.add(min::window::key_code::F3);
        keyboard.add(min::window::key_code::F4);
        keyboard.add(min::window::key_code::ESCAPE);
        keyboard.add(min::window::key_code::KEYQ);
        keyboard.add(min::window::key_code::KEYW);
        keyboard.add(min::window::key_code::KEYS);
        keyboard.add(min::window::key_code::KEYA);
        keyboard.add(min::window::key_code::KEYD);
        keyboard.add(min::window::key_code::KEYE);
        keyboard.add(min::window::key_code::KEYZ);
        keyboard.add(min::window::key_code::KEYX);
        keyboard.add(min::window::key_code::KEYC);
        keyboard.add(min::window::key_code::KEY1);
        keyboard.add(min::window::key_code::KEY2);
        keyboard.add(min::window::key_code::KEY3);
        keyboard.add(min::window::key_code::KEY4);
        keyboard.add(min::window::key_code::KEY5);
        keyboard.add(min::window::key_code::KEY6);
        keyboard.add(min::window::key_code::KEY7);
        keyboard.add(min::window::key_code::KEY8);
        keyboard.add(min::window::key_code::SPACE);

        // Register callback function F1
        keyboard.register_keydown(min::window::key_code::F1, controls::close_window, (void *)_window);

        // Register callback function F2
        keyboard.register_keydown(min::window::key_code::F2, controls::toggle_text, (void *)_ui);

        // Register callback function F3
        keyboard.register_keydown(min::window::key_code::F3, controls::music_down, (void *)_sound);

        // Register callback function F4
        keyboard.register_keydown(min::window::key_code::F4, controls::music_up, (void *)_sound);

        // Register callback function ESCAPE
        keyboard.register_keydown(min::window::key_code::ESCAPE, controls::toggle_pause, (void *)this);

        // Register callback function Q
        keyboard.register_keydown(min::window::key_code::KEYQ, controls::toggle_edit_mode, (void *)this);

        // Register callback function W
        keyboard.register_keydown_per_frame(min::window::key_code::KEYW, controls::forward, (void *)this);

        // Register callback function S
        keyboard.register_keydown_per_frame(min::window::key_code::KEYS, controls::back, (void *)this);

        // Register callback function A
        keyboard.register_keydown_per_frame(min::window::key_code::KEYA, controls::left, (void *)this);

        // Register callback function D
        keyboard.register_keydown_per_frame(min::window::key_code::KEYD, controls::right, (void *)this);

        // Register callback function E
        keyboard.register_keydown(min::window::key_code::KEYE, controls::reset, (void *)this);

        // Register callback function Z
        keyboard.register_keydown(min::window::key_code::KEYZ, controls::add_x, (void *)_world);

        // Register callback function X
        keyboard.register_keydown(min::window::key_code::KEYX, controls::add_y, (void *)_world);

        // Register callback function C
        keyboard.register_keydown(min::window::key_code::KEYC, controls::add_z, (void *)_world);

        // Register callback function KEY1 for switching texture
        keyboard.register_keydown(min::window::key_code::KEY1, controls::key1_down, (void *)this);
        keyboard.register_keyup(min::window::key_code::KEY1, controls::key1_up, (void *)this);

        // Register callback function KEY2 for switching texture
        keyboard.register_keydown(min::window::key_code::KEY2, controls::key2_down, (void *)this);
        keyboard.register_keyup(min::window::key_code::KEY2, controls::key2_up, (void *)this);

        // Register callback function KEY3 for switching texture
        keyboard.register_keydown(min::window::key_code::KEY3, controls::key3_down, (void *)this);
        keyboard.register_keyup(min::window::key_code::KEY3, controls::key3_up, (void *)this);

        // Register callback function KEY4 for switching texture
        keyboard.register_keydown(min::window::key_code::KEY4, controls::key4_down, (void *)this);
        keyboard.register_keyup(min::window::key_code::KEY4, controls::key4_up, (void *)this);

        // Register callback function KEY5 for switching texture
        keyboard.register_keydown(min::window::key_code::KEY5, controls::key5_down, (void *)this);
        keyboard.register_keyup(min::window::key_code::KEY5, controls::key5_up, (void *)this);

        // Register callback function KEY6 for switching texture
        keyboard.register_keydown(min::window::key_code::KEY6, controls::key6_down, (void *)this);
        keyboard.register_keyup(min::window::key_code::KEY6, controls::key6_up, (void *)this);

        // Register callback function KEY7 for switching texture
        keyboard.register_keydown(min::window::key_code::KEY7, controls::key7_down, (void *)this);
        keyboard.register_keyup(min::window::key_code::KEY7, controls::key7_up, (void *)this);

        // Register callback function KEY8 for switching texture
        keyboard.register_keydown(min::window::key_code::KEY8, controls::key8_down, (void *)this);
        keyboard.register_keyup(min::window::key_code::KEY8, controls::key8_up, (void *)this);

        // Register callback function SPACE
        keyboard.register_keydown(min::window::key_code::SPACE, controls::jump, (void *)_world);
    }
    static void close_window(void *ptr, double step)
    {
        // Cast to window pointer type and call shut down on window
        min::window *const win = reinterpret_cast<min::window *>(ptr);
        win->set_shutdown();

        // Alert that we received the call back
        std::cout << "controls: Shutdown called by user" << std::endl;
    }
    static void toggle_text(void *ptr, double step)
    {
        // Cast to ui pointer type and toggle draw
        ui_overlay *const ui = reinterpret_cast<ui_overlay *>(ptr);

        // Enable / Disable drawing debug text
        ui->toggle_debug_text();
    }
    static void music_down(void *ptr, double step)
    {
        // Get the sound pointer
        sound *const sound = reinterpret_cast<game::sound *>(ptr);

        // Decrease the music gain
        sound->bg_gain_down();
    }
    static void music_up(void *ptr, double step)
    {
        // Get the sound pointer
        sound *const sound = reinterpret_cast<game::sound *>(ptr);

        // Increase the music gain
        sound->bg_gain_up();
    }
    static void toggle_pause(void *ptr, double step)
    {
        // Get the state, text, window and ui pointers
        controls *const control = reinterpret_cast<controls *>(ptr);
        state *const state = control->get_state();
        min::window *const win = control->get_window();
        ui_overlay *const ui = control->get_ui();

        // Toggle the game pause
        const bool mode = state->toggle_game_pause();

        // set the game mode caption
        if (mode)
        {
            // Turn on cursor
            win->display_cursor(true);

            // Set state text to pause
            state->set_game_mode("MODE: PAUSE");

            // Turn on the menu
            ui->set_draw_menu(true);
        }
        else
        {
            // Turn off cursor
            win->display_cursor(false);

            // Set state text to play
            state->set_game_mode("MODE: PLAY");

            // Turn off the menu
            ui->set_draw_menu(false);
        }

        // Center cursor in middle of window
        win->set_cursor(win->get_width() / 2, win->get_height() / 2);
    }
    static void toggle_edit_mode(void *ptr, double step)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the world and state pointers
        world *const world = control->get_world();
        skills &skill = world->get_player().get_skills();

        // toggle edit mode
        const bool mode = world->toggle_edit_mode();

        // toggle fire mode if not in edit mode
        skill.set_gun_active(!mode);

        // reset scale
        world->reset_scale();
    }
    static void forward(void *ptr, double step)
    {
        // Get the camera and world pointers
        controls *const control = reinterpret_cast<controls *>(ptr);
        min::camera<float> *const camera = control->get_camera();
        world *const world = control->get_world();

        // Move the character
        const min::vec3<float> &direction = camera->get_forward();
        world->get_player().move(direction);
    }
    static void left(void *ptr, double step)
    {
        // Get the camera and world pointers
        controls *const control = reinterpret_cast<controls *>(ptr);
        min::camera<float> *const camera = control->get_camera();
        world *const world = control->get_world();

        // Move the character
        const min::vec3<float> &right = camera->get_frustum().get_right();
        world->get_player().move(right * -1.0);
    }
    static void right(void *ptr, double step)
    {
        // Get the camera and world pointers
        controls *const control = reinterpret_cast<controls *>(ptr);
        min::camera<float> *const camera = control->get_camera();
        world *const world = control->get_world();

        // Move the character
        const min::vec3<float> &right = camera->get_frustum().get_right();
        world->get_player().move(right);
    }
    static void back(void *ptr, double step)
    {
        // Get the camera and world pointers
        controls *const control = reinterpret_cast<controls *>(ptr);
        min::camera<float> *const camera = control->get_camera();
        world *const world = control->get_world();

        // Move the character
        const min::vec3<float> &direction = camera->get_forward();
        world->get_player().move(direction * -1.0);
    }
    void key_down(const size_t index, const std::function<void(void)> &f)
    {
        // Get the skills pointer
        skills &skill = _world->get_player().get_skills();

        // If gun is active
        if (!skill.is_locked() && skill.is_gun_active())
        {
            // Modify ui toolbar
            _ui->set_key_down(index);

            // Play selection sound
            _sound->play_click();

            // Do something
            if (f)
            {
                f();
            }
        }
        else if (_world->get_edit_mode())
        {
            _world->set_atlas_id(index);
        }
        else
        {
            // Failed to change state
            _ui->set_key_down_fail(index);
        }
    }
    static void key1_down(void *ptr, double step)
    {
        // Get the skills pointer
        controls *const control = reinterpret_cast<controls *>(ptr);
        world *const world = control->get_world();
        skills &skill = world->get_player().get_skills();
        ui_overlay *const ui = control->get_ui();

        // Switch to beam weapon type
        const auto f = [&skill, ui]() {

            // Load beam string in console
            ui->set_console_string(skill.get_beam_string());
            skill.set_beam_mode();
        };

        control->key_down(0, f);
    }
    static void key2_down(void *ptr, double step)
    {
        // Get the skills pointer
        controls *const control = reinterpret_cast<controls *>(ptr);
        world *const world = control->get_world();
        skills &skill = world->get_player().get_skills();
        ui_overlay *const ui = control->get_ui();

        // Switch to missile weapon type
        const auto f = [&skill, ui]() {

            // Load missile string in console
            ui->set_console_string(skill.get_missile_string());
            skill.set_missile_mode();
        };

        control->key_down(1, f);
    }
    static void key3_down(void *ptr, double step)
    {
        // Get the skills pointer
        controls *const control = reinterpret_cast<controls *>(ptr);
        world *const world = control->get_world();
        skills &skill = world->get_player().get_skills();
        ui_overlay *const ui = control->get_ui();

        // Switch to grapple weapon type
        const auto f = [&skill, ui]() {

            // Load grapple string in console
            ui->set_console_string(skill.get_grapple_string());
            skill.set_grapple_mode();
        };

        control->key_down(2, f);
    }
    static void key4_down(void *ptr, double step)
    {
        // Get the skills pointer
        controls *const control = reinterpret_cast<controls *>(ptr);
        world *const world = control->get_world();
        skills &skill = world->get_player().get_skills();
        ui_overlay *const ui = control->get_ui();

        // Switch to jet weapon type
        const auto f = [&skill, ui]() {

            // Load jet string in console
            ui->set_console_string(skill.get_jet_string());
            skill.set_jetpack_mode();
        };

        control->key_down(3, f);
    }
    static void key5_down(void *ptr, double step)
    {
        // Get the skills pointer
        controls *const control = reinterpret_cast<controls *>(ptr);
        world *const world = control->get_world();
        skills &skill = world->get_player().get_skills();
        ui_overlay *const ui = control->get_ui();

        // Switch to scan weapon type
        const auto f = [&skill, ui]() {

            // Load scan string in console
            ui->set_console_string(skill.get_scan_string());
            skill.set_scan_mode();
        };

        control->key_down(4, f);
    }
    static void key6_down(void *ptr, double step)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);
        control->key_down(5, nullptr);
    }
    static void key7_down(void *ptr, double step)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);
        control->key_down(6, nullptr);
    }
    static void key8_down(void *ptr, double step)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);
        control->key_down(7, nullptr);
    }
    static void key1_up(void *ptr, double step)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the ui pointer
        ui_overlay *const ui = control->get_ui();
        ui->set_key_up(0);
    }
    static void key2_up(void *ptr, double step)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the ui pointer
        ui_overlay *const ui = control->get_ui();
        ui->set_key_up(1);
    }
    static void key3_up(void *ptr, double step)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the ui pointer
        ui_overlay *const ui = control->get_ui();
        ui->set_key_up(2);
    }
    static void key4_up(void *ptr, double step)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the ui pointer
        ui_overlay *const ui = control->get_ui();
        ui->set_key_up(3);
    }
    static void key5_up(void *ptr, double step)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the ui pointer
        ui_overlay *const ui = control->get_ui();
        ui->set_key_up(4);
    }
    static void key6_up(void *ptr, double step)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the ui pointer
        ui_overlay *const ui = control->get_ui();
        ui->set_key_up(5);
    }
    static void key7_up(void *ptr, double step)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the ui pointer
        ui_overlay *const ui = control->get_ui();
        ui->set_key_up(6);
    }
    static void key8_up(void *ptr, double step)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the ui pointer
        ui_overlay *const ui = control->get_ui();
        ui->set_key_up(7);
    }
    static void add_x(void *ptr, double step)
    {
        // Cast to world pointer type
        world *const world = reinterpret_cast<game::world *>(ptr);

        // Increase x scale
        world->set_scale_x(1);
    }
    static void add_y(void *ptr, double step)
    {
        // Cast to world pointer type
        world *const world = reinterpret_cast<game::world *>(ptr);

        // Increase x scale
        world->set_scale_y(1);
    }
    static void add_z(void *ptr, double step)
    {
        // Cast to world pointer type
        world *const world = reinterpret_cast<game::world *>(ptr);

        // Increase x scale
        world->set_scale_z(1);
    }
    static void reset(void *ptr, double step)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the world pointer
        world *const world = control->get_world();

        // Reset scale
        world->reset_scale();
    }
    static void left_click_down(void *ptr, const uint16_t x, const uint16_t y)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the ALL the pointers
        min::camera<float> *const cam = control->get_camera();
        character *const character = control->get_character();
        state *const state = control->get_state();
        world *const world = control->get_world();
        player &play = world->get_player();
        skills &skill = play.get_skills();
        sound *const sound = control->get_sound();

        // Are we dead?
        const bool dead = play.is_dead();
        if (dead)
        {
            // Signal for game to respawn
            state->set_respawn(true);

            // return early
            return;
        }

        // Activate gun skill if active
        if (skill.is_gun_active())
        {
            if (skill.is_beam_mode() && skill.is_off_cooldown())
            {
                if (skill.can_consume(_beam_charge_cost))
                {
                    // Play the charge sound
                    sound->play_charge();

                    // Record the start charge time
                    skill.start_charge();

                    // Lock the gun in beam mode
                    skill.lock();
                }
            }
            else if (skill.is_grapple_mode())
            {
                // Try to consume energy to power this resource
                if (skill.can_consume(_grapple_cost))
                {
                    // Calculate new point to add
                    const min::vec3<float> proj = cam->project_point(_project_dist);

                    // Create a ray from camera to destination
                    const min::ray<float, min::vec3> r(cam->get_position(), proj);

                    // Fire grappling hook
                    min::vec3<float> point;
                    const bool hit = world->hook_set(r, point);
                    if (hit)
                    {
                        // Consume energy
                        skill.consume(_grapple_cost);

                        // Activate grapple animation
                        character->set_animation_grapple(point);

                        // Play the grapple sound
                        sound->play_grapple();

                        // Lock the gun in beam mode
                        skill.lock();
                    }
                }
            }
            else if (skill.is_missile_mode() && skill.is_off_cooldown())
            {
                // Lock the gun in missile mode
                skill.lock();
            }
            else if (skill.is_jetpack_mode())
            {
                // Turn on the jets
                world->get_player().set_jet(true);

                // Play the jet sound
                sound->play_jet();

                // Lock the gun in jetpack mode
                skill.lock();
            }
            else if (skill.is_scan_mode())
            {
                // Lock the gun in scan mode
                skill.lock();
            }
        }
    }
    static void left_click_up(void *ptr, const uint16_t x, const uint16_t y)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the ALL the pointers
        min::camera<float> *const cam = control->get_camera();
        character *const character = control->get_character();
        ui_overlay *const ui = control->get_ui();
        world *const world = control->get_world();
        player &play = world->get_player();
        skills &skill = play.get_skills();
        sound *const sound = control->get_sound();

        // Are we dead?
        const bool dead = play.is_dead();
        if (dead && !skill.is_locked())
        {
            // return early
            return;
        }

        // Create explode callback to play sound
        const auto play_ex = [sound](const min::vec3<float> &point, min::body<float, min::vec3> &body) {

            // Play the missile explosion sound - !TODO!
            sound->play_miss_ex(point);
        };

        // Check if we are in edit mode
        const bool mode = world->get_edit_mode();
        if (mode)
        {
            // Try to consume energy to create this resource
            const bool consumed = skill.will_consume(_block_cost);
            if (consumed)
            {
                // Calculate new point to add
                const min::vec3<float> point = cam->project_point(_project_dist);

                // Create a ray from camera to destination
                const min::ray<float, min::vec3> r(cam->get_position(), point);

                // Add block to world
                world->add_block(r);
            }
        }
        else if (skill.is_gun_active())
        {
            // Abort the charge animation
            character->abort_animation_shoot();

            // Calculate point to remove from
            const min::vec3<float> point = cam->project_point(_project_dist);

            // Create a ray from camera to destination
            const min::ray<float, min::vec3> r(cam->get_position(), point);

            // If the gun is locked into a mode
            if (skill.is_locked())
            {
                // If we are in beam mode and charged up
                if (skill.is_beam_charged())
                {
                    // Expolode block with radius, get the ray target value
                    min::vec3<unsigned> explode_radius(3, 3, 3);
                    const int8_t value = world->explode_block(r, explode_radius, play_ex);
                    if (value >= 0)
                    {
                        // Consume energy
                        skill.consume(_beam_charge_cost);

                        // Activate shoot animation
                        character->set_animation_shoot();

                        // Start gun cooldown timer
                        skill.start_cooldown();
                    }

                    // Stop the charge sound
                    sound->stop_charge();

                    // Unlock the gun if beam mode
                    skill.unlock_beam();
                }
                else if (skill.is_beam_mode())
                {
                    // Remove block from world, get the removed block id
                    if (skill.can_consume(_beam_cost))
                    {
                        const int8_t block_id = world->explode_block(r, nullptr, 20.0);
                        if (block_id >= 0)
                        {
                            // Consume energy
                            skill.consume(_beam_cost);

                            // Activate shoot animation
                            character->set_animation_shoot();

                            // Play the shot sound
                            sound->play_shot();
                        }
                    }

                    // Stop the charge sound
                    sound->stop_charge();

                    // Unlock the gun if beam mode
                    skill.unlock_beam();
                }
                else if (skill.is_grapple_mode())
                {
                    //Abort grappling hook
                    world->get_player().hook_abort();

                    // Stop grapple animation
                    character->abort_animation_grapple();

                    // Stop the grapple sound
                    sound->stop_grapple();

                    // Unlock the gun if beam mode
                    skill.unlock_grapple();
                }
                else if (skill.is_missile_mode())
                {
                    // Try to consume energy to create missile
                    const bool consumed = skill.will_consume(_missile_cost);
                    if (consumed)
                    {
                        // Launch a missile
                        bool launched = world->launch_missile(r);
                        if (launched)
                        {
                            // Activate shoot animation
                            character->set_animation_shoot();

                            // Start gun cooldown timer
                            skill.start_cooldown();
                        }
                    }

                    // Unlock the gun if missile mode
                    skill.unlock_missile();
                }
                else if (skill.is_jetpack_mode())
                {
                    // Turn off the jets
                    world->get_player().set_jet(false);

                    // Stop the jet sound
                    sound->stop_jet();

                    // Unlock the gun if jetpack mode
                    skill.unlock_jetpack();
                }
                else if (skill.is_scan_mode())
                {
                    // Scan the block on this ray
                    const int8_t block_id = world->scan_block(r);
                    const std::string &text = skill.get_scan_desc(block_id);

                    // Set the console text
                    ui->set_console_string(text);

                    // Unlock the gun if scan mode
                    skill.unlock_scan();
                }
            }
        }
    }
    static void right_click_down(void *ptr, const uint16_t x, const uint16_t y)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the camera and state pointers
        min::camera<float> *const cam = control->get_camera();
        state *const state = control->get_state();
        world *const world = control->get_world();
        player &play = world->get_player();

        // Are we dead?
        const bool dead = play.is_dead();
        if (dead)
        {
            // Signal for game to respawn
            state->set_respawn(true);

            // return early
            return;
        }

        // Calculate point to remove from
        const min::vec3<float> point = cam->project_point(_project_dist);

        // Create a ray from camera to destination
        const min::ray<float, min::vec3> r(cam->get_position(), point);

        // Shoot a ray at a block and target it if we hit it
        min::vec3<float> target;
        const bool hit = world->target_block(r, target);
        if (hit)
        {
            // Set tracking of target
            state->track_target(target);
        }
    }
    static void right_click_up(void *ptr, const uint16_t x, const uint16_t y)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the state and skill pointers
        state *const state = control->get_state();
        world *const world = control->get_world();
        player &play = world->get_player();
        skills &skill = play.get_skills();

        // Are we dead?
        const bool dead = play.is_dead();
        if (dead && !skill.is_locked())
        {
            // return early
            return;
        }

        // Stop camera tracking
        state->abort_tracking();
    }
    static void jump(void *ptr, double step)
    {
        // Get the world pointer
        world *const world = reinterpret_cast<game::world *>(ptr);
        world->get_player().jump();
    }
    static void on_resize(void *ptr, const uint16_t width, const uint16_t height)
    {
        // Cast to control pointer
        controls *const control = reinterpret_cast<controls *>(ptr);

        // Get the camera and text pointer
        min::camera<float> *const camera = control->get_camera();
        ui_overlay *const ui = control->get_ui();

        // Get camera frustum
        auto &f = camera->get_frustum();

        // Update the aspect ratio
        f.set_aspect_ratio(width, height);
        f.make_dirty();
        camera->make_dirty();

        // Update the screen size for ui and text
        ui->set_screen(width, height);
    }
    inline void update_energy_regen(const float dt)
    {
        // Regen some healthhealth
        player &player = _world->get_player();
        if (!player.is_dead())
        {
            // Rate is units / second
            player.add_health(_health_regen * dt);
        }

        // Regen energy
        skills &skill = _world->get_player().get_skills();
        if (!skill.is_locked())
        {
            // Rate is units / second
            skill.add_energy(_energy_regen * dt);
        }

        // Update the ui health bar
        _ui->set_health(player.get_health_percent());

        // Update the ui energy bar
        _ui->set_energy(skill.get_energy_percent());
    }
    inline void update_skills()
    {
        player &player = _world->get_player();
        skills &skill = player.get_skills();

        // Check if in jetpack mode
        if (skill.is_jetpack_mode() && skill.is_locked())
        {
            const bool active = player.is_jet();
            if (!active)
            {
                // Stop the jet sound
                _sound->stop_jet();

                // Unlock the jetpack
                skill.unlock_jetpack();
            }
        }

        // Activate charge animation
        if (skill.activate_charge())
        {
            _character->set_animation_charge(*_camera);
        }
    }
    inline void update_ui()
    {
        // Check cooldown state and update ui
        skills &skill = _world->get_player().get_skills();
        if (skill.check_cooldown())
        {
            _ui->set_target_cursor();
        }
        else
        {
            _ui->set_reload_cursor();
        }
    }
    void update(const float dt)
    {
        // Update energy regen
        update_energy_regen(dt);

        // Update ui
        update_ui();

        // Update skills
        update_skills();
    }
};
}

#endif
