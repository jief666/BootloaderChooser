/*
 * File: HdaVerbs.h
 *
 * Copyright (c) 2018 John Davis
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _EFI_HDA_VERBS_H_
#define _EFI_HDA_VERBS_H_

// Root node ID.
#define HDA_NID_ROOT    0

// Macro for building verbs. 4-bit verbs will be zero when masked against 0xFF0.
#define HDA_CODEC_VERB(Verb, Payload)   ((UINT32)((((Verb) & 0xFFF) & 0xFF0) ? \
    ((((Verb) & 0xFFF) << 8) | ((Payload) & 0xFF)) : /* 12-bit verbs */ \
    ((((Verb) & 0xF) << 16) | ((Payload) & 0xFFFF)))) /* 4-bit verbs */

// Get/Set Converter Format.
#define HDA_VERB_GET_CONVERTER_FORMAT       0xA
#define HDA_VERB_SET_CONVERTER_FORMAT       0x2
#define HDA_CONVERTER_FORMAT_CHAN(a)        ((UINT8)((a) & 0xF))
#define HDA_CONVERTER_FORMAT_BITS(a)        ((UINT8)(((a) >> 4) & 0x3))
#define HDA_CONVERTER_FORMAT_BITS_8         0x0
#define HDA_CONVERTER_FORMAT_BITS_16        0x1
#define HDA_CONVERTER_FORMAT_BITS_20        0x2
#define HDA_CONVERTER_FORMAT_BITS_24        0x3
#define HDA_CONVERTER_FORMAT_BITS_32        0x4
#define HDA_CONVERTER_FORMAT_DIV(a)         ((UINT8)(((a) >> 8) & 0x3))
#define HDA_CONVERTER_FORMAT_MULT(a)        ((UINT8)(((a) >> 11) & 0x3))
#define HDA_CONVERTER_FORMAT_BASE_44KHZ     BIT14
#define HDA_CONVERTER_FORMAT_SET(chan, bits, div, mult, base) \
    ((UINT16)(((chan) & 0xF) | (((bits) & 0x3) << 4) | (((div) & 0x3) << 8) | \
    (((mult) & 0x3) << 11) | ((base) ? HDA_CONVERTER_FORMAT_BASE_44KHZ : 0)))

// Get Amplifier Gain/Mute.
#define HDA_VERB_GET_AMP_GAIN_MUTE                              0xB
#define HDA_VERB_GET_AMP_GAIN_MUTE_PAYLOAD(index, left, out)    ((UINT16)(index | (left ? BIT13 : 0) | (out ? BIT15 : 0)))
#define HDA_VERB_GET_AMP_GAIN_MUTE_GAIN(a)                      ((UINT8)(a & 0x7F))
#define HDA_VERB_GET_AMP_GAIN_MUTE_MUTE                         BIT7

// Set Amplifier Gain/Mute.
#define HDA_VERB_SET_AMP_GAIN_MUTE  0x3
#define HDA_VERB_SET_AMP_GAIN_MUTE_PAYLOAD(index, gain, mute, right, left, in, out) \
    ((UINT16)((gain & 0x7F) | (mute ? BIT7 : 0) | (index & 0xFF) << 8) \
    | (right ? BIT12 : 0) | (left ? BIT13 : 0) | (in ? BIT14 : 0) | (out ? BIT15 : 0))

// Get/Set Processing Coefficient.
#define HDA_VERB_GET_PROCESSING_COEFFICIENT 0xC
#define HDA_VERB_SET_PROCESSING_COEFFICIENT 0x4

// Get/Set Coefficient Index.
#define HDA_VERB_GET_COEFFICIENT_INDEX  0xD
#define HDA_VERB_SET_COEFFICIENT_INDEX  0x5

// Get/Set Connection Select Control.
#define HDA_VERB_GET_CONN_SELECT_CONTROL    0xF01
#define HDA_VERB_SET_CONN_SELECT_CONTROL    0x701

