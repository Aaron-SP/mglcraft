/* Copyright [2013-2018] [Aaron Springstroh, Minimal Graphics Library]

This file is part of the Beyond Dying Skies.

Beyond Dying Skies is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Beyond Dying Skies is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Beyond Dying Skies.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __UI_TEXT__
#define __UI_TEXT__

#include <game/memory_map.h>
#include <iomanip>
#include <min/program.h>
#include <min/shader.h>
#include <min/text_buffer.h>
#include <sstream>
#include <vector>

namespace game
{

class ui_text
{
  private:
    static constexpr size_t _console = 0;
    static constexpr size_t _ui = _console + 1;
    static constexpr size_t _error = _ui + 2;
    static constexpr size_t _debug = _error + 1;
    static constexpr size_t _end = _debug + 10;
    static constexpr float _y_console = 90.0;
    static constexpr float _y_error = 180.0;
    static constexpr float _x_console_wrap = 400.0;
    static constexpr float _y_console_wrap = 40.0;
    static constexpr float _x_health = 248.0;
    static constexpr float _x_energy = 200.0;
    static constexpr float _y_ui = 150.0;

    // Text OpenGL stuff
    min::shader _vertex;
    min::shader _fragment;
    min::program _prog;

    // Buffer for holding text
    min::text_buffer _text;
    min::text_buffer _text_bg;
    std::vector<size_t> _indices;
    std::ostringstream _stream;
    size_t _font_size;
    bool _draw_console;
    bool _draw_debug;
    bool _draw_error;
    bool _draw_ui;

    inline void add_text(const std::string &s, const float x, const float y)
    {
        const size_t index = _text.add_text(s, x, y);

        // Add text index to index buffer
        _indices.push_back(index);
    }
    inline void bind() const
    {
        // Bind the text_buffer vao, and textures on channel '1'
        _text.bind(0);

        // Bind the text program
        _prog.use();
    }
    inline void clear_stream()
    {
        _stream.clear();
        _stream.str(std::string());
    }
    inline void reposition_text(const uint16_t width, const uint16_t height)
    {
        // Position the console elements
        const uint16_t w2 = (width / 2);
        _text.set_text_center(_console, w2, _y_console);

        // Position the ui elements
        _text.set_text_location(_ui, w2 - _x_health, _y_ui);
        _text.set_text_location(_ui + 1, w2 + _x_energy, _y_ui);

        // Position error elements
        _text.set_text_center(_error, w2, height - _y_error);

        // Rescale all debug text
        uint16_t y = height - 20;
        for (size_t i = _debug; i < _end; i++)
        {
            // Update the text location
            _text.set_text_location(i, 10, y);
            y -= _font_size;
        }
    }
    inline void update_text(const size_t index, const std::string &s)
    {
        _text.set_text(s, index);
    }

  public:
    ui_text(const size_t font_size, const uint16_t width, const uint16_t height)
        : _vertex(memory_map::memory.get_file("data/shader/text.vertex"), GL_VERTEX_SHADER),
          _fragment(memory_map::memory.get_file("data/shader/text.fragment"), GL_FRAGMENT_SHADER),
          _prog(_vertex, _fragment),
          _text("data/fonts/open_sans.ttf", font_size),
          _text_bg("data/fonts/open_sans.ttf", 14),
          _font_size(font_size), _draw_console(false), _draw_debug(false), _draw_error(false), _draw_ui(false)
    {
        // Update the text buffer screen dimensions
        _text.set_screen(width, height);
        _text_bg.set_screen(width, height);

        // Add 1 console entries
        for (size_t i = _console; i < _ui; i++)
        {
            add_text("", 0, 0);
            _text.set_line_wrap(i, _x_console_wrap, _y_console_wrap);
        }

        // Add 2 ui entries
        for (size_t i = _ui; i < _error; i++)
        {
            add_text("", 0, 0);
        }

        // Add 1 error entry
        for (size_t i = _error; i < _debug; i++)
        {
            add_text("", 0, 0);
            _text.set_line_wrap(i, _x_console_wrap, _y_console_wrap);
        }

        // Add 11 debug entries
        for (size_t i = _debug; i < _end; i++)
        {
            add_text("", 0, 0);
        }

        // Reposition all of the text
        reposition_text(width, height);
    }
    void draw(const size_t bg_size) const
    {
        // Minimize draw calls by lumping togetherness
        if (_draw_console && _draw_ui && _draw_error && _draw_debug)
        {
            bind();
            _text.draw_all();
        }
        else if (_draw_console && _draw_ui && _draw_error && !_draw_debug)
        {
            bind();
            _text.draw(_console, _debug - 1);
        }
        else if (_draw_console && _draw_ui && !_draw_error && !_draw_debug)
        {
            bind();
            _text.draw(_console, _error - 1);
        }
        else if (_draw_console && _draw_ui && !_draw_error && _draw_debug)
        {
            bind();
            _text.draw(_console, _error - 1);
            _text.draw(_debug, _end - 1);
        }
        else if (_draw_console && !_draw_ui && !_draw_error && !_draw_debug)
        {
            bind();
            _text.draw(_console);
        }
        else
        {
            // For all other permutations
            bind();
            if (_draw_console)
            {
                _text.draw(_console, _ui - 1);
            }
            if (_draw_ui)
            {
                _text.draw(_ui, _error - 1);
            }
            if (_draw_error)
            {
                _text.draw(_error, _debug - 1);
            }
            if (_draw_debug)
            {
                _text.draw(_debug, _end - 1);
            }
        }

        // Draw the background text
        if (bg_size > 0)
        {
            _text_bg.bind(0);
            _text_bg.draw(0, bg_size - 1);
        }
    }
    inline min::text_buffer &get_bg_text()
    {
        return _text_bg;
    }
    inline bool is_draw_debug() const
    {
        return _draw_debug;
    }
    inline void set_draw_debug(const bool flag)
    {
        _draw_debug = flag;
    }
    inline void set_draw_console(const bool flag)
    {
        _draw_console = flag;
    }
    inline void set_draw_ui(const bool flag)
    {
        _draw_ui = flag;
    }
    inline void set_draw_error(const bool flag)
    {
        _draw_error = flag;
    }
    inline void set_screen(const uint16_t width, const uint16_t height)
    {
        // Update the text buffer screen dimensions
        _text.set_screen(width, height);

        // Rescale all text on the screen
        reposition_text(width, height);

        // Upload new text
        upload();
    }
    inline void set_debug_title(const char *title)
    {
        // Clear and reset the stream
        clear_stream();

        // Title text
        _stream << title;
        update_text(_debug, _stream.str());
    }
    inline void set_debug_vendor(const char *vendor)
    {
        // Clear and reset the stream
        clear_stream();

        // Vendor text
        _stream << vendor;
        update_text(_debug + 1, _stream.str());
    }
    inline void set_debug_renderer(const char *renderer)
    {
        // Clear and reset the stream
        clear_stream();

        // Renderer text
        _stream << renderer;
        update_text(_debug + 2, _stream.str());
    }
    inline void set_debug_position(const min::vec3<float> &p)
    {
        // Clear and reset the _stream
        clear_stream();

        // Update player position debug text
        _stream << std::fixed << std::setprecision(4) << "POS- X: " << p.x() << ", Y: " << p.y() << ", Z: " << p.z();
        update_text(_debug + 3, _stream.str());
    }
    inline void set_debug_direction(const min::vec3<float> &dir)
    {
        // Clear and reset the _stream
        clear_stream();

        // Update player direction debug text
        _stream << "DIR- X: " << dir.x() << ", Y: " << dir.y() << ", Z: " << dir.z();
        update_text(_debug + 4, _stream.str());
    }
    inline void set_debug_health(const float health)
    {
        // Clear and reset the _stream
        clear_stream();

        // Update the energy text
        _stream << "HEALTH: " << health;
        update_text(_debug + 5, _stream.str());
    }
    inline void set_debug_energy(const float energy)
    {
        // Clear and reset the _stream
        clear_stream();

        // Update the energy text
        _stream << "ENERGY: " << energy;
        update_text(_debug + 6, _stream.str());
    }
    inline void set_debug_fps(const float fps)
    {
        // Clear and reset the _stream
        clear_stream();

        // Update FPS and IDLE
        _stream << "FPS: " << std::round(fps);
        update_text(_debug + 7, _stream.str());
    }
    inline void set_debug_idle(const double idle)
    {
        // Clear and reset the _stream
        clear_stream();

        // Update FPS and IDLE
        _stream << "IDLE: " << idle;
        update_text(_debug + 8, _stream.str());
    }
    inline void set_debug_chunks(const size_t chunks)
    {
        // Clear and reset the _stream
        clear_stream();

        // Update FPS and IDLE
        _stream << "CHUNKS: " << chunks;
        update_text(_debug + 9, _stream.str());
    }
    inline void toggle_draw_console()
    {
        _draw_console = !_draw_console;
    }
    inline void toggle_draw_debug()
    {
        _draw_debug = !_draw_debug;
    }
    inline void update_console(const std::string &str)
    {
        // Update console text
        update_text(_console, str);

        // Get the screen dimensions
        const std::pair<float, float> size = _text.get_screen_size();

        // Position the console elements
        const uint16_t w2 = (size.first / 2);
        _text.set_text_center(_console, w2, _y_console);
    }
    inline void update_ui(const float health, const float energy)
    {
        // Clear and reset the _stream
        clear_stream();

        // Update the energy text
        _stream << static_cast<int>(std::round(health));
        update_text(_ui, _stream.str());

        // Clear and reset the _stream
        clear_stream();

        // Update the energy text
        _stream << static_cast<int>(std::round(energy));
        update_text(_ui + 1, _stream.str());
    }
    inline void update_ui_error(const std::string &error)
    {
        // Update the error text
        update_text(_error, error);

        // Get the screen dimensions
        const std::pair<float, float> size = _text.get_screen_size();

        // Position the console elements
        const uint16_t w2 = (size.first / 2);
        _text.set_text_center(_error, w2, size.second - _y_error);
    }
    inline void upload() const
    {
        // Unbind the last VAO to prevent scrambling buffers
        _text.unbind();

        // Upload the text glyphs to the GPU
        _text.upload();
    }
};
}

#endif
