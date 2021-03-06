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
 *   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *   POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef AMINO_RX_SCENE_KIN_H
#define AMINO_RX_SCENE_KIN_H

/**
 * @file scene_kin.h
 */




/**
 * A scenegraph subset.
 */
struct aa_rx_sg_sub;

/**
 * Destroy the scengraph subset.
 *
 * The original scenegraph will remain valid.
 */
AA_API void
aa_rx_sg_sub_destroy( struct aa_rx_sg_sub *sg );

/**
 * Return the number of configuration variables in the scenegraph subset.
 */
AA_API size_t
aa_rx_sg_sub_config_count( const struct aa_rx_sg_sub *sg_sub );


/**
 * Return the number of configuration variables in the full scenegraph.
 */
AA_API size_t
aa_rx_sg_sub_all_config_count( const struct aa_rx_sg_sub *sg_sub );

/**
 * Return the number of frames in the scenegraph subset.
 */
AA_API size_t
aa_rx_sg_sub_frame_count( const struct aa_rx_sg_sub *sg_sub );

/**
 * Return the full scenegraph config id for the i'th configuration of the sub-scenegraph.
 */
AA_API aa_rx_config_id
aa_rx_sg_sub_config( const struct aa_rx_sg_sub *sg_sub, size_t i );

/**
 * Return the full scenegraph frame id for the i'th frame of the sub-scenegraph.
 */
AA_API aa_rx_frame_id
aa_rx_sg_sub_frame( const struct aa_rx_sg_sub *sg_sub, size_t i );

/**
 * Return the array of full scenegraph config ids contained in the sub-scenegraph.
 */
AA_API aa_rx_config_id*
aa_rx_sg_sub_configs( const struct aa_rx_sg_sub *sg_sub );

/**
 * Return the array of full scenegraph frame ids contained in the sub-scenegraph.
 */
AA_API aa_rx_frame_id*
aa_rx_sg_sub_frames( const struct aa_rx_sg_sub *sg_sub );


AA_API void
aa_rx_sg_sub_config_get(
    const struct aa_rx_sg_sub *ssg,
    size_t n_all, const double *config_all,
    size_t n_subset, double *config_subset );

AA_API void
aa_rx_sg_sub_config_set(
    const struct aa_rx_sg_sub *ssg,
    size_t n_sub, const double *config_subset,
    size_t n_all, double *config_all
    );


/**
 * Create a sub-scenegraph for the kinematic chain starting at root and ending a tip.
 */
AA_API struct aa_rx_sg_sub *
aa_rx_sg_chain_create( const struct aa_rx_sg *sg,
                       aa_rx_frame_id root, aa_rx_frame_id tip );


/**
 * Fill q with the centered positions of each configuration.
 */
AA_API void
aa_rx_sg_sub_center_configs( const struct aa_rx_sg_sub *ssg,
                             size_t n, double *q );

/*-- Jacobians --*/

/**
 * Determine the size of the Jacobian matrix for the sub-scenegraph.
 */
AA_API void
aa_rx_sg_sub_jacobian_size( const struct aa_rx_sg_sub *ssg,
                            size_t *rows, size_t *cols );

/**
 * Compute the Jacobian matrix for the sub-scenegraph.
 */
AA_API void
aa_rx_sg_sub_jacobian( const struct aa_rx_sg_sub *ssg,
                       size_t n_tf, const double *TF_abs, size_t ld_TF,
                       double *J, size_t ld_J );

/*-- Kinematic Solvers --*/

/**
 * General type for an IK solver function
 */
typedef int aa_rx_ik_fun( void *context,
                          size_t n_tf, const double *TF, size_t ld_TF,
                          size_t n_q, double *q );


struct aa_rx_ksol_opts;

AA_API struct aa_rx_ksol_opts*
aa_rx_ksol_opts_create();

AA_API void
aa_rx_ksol_opts_destroy( struct aa_rx_ksol_opts *opts);

AA_API void
aa_rx_ksol_opts_set_dt( struct aa_rx_ksol_opts *opts, double dt);