// Get Connection List Entry.
#define HDA_VERB_GET_CONN_LIST_ENTRY                0xF02
#define HDA_VERB_GET_CONN_LIST_ENTRY_SHORT(a, i)    ((UINT8)(a >> (8 * (i))))
#define HDA_VERB_GET_CONN_LIST_ENTRY_LONG(a, i)     ((UINT16)(a >> (16 * (i))))

// Get/Set Processing State.
#define HDA_VERB_GET_PROCESSING_STATE   0xF03
#define HDA_VERB_SET_PROCESSING_STATE   0x703
#define HDA_PROCESSING_STATE_OFF        0x00
#define HDA_PROCESSING_STATE_ON         0x01
#define HDA_PROCESSING_STATE_BENIGN     0x02

// Get/Set Input Converter SDI Select.
#define HDA_VERB_GET_INPUT_CONV_SDI_SELECT  0xF04
#define HDA_VERB_SET_INPUT_CONV_SDI_SELECT  0x704

// Get/Set Power State.
#define HDA_VERB_GET_POWER_STATE    0xF05
#define HDA_VERB_SET_POWER_STATE    0x705

// Get/Set Converter Stream, Channel.
#define HDA_VERB_GET_CONVERTER_STREAM_CHANNEL       0xF06
#define HDA_VERB_SET_CONVERTER_STREAM_CHANNEL       0x706
#define HDA_VERB_GET_CONVERTER_STREAM_CHAN(a)       ((UINT8)(a & 0xF))
#define HDA_VERB_GET_CONVERTER_STREAM_STR(a)        ((UINT8)((a >> 4) & 0xF))
#define HDA_VERB_SET_CONVERTER_STREAM_PAYLOAD(c, s) ((UINT8)(((c) & 0xF) | (((s) & 0xF) << 4)))

// Get/Set Pin Widget Control.
#define HDA_VERB_GET_PIN_WIDGET_CONTROL     0xF07
#define HDA_VERB_SET_PIN_WIDGET_CONTROL     0x707
#define HDA_PIN_WIDGET_CONTROL_VREF(a)      ((UINT8)(a & 0x3))
#define HDA_PIN_WIDGET_CONTROL_VREF_EN      BIT2
#define HDA_PIN_WIDGET_CONTROL_IN_EN        BIT5
#define HDA_PIN_WIDGET_CONTROL_OUT_EN       BIT6
#define HDA_PIN_WIDGET_CONTROL_HP_EN        BIT7
#define HDA_VERB_SET_PIN_WIDGET_CONTROL_PAYLOAD(vref, vrefEn, in, out, hp) \
    ((UINT8)(HDA_PIN_WIDGET_CONTROL_VREF(vref) | (vrefEn ? HDA_PIN_WIDGET_CONTROL_VREF_EN : 0) \
    | (in ? HDA_PIN_WIDGET_CONTROL_IN_EN : 0) | (out ? HDA_PIN_WIDGET_CONTROL_OUT_EN : 0) \
    | (hp ? HDA_PIN_WIDGET_CONTROL_HP_EN : 0)))

// Get/Set Unsolicited Response.
#define HDA_VERB_GET_UNSOL_RESPONSE     0xF08
#define HDA_VERB_SET_UNSOL_RESPONSE     0x708
#define HDA_UNSOL_RESPONSE_EN           BIT7


// Get/Set Digital Converter Control.
#define HDA_VERB_GET_DIGITAL_CONV_CONTROL   0xF0D
#define HDA_VERB_SET_DIGITAL_CONV_CONTROL1  0x70D
#define HDA_VERB_SET_DIGITAL_CONV_CONTROL2  0x70E
#define HDA_VERB_SET_DIGITAL_CONV_CONTROL3  0x73E
#define HDA_VERB_SET_DIGITAL_CONV_CONTROL4  0x73F
#define HDA_DIGITAL_CONV_CONTROL_DIGEN      BIT0
#define HDA_DIGITAL_CONV_CONTROL_V          BIT1
#define HDA_DIGITAL_CONV_CONTROL_VCFG       BIT2
#define HDA_DIGITAL_CONV_CONTROL_PRE        BIT3
#define HDA_DIGITAL_CONV_CONTROL_NO_CP      BIT4
#define HDA_DIGITAL_CONV_CONTROL_NO_AUDIO   BIT5
#define HDA_DIGITAL_CONV_CONTROL_PRO        BIT6
#define HDA_DIGITAL_CONV_CONTROL_L          BIT7
#define HDA_DIGITAL_CONV_CONTROL_CC(a)      ((UINT8)((a >> 8) & 0x7F))
#define HDA_DIGITAL_CONV_CONTROL_ICT        ((UINT8)((a >> 16) & 0xF))
#define HDA_DIGITAL_CONV_CONTROL_KAE        BIT23

