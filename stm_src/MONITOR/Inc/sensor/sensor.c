#include "sensor.h"
#include <stdlib.h> // Cần cho hàm abs()

// ====================================================================
// HÀM TÍNH TOÁN FIXED-POINT (Giữ nguyên)
// ====================================================================

int32_t raw_to_v_fp(uint16_t adc_raw) {
    int64_t numerator = (int64_t)adc_raw * V_REF_VOLTAGE_FIXED;
    return (int32_t)(numerator / ADC_MAX_VALUE);
}

int32_t calc_temp_fp(uint16_t adc_raw) {
    int32_t V_out_V_fp = raw_to_v_fp(adc_raw);
    
    int32_t T_term_fp = (int32_t)(((int64_t)V_out_V_fp * SCALE_FACTOR) / T_SLOPE_FIXED);

    int32_t temp_fp = T_term_fp + T_OFFSET_FIXED;

    return temp_fp;
}

int32_t calc_ph_fp(uint16_t adc_raw) {
    int32_t V_out_V_fp = raw_to_v_fp(adc_raw);
    int64_t numerator = ((int64_t)V_out_V_fp - (int64_t)PH_7_VOLTAGE_V_FIXED) * SCALE_FACTOR;
    int32_t delta_ph_fp = (int32_t)(numerator / PH_SLOPE_V_PER_PH_FIXED);
    int32_t ph_fp = (7 * SCALE_FACTOR) + delta_ph_fp;
    if (ph_fp < 0) ph_fp = 0;
    if (ph_fp > 14 * SCALE_FACTOR) ph_fp = 14 * SCALE_FACTOR;

    return ph_fp;
}

int32_t calc_tds_fp(uint16_t adc_raw) {
    int32_t V_out_V_fp = raw_to_v_fp(adc_raw);
    
    int64_t numerator = (int64_t)TDS_SLOPE_FIXED * V_out_V_fp;
    int32_t tds_fp = (int32_t)(numerator / SCALE_FACTOR);

    if (tds_fp < 0) tds_fp = 0;

    return tds_fp;
}

int32_t calc_turb_fp(uint16_t adc_raw) {
    int32_t V_out_V_fp = raw_to_v_fp(adc_raw);
    
    int64_t numerator = (int64_t)TURB_SLOPE_FIXED * V_out_V_fp;
    int32_t NTU_term_fp = (int32_t)(numerator / SCALE_FACTOR);
    
    int32_t turb_fp = NTU_term_fp + TURB_OFFSET_FIXED;
    
    if (turb_fp < 0) {
        turb_fp = 0;
    }
    
    return turb_fp;
}

// ====================================================================
// HÀM CHUYỂN ĐỔI KẾT QUẢ VỀ 1 CHỮ SỐ THẬP PHÂN
// ====================================================================

sensor_value_t convert_fp_to_struct(int32_t fp_value) {
    sensor_value_t result;
    const int32_t DISPLAY_SCALE = 100; // 1000 / 10 = 100
    const int32_t TARGET_SCALE = 10;   // Mục tiêu là 1 số thập phân
    int32_t rounded_fp10 = fp_value / DISPLAY_SCALE; 
    result.integer = rounded_fp10 / TARGET_SCALE;
    int32_t abs_rounded_fp10 = (rounded_fp10 < 0) ? -rounded_fp10 : rounded_fp10;
    
    result.decimal = abs_rounded_fp10 % TARGET_SCALE;
    
    return result;
}