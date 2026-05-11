#include "onewire_master_protocol.h"
#include "onewire_master_api.h"
#include "rak_ioc_types.h"

#define REC_NUM 1
#define BUFF_SIZE 0x100

#ifndef NULL
#define NULL 0
#endif

#ifndef ENABLE
#define ENABLE 1
#endif

#ifndef DISABLE
#define DISABLE 0
#endif

#define SHORT_SWAP(X) (((X & 0xFF) << 8) | ((X >> 8) & 0xFF))
#define LSB_COMB(M, L) ((L << 8) + M)
#define SUB(A, B) ((A == 0) || (B > A)) ? 0 : (A - B)
#define CKTODO(X)                                                                                                                \
    if (X != NULL)                                                                                                               \
    X

#define f_memset(P, V, L)                                                                                                        \
    do {                                                                                                                         \
        U8 *_p = (U8 *)(P);                                                                                                      \
        for (U32 _i = 0; _i < (U32)(L); _i++)                                                                                    \
            _p[_i] = (U8)(V);                                                                                                    \
    } while (0)
#define f_memcmp(A, B, L) this_memcmp(A, B, L)
#define f_memcpy(A, B, L) this_memcpy(A, B, L)

typedef RET_S32 (*program_process)(U8 *data, U16 len);
typedef RET_S32 (*command_process)(U8 pid, U8 sid, SNHUB_GS_E gset, U8 ptye);

static U16 this_strlen(const char *s)
{
    U16 n = 0;
    if (s == NULL) {
        return 0;
    }
    while (s[n] != '\0' && n < 0xFFFFu) {
        n++;
    }
    return n;
}

/** RAK OneWire Protocol IPSO code table structure definition */
typedef struct {
    U8 size;
} rakipso_tbl_t;

/** RAK OneWire process definition structure */
typedef struct {
    program_process req;
    program_process rsp;
} protocol_process_t;

static SNHub_Record_t record[REC_NUM];
static SNHub_Menu_t menu;
static SNHub_Evt_t on_evt;
static U8 dataBuff[BUFF_SIZE];

static const command_process command_list[];
static const protocol_process_t protocol_list[];
static const rakipso_tbl_t rakipso_tbl[] = {
    [RAK_IPSO_DIGITAL_INPUT] = {.size = 1},
    [RAK_IPSO_DIGITAL_OUTPUT] =
        {
            .size = 1,
        },
    [RAK_IPSO_ANALOG_INPUT] =
        {
            .size = 2,
        },
    [RAK_IPSO_ANALOG_OUT] =
        {
            .size = 2,
        },
    [RAK_IPSO_NITROGEN] =
        {
            .size = 2,
        },
    [RAK_IPSO_PHOSPHORUS] =
        {
            .size = 2,
        },
    [RAK_IPSO_POTASSIUM] =
        {
            .size = 2,
        },
    [RAK_IPSO_SALINITY] =
        {
            .size = 2,
        },
    [RAK_IPSO_DISS_OXYGEN] =
        {
            .size = 2,
        },
    [RAK_IPSO_ORP] =
        {
            .size = 2,
        },
    [RAK_IPSO_COD] =
        {
            .size = 2,
        },
    [RAK_IPSO_TURBIDITY] =
        {
            .size = 2,
        },
    [RAK_IPSO_NO3] =
        {
            .size = 2,
        },
    [RAK_IPSO_NH4PLUS] =
        {
            .size = 2,
        },
    [RAK_IPSO_BOD] =
        {
            .size = 2,
        },
    [RAK_IPSO_ILLUM_SENSOR] =
        {
            .size = 4,
        },
    [RAK_IPSO_PRESENCE_SENSOR] =
        {
            .size = 1,
        },
    [RAK_IPSO_TEMP_SENSOR] =
        {
            .size = 2,
        },
    [RAK_IPSO_HUMIDITY_SENSOR] =
        {
            .size = 1,
        },
    [RAK_IPSO_GAS] =
        {
            .size = 2,
        },
    [RAK_IPSO_HP_HUMIDITY] =
        {
            .size = 2,
        },
    [RAK_IPSO_ACCELEROMETER] =
        {
            .size = 6,
        },
    [RAK_IPSO_BAROMETER] =
        {
            .size = 2,
        },
    [RAK_IPSO_BATTERVALUE] =
        {
            .size = 2,
        },
    [RAK_IPSO_PRECIPITATION] =
        {
            .size = 2,
        },
    [RAK_IPSO_GASPERCENTAGE] =
        {
            .size = 1,
        },
    [RAK_IPSO_CO2] =
        {
            .size = 2,
        },
    [RAK_IPSO_SSN] =
        {
            .size = 3,
        },
    [RAK_IPSO_HP_EC] =
        {
            .size = 4,
        },
    [RAK_IPSO_DISTANCE] =
        {
            .size = 4,
        },
    [RAK_IPSO_GYROMETER] =
        {
            .size = 6,
        },
    [RAK_IPSO_GPS_LOCAL] =
        {
            .size = 9,
        },
    [RAK_IPSO_GNSS_ENHANCED] =
        {
            .size = 11,
        },
    [RAK_IPSO_VOC] =
        {
            .size = 2,
        },
    [RAK_IPSO_GUSTWINDSPEED] =
        {
            .size = 2,
        },
    [RAK_IPSO_STRIKES] =
        {
            .size = 2,
        },
    [RAK_IPSO_CAPACITY] =
        {
            .size = 1,
        },
    [RAK_IPSO_DC_CURRENT] =
        {
            .size = 2,
        },
    [RAK_IPSO_DC_VOLTAGE] =
        {
            .size = 2,
        },
    [RAK_IPSO_MOISTURE] =
        {
            .size = 2,
        },
    [RAK_IPSO_WIND] =
        {
            .size = 2,
        },
    [RAK_IPSO_WIND_DIR] =
        {
            .size = 2,
        },
    [RAK_IPSO_EC] =
        {
            .size = 2,
        },
    [RAK_IPSO_HP_PH] =
        {
            .size = 2,
        },
    [RAK_IPSO_PH] =
        {
            .size = 2,
        },
    [RAK_IPSO_PYRANOMETER] =
        {
            .size = 2,
        },
    [RAK_IPSO_PM10] =
        {
            .size = 2,
        },
    [RAK_IPSO_PM25] =
        {
            .size = 2,
        },
    [RAK_IPSO_XYORIENTATION] =
        {
            .size = 2,
        },
    [RAK_IPSO_NOISE] =
        {
            .size = 2,
        },
    [RAK_IPSO_MODBUS] =
        {
            .size = 64,
        },
    [RAK_IPSO_SDI12] =
        {
            .size = 64,
        },
    [RAK_IPSO_BINARY2BYTE] =
        {
            .size = 2,
        },
    [RAK_IPSO_BINARY4BYTE] =
        {
            .size = 4,
        },
    [RAK_IPSO_FLOAT_IEEE754] =
        {
            .size = 4,
        },
    [RAK_IPSO_INTEGER32] =
        {
            .size = 4,
        },
    [RAK_IPSO_UINTEGER32] =
        {
            .size = 4,
        },
    [RAK_IPSO_BINARYTLV] =
        {
            .size = 64,
        },
};