// Get/Execute Pin Sense.
#define HDA_VERB_GET_PIN_SENSE  0xF09
#define HDA_VERB_EXE_PIN_SENSE  0x709
#define HDA_PIN_SENSE_PD        BIT31

// Get/Set Beep Generation.
#define HDA_VERB_GET_BEEP_GENERATION    0xF0A
#define HDA_VERB_SET_BEEP_GENERATION    0x70A

// Get/Set EAPD/BTL Enable.
#define HDA_VERB_GET_EAPD_BTL_ENABLE    0xF0C
#define HDA_VERB_SET_EAPD_BTL_ENABLE    0x70C
#define HDA_EAPD_BTL_ENABLE_BTL         BIT0
#define HDA_EAPD_BTL_ENABLE_EAPD        BIT1
#define HDA_EAPD_BTL_ENABLE_L_R_SWAP    BIT2

// Get/Set Volume Knob.
#define HDA_VERB_GET_VOLUME_KNOB    0xF0F
#define HDA_VERB_SET_VOLUME_KNOB    0x70F
#define HDA_VOLUME_KNOB_DIRECT      BIT7

// Get/Set GPI Data.
#define HDA_VERB_GET_GPI_DATA   0xF10
#define HDA_VERB_SET_GPI_DATA   0x710

// Get/Set GPI Wake Enable Mask.
#define HDA_VERB_GET_GPI_WAKE_ENABLE_MASK   0xF11
#define HDA_VERB_SET_GPI_WAKE_ENABLE_MASK   0x711

// Get/Set GPI Unsolicited Enable Mask.
#define HDA_VERB_GET_GPI_UNSOL_ENABLE_MASK  0xF12
#define HDA_VERB_SET_GPI_UNSOL_ENABLE_MASK  0x712

// Get/Set GPI Sticky Mask.
#define HDA_VERB_GET_GPI_STICK_MASK 0xF13
#define HDA_VERB_SET_GPI_STICK_MASK 0x713

// Get/Set GPO Data.
#define HDA_VERB_GET_GPO_DATA   0xF14
#define HDA_VERB_SET_GPO_DATA   0x714

// Get/Set GPIO Data.
#define HDA_VERB_GET_GPIO_DATA  0xF15
#define HDA_VERB_SET_GPIO_DATA  0x715

// Get/Set GPIO Enable Mask.
#define HDA_VERB_GET_GPIO_ENABLE_MASK   0xF16
#define HDA_VERB_SET_GPIO_ENABLE_MASK   0x716

// Get/Set GPIO Direction.
#define HDA_VERB_GET_GPIO_DIRECTION 0xF17
#define HDA_VERB_SET_GPIO_DIRECTION 0x717

// Get/Set GPIO Wake Enable Mask.
#define HDA_VERB_GET_GPIO_WAKE_ENABLE_MASK  0xF18
#define HDA_VERB_SET_GPIO_WAKE_ENABLE_MASK  0x718

// Get/Set GPIO Unsolicited Enable Mask.
#define HDA_VERB_GET_GPIO_UNSOL_ENABLE_MASK 0xF19
#define HDA_VERB_SET_GPIO_UNSOL_ENABLE_MASK 0x719

// Get/Set GPIO Sticky Mask.
#define HDA_VERB_GET_GPIO_STICKY_MASK   0xF1A
#define HDA_VERB_SET_GPIO_STICKY_MASK   0x71A

