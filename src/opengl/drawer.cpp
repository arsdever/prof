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

    struct init_context
    {
        bool     _initialized        = false;
        bool     _initialized_shader = false;
        unsigned vao                 = 0;
        unsigned vbo                 = 0;
        unsigned ebo                 = 0;
        unsigned prog                = 0;
        unsigned vs                  = 0;
        unsigned fs                  = 0;
    };

    init_context frames_init_ctx {};
    init_context overall_init_ctx {};

    void load_mesh(init_context& ctx)
    {
        if (!ctx._initialized)
            {
                glGenVertexArrays(1, &ctx.vao);
                glBindVertexArray(ctx.vao);
                glGenBuffers(1, &ctx.vbo);
                glGenBuffers(1, &ctx.ebo);
                glBindBuffer(GL_ARRAY_BUFFER, ctx.vbo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx.ebo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts.data(), GL_STATIC_DRAW);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
                glEnableVertexAttribArray(0);
                ctx._initialized = true;
            }
        else
            {
                glBindVertexArray(ctx.vao);
                glEnableVertexAttribArray(0);
            }
    }

    template <typename T>
    bool load_vertex_shader(init_context& ctx)
    {
        int status;
        if (ctx.vs == 0)
            {
                ctx.vs            = glCreateShader(GL_VERTEX_SHADER);
                const char* vsptr = T::vertex_shader.data();
                glShaderSource(ctx.vs, 1, &vsptr, 0);
                glCompileShader(ctx.vs);
                glGetShaderiv(ctx.vs, GL_COMPILE_STATUS, &status);
                if (status == GL_FALSE)
                    {
                        int  log_length = 0;
                        char msg[ 1024 ];
                        glGetShaderInfoLog(ctx.vs, 1024, &log_length, msg);
                        glDeleteShader(ctx.vs);
                        ctx.vs = 0;
                        std::cerr << msg;
                        return false;
                    }
            }
        return true;
    }

    template <typename T>
    bool load_fragment_shader(init_context& ctx)
    {
        int status;
        if (ctx.fs == 0)
            {
                ctx.fs            = glCreateShader(GL_FRAGMENT_SHADER);
                const char* fsptr = T::fragment_shader.data();
                glShaderSource(ctx.fs, 1, &fsptr, 0);
                glCompileShader(ctx.fs);
                glGetShaderiv(ctx.fs, GL_COMPILE_STATUS, &status);
                if (status == GL_FALSE)
                    {
                        int  log_length = 0;
                        char msg[ 1024 ];
                        glGetShaderInfoLog(ctx.fs, 1024, &log_length, msg);
                        glDeleteShader(ctx.fs);
                        ctx.fs = 0;
                        std::cerr << msg;
                        return false;
                    }
            }
        return true;
    }

    template <typename T>
    bool load_program(init_context& ctx)
    {
        if (!ctx._initialized_shader)
            {
                if (!load_vertex_shader<T>(ctx))
                    {
                        return false;
                    }
                if (!load_fragment_shader<T>(ctx))
                    {
                        return false;
                    }
                int status;
                if (ctx.prog == 0)
                    {
                        ctx.prog = glCreateProgram();
                        glAttachShader(ctx.prog, ctx.vs);
                        glAttachShader(ctx.prog, ctx.fs);

                        glLinkProgram(ctx.prog);
                        glGetProgramiv(ctx.prog, GL_LINK_STATUS, &status);
                        if (status == GL_FALSE)
                            {
                                int  log_length = 0;
                                char msg[ 1024 ];
                                glGetProgramInfoLog(ctx.prog, 1024, &log_length, msg);
                                glDeleteProgram(ctx.prog);
                                ctx.prog = 0;
                                std::cerr << msg;
                                return false;
                            }
                    }
                glUseProgram(ctx.prog);
                ctx._initialized_shader = true;
            }
        else
            {
                glUseProgram(ctx.prog);
            }
        return true;
    }

    void draw_overall_data(std::string_view thread_id, const draw_data& dd)
    {
        load_mesh(overall_init_ctx);
        if (!load_program<overall_shaders>(overall_init_ctx))
            {
                return;
            }

        struct section
        {
            float time;
        };

        std::vector<section> data;
        prof::apply_frames(thread_id, [ &data ](const prof::frame& fr) -> bool {
            data.emplace_back(std::chrono::duration_cast<std::chrono::duration<float>>(fr.end() - fr.start()).count());
            return true;
        });

        glUniform2f(
            glGetUniformLocation(overall_init_ctx.prog, "zoom"), static_cast<float>(.01), static_cast<float>(100));
        glUniform2f(glGetUniformLocation(overall_init_ctx.prog, "_screen_size"),
                    static_cast<float>(dd.width),
                    static_cast<float>(dd.height));

        // glUniform1f(glGetUniformLocation(init_context.prog, "time_offset"), offset);
        glViewport(0, 0, dd.width, dd.height);
        glClear(GL_COLOR_BUFFER_BIT);

        size_t i = 0;
        int    k = 0;
        while (i < data.size())
            {
                size_t j = 0;
                while (i < data.size() && j < MAX_SECTIONS_COUNT)
                    {
                        glUniform1f(
                            glGetUniformLocation(overall_init_ctx.prog,
                                                 (std::string("_sections[") + std::to_string(j) + "].time").c_str()),
                            data[ i ].time);
                        ++i;
                        ++j;
                    }

                glUniform1i(glGetUniformLocation(overall_init_ctx.prog, "_frame_offset"), k);
                glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, std::min<size_t>(j, MAX_SECTIONS_COUNT));
                ++k;
            }
    }

    void draw_frame_data(std::string_view thread_id, uint64_t frame_id, const draw_data& dd)
    {
        load_mesh(frames_init_ctx);
        if (!load_program<frame_shaders>(frames_init_ctx))
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
        prof::apply_frame_data(thread_id, frame_id, [ &offset, &elements_d ](const prof::data_sample& sample) -> bool {
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

        glUniform2f(glGetUniformLocation(frames_init_ctx.prog, "zoom"),
                    static_cast<float>(dd.zoom_x),
                    static_cast<float>(dd.zoom_y));
        glUniform2f(glGetUniformLocation(frames_init_ctx.prog, "_screen_size"),
                    static_cast<float>(dd.width),
                    static_cast<float>(dd.height));

        // glUniform1f(glGetUniformLocation(init_context.prog, "time_offset"), offset);
        for (int i = 0; i < elements.size() % MAX_SECTIONS_COUNT; ++i)
            {
                glUniform1f(
                    glGetUniformLocation(frames_init_ctx.prog,
                                         (std::string("_sections[") + std::to_string(i) + "].duration").c_str()),
                    elements[ i ].duration);
                glUniform1f(glGetUniformLocation(frames_init_ctx.prog,
                                                 (std::string("_sections[") + std::to_string(i) + "].start").c_str()),
                            elements[ i ].start);
                glUniform1i(glGetUniformLocation(frames_init_ctx.prog,
                                                 (std::string("_sections[") + std::to_string(i) + "].layer").c_str()),
                            elements[ i ].layer);
            }

        glViewport(0, 0, dd.width, dd.height);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, elements.size() % MAX_SECTIONS_COUNT);
    }

} // namespace prof
