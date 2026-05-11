#ifndef __rak_ioc_types_h__
#define __rak_ioc_types_h__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef U8
#define U8 unsigned char
#endif

#ifndef U16
#define U16 unsigned short
#endif

#ifndef U32
#define U32 unsigned int
#endif

#ifndef S32
#define S32 signed int
#endif

#ifndef ATT_PACKED
#define ATT_PACKED __attribute__((packed))
#endif

typedef struct {
    U8 id;
    U8 type;
    U8 datalen;
    U8 data[];
} ATT_PACKED rak_ioc_data_t;

typedef struct {
    U8 id;
    U8 type;
    U8 data[];
} ATT_PACKED rak_ioc_value_t;

typedef struct {
    U8 funcode;
    U8 iface;
    U8 action;
    U8 data[];
} ATT_PACKED rak_ioc_gen_frame_t;

typedef struct {
    U32 timeout;
    U8 cmd[];
} ATT_PACKED rak_ioc_passthrh_frame_t;

typedef struct {
    U8 taskid;
    U32 period;
    U32 timeout;
    U8 retry;
    U8 cmd[];
} ATT_PACKED rak_ioc_addpoll_frame_t;

typedef struct {
    U8 taskid;
    U32 period;
    U32 timeout;
    U8 retry;
    U8 IPSO;
    float scale;
    U8 datatype;
    U8 snsr_name[16];
    U8 cmd[];
} ATT_PACKED rak_ioc_addpollex_frame_t;

typedef struct {
    U8 taskid;
    U8 enable;
} ATT_PACKED rak_ioc_enablepoll_frame_t;

typedef struct {
    U8 taskid;
} ATT_PACKED rak_ioc_polltask_frame_t;

typedef struct {
    U8 if_id;
    U8 portid;
} ATT_PACKED rak_ioc_rmpdef_frame_t;

typedef struct {
    union {
        U8 value[12];
        struct {
            U32 baudrate;
            U8 databit;
            U8 stopbit;
            U8 parity;
        };
        struct {
            U8 ach;
            U32 period;
            U32 debouncing;
        };
        struct {
            U8 dch;
            U8 on_off;
        };
    } cfg;
} ATT_PACKED rak_ioc_cfg_frame_t;

typedef struct {
    U8 ach;
    U8 value;
} ATT_PACKED rak_ioc_data_frame_t;

// core-1.2.27: com_if_rak_bank_decode_page_t is a 32-byte page stored in EEPROM BK3.
// It is used by IO_DECODE for AIC/DI/DO mapping (IPSO/min/max/name/offset, etc.).
typedef struct {
    union {
        U8 decode[32];
        struct {
            U8 IPSO;
            S32 min;
            S32 max;
            U8 snsr_name[16];
            float offset;
        } aic;
        struct {
            U8 IPSO;
            S32 trigger_mode;
            S32 debounce;
            U8 snsr_name[16];
        } dig_o;
        struct {
            U8 IPSO;
            S32 trigger_mode;
            S32 debounce;
            U8 snsr_name[16];
        } dig_i;
    } u;
} ATT_PACKED com_if_rak_bank_decode_page_t;

typedef struct {
    U8 taskid;
    com_if_rak_bank_decode_page_t param;
} ATT_PACKED rak_ioc_decode_frame_t;

typedef enum {
    IOPASSTHRH,
    IO_ADDPOLL,
    IO_ENABLEPOLL,
    IO_POLLTASK,
    IO_CFG,
    IO_DATA,
    IO_POLLCNT,
    IO_RMPOLL,
    IO_DECODE,
    IO_POLLTASKEX,
    IO_ADDPOLLEX,
    IO_PSM,
    IO_RMPDEF,
    IO_CNT,
    IOC_MAXCODE,
} COM_TASK_RAK_IOC_FUNCCODE_L;

typedef enum {
    IOC_NO_IFACE = 0,
    IOC_RS485 = 1,
    IOC_SDI12,
    IOC_RS232,
    IOC_CONTROL = 9,
    IOC_UART_END = 10,
    IOC_AIC = 11, // analog input current (4-20mA)
    IOC_AIV,      // analog input voltage
    IOC_DI,       // digital input
    IOC_DO,       // digital output

    // Backward-compatible aliases (older naming in this library)
    IOC_MAMETER = IOC_AIC,
    IOC_VOLMETER = IOC_AIV,
} COM_TASK_RAK_IOC_IFACE_L;

typedef enum {
    IOA_REQ = 1,
    IOA_RSP,
    IOA_PRINT,
    IOA_NOH_PRINT,
} COM_TASK_RAK_IOC_ACTION_L;

typedef enum {
    COM_TASK_RAK_IOC_REQ,
    COM_TASK_RAK_IOC_RSP,
} COM_TASK_RAK_IOC_ID_E;

#ifdef __cplusplus
}
#endif

#endif