// Get/Set Implementation Identification.
#define HDA_VERB_GET_IMPLEMENTATION_ID  0xF20
#define HDA_VERB_SET_IMPLEMENTATION_ID1 0x720
#define HDA_VERB_SET_IMPLEMENTATION_ID2 0x721
#define HDA_VERB_SET_IMPLEMENTATION_ID3 0x722
#define HDA_VERB_SET_IMPLEMENTATION_ID4 0x723

//
// Get/Set Configuration Default.
//
#define HDA_VERB_GET_CONFIGURATION_DEFAULT      0xF1C
#define HDA_VERB_SET_CONFIGURATION_DEFAULT0     0x71C
#define HDA_VERB_SET_CONFIGURATION_DEFAULT1     0x71D
#define HDA_VERB_SET_CONFIGURATION_DEFAULT2     0x71E
#define HDA_VERB_SET_CONFIGURATION_DEFAULT3     0x71F

// Configuration Default sequence.
#define HDA_VERB_GET_CONFIGURATION_DEFAULT_SEQUENCE(a) \
    ((UINT8)(a & 0xF))

// Configuration Default default associaton.
#define HDA_VERB_GET_CONFIGURATION_DEFAULT_ASSOCIATION(a) \
    ((UINT8)((a >> 4) & 0xF))

// Configuration Default misc.
#define HDA_CONFIG_DEFAULT_MISC_JACK_DETECT_OVERRIDE BIT8

// Configuration Default color.
#define HDA_VERB_GET_CONFIGURATION_DEFAULT_COLOR(a) \
    ((UINT8)((a >> 12) & 0xF))
#define HDA_CONFIG_DEFAULT_COLOR_UNKNOWN    0x0
#define HDA_CONFIG_DEFAULT_COLOR_BLACK      0x1
#define HDA_CONFIG_DEFAULT_COLOR_GREY       0x2
#define HDA_CONFIG_DEFAULT_COLOR_BLUE       0x3
#define HDA_CONFIG_DEFAULT_COLOR_GREEN      0x4
#define HDA_CONFIG_DEFAULT_COLOR_RED        0x5
#define HDA_CONFIG_DEFAULT_COLOR_ORANGE     0x6
#define HDA_CONFIG_DEFAULT_COLOR_YELLOW     0x7
#define HDA_CONFIG_DEFAULT_COLOR_PURPLE     0x8
#define HDA_CONFIG_DEFAULT_COLOR_PINK       0x9
#define HDA_CONFIG_DEFAULT_COLOR_WHITE      0xE
#define HDA_CONFIG_DEFAULT_COLOR_OTHER      0xF

// Configuration Default connection type.
#define HDA_VERB_GET_CONFIGURATION_DEFAULT_CONN_TYPE(a) \
    ((UINT8)((a >> 16) & 0xF))
#define HDA_CONFIG_DEFAULT_CONN_UNKNOWN         0x0
#define HDA_CONFIG_DEFAULT_CONN_1_8_STEREO      0x1
#define HDA_CONFIG_DEFAULT_CONN_1_4_STEREO      0x2
#define HDA_CONFIG_DEFAULT_CONN_ATAPI           0x3
#define HDA_CONFIG_DEFAULT_CONN_RCA             0x4
#define HDA_CONFIG_DEFAULT_CONN_OPTICAL         0x5
#define HDA_CONFIG_DEFAULT_CONN_DIGITAL_OTHER   0x6
#define HDA_CONFIG_DEFAULT_CONN_ANALOG_OTHER    0x7
#define HDA_CONFIG_DEFAULT_CONN_MULTI_ANALOG    0x8
#define HDA_CONFIG_DEFAULT_CONN_XLR             0x9
#define HDA_CONFIG_DEFAULT_CONN_RJ11            0xA
#define HDA_CONFIG_DEFAULT_CONN_COMBO           0xB
#define HDA_CONFIG_DEFAULT_CONN_OTHER           0xF

// Configuration Default default device.
#define HDA_VERB_GET_CONFIGURATION_DEFAULT_DEVICE(a) \
    ((UINT8)((a >> 20) & 0xF))
