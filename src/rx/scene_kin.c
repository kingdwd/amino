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

#include "amino.h"
#include "amino/rx/rxtype.h"
#include "amino/rx/scenegraph.h"
#include "amino/rx/scene_kin.h"
#include "amino/rx/scene_kin_internal.h"

AA_API void
aa_rx_sg_sub_destroy( struct aa_rx_sg_sub *ssg )
{
    if( ssg->frames ) free( ssg->frames );
    if( ssg->configs ) free( ssg->configs );

    free(ssg);
}

AA_API size_t
aa_rx_sg_sub_config_count( const struct aa_rx_sg_sub *sg_sub )
{
    return sg_sub->config_count;
}

AA_API size_t
aa_rx_sg_sub_all_config_count( const struct aa_rx_sg_sub *sg_sub )
{
    return aa_rx_sg_config_count( sg_sub->scenegraph );
}

AA_API size_t
aa_rx_sg_sub_frame_count( const struct aa_rx_sg_sub *sg_sub )
{
    return sg_sub->frame_count;
}

AA_API aa_rx_config_id
aa_rx_sg_sub_config( const struct aa_rx_sg_sub *sg_sub, size_t i )
{
    return sg_sub->configs[i];
}

AA_API aa_rx_frame_id
aa_rx_sg_sub_frame( const struct aa_rx_sg_sub *sg_sub, size_t i )
{
    return sg_sub->frames[i];
}

AA_API aa_rx_config_id*
aa_rx_sg_sub_configs( const struct aa_rx_sg_sub *sg_sub )
{
    return sg_sub->configs;
}

AA_API aa_rx_frame_id*
aa_rx_sg_sub_frames( const struct aa_rx_sg_sub *sg_sub )
{
    return sg_sub->frames;
}


AA_API size_t
aa_rx_sg_chain_frame_count( const struct aa_rx_sg *sg,
                            aa_rx_frame_id root, aa_rx_frame_id tip )
{
    size_t a = 0;
    while( tip != root && tip != AA_RX_FRAME_ROOT ) {
        enum aa_rx_frame_type ft = aa_rx_sg_frame_type( sg, tip );
        switch(ft) {
        case AA_RX_FRAME_FIXED:
            /* break; */
        case AA_RX_FRAME_REVOLUTE:
        case AA_RX_FRAME_PRISMATIC:
            a++;
            break;
        }
        tip = aa_rx_sg_frame_parent(sg, tip);
    }

    return a;
}

AA_API void
aa_rx_sg_chain_frames( const struct aa_rx_sg *sg,
                       aa_rx_frame_id root, aa_rx_frame_id tip,
                       size_t n_frames, aa_rx_frame_id *chain_frames )
{
    aa_rx_frame_id *ptr = chain_frames + n_frames - 1;

    while( tip != root && tip != AA_RX_FRAME_ROOT && ptr >= chain_frames ) {
        enum aa_rx_frame_type ft = aa_rx_sg_frame_type( sg, tip );
        switch(ft) {
        case AA_RX_FRAME_FIXED:
        /*     break; */
        case AA_RX_FRAME_REVOLUTE:
        case AA_RX_FRAME_PRISMATIC:
            *ptr = tip;
            ptr--;
            break;
        }

        tip = aa_rx_sg_frame_parent(sg, tip);
    }
}

AA_API size_t
aa_rx_sg_chain_config_count( const struct aa_rx_sg *sg,
                             size_t n_frames, const aa_rx_frame_id  *chain_frames )
{
    size_t a = 0;
    while( n_frames ) {
        enum aa_rx_frame_type ft = aa_rx_sg_frame_type(sg, chain_frames[--n_frames]);
        switch(ft) {
        case AA_RX_FRAME_FIXED:
            break;
        case AA_RX_FRAME_REVOLUTE:
        case AA_RX_FRAME_PRISMATIC:
            a++;
            break;
        }
    }

    return a;
}


AA_API void
aa_rx_sg_chain_configs( const struct aa_rx_sg *sg,
                        size_t n_frames, const aa_rx_frame_id *chain_frames,
                        size_t n_configs, aa_rx_config_id *chain_configs )
{
    /* TODO: handle duplicate configs */
    for( size_t i_config = 0, i_frame = 0;
         i_frame < n_frames && i_config < n_configs;
         i_frame++ )
    {
        enum aa_rx_frame_type ft = aa_rx_sg_frame_type(sg, chain_frames[i_frame]);

        switch(ft) {
        case AA_RX_FRAME_FIXED:
            break;
        case AA_RX_FRAME_REVOLUTE:
        case AA_RX_FRAME_PRISMATIC:
            chain_configs[i_config++] = aa_rx_sg_frame_config(sg, chain_frames[i_frame]);
            break;
        }
    }
}


AA_API void
aa_rx_sg_sub_config_get(
    const struct aa_rx_sg_sub *ssg,
    size_t n_all, const double *config_all,
    size_t n_subset, double *config_subset )
{
    aa_rx_sg_config_get( ssg->scenegraph,
                         n_all, n_subset,
                         aa_rx_sg_sub_configs(ssg),
                         config_all, config_subset );
}