/**
 * @brief PopCount, used to calculate the checksum
 * 
 * @param u value to get popcount
 * @return unsigned popcount
 */
static unsigned builtin_popcount(unsigned u)
{
    u = (u & 0x55555555) + ((u >> 1) & 0x55555555);
    u = (u & 0x33333333) + ((u >> 2) & 0x33333333);
    u = (u & 0x0F0F0F0F) + ((u >> 4) & 0x0F0F0F0F);
    u = (u & 0x00FF00FF) + ((u >> 8) & 0x00FF00FF);
    u = (u & 0x0000FFFF) + ((u >> 16) & 0x0000FFFF);
    return u;
}

/**
 * @brief Calculate the checksum of the received data
 * 
 * @param data received data
 * @param len length of received data
 * @return U8 calculated checksum
 */
static U8 cal_chksum(U8 *data, U16 len)
{
    RUI3_Api_t *rui3_api = (RUI3_Api_t *)data;

    U8 chsum = 0;
    chsum += builtin_popcount(rui3_api->type);
    chsum += builtin_popcount(rui3_api->flag);

    U16 payload_len = LSB_COMB(rui3_api->length.hbyte, rui3_api->length.lbyte);

    if (payload_len > len) {
        return 0;
    }

    for (U16 i = 0; i < payload_len; i++) {
        chsum += builtin_popcount(rui3_api->payload[i]);
    }
    return chsum;
}

/**
 * @brief Simple memory compare
 * 
 * @param A pointer to source A as uint8_t array
 * @param B pointer to source B as uint8_t array
 * @param len length of array
 * @return RET_S32 0 == identical, else position of difference
 */
static RET_S32 this_memcmp(U8 *A, U8 *B, U16 len)
{
    for (U16 i = 0; i < len; i++) {
        if (A[i] != B[i]) {
            return i;
        }
    }
    return 0;
}

/**
 * @brief Simple memory copy
 *
 * @param A pointer to destination as uint8_t array
 * @param B pointer to source as uint8_t array
 * @param len length of array
 * @return RET_S32 always 0
 */
static RET_S32 this_memcpy(U8 *A, U8 *B, U16 len)
{
    for (U16 i = 0; i < len; i++) {
        A[i] = B[i];
    }
    return 0;
}

static U8 get_sdata_value_len(SNHub_Api_t *hub_api, U16 ofs, SNHub_Api_sData_Snsr_t *sData)
{
    U16 remaining;

    if (hub_api->payload_length <= (ofs + sizeof(SNHub_Api_sData_Snsr_t))) {
        return 0;
    }

    remaining = (U16)(hub_api->payload_length - ofs - sizeof(SNHub_Api_sData_Snsr_t));

    if (sData->ipso == RAK_IPSO_MODBUS || sData->ipso == RAK_IPSO_SDI12) {
        /* Two possible payload layouts exist in the field:
         * A) [len][data...]
         * B) rak_ioc_data_t: [id][type=0xF1/0xF2][datalen][data...]
         */
        if (remaining >= 3u && sData->value[1] == RAK_IPSO_MODBUS) {
            U8 datalen = sData->value[2];
            if ((U16)datalen + 3u <= remaining) {
                return (U8)(datalen + 3u); // id + type + datalen + data
            }
            return (remaining > 255u) ? 255u : (U8)remaining;
        }

        U8 data_len = sData->value[0];
        if ((U16)data_len + 1u <= remaining) {
            return (U8)(data_len + 1u); // include the variable-length prefix byte
        }
        return (remaining > 255u) ? 255u : (U8)remaining;
    }

    return rakipso_tbl[sData->ipso].size;
}

/**
 * @brief Send request for Sensor data
 * 
 * @param data data to be sent
 * @param len size of data
 * @return RET_S32 always RET_OK
 */
static RET_S32 snhub_snsrdat_req_program(U8 *data, U16 len)
{
    RUI3_Api_t *rui3_api = (RUI3_Api_t *)data;
    SNHub_Api_t *hub_api = (SNHub_Api_t *)(rui3_api->payload);
    U16 ofs = 0;
    U8 pid = hub_api->source;

    /* Some probes may respond to SENDAT with short/ack-style payloads.
     * Guard the parser strictly to avoid decoding garbage as IPSO[00].
     */
    while ((ofs + sizeof(SNHub_Api_sData_Snsr_t)) <= hub_api->payload_length) {
        SNHub_Api_sData_Snsr_t *sData = (SNHub_Api_sData_Snsr_t *)&hub_api->payload[ofs];
        U8 val_len = get_sdata_value_len(hub_api, ofs, sData);
        U16 item_len = (U16)sizeof(SNHub_Api_sData_Snsr_t) + val_len;

        if (val_len == 0 || (ofs + item_len) > hub_api->payload_length) {
            break;
        }

        on_evt(pid, sData->sid, SNHUBAPI_EVT_REPORT, &(sData->ipso), val_len + 1 /* include ipso byte */);
        ofs += item_len;
    }

    return RET_OK;
}

/**
 * @brief Send request for provisioning data
 *
 * @param data data to be sent
 * @param len size of data
 * @return RET_S32 RET_ERROR if invalid request, else RET_OK
 */