#define HDA_CONFIG_DEFAULT_DEVICE_LINE_OUT          0x0
#define HDA_CONFIG_DEFAULT_DEVICE_SPEAKER           0x1
#define HDA_CONFIG_DEFAULT_DEVICE_HEADPHONE_OUT     0x2
#define HDA_CONFIG_DEFAULT_DEVICE_CD                0x3
#define HDA_CONFIG_DEFAULT_DEVICE_SPDIF_OUT         0x4
#define HDA_CONFIG_DEFAULT_DEVICE_OTHER_DIGITAL_OUT 0x5
#define HDA_CONFIG_DEFAULT_DEVICE_MODEM_LINE        0x6
#define HDA_CONFIG_DEFAULT_DEVICE_MODEM_HANDSET     0x7
#define HDA_CONFIG_DEFAULT_DEVICE_LINE_IN           0x8
#define HDA_CONFIG_DEFAULT_DEVICE_AUX               0x9
#define HDA_CONFIG_DEFAULT_DEVICE_MIC_IN            0xA
#define HDA_CONFIG_DEFAULT_DEVICE_TELEPHONY         0xB
#define HDA_CONFIG_DEFAULT_DEVICE_SPDIF_IN          0xC
#define HDA_CONFIG_DEFAULT_DEVICE_OTHER_DIGITAL_IN  0xD
#define HDA_CONFIG_DEFAULT_DEVICE_OTHER             0xF

// Configuration Default location.
#define HDA_VERB_GET_CONFIGURATION_DEFAULT_LOC(a) \
    ((UINT8)((a >> 24) & 0xF))
#define HDA_CONFIG_DEFAULT_LOC_SPEC_NA      0x0
#define HDA_CONFIG_DEFAULT_LOC_SPEC_REAR    0x1
#define HDA_CONFIG_DEFAULT_LOC_SPEC_FRONT   0x2
#define HDA_CONFIG_DEFAULT_LOC_SPEC_LEFT    0x3
#define HDA_CONFIG_DEFAULT_LOC_SPEC_RIGHT   0x4
#define HDA_CONFIG_DEFAULT_LOC_SPEC_TOP     0x5
#define HDA_CONFIG_DEFAULT_LOC_SPEC_BOTTOM  0x6

#define HDA_VERB_GET_CONFIGURATION_DEFAULT_SURF(a) \
    ((UINT8)((a >> 28) & 0x3))
#define HDA_CONFIG_DEFAULT_LOC_SURF_EXTERNAL    0x0
#define HDA_CONFIG_DEFAULT_LOC_SURF_INTERNAL    0x1
#define HDA_CONFIG_DEFAULT_LOC_SURF_SEPARATE    0x2
#define HDA_CONFIG_DEFAULT_LOC_SURF_OTHER       0x3

// Configuration Default port connectivity.
#define HDA_VERB_GET_CONFIGURATION_DEFAULT_PORT_CONN(a) \
    ((UINT8)((a >> 30) & 0x3))
#define HDA_CONFIG_DEFAULT_PORT_CONN_JACK           0x0
#define HDA_CONFIG_DEFAULT_PORT_CONN_NONE           0x1
#define HDA_CONFIG_DEFAULT_PORT_CONN_FIXED_DEVICE   0x2
#define HDA_CONFIG_DEFAULT_PORT_CONN_INT_JACK       0x3

// Configuration Default payload set.
#define HDA_VERB_SET_CONFIGURATION_DEFAULT_PAYLOAD(seq, assoc, jacko, color, connType, device, loc, surf, portConn) \
    ((UINT32)(((seq) & 0xF) | (((assoc) & 0xF) << 4) | (jacko ? HDA_CONFIG_DEFAULT_MISC_JACK_DETECT_OVERRIDE : 0) \
    | (((color) & 0xF) << 12) | (((connType) & 0xF) << 16) | (((device) & 0xF) << 20) | (((loc) & 0x3) << 24) \
    | (((surf) & 0x3) << 28) | (((portConn) & 0x3) << 30)))

