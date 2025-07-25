/*
 *  Hamlib Interface - CM108 HID communication low-level support
 *  Copyright (c) 2000-2012 by Stephane Fillod
 *  Copyright (c) 2011 by Andrew Errington
 *  CM108 detection code Copyright (c) Thomas Sailer used with permission
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/**
 * \addtogroup rig_internal
 * @{
 */

/**
 * \file cm108.c
 * \brief CM108 GPIO support.
 *
 * CM108 Audio chips found on many USB audio interfaces have controllable
 * General Purpose Input/Output pins.
 */
#include "hamlib/rig.h"
#include "hamlib/config.h"

#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <sys/types.h>

#ifdef HAVE_SYS_IOCTL_H
#  include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#  include <sys/param.h>
#endif

#ifdef HAVE_WINDOWS_H
#  include <windows.h>
#  include "par_nt.h"
#endif

#ifdef HAVE_WINIOCTL_H
#  include <winioctl.h>
#endif

#ifdef HAVE_WINBASE_H
#  include <winbase.h>
#endif

#ifdef HAVE_LINUX_HIDRAW_H
#  include <linux/hidraw.h>
#endif

#include "cm108.h"

#include <stdio.h>

const char *get_usb_device_class_string(int device_class)
{
    switch (device_class)
    {
    case 0x00:
        return "Device Unspecified (Defined at Interface level)";

    case 0x01:
        return "Audio";

    case 0x02:
        return "Communications and CDC Control";

    case 0x03:
        return "Human Interface Device (HID)";

    case 0x05:
        return "Physical Interface Device";

    case 0x06:
        return "Image (Scanner, Camera)";

    case 0x07:
        return "Printer";

    case 0x08:
        return "Mass Storage";

    case 0x09:
        return "Hub";

    case 0x0A:
        return "CDC Data";

    case 0x0B:
        return "Smart Card";

    case 0x0D:
        return "Content Security";

    case 0x0E:
        return "Video";

    case 0x0F:
        return "Personal Healthcare";

    case 0x10:
        return "Audio/Video Devices";

    case 0x11:
        return "Billboard Device Class";

    case 0x12:
        return "USB Type-C Bridge Class";

    case 0x13:
        return "Bulk Display";

    case 0x14:
        return "MCTCP over USB";

    case 0x3C:
        return "I3C";

    case 0x58:
        return "Xbox";

    case 0xDC:
        return "Diagnostic Device";

    case 0xE0:
        return "Wireless Controller";

    case 0xEF:
        return "Miscellaneous";

    case 0xFE:
        return "Application Specific";

    case 0xFF:
        return "Vendor Specific";

    default:
        return "Unknown Device Class";
    }
}

/**
 * \brief Open CM108 HID port (/dev/hidraw<i>X</i>).
 *
 * \param port The port structure.
 *
 * \return File descriptor, otherwise a **negative value** if an error
 * occurred (in which case, cause is set appropriately).
 *
 * \retval -RIG_EINVAL The port pathname is empty or no CM108 device detected.
 * \retval -RIG_EIO The `open`(2) system call returned a **negative value**.
 */
int cm108_open(hamlib_port_t *port)
{
    int fd;
#ifdef HAVE_LINUX_HIDRAW_H
    struct hidraw_devinfo hiddevinfo;
#endif

    rig_debug(RIG_DEBUG_VERBOSE, "%s called\n", __func__);

    if (!port->pathname[0])
    {
        return -RIG_EINVAL;
    }

    fd = open(port->pathname, O_RDWR);

    if (fd < 0)
    {
        rig_debug(RIG_DEBUG_ERR,
                  "%s: opening device \"%s\": %s\n",
                  __func__,
                  port->pathname,
                  strerror(errno));
        return -RIG_EIO;
    }

#ifdef HAVE_LINUX_HIDRAW_H
    // CM108 detection copied from Thomas Sailer's soundmodem code
    rig_debug(RIG_DEBUG_VERBOSE,
              "%s: checking for cm108 (or compatible) device\n",
              __func__);

    if (!ioctl(fd, HIDIOCGRAWINFO, &hiddevinfo)
            && ((hiddevinfo.vendor == 0x0d8c
                 // CM108/108B/109/119/119A/119B
                 && ((hiddevinfo.product >= 0x0008
                      && hiddevinfo.product <= 0x000f)
                     || hiddevinfo.product == 0x0012
                     || hiddevinfo.product == 0x013a
                     || hiddevinfo.product == 0x013c
                     || hiddevinfo.product == 0x0013))
                // SSS1621/23
                || (hiddevinfo.vendor == 0x0c76
                    && (hiddevinfo.product == 0x1605
                        || hiddevinfo.product == 0x1607
                        || hiddevinfo.product == 0x160b))))
    {
        rig_debug(RIG_DEBUG_VERBOSE,
                  "%s: cm108 compatible device detected\n",
                  __func__);
    }
    else
    {
        close(fd);
        rig_debug(RIG_DEBUG_VERBOSE,
                  "%s: no cm108 (or compatible) device detected\n",
                  __func__);
        return -RIG_EINVAL;
    }

#endif

    port->fd = fd;
    return fd;
}


