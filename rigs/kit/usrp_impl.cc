/*
 *  Hamlib KIT backend - Universal Software Radio Peripheral
 *  Copyright (c) 2010 by Stephane Fillod
 *
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <hamlib/config.h>

/*
 * Compile only this model if usrp is available
 */
#if defined(HAVE_USRP)


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <usrp/usrp_standard.h>

#include "usrp_impl.h"
#include "token.h"


struct usrp_priv_data {
	usrp_standard_rx *urx;
	usrp_standard_tx *utx;
	freq_t if_mix_freq;
};


int usrp_init(RIG *rig)
{
    // cppcheck-suppress leakReturnValNotUsed
	STATE(rig)->priv = static_cast<struct usrp_priv_data*>(malloc(sizeof(struct usrp_priv_data)));
	if (!STATE(rig)->priv) {
		/* whoops! memory shortage! */
		return -RIG_ENOMEM;
	}

	return RIG_OK;
}

int usrp_cleanup(RIG *rig)
{
	if (!rig)
		return -RIG_EINVAL;

	if (STATE(rig)->priv)
		free(STATE(rig)->priv);
	STATE(rig)->priv = NULL;

	return RIG_OK;
}

int usrp_open(RIG *rig)
{
	struct usrp_priv_data *priv = static_cast<struct usrp_priv_data*>(STATE(rig)->priv);

	int which_board = 0;
	int decim = 125;

	priv->urx = usrp_standard_rx::make (which_board, decim, 1, -1, usrp_standard_rx::FPGA_MODE_NORMAL).get();
	if (priv->urx == 0)
		return -RIG_EIO;

	return RIG_OK;
}

int usrp_close(RIG *rig)
{
	struct usrp_priv_data *priv = static_cast<struct usrp_priv_data*>(STATE(rig)->priv);

    if (!priv)
    {
        rig_debug(RIG_DEBUG_ERR, "%s: priv == NULL?\n", __func__);
        return -RIG_EARG;
    }

	delete priv->urx;

	return RIG_OK;
}

/*
 * Assumes rig!=NULL, STATE(rig)->priv!=NULL
 */
int usrp_set_conf(RIG *rig, hamlib_token_t token, const char *val)
{
	struct usrp_priv_data *priv = static_cast<struct usrp_priv_data*>(STATE(rig)->priv);

    if (!priv)
    {
        rig_debug(RIG_DEBUG_ERR, "%s: priv == NULL?\n", __func__);
        return -RIG_EARG;
    }

	switch(token) {
		case TOK_IFMIXFREQ:
			sscanf(val, "%" SCNfreq, &priv->if_mix_freq);
			break;
		default:
			return -RIG_EINVAL;
	}
	return RIG_OK;
}

/*
 * assumes rig!=NULL,
 * Assumes rig!=NULL, STATE(rig)->priv!=NULL
 *  and val points to a buffer big enough to hold the conf value.
 */
int usrp_get_conf(RIG *rig, hamlib_token_t token, char *val)
{
	const struct usrp_priv_data *priv = static_cast<struct usrp_priv_data*>(STATE(rig)->priv);

    if (!priv)
    {
        rig_debug(RIG_DEBUG_ERR, "%s: priv == NULL?\n", __func__);
        return -RIG_EARG;
    }

	switch(token) {
		case TOK_IFMIXFREQ:
			sprintf(val, "%" PRIfreq, priv->if_mix_freq);
			break;
		default:
			return -RIG_EINVAL;
	}
	return RIG_OK;
}



int usrp_set_freq(RIG *rig, vfo_t vfo, freq_t freq)
{
	const struct usrp_priv_data *priv = static_cast<struct usrp_priv_data*>(STATE(rig)->priv);
	int chan = 0;

    if (!priv)
    {
        rig_debug(RIG_DEBUG_ERR, "%s: priv == NULL?\n", __func__);
        return -RIG_EARG;
    }

	if (!priv->urx->set_rx_freq (chan, freq))
		return -RIG_EPROTO;

	return RIG_OK;
}


int usrp_get_freq(RIG *rig, vfo_t vfo, freq_t *freq)
{
	const struct usrp_priv_data *priv = static_cast<struct usrp_priv_data*>(STATE(rig)->priv);
	int chan = 0;

    if (!priv)
    {
        rig_debug(RIG_DEBUG_ERR, "%s: priv == NULL?\n", __func__);
        return -RIG_EARG;
    }

	*freq = priv->urx->rx_freq (chan);

	return RIG_OK;
}

const char * usrp_get_info(RIG *rig)
{
	return NULL;
}

#endif	/* HAVE_USRP */