// Get/Set Stripe Control.
#define HDA_VERB_GET_STRIPE_CONTROL 0xF24
#define HDA_VERB_SET_STRIPE_CONTROL 0x724

// Get/Set Converter Channel Count.
#define HDA_VERB_GET_CONVERTER_CHANNEL_COUNT    0xF2D
#define HDA_VERB_SET_CONVERTER_CHANNEL_COUNT    0x72D

// Get Data Island Packet - Size Info.
#define HDA_VERB_GET_DATA_ISLAND_PACKET_SIZE    0xF2E

// Get EDID-Like Data (ELD).
#define HDA_VERB_GET_EDID_LIKE_DATA 0xF2F

// Get/Set Data Island Packet - Index.
#define HDA_VERB_GET_DATA_ISLAND_PACKET_INDEX   0xF30
#define HDA_VERB_SET_DATA_ISLAND_PACKET_INDEX   0x730

// Get/Set Data Island Packet – Data.
#define HDA_VERB_GET_DATA_ISLAND_PACKET_DATA    0xF31
#define HDA_VERB_SET_DATA_ISLAND_PACKET_DATA    0x731

// Get/Set Data Island Packet – Transmit Control.
#define HDA_VERB_GET_DATA_ISLAND_PACKET_XMIT    0xF32
#define HDA_VERB_SET_DATA_ISLAND_PACKET_XMIT    0x732

// Get/Set Content Protection Control (CP_CONTROL).
#define HDA_VERB_GET_CP_CONTROL 0xF33
#define HDA_VERB_SET_CP_CONTROL 0x733

 // Get/Set ASP Channel Mapping.
#define HDA_VERB_GET_ASP_MAPPING    0xF34
#define HDA_VERB_SET_ASP_MAPPING    0x734

 // Execute Function Reset.
#define HDA_VERB_EXE_FUNC_RESET 0x7FF

// Get Parameter.
#define HDA_VERB_GET_PARAMETER                  0xF00

//
// Parameters.
//
// Vendor ID.
#define HDA_PARAMETER_VENDOR_ID                 0x00
#define HDA_PARAMETER_VENDOR_ID_DEV(a)          ((UINT16)a)
#define HDA_PARAMETER_VENDOR_ID_VEN(a)          ((UINT16)(a >> 16))

// Revision ID.
#define HDA_PARAMETER_REVISION_ID               0x02
#define HDA_PARAMETER_REVISION_ID_STEPPING(a)   ((UINT8)a)
#define HDA_PARAMETER_REVISION_ID_REV_ID(a)     ((UINT8)(a >> 8))
#define HDA_PARAMETER_REVISION_ID_MINOR_REV(a)  ((UINT8)((a >> 16) & 0xF))
#define HDA_PARAMETER_REVISION_ID_MAJOR_REV(a)  ((UINT8)((a >> 20) & 0xF))

// Subordinate Node Count.
#define HDA_PARAMETER_SUBNODE_COUNT             0x04
#define HDA_PARAMETER_SUBNODE_COUNT_TOTAL(a)    ((UINT8)a);
#define HDA_PARAMETER_SUBNODE_COUNT_START(a)    ((UINT8)(a >> 16));

// Function Group Type.
#define HDA_PARAMETER_FUNC_GROUP_TYPE               0x05
#define HDA_PARAMETER_FUNC_GROUP_TYPE_NODETYPE(a)   ((UINT8)a)
#define HDA_PARAMETER_FUNC_GROUP_TYPE_UNSOL         BIT8
#define HDA_FUNC_GROUP_TYPE_AUDIO   0x01
#define HDA_FUNC_GROUP_TYPE_MODEM   0x02

// Audio Function Group Capabilities.
#define HDA_PARAMETER_FUNC_GROUP_CAPS               0x08
#define HDA_PARAMETER_FUNC_GROUP_CAPS_OUT_DELAY(a)  ((UINT8)(a & 0xF))
#define HDA_PARAMETER_FUNC_GROUP_CAPS_IN_DELAY(a)   ((UINT8)((a >> 8) & 0xF))
#define HDA_PARAMETER_FUNC_GROUP_CAPS_BEEP_GEN      BIT16

