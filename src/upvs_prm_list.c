#include "upvs_prm.h"
#include "stdlib.h"

// Список всех Параметров согласно Протоколу
const prm_t axConstList[UPVS_PRM_LIST_LENGHT] = {
// ACTIONS
  {
    //.itemArray = 0,
    //.itemGroup = 1,
    .pcTitle = "action",
    .pcSrvc = NULL,
    .pcName = "reset_faults",
    .pcTopic = "action/CSC/reset_faults",
    .xValue.type = BOOL,
    .xValue.mag.b = false
  },
  {
    //.itemArray = 1,
    //.itemGroup = 2,
    .pcTitle = "action",
    .pcSrvc = NULL,
    .pcName = "all",
    .pcTopic = "action/CSC",
    .xValue.type = GETALL,
    //.xValue.mag.b = false
  },
  {
    //.itemArray = 2,
    //.itemGroup = 3,
    .pcTitle = "action",
    .pcSrvc = NULL,
    .pcName = "channel_1",
    .pcTopic = "action/CSC/channel_1",
    .xValue.type = BOOL,
    .xValue.mag.b = false
  },
  {
    //.itemArray = 3,
    //.itemGroup = 4,
    .pcTitle = "action",
    .pcSrvc = NULL,
    .pcName = "channel_1_frequency",
    .pcTopic = "action/CSC/channel_1_frequency",
    .xValue.type = FLOAT,
    .xValue.mag.f = 0.0f
  },
  {
    //.itemArray = 4,
    //.itemGroup = 5,
    .pcTitle = "action",
    .pcSrvc = NULL,
    .pcName = "channel_2",
    .pcTopic = "action/CSC/channel_2",
    .xValue.type = BOOL,
    .xValue.mag.b = false
  },
  {
    //.itemArray = 5,
    //.itemGroup = 6,
    .pcTitle = "action",
    .pcSrvc = NULL,
    .pcName = "channel_3",
    .pcTopic = "action/CSC/channel_3",
    .xValue.type = BOOL,
    .xValue.mag.b = false
  },
  {
    //.itemArray = 6,
    //.itemGroup = 7,
    .pcTitle = "action",
    .pcSrvc = NULL,
    .pcName = "datetime",
    .pcTopic = "action/CSC/datetime",
  },
// Сервис INFO
  {
    //.itemArray = 7,
    //.itemGroup = 1,
    .pcTitle = "value",
    .pcSrvc = "INFO",
    .pcName = "uuid",
    .pcTopic = "value/CSC/INFO/uuid",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL
    /*.xValue.mag.ac[0] = '0',
    .xValue.mag.ac[1] = '1',
    .xValue.mag.ac[2] = ':',
    .xValue.mag.ac[3] = '0',
    .xValue.mag.ac[4] = '0',
    .xValue.mag.ac[5] = ':',
    .xValue.mag.ac[6] = '0',
    .xValue.mag.ac[7] = '0',
    .xValue.mag.ac[8] = ':',
    .xValue.mag.ac[9] = '0',
    .xValue.mag.ac[10] = '0',
    .xValue.mag.ac[11] = ':',
    .xValue.mag.ac[12] = '0',
    .xValue.mag.ac[13] = '0',
    .xValue.mag.ac[14] = ':',
    .xValue.mag.ac[15] = '1',
    .xValue.mag.ac[16] = '0',
    .xValue.mag.ac[18] = '\0'*/
  },
  {
    //.itemArray = 8,
    //.itemGroup = 2,
    .pcTitle = "value",
    .pcSrvc = "INFO",
    .pcName = "modelname",
    .pcTopic = "value/CSC/INFO/modelname",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = '\0'
  },
  {
    //.itemArray = 9,
    //.itemGroup = 3,
    .pcTitle = "value",
    .pcSrvc = "INFO",
    .pcName = "company",
    .pcTopic = "value/CSC/INFO/company",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = '\0'
  },
  {
    //.itemArray = 10,
    //.itemGroup = 4,
    .pcTitle = "value",
    .pcSrvc = "INFO",
    .pcName = "country",
    .pcTopic = "value/CSC/INFO/country",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = '\0'
  },
  {
    //.itemArray = 11,
    //.itemGroup = 5,
    .pcTitle = "value",
    .pcSrvc = "INFO",
    .pcName = "serialnumber",
    .pcTopic = "value/CSC/INFO/serialnumber",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL
    /*.xValue.mag.ac[0] = '0',
    .xValue.mag.ac[1] = '0',
    .xValue.mag.ac[2] = '1',
    .xValue.mag.ac[3] = 0xd0,
    .xValue.mag.ac[4] = 0x90,
    .xValue.mag.ac[5] = 0xd0,
    .xValue.mag.ac[6] = 0x9e,
    .xValue.mag.ac[7] = '0',
    .xValue.mag.ac[8] = '1',
    .xValue.mag.ac[9] = '2',
    .xValue.mag.ac[10] = '0',
    .xValue.mag.ac[11] = '2',
    .xValue.mag.ac[12] = '5',
    .xValue.mag.ac[13] = NULL,*/
  },
  {
    //.itemArray = 12,
    //.itemGroup = 6,
    .pcTitle = "value",
    .pcSrvc = "INFO",
    .pcName = "version",
    .pcTopic = "value/CSC/INFO/version",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL
  },
  
// Сервис VSP

  {
    //.itemArray = 13,
    //.itemGroup = 1,
    .pcTitle = "value",
    .pcSrvc = "VSP",
    .pcName = "state",
    .pcTopic = "value/CSC/VSP/state",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL
    /*.xValue.mag.ac[0] = 'r',
    .xValue.mag.ac[1] = 'e',
    .xValue.mag.ac[2] = 'a',
    .xValue.mag.ac[3] = 'd',
    .xValue.mag.ac[4] = 'y',
    .xValue.mag.ac[5] = '\0'*/
  },
  {
    //.itemArray = 14,
    //.itemGroup = 2,
    .pcTitle = "value",
    .pcSrvc = "VSP",
    .pcName = "door_is_open",
    .pcTopic = "value/CSC/VSP/door_is_open",
    .xValue.type = BOOL,
    .xValue.mag.b = false
  },
  {
    //.itemArray = 15,
    //.itemGroup = 3,
    .pcTitle = "value",
    .pcSrvc = "VSP",
    .pcName = "charging_contactor",
    .pcTopic = "value/CSC/VSP/charging_contactor",
    .xValue.type = BOOL,
    .xValue.mag.b = false
  },
  {
    //.itemArray = 16,
    //.itemGroup = 4,
    .pcTitle = "value",
    .pcSrvc = "VSP",
    .pcName = "main_contactor",
    .pcTopic = "value/CSC/VSP/main_contactor",
    .xValue.type = BOOL,
    .xValue.mag.b = false
  },
  {
    //.itemArray = 17,
    //.itemGroup = 5,
    .pcTitle = "value",
    .pcSrvc = "VSP",
    .pcName = "input_current",
    .pcTopic = "value/CSC/VSP/input_current",
    .xValue.type = FLOAT,
    .xValue.mag.f = 0.0f
  },
  {
    //.itemArray = 18,
    //.itemGroup = 6,
    .pcTitle = "value",
    .pcSrvc = "VSP",
    .pcName = "input_power",
    .pcTopic = "value/CSC/VSP/input_power",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 19,
    //.itemGroup = 7,
    .pcTitle = "value",
    .pcSrvc = "VSP",
    .pcName = "overhead_line_voltage",
    .pcTopic = "value/CSC/VSP/overhead_line_voltage",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 20,
    //.itemGroup = 8,
    .pcTitle = "value",
    .pcSrvc = "VSP",
    .pcName = "output_power",
    .pcTopic = "value/CSC/VSP/output_power",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 21,
    //.itemGroup = 9,
    .pcTitle = "value",
    .pcSrvc = "VSP",
    .pcName = "cooler_temperature",
    .pcTopic = "value/CSC/VSP/cooler_temperature",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 22,
    //.itemGroup = 10,
    .pcTitle = "value",
    .pcSrvc = "VSP",
    .pcName = "version",
    .pcTopic = "value/CSC/VSP/version",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL
    /*.xValue.mag.ac[0] = '2',
    .xValue.mag.ac[1] = '\0'*/
  },
  
// Сервис NSP

  {
    //.itemArray = 23,
    //.itemGroup = 1,
    .pcTitle = "value",
    .pcSrvc = "NSP",
    .pcName = "state",
    .pcTopic = "value/CSC/NSP/state",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL
    /*.xValue.mag.ac[0] = 'r',
    .xValue.mag.ac[1] = 'e',
    .xValue.mag.ac[2] = 'a',
    .xValue.mag.ac[3] = 'd',
    .xValue.mag.ac[4] = 'y',
    .xValue.mag.ac[5] = '\0'*/
  },
  {
    //.itemArray = 24,
    //.itemGroup = 2,
    .pcTitle = "value",
    .pcSrvc = "NSP",
    .pcName = "power_supply",
    .pcTopic = "value/CSC/NSP/power_supply",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL
    /*.xValue.mag.ac[0] = 'd',
    .xValue.mag.ac[1] = 'i',
    .xValue.mag.ac[2] = 'e',
    .xValue.mag.ac[3] = 's',
    .xValue.mag.ac[4] = 'e',
    .xValue.mag.ac[5] = 'l',
    .xValue.mag.ac[6] = '\0'*/
  },
  {
    //.itemArray = 25,
    //.itemGroup = 3,
    .pcTitle = "value",
    .pcSrvc = "NSP",
    .pcName = "input_voltage_vsp",
    .pcTopic = "value/CSC/NSP/input_voltage_vsp",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 26,
    //.itemGroup = 4,
    .pcTitle = "value",
    .pcSrvc = "NSP",
    .pcName = "input_voltage_nv",
    .pcTopic = "value/CSC/NSP/input_voltage_nv",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 27,
    //.itemGroup = 5,
    .pcTitle = "value",
    .pcSrvc = "NSP",
    .pcName = "input_current_nv",
    .pcTopic = "value/CSC/NSP/input_current_nv",
    .xValue.type = FLOAT,
    .xValue.mag.f = 0.0f
  },
  {
    //.itemArray = 28,
    //.itemGroup = 6,
    .pcTitle = "value",
    .pcSrvc = "NSP",
    .pcName = "input_power_nv",
    .pcTopic = "value/CSC/NSP/input_power_nv",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 29,
    //.itemGroup = 7,
    .pcTitle = "value",
    .pcSrvc = "NSP",
    .pcName = "max_input_power",
    .pcTopic = "value/CSC/NSP/max_input_power",
    .xValue.type = BOOL,
    .xValue.mag.b = false
  },
  {
    //.itemArray = 30,
    //.itemGroup = 8,
    .pcTitle = "value",
    .pcSrvc = "NSP",
    .pcName = "redundancy_contactor_km3",
    .pcTopic = "value/CSC/NSP/redundancy_contactor_km3",
    .xValue.type = BOOL,
    .xValue.mag.b = false
  },
  {
    //.itemArray = 31,
    //.itemGroup = 9,
    .pcTitle = "value",
    .pcSrvc = "NSP",
    .pcName = "redundancy_contactor_km4",
    .pcTopic = "value/CSC/NSP/redundancy_contactor_km4",
    .xValue.type = BOOL,
    .xValue.mag.b = false
  },
  {
    //.itemArray = 32,
    //.itemGroup = 10,
    .pcTitle = "value",
    .pcSrvc = "NSP",
    .pcName = "redundancy_contactor_km5",
    .pcTopic = "value/CSC/NSP/redundancy_contactor_km5",
    .xValue.type = BOOL,
    .xValue.mag.b = false
  },
  {
    //.itemArray = 33,
    //.itemGroup = 11,
    .pcTitle = "value",
    .pcSrvc = "NSP",
    .pcName = "cooler_temperature",
    .pcTopic = "value/CSC/NSP/cooler_temperature",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 34,
    //.itemGroup = 12,
    .pcTitle = "value",
    .pcSrvc = "NSP",
    .pcName = "version",
    .pcTopic = "value/CSC/NSP/version",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL,
    /*.xValue.mag.ac[0] = '2',
    .xValue.mag.ac[1] = '4',
    .xValue.mag.ac[2] = '\0'*/
  },
  
// Сервис CHANNEL_1

  {
    //.itemArray = 35,
    //.itemGroup = 1,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_1",
    .pcName = "state",
    .pcTopic = "value/CSC/CHANNEL_1/state",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL
    /*.xValue.mag.ac[0] = 'r',
    .xValue.mag.ac[1] = 'e',
    .xValue.mag.ac[2] = 'a',
    .xValue.mag.ac[3] = 'd',
    .xValue.mag.ac[4] = 'y',
    .xValue.mag.ac[5] = '\0'*/
  },
  {
    //.itemArray = 36,
    //.itemGroup = 2,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_1",
    .pcName = "output_power_setting",
    .pcTopic = "value/CSC/CHANNEL_1/output_power_setting",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 37,
    //.itemGroup = 3,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_1",
    .pcName = "frequency_setting",
    .pcTopic = "value/CSC/CHANNEL_1/frequency_setting",
    .xValue.type = FLOAT,
    .xValue.mag.f = 0.0f
  },
  {
    //.itemArray = 38,
    //.itemGroup = 4,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_1",
    .pcName = "output_frequency",
    .pcTopic = "value/CSC/CHANNEL_1/output_frequency",
    .xValue.type = FLOAT,
    .xValue.mag.f = 0.0f
  },
  {
    //.itemArray = 39,
    //.itemGroup = 5,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_1",
    .pcName = "output_current",
    .pcTopic = "value/CSC/CHANNEL_1/output_current",
    .xValue.type = FLOAT,
    .xValue.mag.f = 0.0f
  },
  {
    //.itemArray = 40,
    //.itemGroup = 6,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_1",
    .pcName = "output_voltage",
    .pcTopic = "value/CSC/CHANNEL_1/output_voltage",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 41,
    //.itemGroup = 7,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_1",
    .pcName = "output_power",
    .pcTopic = "value/CSC/CHANNEL_1/output_power",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 42,
    //.itemGroup = 8,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_1",
    .pcName = "cooler_temperature",
    .pcTopic = "value/CSC/CHANNEL_1/cooler_temperature",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 43,
    //.itemGroup = 9,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_1",
    .pcName = "version",
    .pcTopic = "value/CSC/CHANNEL_1/version",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = '\0'
    /*.xValue.mag.ac[0] = '3',
    .xValue.mag.ac[1] = '6',
    .xValue.mag.ac[2] = '\0'*/
  },
  
// Сервис CHANNEL_2

  {
    //.itemArray = 44,
    //.itemGroup = 1,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_2",
    .pcName = "state",
    .pcTopic = "value/CSC/CHANNEL_2/state",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL
    /*.xValue.mag.ac[0] = 'r',
    .xValue.mag.ac[1] = 'e',
    .xValue.mag.ac[2] = 'a',
    .xValue.mag.ac[3] = 'd',
    .xValue.mag.ac[4] = 'y',
    .xValue.mag.ac[5] = '\0'*/
  },
  {
    //.itemArray = 45,
    //.itemGroup = 2,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_2",
    .pcName = "output_power_setting",
    .pcTopic = "value/CSC/CHANNEL_2/output_power_setting",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 46,
    //.itemGroup = 3,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_2",
    .pcName = "output_frequency",
    .pcTopic = "value/CSC/CHANNEL_2/output_frequency",
    .xValue.type = FLOAT,
    .xValue.mag.f = 0.0f
  },
  {
    //.itemArray = 47,
    //.itemGroup = 4,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_2",
    .pcName = "output_current",
    .pcTopic = "value/CSC/CHANNEL_2/output_current",
    .xValue.type = FLOAT,
    .xValue.mag.f = 0.0f
  },
  {
    //.itemArray = 48,
    //.itemGroup = 5,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_2",
    .pcName = "output_voltage",
    .pcTopic = "value/CSC/CHANNEL_2/output_voltage",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 49,
    //.itemGroup = 6,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_2",
    .pcName = "output_power",
    .pcTopic = "value/CSC/CHANNEL_2/output_power",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 50,
    //.itemGroup = 7,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_2",
    .pcName = "cooler_temperature",
    .pcTopic = "value/CSC/CHANNEL_2/cooler_temperature",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 51,
    //.itemGroup = 8,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_2",
    .pcName = "version",
    .pcTopic = "value/CSC/CHANNEL_2/version",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL
    //.xValue.mag.ac[1] = '4',
    //.xValue.mag.ac[2] = '\0'
  },
  
  
// Сервис CHANNEL_3

  {
    //.itemArray = 52,
    //.itemGroup = 1,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_3",
    .pcName = "state",
    .pcTopic = "value/CSC/CHANNEL_3/state",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL
    /*.xValue.mag.ac[0] = 'r',
    .xValue.mag.ac[1] = 'e',
    .xValue.mag.ac[2] = 'a',
    .xValue.mag.ac[3] = 'd',
    .xValue.mag.ac[4] = 'y',
    .xValue.mag.ac[5] = '\0'*/
  },
  {
    //.itemArray = 53,
    //.itemGroup = 2,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_3",
    .pcName = "cooler_temperature",
    .pcTopic = "value/CSC/CHANNEL_3/cooler_temperature",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 54,
    //.itemGroup = 3,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_3",
    .pcName = "output_frequency",
    .pcTopic = "value/CSC/CHANNEL_3/output_frequency",
    .xValue.type = FLOAT,
    .xValue.mag.f = 0.0f
  },
  {
    //.itemArray = 55,
    //.itemGroup = 4,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_3",
    .pcName = "output_current",
    .pcTopic = "value/CSC/CHANNEL_3/output_current",
    .xValue.type = FLOAT,
    .xValue.mag.f = 0.0f
  },
  {
    //.itemArray = 56,
    //.itemGroup = 5,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_3",
    .pcName = "output_voltage",
    .pcTopic = "value/CSC/CHANNEL_3/output_voltage",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 57,
    //.itemGroup = 6,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_3",
    .pcName = "output_power",
    .pcTopic = "value/CSC/CHANNEL_3/output_power",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 58,
    //.itemGroup = 7,
    .pcTitle = "value",
    .pcSrvc = "CHANNEL_3",
    .pcName = "version",
    .pcTopic = "value/CSC/CHANNEL_3/version",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL,
    //.xValue.mag.ac[1] = '1',
    //.xValue.mag.ac[2] = '\0'
  },

// Сервис CHARGER

  {
    //.itemArray = 59,
    //.itemGroup = 1,
    .pcTitle = "value",
    .pcSrvc = "CHARGER",
    .pcName = "state",
    .pcTopic = "value/CSC/CHARGER/state",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL
    /*.xValue.mag.ac[0] = 'r',
    .xValue.mag.ac[1] = 'e',
    .xValue.mag.ac[2] = 'a',
    .xValue.mag.ac[3] = 'd',
    .xValue.mag.ac[4] = 'y',
    .xValue.mag.ac[5] = '\0'*/
  },
  {
    //.itemArray = 60,
    //.itemGroup = 2,
    .pcTitle = "value",
    .pcSrvc = "CHARGER",
    .pcName = "charge_current",
    .pcTopic = "value/CSC/CHARGER/charge_current",
    .xValue.type = FLOAT,
    .xValue.mag.f = 0.0f
  },
  {
    //.itemArray = 61,
    //.itemGroup = 3,
    .pcTitle = "value",
    .pcSrvc = "CHARGER",
    .pcName = "output_current_total",
    .pcTopic = "value/CSC/CHARGER/output_current_total",
    .xValue.type = FLOAT,
    .xValue.mag.f = 0.0f
  },
  {
    //.itemArray = 62,
    //.itemGroup = 4,
    .pcTitle = "value",
    .pcSrvc = "CHARGER",
    .pcName = "output_voltage",
    .pcTopic = "value/CSC/CHARGER/output_voltage",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 63,
    //.itemGroup = 5,
    .pcTitle = "value",
    .pcSrvc = "CHARGER",
    .pcName = "temperature_ab_1",
    .pcTopic = "value/CSC/CHARGER/temperature_ab_1",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 64,
    //.itemGroup = 6,
    .pcTitle = "value",
    .pcSrvc = "CHARGER",
    .pcName = "temperature_ab_2",
    .pcTopic = "value/CSC/CHARGER/temperature_ab_2",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 65,
    //.itemGroup = 7,
    .pcTitle = "value",
    .pcSrvc = "CHARGER",
    .pcName = "ab_type",
    .pcTopic = "value/CSC/CHARGER/ab_type",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = '\0'
  },
  {
    //.itemArray = 66,
    //.itemGroup = 8,
    .pcTitle = "value",
    .pcSrvc = "CHARGER",
    .pcName = "output_power",
    .pcTopic = "value/CSC/CHARGER/output_power",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 67,
    //.itemGroup = 9,
    .pcTitle = "value",
    .pcSrvc = "CHARGER",
    .pcName = "cooler_temperature",
    .pcTopic = "value/CSC/CHARGER/cooler_temperature",
    .xValue.type = INT32,
    .xValue.mag.sl = 0L
  },
  {
    //.itemArray = 68,
    //.itemGroup = 10,
    .pcTitle = "value",
    .pcSrvc = "CHARGER",
    .pcName = "power_supply",
    .pcTopic = "value/CSC/CHARGER/power_supply",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL
    /*.xValue.mag.ac[0] = 'd',
    .xValue.mag.ac[1] = 'i',
    .xValue.mag.ac[2] = 'e',
    .xValue.mag.ac[3] = 's',
    .xValue.mag.ac[4] = 'e',
    .xValue.mag.ac[5] = 'l',
    .xValue.mag.ac[6] = '\0'*/
  },
  {
    //.itemArray = 69,
    //.itemGroup = 11,
    .pcTitle = "value",
    .pcSrvc = "CHARGER",
    .pcName = "version",
    .pcTopic = "value/CSC/CHARGER/version",
    .xValue.type = STRING,
    .xValue.mag.ac[0] = NULL
    /*.xValue.mag.ac[0] = '6',
    .xValue.mag.ac[1] = '2',
    .xValue.mag.ac[2] = '\0'*/
  },

// SETTINGS

  {
    //.itemArray = 70,
    //.itemGroup = 11,
    .pcTitle = "value",
    .pcSrvc = "SETTINGS",
    .pcName = "datetime",
    .pcTopic = "value/CSC/SETTINGS/datetime",
    .xValue.type = STRING, //FIXME DATETIME
    .xValue.mag.ac[0] = '\0'
  },
  
// Конец. нулевой элемент

  {
    //.itemArray = 71,
    //.itemGroup = 0,
    .pcName = "",
    .pcTopic = NULL,
    .xValue.type = STRING, //FIXME DATETIME
    .xValue.mag.ac[0] = '\0'
  }
};