#include <array>
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>

#include "prof/profiler.hpp"
#include "shader.hpp"
#include <glad/gl.h>

namespace prof
{
    static constexpr unsigned                    MAX_SECTIONS_COUNT { 256 };
    static constexpr std::array<float, 8>        verts { 0, 0, 0, 1, 1, 1, 1, 0 };
    static constexpr std::array<unsigned int, 6> indices { 0, 1, 2, 2, 3, 0 };

    struct section
    {
        float start;
        float duration;
        int   layer;
    };

    struct
    {
        bool     _initialized        = false;
        bool     _initialized_shader = false;
        unsigned vao                 = 0;
        unsigned vbo                 = 0;
        unsigned ebo                 = 0;
        unsigned prog                = 0;
        unsigned vs                  = 0;
        unsigned fs                  = 0;
    } init_context;

    void load_mesh()
    {
        if (!init_context._initialized)
            {
                glGenVertexArrays(1, &init_context.vao);
                glBindVertexArray(init_context.vao);
                glGenBuffers(1, &init_context.vbo);
                glGenBuffers(1, &init_context.ebo);
                glBindBuffer(GL_ARRAY_BUFFER, init_context.vbo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, init_context.ebo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts.data(), GL_STATIC_DRAW);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
                glEnableVertexAttribArray(0);
                init_context._initialized = true;
            }
        else
            {
                glBindVertexArray(init_context.vao);
                glEnableVertexAttribArray(0);
            }
    }

    bool load_vertex_shader()
    {
        int status;
        if (init_context.vs == 0)
            {
                init_context.vs   = glCreateShader(GL_VERTEX_SHADER);
                const char* vsptr = vertex_shader.data();
                glShaderSource(init_context.vs, 1, &vsptr, 0);
                glCompileShader(init_context.vs);
                glGetShaderiv(init_context.vs, GL_COMPILE_STATUS, &status);
                if (status == GL_FALSE)
                    {
                        int  log_length = 0;
                        char msg[ 1024 ];
                        glGetShaderInfoLog(init_context.vs, 1024, &log_length, msg);
                        glDeleteShader(init_context.vs);
                        init_context.vs = 0;
                        std::cerr << msg;
                        return false;
                    }
            }
        return true;
    }

    bool load_fragment_shader()
    {
        int status;
        if (init_context.fs == 0)
            {
                init_context.fs   = glCreateShader(GL_FRAGMENT_SHADER);
                const char* fsptr = fragment_shader.data();
                glShaderSource(init_context.fs, 1, &fsptr, 0);
                glCompileShader(init_context.fs);
                glGetShaderiv(init_context.fs, GL_COMPILE_STATUS, &status);
                if (status == GL_FALSE)
                    {
                        int  log_length = 0;
                        char msg[ 1024 ];
                        glGetShaderInfoLog(init_context.fs, 1024, &log_length, msg);
                        glDeleteShader(init_context.fs);
                        init_context.fs = 0;
                        std::cerr << msg;
                        return false;
                    }
            }
        return true;
    }

    bool load_program()
    {
        if (!init_context._initialized_shader)
            {
                if (!load_vertex_shader())
                    {
                        return false;
                    }
                if (!load_fragment_shader())
                    {
                        return false;
                    }
                int status;
                if (init_context.prog == 0)
                    {
                        init_context.prog = glCreateProgram();
                        glAttachShader(init_context.prog, init_context.vs);
                        glAttachShader(init_context.prog, init_context.fs);

                        glLinkProgram(init_context.prog);
                        glGetProgramiv(init_context.prog, GL_LINK_STATUS, &status);
                        if (status == GL_FALSE)
                            {
                                int  log_length = 0;
                                char msg[ 1024 ];
                                glGetProgramInfoLog(init_context.prog, 1024, &log_length, msg);
                                glDeleteProgram(init_context.prog);
                                init_context.prog = 0;
                                std::cerr << msg;
                                return false;
                            }
                    }
                glUseProgram(init_context.prog);
                init_context._initialized_shader = true;
            }
        else
            {
                glUseProgram(init_context.prog);
            }
        return true;
    }

    void draw_frame_data(std::string_view thread_id, const draw_data& dd)
    {
        load_mesh();
        if (!load_program())
            {
                return;
            }

        struct section_double
        {
            double start;
            double duration;
            int    layer;
        };

        std::vector<section_double> elements_d;
        std::vector<section>        elements;
        double                      offset = INFINITY;
        prof::apply_for_data(thread_id, [ &offset, &elements_d ](const prof::data_sample& sample) -> bool {
            elements_d.push_back(
                { std::chrono::duration_cast<std::chrono::duration<double>>(sample.start().time_since_epoch()).count(),
                  std::chrono::duration_cast<std::chrono::duration<double>>(sample.end() - sample.start()).count(),
                  static_cast<int>(sample.depth()) });
            offset = std::min<double>(offset, elements_d.back().start);
            return true;
        });

        for (auto& elem : elements_d)
            {
                elements.emplace_back(elem.start - offset, elem.duration, elem.layer);
            }

        glUniform2f(glGetUniformLocation(init_context.prog, "zoom"),
                    static_cast<float>(dd.zoom_x),
                    static_cast<float>(dd.zoom_y));
        glUniform2f(glGetUniformLocation(init_context.prog, "_screen_size"),
                    static_cast<float>(dd.width),
                    static_cast<float>(dd.height));

        // glUniform1f(glGetUniformLocation(init_context.prog, "time_offset"), offset);
        for (int i = 0; i < elements.size() % MAX_SECTIONS_COUNT; ++i)
            {
                glUniform1f(
                    glGetUniformLocation(init_context.prog,
                                         (std::string("_sections[") + std::to_string(i) + "].duration").c_str()),
                    elements[ i ].duration);
                glUniform1f(glGetUniformLocation(init_context.prog,
                                                 (std::string("_sections[") + std::to_string(i) + "].start").c_str()),
                            elements[ i ].start);
                glUniform1i(glGetUniformLocation(init_context.prog,
                                                 (std::string("_sections[") + std::to_string(i) + "].layer").c_str()),
                            elements[ i ].layer);
            }

        glViewport(0, 0, dd.width, dd.height);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, elements.size() % MAX_SECTIONS_COUNT);
    }

} // namespace prof