// Audio Widget Capabilities.
#define HDA_PARAMETER_WIDGET_CAPS                   0x09
#define HDA_PARAMETER_WIDGET_CAPS_STEREO            BIT0
#define HDA_PARAMETER_WIDGET_CAPS_IN_AMP            BIT1
#define HDA_PARAMETER_WIDGET_CAPS_OUT_AMP           BIT2
#define HDA_PARAMETER_WIDGET_CAPS_AMP_OVERRIDE      BIT3
#define HDA_PARAMETER_WIDGET_CAPS_FORMAT_OVERRIDE   BIT4
#define HDA_PARAMETER_WIDGET_CAPS_STRIPE            BIT5
#define HDA_PARAMETER_WIDGET_CAPS_PROC_WIDGET       BIT6
#define HDA_PARAMETER_WIDGET_CAPS_UNSOL_CAPABLE     BIT7
#define HDA_PARAMETER_WIDGET_CAPS_CONN_LIST         BIT8
#define HDA_PARAMETER_WIDGET_CAPS_DIGITAL           BIT9
#define HDA_PARAMETER_WIDGET_CAPS_POWER_CNTRL       BIT10
#define HDA_PARAMETER_WIDGET_CAPS_L_R_SWAP          BIT11
#define HDA_PARAMETER_WIDGET_CAPS_CP_CAPS           BIT12
#define HDA_PARAMETER_WIDGET_CAPS_CHAN_COUNT(a)     ((UINT8)(((a >> 12) & 0xE) | (a & 0x1)))
#define HDA_PARAMETER_WIDGET_CAPS_DELAY(a)          ((UINT8)((a >> 16) & 0xF))
#define HDA_PARAMETER_WIDGET_CAPS_TYPE(a)           ((UINT8)((a >> 20) & 0xF))

// Widget types.
#define HDA_WIDGET_TYPE_OUTPUT          0x0
#define HDA_WIDGET_TYPE_INPUT           0x1
#define HDA_WIDGET_TYPE_MIXER           0x2
#define HDA_WIDGET_TYPE_SELECTOR        0x3
#define HDA_WIDGET_TYPE_PIN_COMPLEX     0x4
#define HDA_WIDGET_TYPE_POWER           0x5
#define HDA_WIDGET_TYPE_VOLUME_KNOB     0x6
#define HDA_WIDGET_TYPE_BEEP_GEN        0x7
#define HDA_WIDGET_TYPE_VENDOR          0xF

// Supported PCM Size, Rates.
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES          0x0A
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_8KHZ     BIT0
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_11KHZ    BIT1
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_16KHZ    BIT2
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_22KHZ    BIT3
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_32KHZ    BIT4
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_44KHZ    BIT5
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_48KHZ    BIT6
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_88KHZ    BIT7
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_96KHZ    BIT8
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_176KHZ   BIT9
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_192KHZ   BIT10
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_384KHZ   BIT11
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_8BIT     BIT16
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_16BIT    BIT17
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_20BIT    BIT18
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_24BIT    BIT19
#define HDA_PARAMETER_SUPPORTED_PCM_SIZE_RATES_32BIT    BIT20

// Supported Stream Formats.
#define HDA_PARAMETER_SUPPORTED_STREAM_FORMATS          0x0B
#define HDA_PARAMETER_SUPPORTED_STREAM_FORMATS_PCM      BIT0
#define HDA_PARAMETER_SUPPORTED_STREAM_FORMATS_FLOAT32  BIT1
#define HDA_PARAMETER_SUPPORTED_STREAM_FORMATS_AC3      BIT2