AA_API void
aa_rx_ksol_opts_set_tol_angle( struct aa_rx_ksol_opts *opts, double tol);

AA_API void
aa_rx_ksol_opts_set_tol_trans( struct aa_rx_ksol_opts *opts, double tol);

AA_API void
aa_rx_ksol_opts_set_tol_angle_svd( struct aa_rx_ksol_opts *opts, double tol);

AA_API void
aa_rx_ksol_opts_set_tol_trans_svd( struct aa_rx_ksol_opts *opts, double tol);

AA_API void
aa_rx_ksol_opts_set_tol_dq( struct aa_rx_ksol_opts *opts, double tol);

AA_API void
aa_rx_ksol_opts_set_tol_k_dls( struct aa_rx_ksol_opts *opts, double s2min);

AA_API void
aa_rx_ksol_opts_set_tol_s2min( struct aa_rx_ksol_opts *opts, double s2min);

AA_API void
aa_rx_ksol_opts_set_gain_angle( struct aa_rx_ksol_opts *opts, double k );

AA_API void
aa_rx_ksol_opts_set_gain_trans( struct aa_rx_ksol_opts *opts, double k );

AA_API void
aa_rx_ksol_opts_set_max_iterations( struct aa_rx_ksol_opts *opts, size_t n );

AA_API void
aa_rx_ksol_opts_take_config( struct aa_rx_ksol_opts *opts, size_t n_q,
                             double *q, enum aa_mem_refop refop );

AA_API void
aa_rx_ksol_opts_take_gain_config( struct aa_rx_ksol_opts *opts, size_t n_q,
                                  double *q, enum aa_mem_refop refop );

AA_API void
aa_rx_ksol_opts_take_seed( struct aa_rx_ksol_opts *opts, size_t n_q,
                           double *q_all, enum aa_mem_refop refop );


AA_API void
aa_rx_ksol_opts_center_seed( struct aa_rx_ksol_opts *opts,
                             const struct aa_rx_sg_sub *ssg );

/**
 * Convenience function to set IK options to center joints
 */
AA_API void
aa_rx_ksol_opts_center_configs( struct aa_rx_ksol_opts *opts,
                                const struct aa_rx_sg_sub *ssg,
                                double gain );

/*-- Jacobian IK Solver --*/

struct aa_rx_ik_jac_cx;

AA_API struct aa_rx_ik_jac_cx *
aa_rx_ik_jac_cx_create(const struct aa_rx_sg_sub *ssg, const struct aa_rx_ksol_opts *opts );

AA_API void
aa_rx_ik_jac_cx_destroy( struct aa_rx_ik_jac_cx *cx );


AA_API int aa_rx_ik_jac_solve( const struct aa_rx_ik_jac_cx *context,
                               size_t n_tf, const double *TF, size_t ld_TF,
                               size_t n_q, double *q );

AA_API int aa_rx_ik_jac_fun( void *context,
                             size_t n_tf, const double *TF, size_t ld_TF,
                             size_t n_q, double *q );


/* AA_API int */
/* aa_rx_sg_sub_ksol_dls( const struct aa_rx_sg_sub *ssg, */
/*                        const struct aa_rx_ksol_opts *opts, */
/*                        size_t n_tf, const double *TF, size_t ld_TF, */
/*                        size_t n_q_all, const double *q_start_all, */
/*                        size_t n_q, double *q_subset ); */

/* static inline int */
/* aa_rx_sg_chain_ksol_dls( const struct aa_rx_sg_sub *ssg, */
/*                          const struct aa_rx_ksol_opts *opts, */
/*                          const double *TF, */
/*                          size_t n_q_all, const double *q_start_all, */
/*                          size_t n_qs, double *q_subset ) */
/* { */
/*     return aa_rx_sg_sub_ksol_dls( ssg, opts, 1, TF, 7, */
/*                                   n_q_all, q_start_all, */
/*                                   n_qs, q_subset ); */
/* } */


#endif /*AMINO_RX_SCENE_KIN_H*/