static RET_S32 snhub_provision_req_program(U8 *data, U16 len)
{
    RUI3_Api_t *rui3_api = (RUI3_Api_t *)data;
    SNHub_Api_t *hub_api = (SNHub_Api_t *)(rui3_api->payload);
    SNHub_Api_Provision_t *hub_api_prov = (SNHub_Api_Provision_t *)(hub_api->payload);

    U8 pktBuff[BUFF_SIZE];
    U16 pktLen = 0;

    switch (hub_api->payload_type) {
    case PLD_PROVI_TYPE_VER3:
        /* do nothing, just bypass */
        break;
    case PLD_PROVI_TYPE_VER1:
    case PLD_PROVI_TYPE_VER2:
    case PLD_PROVI_TYPE_BOOT:
    default:
        /* not support */
        return RET_ERROR;
    }

    U16 aid = 0;
    do {
        if (aid == REC_NUM) {
            return RET_ERROR;
        }

        if (record[aid].alive == DISABLE) {
            break;
        } else {
            if (f_memcmp(record[aid].info.sn.u, hub_api_prov->sn.u, sizeof(SERIALNUM)) == 0) {
                break;
            }
        }
    } while (aid++);

    if (record[aid].alive == DISABLE) {
        record[aid].alive = ENABLE;
        record[aid].snsrnum = hub_api_prov->snsr_num;
        f_memcpy((U8 *)&(record[aid].info), (U8 *)hub_api_prov, sizeof(SNHub_Api_Provision_t));
        for (U16 i = 0; i < record[aid].snsrnum; i++) {
            record[aid].snsrlist[i] = hub_api_prov->snsr_type[i].sid;
        }
    }

    f_memcpy(pktBuff, data, len);
    pktLen = len;

    rui3_api = (RUI3_Api_t *)pktBuff;
    hub_api = (SNHub_Api_t *)(rui3_api->payload);
    hub_api_prov = (SNHub_Api_Provision_t *)(hub_api->payload);

    U8 dest = hub_api->dest;
    U8 source = hub_api->source;

    rui3_api->flag = RUI3API_FLG_RSP;

    hub_api->dest = source;
    hub_api->source = dest;

    U8 pid = aid + 1; /* arry index to pid index */
    U8 sid = 0;
    hub_api_prov->provId = pid;

    U8 chsum = cal_chksum((U8 *)rui3_api, pktLen);
    U16 recv_len = LSB_COMB(rui3_api->length.hbyte, rui3_api->length.lbyte);
    rui3_api->payload[recv_len] = chsum;

    on_evt(source, 0, SNHUBAPI_EVT_QSEND, pktBuff, pktLen);
    on_evt(source, 0, SNHUBAPI_EVT_ADD_PID, &pid, sizeof(pid));

    for (U16 i = 0; i < record[aid].snsrnum; i++) {
        sid = hub_api_prov->snsr_type[i].sid;
        on_evt(source, 0, SNHUBAPI_EVT_ADD_SID, &sid, sizeof(sid));
    }
    return RET_OK;
}

/**
 * @brief Get response for sensor data
 *
 * @param data data to be sent
 * @param len size of data
 * @return RET_S32 always RET_OK
 */
static RET_S32 snhub_snsrdat_rsp_program(U8 *data, U16 len)
{
    RUI3_Api_t *rui3_api = (RUI3_Api_t *)data;
    SNHub_Api_t *hub_api = (SNHub_Api_t *)(rui3_api->payload);
    U16 ofs = 0;
    U8 pid = hub_api->source;

    /* Robust parse for mixed firmware behavior (normal data frames + short ack frames). */
    while ((ofs + sizeof(SNHub_Api_sData_Snsr_t)) <= hub_api->payload_length) {
        SNHub_Api_sData_Snsr_t *sData = (SNHub_Api_sData_Snsr_t *)&hub_api->payload[ofs];
        U8 val_len = get_sdata_value_len(hub_api, ofs, sData);
        U16 item_len = (U16)sizeof(SNHub_Api_sData_Snsr_t) + val_len;

        if (val_len == 0 || (ofs + item_len) > hub_api->payload_length) {
            break;
        }

        on_evt(pid, sData->sid, SNHUBAPI_EVT_SDATA_REQ, &(sData->ipso), val_len + 1 /* include ipso byte */);
        ofs += item_len;
    }

    return RET_OK;
}

/**
 * @brief Get response for parameter request
 *
 * @param data data to be sent
 * @param len size of data
 * @return RET_S32 always RET_OK
 */
static RET_S32 snhub_paramget_rsp_program(U8 *data, U16 len)
{
    RUI3_Api_t *rui3_api = (RUI3_Api_t *)data;
    SNHub_Api_t *hub_api = (SNHub_Api_t *)(rui3_api->payload);
    SNHub_Api_Param_Snsr_t *paramget = (SNHub_Api_Param_Snsr_t *)(hub_api->payload);

    U8 source = hub_api->source;

    on_evt(source, paramget->sid, SNHUBAPI_EVT_GET_INTV, (U8 *)&(paramget->intv), sizeof(paramget->intv));

    U8 enable = (paramget->rule == RULE_PERIODIC) ? ENABLE : DISABLE;

    on_evt(source, paramget->sid, SNHUBAPI_EVT_GET_ENABLE, (U8 *)&enable, sizeof(enable));
    return RET_OK;
}

/**
 * @brief Forward IOC response payload to the application (funcode, iface, action, …).
 */
static RET_S32 snhub_ioc_rsp_program(U8 *data, U16 len)
{
    (void)len;
    RUI3_Api_t *rui3_api = (RUI3_Api_t *)data;
    SNHub_Api_t *hub_api = (SNHub_Api_t *)(rui3_api->payload);

    // ProbeIO core-1.2.27+ uses:
    // - hub_api->payload_type as IOC funcode (IO_CFG/IO_ADDPOLLEX/...)
    // - hub_api->payload as rak_ioc_gen_frame_t: [iface][action][data...]
    //
    // For host convenience, forward a normalized buffer: [funcode][iface][action][data...]
    // (this matches RakSNHub_IOC_ParseRsp() expectations).
    do {
        U8 out[224];
        U16 out_len = 0;

        if (hub_api->payload_length < 2) {
            break;
        }
        if ((U32)hub_api->payload_length + 1u > sizeof(out)) {
            break;
        }

        out[0] = hub_api->payload_type;
        f_memcpy(&out[1], hub_api->payload, hub_api->payload_length);
        out_len = (U16)(hub_api->payload_length + 1u);
        on_evt(hub_api->source, 0, SNHUBAPI_EVT_IOC_RSP, out, out_len);
    } while (0);
    return RET_OK;
}

/**
 * @brief Build and queue an IOC request (inner body = rak_ioc_gen_frame + data).
 */