// Pin Capabilities.
#define HDA_PARAMETER_PIN_CAPS              0x0C
#define HDA_PARAMETER_PIN_CAPS_IMPEDANCE    BIT0
#define HDA_PARAMETER_PIN_CAPS_TRIGGER      BIT1
#define HDA_PARAMETER_PIN_CAPS_PRESENCE     BIT2
#define HDA_PARAMETER_PIN_CAPS_HEADPHONE    BIT3
#define HDA_PARAMETER_PIN_CAPS_OUTPUT       BIT4
#define HDA_PARAMETER_PIN_CAPS_INPUT        BIT5
#define HDA_PARAMETER_PIN_CAPS_BALANCED     BIT6
#define HDA_PARAMETER_PIN_CAPS_HDMI         BIT7
#define HDA_PARAMETER_PIN_CAPS_VREF(a)      ((UINT8)(a >> 8))
#define HDA_PARAMETER_PIN_CAPS_EAPD         BIT16
#define HDA_PARAMETER_PIN_CAPS_DISPLAYPORT  BIT24
#define HDA_PARAMETER_PIN_CAPS_HBR          BIT27

// Amplifier Capabilities (input and output).
#define HDA_PARAMETER_AMP_CAPS_INPUT            0x0D
#define HDA_PARAMETER_AMP_CAPS_OUTPUT           0x12
#define HDA_PARAMETER_AMP_CAPS_OFFSET(a)        ((UINT8)(a & 0x7F))
#define HDA_PARAMETER_AMP_CAPS_NUM_STEPS(a)     ((UINT8)((a >> 8) & 0x7F))
#define HDA_PARAMETER_AMP_CAPS_STEP_SIZE(a)     ((UINT8)((a >> 16) & 0x7F))
#define HDA_PARAMETER_AMP_CAPS_MUTE             BIT31

// Connection List Length.
#define HDA_PARAMETER_CONN_LIST_LENGTH          0x0E
#define HDA_PARAMETER_CONN_LIST_LENGTH_LEN(a)   ((UINT8)(a & 0x7F))
#define HDA_PARAMETER_CONN_LIST_LENGTH_LONG     BIT7

// Supported Power States.
#define HDA_PARAMETER_SUPPORTED_POWER_STATES            0x0F
#define HDA_PARAMETER_SUPPORTED_POWER_STATES_D0         BIT0
#define HDA_PARAMETER_SUPPORTED_POWER_STATES_D1         BIT1
#define HDA_PARAMETER_SUPPORTED_POWER_STATES_D2         BIT2
#define HDA_PARAMETER_SUPPORTED_POWER_STATES_D3         BIT3
#define HDA_PARAMETER_SUPPORTED_POWER_STATES_D3_COLD    BIT4
#define HDA_PARAMETER_SUPPORTED_POWER_STATES_S3_D3_COLD BIT29
#define HDA_PARAMETER_SUPPORTED_POWER_STATES_CLKSTOP    BIT30
#define HDA_PARAMETER_SUPPORTED_POWER_STATES_EPSS       BIT31

// Processing Capabilities.
#define HDA_PARAMETER_PROCESSING_CAPS               0x10
#define HDA_PARAMETER_PROCESSING_CAPS_BENIGN        BIT0
#define HDA_PARAMETER_PROCESSING_CAPS_NUM_COEFF(a)  ((UINT8)(a >> 8))

// GPIO Count.
#define HDA_PARAMETER_GPIO_COUNT                0x11
#define HDA_PARAMETER_GPIO_COUNT_NUM_GPIOS(a)   ((UINT8)a)
#define HDA_PARAMETER_GPIO_COUNT_NUM_GPOS(a)    ((UINT8)(a >> 8))
#define HDA_PARAMETER_GPIO_COUNT_NUM_GPIS(a)    ((UINT8)(a >> 16))
#define HDA_PARAMETER_GPIO_COUNT_GPI_UNSOL      BIT30
#define HDA_PARAMETER_GPIO_COUNT_GPI_WAKE       BIT31

// Volume Knob Capabilities.
#define HDA_PARAMETER_VOLUME_KNOB_CAPS              0x13
#define HDA_PARAMETER_VOLUME_KNOB_CAPS_NUM_STEPS(a) ((UINT8)(a & 0x7F))
#define HDA_PARAMETER_VOLUME_KNOB_CAPS_DELTA        BIT7

#endif