AA_API void
aa_rx_sg_sub_config_set(
    const struct aa_rx_sg_sub *ssg,
    size_t n_sub, const double *config_subset,
    size_t n_all, double *config_all
    )
{
    aa_rx_sg_config_set( ssg->scenegraph,
                         n_all, n_sub,
                         aa_rx_sg_sub_configs(ssg),
                         config_subset, config_all );

}


AA_API struct aa_rx_sg_sub *
aa_rx_sg_chain_create( const struct aa_rx_sg *sg,
                       aa_rx_frame_id root, aa_rx_frame_id tip )
{
    struct aa_rx_sg_sub *ssg = AA_NEW( struct aa_rx_sg_sub );
    ssg->scenegraph = sg;

    ssg->frame_count = aa_rx_sg_chain_frame_count( sg, root, tip);
    ssg->frames =  AA_NEW_AR(aa_rx_frame_id, ssg->frame_count );
    aa_rx_sg_chain_frames( sg, root, tip,
                           ssg->frame_count, ssg->frames );

    ssg->config_count = aa_rx_sg_chain_config_count( sg, ssg->frame_count, ssg->frames );
    ssg->configs = AA_NEW_AR(aa_rx_config_id, ssg->config_count );
    aa_rx_sg_chain_configs( sg, ssg->frame_count, ssg->frames,
                            ssg->config_count, ssg->configs );

    return ssg;
}

AA_API void
aa_rx_sg_chain_jacobian( const struct aa_rx_sg *sg,
                         size_t n_tf, const double *TF_abs, size_t ld_TF,
                         size_t n_frames, aa_rx_frame_id *chain_frames,
                         size_t n_configs, aa_rx_frame_id *chain_configs,
                         double *J, size_t ld_J )
{
    (void)chain_configs; /* Use these for multivariate frame support */

    aa_rx_frame_id frame_ee = chain_frames[n_frames-1];
    const double *pe = &TF_abs[(size_t)frame_ee * ld_TF] + AA_TF_QUTR_T;

    for( size_t i_frame=0, i_config=0;
         i_frame < n_frames && i_config < n_configs;
         i_frame++ )
    {
        double *Jr = J + AA_TF_DX_W; // rotational part
        double *Jt = J + AA_TF_DX_V; // translational part

        aa_rx_frame_id frame = chain_frames[i_frame];
        assert( frame >= 0 );
        assert( (size_t)frame < n_tf );
        enum aa_rx_frame_type ft = aa_rx_sg_frame_type(sg, frame);

        switch(ft) {
        case AA_RX_FRAME_FIXED:
            break;

        case AA_RX_FRAME_REVOLUTE:
        case AA_RX_FRAME_PRISMATIC: {

            const double *a = aa_rx_sg_frame_axis(sg, frame);
            const double *E = TF_abs + (size_t)frame*ld_TF;
            const double *q = E+AA_TF_QUTR_Q;
            const double *t = E+AA_TF_QUTR_T;

            switch(ft)  {
            case AA_RX_FRAME_REVOLUTE: {
                aa_tf_qrot(q,a,Jr);
                double tmp[3];
                for( size_t j = 0; j < 3; j++ ) tmp[j] = pe[j] - t[j];
                aa_tf_cross(Jr, tmp, Jt);
                break;
            }
            case AA_RX_FRAME_PRISMATIC:
                AA_MEM_ZERO(Jr, 3);
                aa_tf_qrot(q,a,Jt);
                break;
            default: assert(0);
            }

            i_config++;
            J += ld_J;
            break;
        } /* end joint */
        } /* end switch */
    } /* end for */
}

AA_API void
aa_rx_sg_sub_jacobian_size( const struct aa_rx_sg_sub *ssg,
                            size_t *rows, size_t *cols )
{
    *rows = 6;
    *cols = ssg->config_count;

}

AA_API void
aa_rx_sg_sub_jacobian( const struct aa_rx_sg_sub *ssg,
                       size_t n_tf, const double *TF_abs, size_t ld_TF,
                       double *J, size_t ld_J )
{
    aa_rx_sg_chain_jacobian( ssg->scenegraph,
                             n_tf, TF_abs, ld_TF,
                             ssg->frame_count, ssg->frames,
                             ssg->config_count, ssg->configs,
                             J, ld_J );
}

AA_API void
aa_rx_sg_sub_center_configs( const struct aa_rx_sg_sub *ssg,
                             size_t n, double *q )
{
    size_t n_qs = aa_rx_sg_sub_config_count(ssg);
    size_t n_min = AA_MIN(n,n_qs);
    for( size_t i = 0; i < n_min; i ++ ) {
        double min=0 ,max=0;
        aa_rx_config_id config_id = aa_rx_sg_sub_config(ssg, i);
        int r = aa_rx_sg_get_limit_pos( ssg->scenegraph, config_id, &min, &max );
        if( 0 == r ) {
            q[i] = (max + min) / 2;
        } else {
            q[i] = 0;
        }
    }
}
