#ifndef __onewire_master_api_h__
#define __onewire_master_api_h__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef S32
#define S32 int
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

#ifndef ATT_PACKED
#define ATT_PACKED __attribute__((packed))
#endif

#ifndef RET_OK
#define RET_OK 0
#endif

#ifndef RET_ERROR
#define RET_ERROR -1
#endif

#ifndef RET_S32
#define RET_S32 S32
#endif

/** IPSO data type offset to reduce to 8 bit data types used in CayenneLPP */
#define RAK_IPSO_DATA_TYPE_OFFSET 3200

/** Known data types */
typedef enum {
    RAK_IPSO_DIGITAL_INPUT = (3200 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_DIGITAL_OUTPUT = (3201 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_ANALOG_INPUT = (3202 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_ANALOG_OUT = (3203 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_NITROGEN = (3216 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_PHOSPHORUS = (3217 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_POTASSIUM = (3218 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_SALINITY = (3219 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_DISS_OXYGEN = (3220 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_ORP = (3221 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_COD = (3222 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_TURBIDITY = (3223 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_NO3 = (3224 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_NH4PLUS = (3225 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_BOD = (3226 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_ILLUM_SENSOR = (3301 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_PRESENCE_SENSOR = (3302 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_TEMP_SENSOR = (3303 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_HUMIDITY_SENSOR = (3304 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_GAS = (3305 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_HP_HUMIDITY = (3312 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_ACCELEROMETER = (3313 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_BAROMETER = (3315 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_BATTERVALUE = (3316 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_PRECIPITATION = (3319 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_GASPERCENTAGE = (3320 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_CO2 = (3325 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_SSN = (3326 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_HP_EC = (3327 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_DISTANCE = (3330 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_GYROMETER = (3334 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_GPS_LOCAL = (3336 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_GNSS_ENHANCED = (3337 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_VOC = (3338 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_GUSTWINDSPEED = (3346 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_STRIKES = (3347 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_CAPACITY = (3384 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_DC_CURRENT = (3385 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_DC_VOLTAGE = (3386 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_MOISTURE = (3388 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_WIND = (3390 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_WIND_DIR = (3391 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_EC = (3392 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_HP_PH = (3393 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_PH = (3394 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_PYRANOMETER = (3395 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_PM10 = (3427 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_PM25 = (3428 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_XYORIENTATION = (3429 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_NOISE = (3433 - RAK_IPSO_DATA_TYPE_OFFSET),
    RAK_IPSO_MODBUS = 0xF1,
    RAK_IPSO_SDI12 = 0xF2,
    RAK_IPSO_BINARY2BYTE = 0xF3,
    RAK_IPSO_BINARY4BYTE = 0xF4,
    RAK_IPSO_FLOAT_IEEE754 = 0xF5,
    RAK_IPSO_INTEGER32 = 0xF6,
    RAK_IPSO_UINTEGER32 = 0xF7,
    RAK_IPSO_BINARYTLV = 0xF8,
} RAK_IPSO_DEF_E;

/** RAK OneWireProtocol event types */
typedef enum {
    SNHUBAPI_EVT_RECV_REQ = 0,
    SNHUBAPI_EVT_RECV_RSP,
    SNHUBAPI_EVT_ADD_PID,
    SNHUBAPI_EVT_ADD_SID,
    SNHUBAPI_EVT_GET_INTV,
    SNHUBAPI_EVT_GET_ENABLE,
    SNHUBAPI_EVT_SEQ_ERR,
    SNHUBAPI_EVT_CHKSUM_ERR,
    SNHUBAPI_EVT_QSEND,
    SNHUBAPI_EVT_REPORT,
    SNHUBAPI_EVT_SDATA_REQ,
    /** IOC response: msg points to hub inner payload (funcode, iface, action, …). */
    SNHUBAPI_EVT_IOC_RSP,
    SNHUBAPI_EVT_MAX,
} SNHUBAPI_EVT_E;

/** Definition of RAK OneWire Protocol event handler */
typedef void (*SNHub_Evt_t)(const U8 pid, const U8 sid, const SNHUBAPI_EVT_E eid, U8 *msg, U16 len);

/** Definition of RAK OneWire Protocol API calls */
typedef struct {
    void (*init)(SNHub_Evt_t onevt);

    void (*process)(U8 *msg, U16 len);

    struct {
        void (*data)(U8 pid);
        void (*param)(U8 pid, U8 sid);
    } get;

    struct {
        void (*param)(U8 pid, U8 sid, U8 enable, U32 intv);
    } set;

    struct {
        void (*send)(U8 pid, U8 funcode, U8 iface, U8 action, const U8 *data, U16 data_len);
    } ioc;

    void (*reboot)(void);

} ATT_PACKED RakSNHub_Protocl_API_t;

extern const RakSNHub_Protocl_API_t RakSNHub_Protocl_API;

/**
 * IOC typed helper APIs (MVP: IO_CFG / IO_ADDPOLL / IO_ENABLEPOLL).
 */
typedef struct {
    U8 funcode;
    U8 iface;
    U8 action;
    const U8 *data;
    U16 data_len;
} ATT_PACKED RakSNHub_IOC_Rsp_t;

RET_S32 RakSNHub_IOC_ParseRsp(const U8 *msg, U16 len, RakSNHub_IOC_Rsp_t *rsp);
RET_S32 RakSNHub_IOC_ConfigRS485(U8 pid, U32 baudrate, U8 databit, U8 stopbit, U8 parity);
RET_S32 RakSNHub_IOC_AddPoll(U8 pid, U8 iface, U8 taskid, const U8 *cmd, U8 cmd_len, U32 period, U32 timeout, U8 retry);
RET_S32 RakSNHub_IOC_AddPollHex(U8 pid, U8 iface, U8 taskid, const char *cmd_hex, U32 period, U32 timeout, U8 retry);
RET_S32 RakSNHub_IOC_AddPollEx(U8 pid, U8 iface, U8 taskid, const U8 *cmd, U8 cmd_len, U32 period, U32 timeout, U8 retry,
                              U8 ipso, float scale, U8 datatype, const char *snsr_name);
RET_S32 RakSNHub_IOC_EnablePoll(U8 pid, U8 iface, U8 taskid, U8 enable);
RET_S32 RakSNHub_IOC_PollTask(U8 pid, U8 iface, U8 taskid);
RET_S32 RakSNHub_IOC_PassThrough(U8 pid, U8 iface, const U8 *cmd, U8 cmd_len, U32 timeout);
RET_S32 RakSNHub_IOC_DecodeAIC(U8 pid, U8 taskid, U8 ipso, S32 min, S32 max, float offset, const char *snsr_name);
RET_S32 RakSNHub_IOC_RmPollDef(U8 pid, U8 iface, U8 portid);

#ifdef __cplusplus
}
#endif

#endif