static RET_S32 snhub_ioc_send_raw(U8 pid, U8 funcode, const U8 *ioc_body, U16 ioc_body_len)
{
    U8 pktBuff[BUFF_SIZE];
    U16 pktLen = 0;
    U16 pldLen = ioc_body_len;

    if (pid == PID_MASTER) {
        return RET_ERROR;
    }
    if ((U32)sizeof(SNHub_Api_t) + (U32)pldLen + sizeof(RUI3_Api_t) + 1 > BUFF_SIZE) {
        return RET_ERROR;
    }

    RUI3_Api_t *rui3_api = (RUI3_Api_t *)pktBuff;
    SNHub_Api_t *hub_api = (SNHub_Api_t *)(rui3_api->payload);

    f_memset(pktBuff, 0, BUFF_SIZE);

    rui3_api->wakeup = WAKEUPBYTE;
    rui3_api->start = DELIMTER;
    rui3_api->type = RUI3API_TYPE_SENSORHUB;
    rui3_api->flag = RUI3API_FLG_REQ;
    hub_api->source = PID_MASTER;
    hub_api->dest = pid;
    hub_api->sequence = ++menu.seq;
    pktLen = sizeof(SNHub_Api_t);

    hub_api->type = SNHUB_TYPE_IOC;
    hub_api->payload_length = pldLen;
    // core-1.2.27+: payload_type carries IOC funcode
    hub_api->payload_type = funcode;
    f_memcpy(hub_api->payload, (U8 *)ioc_body, pldLen);
    pktLen += pldLen;

    rui3_api->length.value = pktLen;
    rui3_api->length.value = SHORT_SWAP(rui3_api->length.value);
    rui3_api->payload[pktLen] = cal_chksum((U8 *)rui3_api, BUFF_SIZE);

    pktLen += sizeof(RUI3_Api_t);
    pktLen += 1;

    on_evt(PID_MASTER, 0, SNHUBAPI_EVT_QSEND, pktBuff, pktLen);

    return RET_OK;
}

static void api_ioc_send(U8 pid, U8 funcode, U8 iface, U8 action, const U8 *data, U16 data_len)
{
    U8 body[220];

    if (pid == PID_MASTER) {
        return;
    }
    // core-1.2.27+: IOC inner payload is [iface][action][data...]
    if ((U32)data_len + 2u > sizeof(body)) {
        return;
    }
    body[0] = iface;
    body[1] = action;
    if (data_len != 0 && data != NULL) {
        f_memcpy(body + 2, (U8 *)data, data_len);
    }
    menu.result = SNHUB_RES_BUSY;
    (void)snhub_ioc_send_raw(pid, funcode, body, (U16)(2 + data_len));
}

/**
 * IOC command adapter for command_list[] compatibility.
 * IOC requests with payload should use api_ioc_send()/RakSNHub_IOC_* helpers.
 */
static RET_S32 snhub_ioc_command(U8 pid, U8 sid, SNHUB_GS_E gset, U8 ptye)
{
    (void)pid;
    (void)sid;
    (void)gset;
    (void)ptye;
    return RET_ERROR;
}

static U8 is_hex_ascii(U8 ch)
{
    return ((ch >= (U8)'0' && ch <= (U8)'9') || (ch >= (U8)'a' && ch <= (U8)'f') || (ch >= (U8)'A' && ch <= (U8)'F'));
}

static RET_S32 validate_hex_cmd(const U8 *cmd, U16 cmd_len)
{
    U16 i;

    if (cmd == NULL || cmd_len == 0 || (cmd_len & 1u) != 0) {
        return RET_ERROR;
    }

    for (i = 0; i < cmd_len; i++) {
        if (!is_hex_ascii(cmd[i])) {
            return RET_ERROR;
        }
    }

    return RET_OK;
}

// NOTE: ATCMD frames are not supported in IOC mode; keep library IOC-only.

/**
 * @brief Send request for parameter
 *
 * @param pid PID to be addressed
 * @param sid SID to be addressed
 * @param gset SNHUB_GS_GET = get parameter, SNHUB_GS_SET = set parameter
 * @param ptye parameter type
 * @return RET_S32 RET_ERROR if invalid request, else RET_OK
 */
static RET_S32 snhub_paramget_command(U8 pid, U8 sid, SNHUB_GS_E gset, U8 ptye)
{
    U8 pktBuff[BUFF_SIZE];
    U16 pktLen = 0;
    U16 pldLen = 0;

    RUI3_Api_t *rui3_api = (RUI3_Api_t *)pktBuff;
    SNHub_Api_t *hub_api = (SNHub_Api_t *)(rui3_api->payload);
    SNHub_Api_Param_Snsr_t *paramset = (SNHub_Api_Param_Snsr_t *)(hub_api->payload);

    f_memset(pktBuff, 0, BUFF_SIZE);

    rui3_api->wakeup = WAKEUPBYTE;
    rui3_api->start = DELIMTER;
    rui3_api->type = RUI3API_TYPE_SENSORHUB;
    rui3_api->flag = RUI3API_FLG_REQ;
    hub_api->source = PID_MASTER;
    hub_api->dest = pid;
    hub_api->sequence = ++menu.seq;
    pktLen = sizeof(SNHub_Api_t);

    switch (gset) {
    case SNHUB_GS_GET:
        hub_api->type = SNHUB_TYPE_PARAMGET;
        pldLen += 1; // add sid byte
        break;
    case SNHUB_GS_SET:
        hub_api->type = SNHUB_TYPE_PARAMSET;
        pldLen += sizeof(SNHub_Api_Param_Snsr_t);
        break;
    default:
        return RET_ERROR;
    }

    pktLen += pldLen;
    hub_api->payload_length = pldLen;
    hub_api->payload_type = ptye;

    switch (ptye) {
    case PLD_PARMGSET_TYPE_SNSR_UPDATE:
        hub_api->payload[0] = sid;
        break;
    case PLD_PARMGSET_TYPE_RULE:
    case PLD_PARMGSET_TYPE_SNSR_INTV:
    case PLD_PARMGSET_TYPE_PRB_INTV:
    case PLD_PARMGSET_TYPE_SNSR_HTHR:
    case PLD_PARMGSET_TYPE_SNSR_LTHR:
    case PLD_PARMGSET_TYPE_PRB_TAGID:
    case PLD_PARMGSET_TYPE_PRB_TAGEN:
    case PLD_PARMGSET_TYPE_PRB_UPDATE:
    case PLD_PARMGSET_TYPE_CONF_UPDATE:
        /* not support */
        return RET_ERROR;
    }

    if (gset == SNHUB_GS_SET) {
        paramset->intv = menu.intv;
        paramset->rule = menu.rule;
        menu.intv = 0;
        menu.rule = 0;
    }

    rui3_api->length.value = pktLen;
    rui3_api->length.value = SHORT_SWAP(rui3_api->length.value);
    rui3_api->payload[pktLen] = cal_chksum((U8 *)rui3_api, BUFF_SIZE);

    pktLen += sizeof(RUI3_Api_t);
    pktLen += 1 /* add checksum len*/;

    on_evt(PID_MASTER, sid, SNHUBAPI_EVT_QSEND, pktBuff, pktLen);

    return RET_OK;
}

