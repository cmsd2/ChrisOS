#ifndef IO_PORTS_H
#define IO_PORTS_H

#define NMI_STATUS_AND_CONTROL 0x61
#define NMI_SC_MASK(unmasked) ((uint8_t)(~(unmasked)))
#define NMI_SC_TMR2_ENABLE 1
#define NMI_SC_SPEAKER_ENABLE 2
#define NMI_SC_PCI_SERR_ENABLE 4
#define NMI_SC_IOCHK_ENABLE 8
#define NMI_SC_REF_TOGGLE 0x10
#define NMI_SC_TMR2_OUT_STS 0x20
#define NMI_SC_IOCHK_STS 0x40
#define NMI_SC_SERR_STS 0x80

#endif
