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

// 1. CHUNG: Điện áp tham chiếu V_REF = 5.0V
#define V_REF_VOLTAGE_FIXED 5000L // 5.0V * 1000

// 2. NHIỆT ĐỘ: T = (V_out_V * 1000) / T_SLOPE + T_OFFSET
#define T_SLOPE_FIXED       10L     // 0.01 V/C * 1000
#define T_OFFSET_FIXED      (-50000L) // -50 °C * 1000

// 3. PH: pH = (V_out_V * 1000) / PH_SLOPE + PH_OFFSET
#define PH_7_VOLTAGE_V_FIXED 3663L  // 3.663V * 1000
#define PH_SLOPE_V_PER_PH_FIXED (-59L) // -0.059 V/pH * 1000

// 4. TDS: TDS = TDS_SLOPE * V_out_Compensated (KHÔNG BÙ NHIỆT)
#define TDS_SLOPE_FIXED     500000L // 500 PPM/V * 1000 
// Không cần TDS_TEMP_FACTOR_FIXED

// 5. ĐỘ ĐỤC: NTU = TURB_SLOPE * V_out_V + TURB_OFFSET
// Giả định: 0 NTU tại 3.0V (V_out cao khi trong)
#define TURB_SLOPE_FIXED    (-1000000L) // Độ dốc ÂM: -1000 NTU/V * 1000
#define TURB_OFFSET_FIXED   (3000000L) // Offset để 0 NTU tại 3.0V

// ====================================================================
// KHAI BÁO HÀM (FIXED-POINT VÀ CHUYỂN ĐỔI)
// ====================================================================

/** Chuyển ADC raw sang Điện áp Fixed-Point (Volt * 1000) */
int32_t raw_to_v_fp(uint16_t adc_raw);

/** Tính Nhiệt độ Fixed-Point (°C * 1000) */
int32_t calc_temp_fp(uint16_t adc_raw);

/** Tính pH Fixed-Point (pH * 1000) */
int32_t calc_ph_fp(uint16_t adc_raw);

/** Tính TDS Fixed-Point (PPM * 1000) - KHÔNG BÙ NHIỆT */
int32_t calc_tds_fp(uint16_t adc_raw);

/** Tính Độ đục Fixed-Point (NTU * 1000) */
int32_t calc_turb_fp(uint16_t adc_raw);

/** Chuyển đổi kết quả FP (x1000) sang struct (x10) để hiển thị 1 số sau dấu . */
sensor_value_t convert_fp_to_struct(int32_t fp_value);

#endif /* __SENSOR_H */