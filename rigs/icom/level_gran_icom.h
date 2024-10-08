/*
LVL_IF
LVL_RAWSTR

LVL_SPECTRUM_AVG
LVL_SPECTRUM_REF
LVL_SPECTRUM_SPEED
LVL_PBT_IN
LVL_PBT_OUT
LVL_KEYSPD
*/

        // Once included these values can be overridden in the back-end
        // Known variants are AGC_TIME, PREAMP, ATT, NB, CWPITCH, IF, NOTCHF, VOXDELAY, BKINDL, BKIN_DLYMS, RFPOWER_METER(255 or 100), RFPOWER_METER_WATTS(255 or 100)
        // cppcheck-suppress *
        /* raw data */
        [LVL_RAWSTR]        = { .min = { .i = 0 },     .max = { .i = 255 } },
        /* levels with dB units */
        [LVL_PREAMP]        = { .min = { .i = 0 },     .max = { .i = 20 },   .step = { .i = 10 } },
        [LVL_ATT]           = { .min = { .i = 0 },     .max = { .i = 12 },   .step = { .i = 0 } },
        [LVL_STRENGTH]      = { .min = { .i = 0 },     .max = { .i = 60 },   .step = { .i = 0 } },
        [LVL_NB]            = { .min = { .f = 0 },     .max = { .f = 10 },   .step = { .f = 1 } },
        /* levels with WPM units */
#if !defined(NO_LVL_KEYSPD) 
        [LVL_KEYSPD]  = { .min = { .i = 4 },           .max = { .i = 60 },   .step = { .i = 1 } },
#endif
        /* levels with Hz units */
#if !defined(NO_LVL_CWPITCH) 
        [LVL_CWPITCH]       = { .min = { .i = 300 },   .max = { .i = 900  }, .step = { .i = 5  } },
#endif
        [LVL_IF]            = { .min = { .i = -1200 }, .max = { .i = 1200 }, .step = { .i = 20 } },
        [LVL_NOTCHF]        = { .min = { .i = 1 },     .max = { .i = 3200 }, .step = { .i = 10 } },
        /* levels with time units */
        [LVL_VOXDELAY]      = { .min = { .i = 0 },     .max = { .i = 20 },   .step = { .i = 1 } },
        [LVL_BKINDL]        = { .min = { .i = 30 },    .max = { .i = 3000 }, .step = { .i = 1 } },
        [LVL_BKIN_DLYMS]    = { .min = { .i = 30 },    .max = { .i = 3000 }, .step = { .i = 1 } },
        [LVL_AGC_TIME]      = { .min = { .f = 0.0f },  .max = { .f = 8.0f }, .step = { .f = 0.1f } },
        /* level with misc units */
        [LVL_SWR]           = { .min = { .f = 0 },     .max = { .f = 5.0 },  .step = { .f = 1.0f/255.0f } },
        [LVL_BAND_SELECT]   = { .min = { .i = 0 },     .max = { .i = 16 },   .step = { .i = 1 } },
        /* levels with 0-1 values -- increment based on rig's range */
        [LVL_NR]            = { .min = { .f = 0 },     .max = { .f = 1 },    .step = { .f = 1.0f/15.0f } },
        [LVL_AF]            = { .min = { .f = 0 },     .max = { .f = 1.0 },  .step = { .f = 1.0f/255.0f } },
        [LVL_RF]            = { .min = { .f = 0 },     .max = { .f = 1.0 },  .step = { .f = 1.0f/255.0f } },
        [LVL_RFPOWER]       = { .min = { .f = .05 },   .max = { .f = 1 },    .step = { .f = 1.0f/255.0f } },
        [LVL_RFPOWER_METER] = { .min = { .f = .0 },    .max = { .f = 1 },    .step = { .f = 1.0f/255.0f } },
        [LVL_RFPOWER_METER_WATTS] = { .min = { .f = .0 },    .max = { .f = 100 },    .step = { .f = 1.0f/255.0f } },
        [LVL_COMP_METER]    = { .min = { .f = .0 },    .max = { .f = 1 },    .step = { .f = 1.0f/255.0f } },
        [LVL_ID_METER]      = { .min = { .f = .0 },    .max = { .f = 1 },    .step = { .f = 1.0f/255.0f } },
        [LVL_VD_METER]      = { .min = { .f = .0 },    .max = { .f = 1 },    .step = { .f = 1.0f/255.0f } },
        [LVL_SQL]           = { .min = { .f = 0 },     .max = { .f = 1.0 },  .step = { .f = 1.0f/255.0f } },
        [LVL_MICGAIN]       = { .min = { .f = .0 },    .max = { .f = 1 },    .step = { .f = 1.0f/255.0f } },
        [LVL_MONITOR_GAIN]  = { .min = { .f = .0 },    .max = { .f = 1 },    .step = { .f = 1.0f/255.0f } },
        [LVL_COMP]          = { .min = { .f = .0 },    .max = { .f = 1 },    .step = { .f = 1.0f/100.0f } },
        [LVL_VOXGAIN]       = { .min = { .f = .0 },    .max = { .f = 1 },    .step = { .f = 1.0f/255.0f } },
        [LVL_ANTIVOX]       = { .min = { .f = .0 },    .max = { .f = 1 },    .step = { .f = 1.0f/255.0f } },
        [LVL_ALC]           = { .min = { .f = .0 },    .max = { .f = 1 },    .step = { .f = 1.0f/120.0f } },
#if !defined(NO_LVL_USB_AF) 
        [LVL_USB_AF]        = { .min = { .f = .0 },    .max = { .f = 1 },    .step = { .f = 1.0f/255.0f } },
#endif
#if !defined(NO_LVL_PBT_IN) 
        [LVL_PBT_IN]        = { .min = { .f = .0 },    .max = { .f = 1 },    .step = { .f = 1.0f/255.0f } },
#endif
#if !defined(NO_LVL_PBT_OUT) 
        [LVL_PBT_OUT]       = { .min = { .f = .0 },    .max = { .f = 1 },    .step = { .f = 1.0f/255.0f } },
#endif 