/**
 * @brief Send request for provisioning
 *
 * @param pid PID to be addressed
 * @param sid SID to be addressed
 * @param gset SNHUB_GS_GET = get provisioning, SNHUB_GS_SET = set provisioning
 * @param ptye parameter type
 * @return RET_S32 RET_ERROR if invalid request, else RET_OK
 */
static RET_S32 snhub_provision_command(U8 pid, U8 sid, SNHUB_GS_E gset, U8 ptye)
{
    U8 pktBuff[BUFF_SIZE];
    U16 pktLen = 0;
    U16 pldLen = 0;

    RUI3_Api_t *rui3_api = (RUI3_Api_t *)pktBuff;
    SNHub_Api_t *hub_api = (SNHub_Api_t *)(rui3_api->payload);

    f_memset(pktBuff, 0, BUFF_SIZE);

    rui3_api->wakeup = WAKEUPBYTE;
    rui3_api->start = DELIMTER;
    rui3_api->type = RUI3API_TYPE_SENSORHUB;
    rui3_api->flag = RUI3API_FLG_REQ;
    hub_api->source = PID_MASTER;
    hub_api->dest = pid;
    hub_api->sequence = ++menu.seq;
    pktLen = sizeof(SNHub_Api_t);

    switch (gset) {
    case SNHUB_GS_SET:
        hub_api->type = SNHUB_TYPE_PROVISION;
        break;
    default:
    case SNHUB_GS_GET:
        return RET_ERROR;
    }

    pktLen += pldLen;
    hub_api->payload_length = pldLen;
    hub_api->payload_type = ptye;

    switch (ptye) {
    default:
    case PLD_PROVI_TYPE_VER1:
    case PLD_PROVI_TYPE_VER2:
    case PLD_PROVI_TYPE_VER3:
        /* not support */
        return RET_ERROR;
    case PLD_PROVI_TYPE_BOOT:
        break;
    }

    rui3_api->length.value = pktLen;
#define SHORT_SWAP(X) (((X & 0xFF) << 8) | ((X >> 8) & 0xFF))
    rui3_api->length.value = SHORT_SWAP(rui3_api->length.value);
    rui3_api->payload[pktLen] = cal_chksum((U8 *)rui3_api, BUFF_SIZE);

    pktLen += sizeof(RUI3_Api_t);
    pktLen += 1 /* add checksum len*/;

    on_evt(PID_MASTER, sid, SNHUBAPI_EVT_QSEND, pktBuff, pktLen);

    return RET_OK;
}

/**
 * @brief Send request for sensor data
 *
 * @param pid PID to be addressed
 * @param sid SID to be addressed
 * @param gset SNHUB_GS_GET = get sensor data
 * @param ptye parameter type
 * @return RET_S32 RET_ERROR if invalid request, else RET_OK
 */

static RET_S32 snhub_snsrdat_command(U8 pid, U8 sid, SNHUB_GS_E gset, U8 ptye)
{
    U8 pktBuff[BUFF_SIZE];
    U16 pktLen = 0;
    U16 pldLen = 0;

    RUI3_Api_t *rui3_api = (RUI3_Api_t *)pktBuff;
    SNHub_Api_t *hub_api = (SNHub_Api_t *)(rui3_api->payload);

    f_memset(pktBuff, 0, BUFF_SIZE);

    rui3_api->wakeup = WAKEUPBYTE;
    rui3_api->start = DELIMTER;
    rui3_api->type = RUI3API_TYPE_SENSORHUB;
    rui3_api->flag = RUI3API_FLG_REQ;
    hub_api->source = PID_MASTER;
    hub_api->dest = pid;
    hub_api->sequence = ++menu.seq;
    pktLen = sizeof(SNHub_Api_t);

    switch (gset) {
    case SNHUB_GS_GET:
        hub_api->type = SNHUB_TYPE_SENDAT;
        break;
    default:
    case SNHUB_GS_SET:
        return RET_ERROR;
    }

    pktLen += pldLen;
    hub_api->payload_length = pldLen;
    hub_api->payload_type = ptye;

    switch (ptye) {
    case PLD_SDATA_TPYE_SENDAT:
        hub_api->payload[0] = sid;
        break;

    case PLD_SDATA_TPYE_VER:
    case PLD_SDATA_TPYE_PARAMSET:
    case PLD_SDATA_TPYE_CONTROL:
        /* not support */
        return RET_ERROR;
    }

    rui3_api->length.value = pktLen;
    rui3_api->length.value = SHORT_SWAP(rui3_api->length.value);
    rui3_api->payload[pktLen] = cal_chksum((U8 *)rui3_api, BUFF_SIZE);

    pktLen += sizeof(RUI3_Api_t);
    pktLen += 1 /* add checksum len*/;

    on_evt(PID_MASTER, sid, SNHUBAPI_EVT_QSEND, pktBuff, pktLen);

    return RET_OK;
}

/**
 * @brief Check sequence correctness
 * calls SNHUBAPI_EVT_SEQ_ERR if sequence is not correct
 * 
 * @param data Pointer to data as uint8_t array
 * @param len Length of data
 * @return RET_S32 Always RE_OK
 */
static RET_S32 verify_sequence(U8 *data, U16 len)
{
    RUI3_Api_t *rui3_api = (RUI3_Api_t *)data;
    SNHub_Api_t *hub_api = (SNHub_Api_t *)(rui3_api->payload);

    do {
        if (rui3_api->flag == RUI3API_FLG_REQ) {
            break;
        }

        if (menu.result != SNHUB_RES_OK) {
            if (hub_api->sequence == menu.seq) {
                menu.result = SNHUB_RES_OK;
            } else {
                on_evt(PID_MASTER, 0, SNHUBAPI_EVT_SEQ_ERR, NULL, 0);
            }
        }
    } while (0);

    return RET_OK;
}

