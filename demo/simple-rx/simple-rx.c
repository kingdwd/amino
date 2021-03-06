/* -*- mode: C; c-basic-offset: 4; -*- */
/* ex: set shiftwidth=4 tabstop=4 expandtab: */
/*
 * Copyright (c) 2015, Rice University
 * All rights reserved.
 *
 * Author(s): Neil T. Dantam <ntd@rice.edu>
 *
 *   Redistribution and use in source and binary forms, with or
 *   without modification, are permitted provided that the following
 *   conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of copyright holder the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *   USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *   AND ON ANY HEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *   POSSIBILITY OF SUCH DAMAGE.
 *
 */
#define GL_GLEXT_PROTOTYPES

#include <error.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL.h>

#include "amino.h"
#include "amino/rx/rxtype.h"
#include "amino/rx/scenegraph.h"
#include "amino/rx/scene_gl.h"
#include "amino/rx/scene_sdl.h"
#include "amino/rx/scene_geom.h"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;


struct aa_rx_sg *aa_rx_dl_sg__scenegraph(struct aa_rx_sg *sg);
struct aa_rx_sg *scenegraph;

void check_error( const char *name ){
    for (GLenum err = glGetError(); err != GL_NO_ERROR; err = glGetError()) {
        fprintf(stderr, "error %s: %d: %s\n",  name,  (int)err, gluErrorString(err));
    }
}

int display( void *context, struct aa_sdl_display_params *params)
{
    const struct aa_gl_globals *globals = (const struct aa_gl_globals *) context;

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    check_error("glClearColor");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    check_error("glClear");

    aa_rx_frame_id n = aa_rx_sg_frame_count(scenegraph);
    aa_rx_frame_id m = aa_rx_sg_config_count(scenegraph);
    double q[m];
    AA_MEM_ZERO(q,m);
    double TF_rel[7*n];
    double TF_abs[7*n];
    aa_rx_sg_tf(scenegraph, m, q,
                n,
                TF_rel, 7,
                TF_abs, 7 );
    aa_rx_sg_render( scenegraph, globals,
                     (size_t)n, TF_abs, 7 );
    return 0;
}

int main(int argc, char *argv[])
{
    (void)argc; (void)argv;

    SDL_Window* window = NULL;
    SDL_GLContext gContext = NULL;

    aa_sdl_gl_window( "SDL Test",
                      SDL_WINDOWPOS_UNDEFINED,
                      SDL_WINDOWPOS_UNDEFINED,
                      SCREEN_WIDTH,
                      SCREEN_HEIGHT,
                      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
                      &window, &gContext);

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    // Initialize scene graph
    scenegraph = aa_rx_dl_sg__scenegraph(NULL);

    // mesh
    {
        double v[3] = {0,0,-1e-3};
        aa_rx_sg_add_frame_fixed(scenegraph, "", "mesh", aa_tf_quat_ident, v);
        struct aa_rx_mesh * mesh = aa_rx_mesh_create();
#define F .5
        static const float vertices[4*3] = { -F,-F,0,
                                             -F,F,0,
                                             F,-F,0,
                                             F,F,0 };
        static const float normals[4*3] = { 0,0,1,
                                            0,0,1,
                                            0,0,1,
                                            0,0,1 };
        static const unsigned indices [3*2] = {0, 1, 2,
                                               3,1,2};
        aa_rx_mesh_set_vertices(mesh, 4, vertices, 0);
        aa_rx_mesh_set_normals(mesh, 4, normals, 0);
        aa_rx_mesh_set_indices(mesh, 2, indices, 0);

        struct aa_rx_geom * geom;
        struct aa_rx_geom_opt * opt = aa_rx_geom_opt_create();
        double d = .5;
        aa_rx_geom_opt_set_color3(opt, d*.91, d*.96, d*.88);

        aa_rx_geom_opt_set_alpha(opt, 1.0);
        aa_rx_geom_opt_set_visual(opt, 1);
        aa_rx_geom_opt_set_collision(opt, 0);
        aa_rx_geom_opt_set_no_shadow(opt, 0);
        aa_rx_mesh_set_texture(mesh, opt);
        (geom) = (aa_rx_geom_mesh(opt, mesh));
        aa_rx_geom_attach(scenegraph, "mesh", geom);
        aa_rx_geom_opt_destroy(opt);
    }

    aa_rx_sg_init(scenegraph);
    aa_rx_sg_gl_init(scenegraph);

    // Initialize globals
    struct aa_gl_globals *globals = aa_gl_globals_create();
    // global camera
    {
        double world_E_camera_home[7] = AA_TF_QUTR_IDENT_INITIALIZER;
        double eye[3] = {1,1,0.5};
        double target[3] = {0,0,0};
        double up[3] = {0,0,1};
        aa_tf_qutr_mzlook( eye, target, up, world_E_camera_home );
        aa_gl_globals_set_camera_home( globals, world_E_camera_home );
        aa_gl_globals_home_camera( globals );

    }

    // global lighting
    {
        double v_light[3] = {.5,1,5};
        double ambient[3] = {.1,.1,.1};
        aa_gl_globals_set_light_position( globals, v_light );
        aa_gl_globals_set_ambient(globals, ambient);
    }


    aa_sdl_display_loop( window, globals,
                         display,
                         globals );

    SDL_GL_DeleteContext(gContext);
    SDL_DestroyWindow( window );

    SDL_Quit();
    return 0;
}