/**
 * \brief Close a CM108 HID port.
 *
 * \param port The port structure
 *
 * \return Zero if the port was closed successfully, otherwise -1 if an error
 * occurred (in which case, the system `errno`(3) is set appropriately).
 *
 * \sa The `close`(2) system call.
 */
int cm108_close(hamlib_port_t *port)
{
    rig_debug(RIG_DEBUG_VERBOSE, "%s called\n", __func__);

    return close(port->fd);
}


/**
 * \brief Set or unset the Push To Talk bit on a CM108 GPIO.
 *
 * \param p The port structure.
 * \param pttx RIG_PTT_ON --> Set PTT, else unset PTT.
 *
 * \return RIG_OK on success, otherwise a **negative value** if an error
 * occurred (in which case, cause is set appropriately).
 *
 * \retval RIG_OK Setting or unsetting the PTT was successful.
 * \retval -RIG_EINVAL The file descriptor is invalid or the PTT type is
 * unsupported.
 * \retval -RIG_EIO The `write`(2) system call returned a **negative value**.
 */
int cm108_ptt_set(hamlib_port_t *p, ptt_t pttx)
{
    rig_debug(RIG_DEBUG_VERBOSE, "%s called\n", __func__);

    // For a CM108 USB audio device PTT is wired up to one of the GPIO
    // pins.  Usually this is GPIO3 (bit 2 of the GPIO register) because it
    // is on the corner of the chip package (pin 13) so it's easily accessible.
    // Some CM108 chips are epoxy-blobbed onto the PCB, so no GPIO
    // pins are accessible.  The SSS1623 chips have a different pinout, so
    // we make the GPIO bit number configurable.

    switch (p->type.ptt)
    {

    case RIG_PTT_CM108:
    {
        ssize_t bytes;
        char out_rep[] =
        {
            0x00, // report number
            // HID output report
            0x00,
            (pttx == RIG_PTT_ON) ? (1 << p->parm.cm108.ptt_bitnum) : 0, // set GPIO
            1 << p->parm.cm108.ptt_bitnum, // Data direction register (1=output)
              0x00
        };

        // Build a packet for CM108 HID to turn GPIO bit on or off.
        // Packet is 4 bytes, preceded by a 'report number' byte
        // 0x00 report number
        // Write data packet (from CM108 documentation)
        // byte 0: 00xx xxxx     Write GPIO
        // byte 1: xxxx dcba     GPIO3-0 output values (1=high)
        // byte 2: xxxx dcba     GPIO3-0 data-direction register (1=output)
        // byte 3: xxxx xxxx     SPDIF

        rig_debug(RIG_DEBUG_VERBOSE,
                  "%s: bit number %d to state %d\n",
                  __func__,
                  p->parm.cm108.ptt_bitnum,
                  (pttx == RIG_PTT_ON) ? 1 : 0);

        if (p->fd == -1)
        {
            return -RIG_EINVAL;
        }

        // Send the HID packet
        bytes = write(p->fd, out_rep, sizeof(out_rep));

        if (bytes < 0)
        {
            return -RIG_EIO;
        }

        return RIG_OK;
    }

    default:
        rig_debug(RIG_DEBUG_ERR,
                  "%s: unsupported PTT type %d\n",
                  __func__,
                  p->type.ptt);
        return -RIG_EINVAL;
    }

    return RIG_OK;
}


/**
 * \brief Get the state of Push To Talk from a CM108 GPIO.
 *
 * \param p The port structure.
 * \param pttx Return value (must be non NULL).
 *
 * \return RIG_OK on success, otherwise a **negative value** if an error
 * occurred (in which case, cause is set appropriately).
 *
 * \retval RIG_OK Getting the PTT state was successful.
 * \retval -RIG_ENIMPL Getting the state is not yet implemented.
 * \retval -RIG_ENAVAIL Getting the state is not available for this PTT type.
 */