/**
 * @brief Verify data length
 *
 * @param data data to be checked as uint8_t array
 * @param len length of the data
 * @return RET_S32 RET_OK = correct, RET_ERROR = wrong
 */
static RET_S32 verify_snhublen(U8 *data, U16 len)
{
    RUI3_Api_t *rui3_api = (RUI3_Api_t *)data;
    SNHub_Api_t *hub_api = (SNHub_Api_t *)(rui3_api->payload);

    U16 rui3_api_len = LSB_COMB(rui3_api->length.hbyte, rui3_api->length.lbyte);
    U16 hub_api_len = hub_api->payload_length;
    if (rui3_api_len == (hub_api_len + sizeof(SNHub_Api_t))) {
        return RET_OK;
    }

    /* old version will add crc byte, keep this issue */
    if (rui3_api_len == ((hub_api_len + sizeof(SNHub_Api_t)) - 1)) {
        return RET_OK;
    }

    return RET_ERROR;
}

/**
 * @brief Get delimiter index
 *
 * @param data data to be checked as uint8_t array
 * @param len length of the data
 * @return RET_S32 index of delimiter or RET_ERROR if no delimiter was found
 */
static RET_S32 verify_delimter(U8 *data, U16 len)
{
    for (U16 ofs = 0; ofs < len; ofs++) {
        if (data[ofs] == DELIMTER) {
            return ofs; /* recover wakeup byte*/
            ;
        }
    }
    return RET_ERROR;
}

/**
 * @brief Verify if data is type SensorHub
 *
 * @param data data to be checked as uint8_t array
 * @param len length of the data
 * @return RET_S32 RET_OK if type is correct, else RET_ERROR
 */
static RET_S32 verify_rui3type(U8 *data, U16 len)
{
    RUI3_Api_t *rui3_api = (RUI3_Api_t *)data;

    switch (rui3_api->type) {
    case RUI3API_TYPE_SENSORHUB:
        return RET_OK;
    case RUI3API_TYPE_ECHO:
    default:
        break;
    }
    return RET_ERROR;
}

/**
 * @brief Verify checksum
 *
 * @param data data to be checked as uint8_t array
 * @param len length of the data
 * @return RET_S32 RET_OK if correct, else RET_ERROR
 */
static RET_S32 verify_checksum(U8 *data, U16 len)
{
    RUI3_Api_t *rui3_api = (RUI3_Api_t *)data;

    U8 chsum = cal_chksum((U8 *)rui3_api, len);
    U16 payload_len = LSB_COMB(rui3_api->length.hbyte, rui3_api->length.lbyte);

    if (chsum == rui3_api->payload[payload_len]) {
        return RET_OK;
    }

    on_evt(PID_MASTER, 0, SNHUBAPI_EVT_CHKSUM_ERR, NULL, 0);
    return RET_ERROR;
}

/**
 * @brief Verify action
 * Calls event depending on action
 *
 * @param data data to be checked as uint8_t array
 * @param len length of the data
 * @return RET_S32 RET_OK if correct action, else RET_ERROR
 */
static RET_S32 verify_action(U8 *data, U16 len)
{
    RUI3_Api_t *rui3_api = (RUI3_Api_t *)data;
    SNHub_Api_t *hub_api = (SNHub_Api_t *)(rui3_api->payload);
    RET_S32 ret = 0;
    program_process program = NULL;

    switch (rui3_api->flag) {
    case RUI3API_FLG_REQ:
        program = protocol_list[hub_api->type].req;
        on_evt(hub_api->source, 0, SNHUBAPI_EVT_RECV_REQ, data, len);
        break;

    case RUI3API_FLG_RSP:
        program = protocol_list[hub_api->type].rsp;
        on_evt(hub_api->source, 0, SNHUBAPI_EVT_RECV_RSP, data, len);
        break;

    default:
        return RET_ERROR;
    }

    if (program != NULL) {
        program(data, len);
    }

    return ret;
}

/**
 * @brief Initialize API
 * 
 * @param this_on_evt pointer to event handler function
 */
static void api_init(SNHub_Evt_t this_on_evt)
{
    on_evt = this_on_evt;

    command_list[SNHUB_TYPE_PROVISION](PID_UNKNOW, 0, SNHUB_GS_SET, PLD_PROVI_TYPE_BOOT);
}

/**
 * @brief Process received data
 *
 * @param msg received data to be processed as uint8_t array
 * @param len length of the data
 */
static void api_process(U8 *msg, U16 len)
{
    U16 dataLen = 0;
    RET_S32 dataOffset = 0;
    U8 *pdata = NULL;

    do {
        /* check default packet len */
        if (len < (sizeof(RUI3_Api_t) + 1 /* checksum */)) {
            break;
        }

        /* keep wakeup byte set */
        dataOffset = verify_delimter(msg, len);
        if (dataOffset == RET_ERROR) {
            break;
        }

        /* copy data to local buff */
        f_memset(dataBuff, 0, BUFF_SIZE);
        f_memcpy(&dataBuff[1], &msg[(U16)dataOffset], (U8)len);

        dataBuff[0] = WAKEUPBYTE;
        pdata = &dataBuff[0];
        dataLen = len - dataOffset + 1 /* recover wakeup byte*/;

        /* verify packet checksum */
        if (verify_checksum(pdata, dataLen) != RET_OK) {
            break;
        }

        /* verify rui3 api type, type have to eq RUI3API_TYPE_SENSORHUB */
        if (verify_rui3type(pdata, dataLen) != RET_OK) {
            break;
        }

        /* verify rui3 api len and snhub len is mapping */
        if (verify_snhublen(pdata, dataLen) != RET_OK) {
            break;
        }

        /* verify sequence */
        verify_sequence(pdata, dataLen);

        /* todo request/response program */
        if (verify_action(pdata, dataLen) != RET_OK) {
            break;
        }
    } while (0);
}

/**
 * @brief SensorHub commands
 * 
 */
