#ifndef __SENSOR_H
#define __SENSOR_H

#include <stdint.h>
#include <stdlib.h> // Cần cho abs()

// ====================================================================
// CẤU TRÚC DỮ LIỆU ĐÃ CHUYỂN ĐỔI (Chỉ hiển thị 1 số sau dấu .)
// ====================================================================
typedef struct {
    int32_t integer;    // Phần nguyên
    int32_t decimal;    // Phần thập phân (chỉ 1 chữ số, từ 0-9)
} sensor_value_t;

// ====================================================================
// KHAI BÁO CÁC HẰNG SỐ FIXED-POINT (Scale Factor = 1000)
// ====================================================================

#define SCALE_FACTOR 1000L 
#define ADC_MAX_VALUE 4095

#define V_REF_VOLTAGE_FIXED 3300L   // VDDA 3.3V (mV)
#define ADC_VDDA_MV_FIXED 3300L

// 3. PH: pH = (V_out_V * 1000) / PH_SLOPE + PH_OFFSET
#define PH_7_VOLTAGE_V_FIXED 3663L  // 3.663V * 1000
#define PH_SLOPE_V_PER_PH_FIXED (-59L) // -0.059 V/pH * 1000

// DFRobot TDS polynomial coefficients (scaled by 1000)
#define TDS_A_FIXED   133420L    // 133.42 * 1000
#define TDS_B_FIXED  -255860L    // -255.86 * 1000
#define TDS_C_FIXED   857390L    // 857.39 * 1000
#define TDS_TEMP_C_DEFAULT_FIXED 25000L  // 25.000°C * 1000 (nếu bạn lưu nhiệt độ dạng *1000)
// Không cần TDS_TEMP_FACTOR_FIXED

// TURBIDITY (DFRobot): NTU = -1120.4*V^2 + 5742.3*V - 4352.9
// Fixed-point scale: NTU*1000, V*1000
#define TURB_A_FIXED   (-1120400L)  // -1120.4 * 1000
#define TURB_B_FIXED   ( 5742300L)  //  5742.3 * 1000
#define TURB_C_FIXED   (-4352900L)  // -4352.9 * 1000
#define TURB_VOUT_SCALE_NUM 30L
#define TURB_VOUT_SCALE_DEN 20L
#define ADC_SAT_THRESHOLD 4090
// ====================================================================
// KHAI BÁO HÀM (FIXED-POINT VÀ CHUYỂN ĐỔI)
// ====================================================================

/** Chuyển ADC raw sang Điện áp Fixed-Point (Volt * 1000) */
int32_t raw_to_v_fp(uint16_t adc_raw);


/** Tính pH Fixed-Point (pH * 1000) */
int32_t calc_ph_fp(uint16_t adc_raw);

/** Tính TDS Fixed-Point (PPM * 1000) - KHÔNG BÙ NHIỆT */
int32_t calc_tds_fp(uint16_t adc_raw);

/** Tính Độ đục Fixed-Point (NTU * 1000) */
int32_t calc_turb_fp(uint16_t adc_raw);

/** Chuyển đổi kết quả FP (x1000) sang struct (x10) để hiển thị 1 số sau dấu . */
sensor_value_t convert_fp_to_struct(int32_t fp_value);

#endif /* __SENSOR_H */