int cm108_ptt_get(hamlib_port_t *p, ptt_t *pttx)
{
    rig_debug(RIG_DEBUG_VERBOSE, "%s called\n", __func__);

    switch (p->type.ptt)
    {
    case RIG_PTT_CM108:
    {
        return -RIG_ENIMPL;
    }

    default:
        rig_debug(RIG_DEBUG_ERR,
                  "%s: unsupported PTT type %d\n",
                  __func__,
                  p->type.ptt);
        return -RIG_ENAVAIL;
    }

    return RIG_OK;
}


#ifdef XXREMOVEXX
// Not referenced anywhere
/**
 * \brief get Data Carrier Detect (squelch) from CM108 GPIO
 * \param p
 * \param dcdx return value (Must be non NULL)
 * \return RIG_OK or < 0 error
 */
int cm108_dcd_get(hamlib_port_t *p, dcd_t *dcdx)
{
    rig_debug(RIG_DEBUG_VERBOSE, "%s called\n", __func__);

    return RIG_OK;
}
#endif

int cm108_set_bit(hamlib_port_t *p, enum GPIO gpio, int bit)
{
    rig_debug(RIG_DEBUG_VERBOSE, "%s called\n", __func__);

    ssize_t bytes;
    char out_rep[] =
    {
        0x00, // report number
        // HID output report
        0x00,
        bit ? (1 << (gpio - 1)) : 0, // set GPIO
        1 << (gpio - 1), // Data direction register (1=output)
          0x00
    };
    rig_debug(RIG_DEBUG_VERBOSE, "%s: out_rep = 0x%02x 0x%02x\n", __func__,
              out_rep[2], out_rep[3]);

    // Build a packet for CM108 HID to turn GPIO bit on or off.
    // Packet is 4 bytes, preceded by a 'report number' byte
    // 0x00 report number
    // Write data packet (from CM108 documentation)
    // byte 0: 00xx xxxx     Write GPIO
    // byte 1: xxxx dcba     GPIO3-0 output values (1=high)
    // byte 2: xxxx dcba     GPIO3-0 data-direction register (1=output)
    // byte 3: xxxx xxxx     SPDIF

    // Send the HID packet
    bytes = write(p->fd, out_rep, sizeof(out_rep));

    if (bytes < 0)
    {
        return -RIG_EIO;
    }

    return RIG_OK;
}

int cm108_get_bit(hamlib_port_t *p, enum GPIO gpio, int *bit)
{
    ssize_t bytes;
    char reply[4];
    rig_debug(RIG_DEBUG_VERBOSE, "%s called\n", __func__);
    char in_rep[] =
    {
        0x00, // report number
        0x01, // HID input report
        0x00,
        0x00
    };

    // Send the HID packet
    bytes = write(p->fd, in_rep, sizeof(in_rep));

    if (bytes < 0)
    {
        return -RIG_EIO;
    }

    bytes = read(p->fd, reply, sizeof(reply));

    if (bytes <= 0)
    {
        rig_debug(RIG_DEBUG_ERR, "%s: read error: %s\n", __func__, strerror(errno));
        return -RIG_EPROTO;
    }

    int mask = 1 << (gpio - 1);
    *bit = (reply[1] & mask) != 0;
    rig_debug(RIG_DEBUG_VERBOSE,
              "%s: mask=0x%02x, reply=0x%02x 0x%02x 0x%02x 0x%02x, bit=%d\n", __func__, mask,
              reply[0], reply[1], reply[2], reply[3], *bit);
    return RIG_OK;
}

int rig_cm108_get_bit(hamlib_port_t *p, enum GPIO gpio, int *bit)
{
    if (gpio < 1 || gpio > 4)
    {
        rig_debug(RIG_DEBUG_ERR, "%s: gpio must be 1,2,3,4 for cm108\n", __func__);
        return -RIG_EINVAL;
    }

    cm108_get_bit(p, gpio, bit);
    rig_debug(RIG_DEBUG_TRACE, "%s: gpio=%d bit=%d\n", __func__, gpio, *bit);
    return RIG_OK;
}


int rig_cm108_set_bit(hamlib_port_t *p, enum GPIO gpio, int bit)
{
    if (gpio < 1 || gpio > 4)
    {
        rig_debug(RIG_DEBUG_ERR, "%s: gpio must be 1,2,3,4 for cm108\n", __func__);
        return -RIG_EINVAL;
    }

    int retval = cm108_set_bit(p, gpio, bit);

    if (retval != RIG_OK)
    {
        rig_debug(RIG_DEBUG_ERR, "%s: cm108_set_bit: %s\n", __func__, strerror(retval));
        return retval;
    }

    rig_debug(RIG_DEBUG_TRACE, "%s: simulated gpio=%d bit=%d\n", __func__, gpio,
              bit);
    return RIG_OK;
}


/** @} */