static const command_process command_list[] = {
    [SNHUB_TYPE_WAKUP] = NULL,    [SNHUB_TYPE_PROVISION] = snhub_provision_command,
    [SNHUB_TYPE_PARAMSET] = NULL, [SNHUB_TYPE_SENDAT] = snhub_snsrdat_command,
    [SNHUB_TYPE_CONTROL] = NULL,  [SNHUB_TYPE_PARAMGET] = snhub_paramget_command,
    [SNHUB_TYPE_ALERT] = NULL,    [SNHUB_TYPE_ERR] = NULL,
    [SNHUB_TYPE_ACK] = NULL,      [SNHUB_TYPE_NACK] = NULL,
    [SNHUB_TYPE_YMDM] = NULL,     [SNHUB_TYPE_IOC] = snhub_ioc_command,
    [SNHUB_TYPE_MODBUS] = NULL,   [SNHUB_TYPE_SDI12] = NULL,
    [SNHUB_TYPE_ADC] = NULL,      [SNHUB_TYPE_DIO] = NULL,
};

/**
 * @brief Sensorhub process list
 * 
 */
static const protocol_process_t protocol_list[] = {

    [SNHUB_TYPE_WAKUP] = {.req = NULL, .rsp = NULL},
    [SNHUB_TYPE_PROVISION] = {.req = snhub_provision_req_program, .rsp = NULL},
    [SNHUB_TYPE_PARAMSET] = {.req = NULL, .rsp = NULL},
    [SNHUB_TYPE_SENDAT] = {.req = snhub_snsrdat_req_program, .rsp = snhub_snsrdat_rsp_program},
    [SNHUB_TYPE_CONTROL] = {.req = NULL, .rsp = NULL},
    [SNHUB_TYPE_PARAMGET] = {.req = NULL, .rsp = snhub_paramget_rsp_program},
    [SNHUB_TYPE_ALERT] = {.req = NULL, .rsp = NULL},
    [SNHUB_TYPE_ERR] = {.req = NULL, .rsp = NULL},
    [SNHUB_TYPE_ACK] = {.req = NULL, .rsp = NULL},
    [SNHUB_TYPE_NACK] = {.req = NULL, .rsp = NULL},
    [SNHUB_TYPE_YMDM] = {.req = NULL, .rsp = NULL},
    [SNHUB_TYPE_IOC] = {.req = NULL, .rsp = snhub_ioc_rsp_program},
    [SNHUB_TYPE_MODBUS] = {.req = NULL, .rsp = NULL},
    [SNHUB_TYPE_SDI12] = {.req = NULL, .rsp = NULL},
    [SNHUB_TYPE_ADC] = {.req = NULL, .rsp = NULL},
    [SNHUB_TYPE_DIO] = {.req = NULL, .rsp = NULL},
};

/**
 * @brief Get sensor data
 * 
 * @param pid source PID of the data
 */
static void api_get_snsr_data(U8 pid)
{
    if (pid == PID_MASTER) {
        return;
    }

    menu.result = SNHUB_RES_BUSY;
    CKTODO(command_list[SNHUB_TYPE_SENDAT])(pid, 0, SNHUB_GS_GET, PLD_SDATA_TPYE_SENDAT);
}

/**
 * @brief Get sensor parameters
 *
 * @param pid source PID of the parameters
 * @param sid source SID of the parameters
 */
static void api_get_snsr_param(U8 pid, U8 sid)
{
    if (pid == PID_MASTER) {
        return;
    }

    menu.result = SNHUB_RES_BUSY;
    CKTODO(command_list[SNHUB_TYPE_PARAMGET])(pid, sid, SNHUB_GS_GET, PLD_PARMGSET_TYPE_SNSR_UPDATE);
}

/**
 * @brief Set sensor parameters
 *
 * @param pid target
 * @param sid target
 * @param enb type of rule to be applied
 * @param intv rule to be applied
 */
static void api_set_snsr_param(U8 pid, U8 sid, U8 enb, U32 intv)
{
    if (pid == PID_MASTER) {
        return;
    }

    menu.intv = intv;
    menu.rule = (enb == 0) ? RULE_DISABLE : RULE_PERIODIC;
    menu.result = SNHUB_RES_BUSY;
    CKTODO(command_list[SNHUB_TYPE_PARAMGET])(pid, sid, SNHUB_GS_SET, PLD_PARMGSET_TYPE_SNSR_UPDATE);
}

/**
 * @brief Set sensor provisioning
 * 
 */
static void api_set_provision()
{
    menu.result = SNHUB_RES_BUSY;
    CKTODO(command_list[SNHUB_TYPE_PROVISION])(PID_UNKNOW, 0, SNHUB_GS_SET, PLD_PROVI_TYPE_BOOT);
}

/** API functions */
const RakSNHub_Protocl_API_t RakSNHub_Protocl_API = {
    .init = api_init,
    .process = api_process,

    .get.data = api_get_snsr_data,
    .get.param = api_get_snsr_param,

    .set.param = api_set_snsr_param,

    .ioc.send = api_ioc_send,

    .reboot = api_set_provision,
};

RET_S32 RakSNHub_IOC_ParseRsp(const U8 *msg, U16 len, RakSNHub_IOC_Rsp_t *rsp)
{
    if (msg == NULL || rsp == NULL || len < 3) {
        return RET_ERROR;
    }

    rsp->funcode = msg[0];
    rsp->iface = msg[1];
    rsp->action = msg[2];
    rsp->data = (len > 3) ? (msg + 3) : NULL;
    rsp->data_len = (len > 3) ? (U16)(len - 3) : 0;

    return RET_OK;
}

RET_S32 RakSNHub_IOC_ConfigRS485(U8 pid, U32 baudrate, U8 databit, U8 stopbit, U8 parity)
{
    rak_ioc_cfg_frame_t cfg;
    U8 *cfg_raw = (U8 *)&cfg;
    f_memset(cfg_raw, 0, sizeof(cfg));
    cfg.cfg.baudrate = baudrate;
    cfg.cfg.databit = databit;
    cfg.cfg.stopbit = stopbit;
    cfg.cfg.parity = parity;
    api_ioc_send(pid, IO_CFG, IOC_RS485, IOA_REQ, (const U8 *)&cfg, sizeof(cfg));
    return RET_OK;
}

RET_S32 RakSNHub_IOC_AddPoll(U8 pid, U8 iface, U8 taskid, const U8 *cmd, U8 cmd_len, U32 period, U32 timeout, U8 retry)
{
    U8 payload[220];
    U16 ofs = 0;

    if (validate_hex_cmd(cmd, cmd_len) != RET_OK)
        return RET_ERROR;
    if ((U16)(sizeof(rak_ioc_addpoll_frame_t) + cmd_len) > sizeof(payload)) {
        return RET_ERROR;
    }

    payload[ofs++] = taskid;
    f_memcpy(&payload[ofs], (U8 *)&period, sizeof(period));
    ofs += sizeof(period);
    f_memcpy(&payload[ofs], (U8 *)&timeout, sizeof(timeout));
    ofs += sizeof(timeout);
    payload[ofs++] = retry;
    f_memcpy(&payload[ofs], (U8 *)cmd, cmd_len);
    ofs += cmd_len;

    api_ioc_send(pid, IO_ADDPOLL, iface, IOA_REQ, payload, ofs);
    return RET_OK;
}

RET_S32 RakSNHub_IOC_AddPollHex(U8 pid, U8 iface, U8 taskid, const char *cmd_hex, U32 period, U32 timeout, U8 retry)
{
    U16 cmd_len;

    if (cmd_hex == NULL) {
        return RET_ERROR;
    }

    cmd_len = this_strlen(cmd_hex);
    if (cmd_len > 255u) {
        return RET_ERROR;
    }

    return RakSNHub_IOC_AddPoll(pid, iface, taskid, (const U8 *)cmd_hex, (U8)cmd_len, period, timeout, retry);
}

RET_S32 RakSNHub_IOC_AddPollEx(U8 pid, U8 iface, U8 taskid, const U8 *cmd, U8 cmd_len, U32 period, U32 timeout, U8 retry,
                              U8 ipso, float scale, U8 datatype, const char *snsr_name)
{
    U8 payload[220];
    U16 ofs = 0;

    if (cmd == NULL || cmd_len == 0) {
        return RET_ERROR;
    }
    if ((U16)(sizeof(rak_ioc_addpollex_frame_t) + cmd_len) > sizeof(payload)) {
        return RET_ERROR;
    }

    payload[ofs++] = taskid;
    f_memcpy(&payload[ofs], (U8 *)&period, sizeof(period));
    ofs += sizeof(period);
    f_memcpy(&payload[ofs], (U8 *)&timeout, sizeof(timeout));
    ofs += sizeof(timeout);
    payload[ofs++] = retry;

    payload[ofs++] = ipso;
    f_memcpy(&payload[ofs], (U8 *)&scale, sizeof(scale));
    ofs += sizeof(scale);
    payload[ofs++] = datatype;

    // snsr_name is a fixed 16-byte field on ProbeIO (1.2.27+). It is not required to be zero-terminated.
    {
        U8 *namep = &payload[ofs];
        f_memset(namep, 0, 16);
        if (snsr_name != NULL && snsr_name[0] != '\0') {
            // Avoid libc strlen() dependency: copy up to 16 bytes or until '\0'.
            U8 n = 0;
            while (n < 16u && snsr_name[n] != '\0') {
                namep[n] = (U8)snsr_name[n];
                n++;
            }
        }
    }
    ofs += 16;

    f_memcpy(&payload[ofs], (U8 *)cmd, cmd_len);
    ofs += cmd_len;

    api_ioc_send(pid, IO_ADDPOLLEX, iface, IOA_REQ, payload, ofs);
    return RET_OK;
}

RET_S32 RakSNHub_IOC_EnablePoll(U8 pid, U8 iface, U8 taskid, U8 enable)
{
    rak_ioc_enablepoll_frame_t en;
    en.taskid = taskid;
    en.enable = enable;
    api_ioc_send(pid, IO_ENABLEPOLL, iface, IOA_REQ, (const U8 *)&en, sizeof(en));
    return RET_OK;
}

RET_S32 RakSNHub_IOC_PollTask(U8 pid, U8 iface, U8 taskid)
{
    rak_ioc_polltask_frame_t poll;
    poll.taskid = taskid;
    api_ioc_send(pid, IO_POLLTASK, iface, IOA_REQ, (const U8 *)&poll, sizeof(poll));
    return RET_OK;
}

RET_S32 RakSNHub_IOC_PassThrough(U8 pid, U8 iface, const U8 *cmd, U8 cmd_len, U32 timeout)
{
    U8 payload[220];
    U16 ofs = 0;

    if (cmd == NULL || cmd_len == 0) {
        return RET_ERROR;
    }
    if ((U16)(sizeof(rak_ioc_passthrh_frame_t) + cmd_len) > sizeof(payload)) {
        return RET_ERROR;
    }

    f_memcpy(&payload[ofs], (U8 *)&timeout, sizeof(timeout));
    ofs += sizeof(timeout);
    f_memcpy(&payload[ofs], (U8 *)cmd, cmd_len);
    ofs += cmd_len;

    api_ioc_send(pid, IOPASSTHRH, iface, IOA_REQ, payload, ofs);
    return RET_OK;
}

RET_S32 RakSNHub_IOC_DecodeAIC(U8 pid, U8 taskid, U8 ipso, S32 min, S32 max, float offset, const char *snsr_name)
{
    rak_ioc_decode_frame_t dec;
    U8 *raw = (U8 *)&dec;
    f_memset(raw, 0, sizeof(dec));

    dec.taskid = taskid;
    dec.param.u.aic.IPSO = ipso;
    dec.param.u.aic.min = min;
    dec.param.u.aic.max = max;
    dec.param.u.aic.offset = offset;
    if (snsr_name != NULL && snsr_name[0] != '\0') {
        // Fixed 16 bytes; not required to be zero-terminated.
        U8 n = 0;
        while (n < 16u && snsr_name[n] != '\0') {
            dec.param.u.aic.snsr_name[n] = (U8)snsr_name[n];
            n++;
        }
    }

    api_ioc_send(pid, IO_DECODE, IOC_AIC, IOA_REQ, (const U8 *)&dec, sizeof(dec));
    return RET_OK;
}

RET_S32 RakSNHub_IOC_RmPollDef(U8 pid, U8 iface, U8 portid)
{
    rak_ioc_rmpdef_frame_t rmpdef;
    f_memset(&rmpdef, 0, sizeof(rmpdef));
    rmpdef.if_id = iface;
    rmpdef.portid = portid;
    api_ioc_send(pid, IO_RMPDEF, iface, IOA_REQ, (const U8 *)&rmpdef, sizeof(rmpdef));
    return RET_OK;
}
