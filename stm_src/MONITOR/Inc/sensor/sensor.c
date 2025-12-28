#include "sensor.h"
#include <stdlib.h> // Cần cho hàm abs()
#define NTU_CLEAR_FP  (320L * 1000L)

int32_t raw_to_v_fp(uint16_t adc_raw) {
    int64_t numerator = (int64_t)adc_raw * V_REF_VOLTAGE_FIXED;
    return (int32_t)(numerator / ADC_MAX_VALUE);
}
static inline int32_t adc_to_mV_fp(uint16_t adc_raw)
{
    return (int32_t)((int64_t)adc_raw * ADC_VDDA_MV_FIXED / ADC_MAX_VALUE); // mV
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

//int32_t calc_tds_fp(uint16_t adc_raw) {
//    int32_t V_out_V_fp = raw_to_v_fp(adc_raw);
//
//    int64_t numerator = (int64_t)TDS_SLOPE_FIXED * V_out_V_fp;
//    int32_t tds_fp = (int32_t)(numerator / SCALE_FACTOR);
//
//    if (tds_fp < 0) tds_fp = 0;
//
//    return tds_fp;
//}
int32_t calc_tds_fp(uint16_t adc_raw)
{
    // V_out_V_fp = V * 1000
    int32_t V_fp = raw_to_v_fp(adc_raw);
    if (V_fp <= 0) return 0;

    // === 1) Temperature compensation ===
    // T = 25°C => coeff = 1.0
    // coeff_fp = (1 + 0.02*(T-25)) * 1000
    // Với T = 25°C => coeff_fp = 1000
    int32_t coeff_fp = 1000;
    int32_t V25_fp = (int32_t)(((int64_t)V_fp * SCALE_FACTOR) / coeff_fp);
    int64_t V2_fp = ((int64_t)V25_fp * (int64_t)V25_fp) / SCALE_FACTOR;
    int64_t V3_fp = (V2_fp * (int64_t)V25_fp) / SCALE_FACTOR;
    int64_t term1_fp = ((int64_t)TDS_A_FIXED * V3_fp) / SCALE_FACTOR; // 133.42*V^3
    int64_t term2_fp = ((int64_t)TDS_B_FIXED * V2_fp) / SCALE_FACTOR; // -255.86*V^2
    int64_t term3_fp = ((int64_t)TDS_C_FIXED * (int64_t)V25_fp) / SCALE_FACTOR; // 857.39*V
    int64_t tds_fp64 = term1_fp + term2_fp + term3_fp;
    tds_fp64 /= 2;
    if (tds_fp64 < 0) tds_fp64 = 0;
    if (tds_fp64 > INT32_MAX) tds_fp64 = INT32_MAX;
    return (int32_t)tds_fp64; // ppm * 1000
}

//int32_t calc_turb_fp(uint16_t adc_raw) {
//    int32_t V_out_V_fp = raw_to_v_fp(adc_raw);
//
//    int64_t numerator = (int64_t)TURB_SLOPE_FIXED * V_out_V_fp;
//    int32_t NTU_term_fp = (int32_t)(numerator / SCALE_FACTOR);
//
//    int32_t turb_fp = NTU_term_fp + TURB_OFFSET_FIXED;
//
//    if (turb_fp < 0) {
//        turb_fp = 0;
//    }
//
//    return turb_fp;
//}

//int32_t calc_turb_fp(uint16_t adc_raw)
//{
//    if (adc_raw >= ADC_SAT_THRESHOLD) {
//        return 0; // NTU*1000
//    }
//    int32_t Vadc_mV = adc_to_mV_fp(adc_raw);
//    int32_t Vout_mV = (int32_t)((int64_t)Vadc_mV * TURB_VOUT_SCALE_NUM / TURB_VOUT_SCALE_DEN);
//    int32_t V_fp = Vout_mV; // (Volt * 1000)
//
//    if (V_fp <= 0) return 0;
//    int64_t V2_fp = ((int64_t)V_fp * (int64_t)V_fp) / SCALE_FACTOR;
//    int64_t termA = ((int64_t)TURB_A_FIXED * V2_fp) / SCALE_FACTOR;
//    int64_t termB = ((int64_t)TURB_B_FIXED * (int64_t)V_fp) / SCALE_FACTOR;
//    int64_t termC = (int64_t)TURB_C_FIXED;
//    int64_t ntu_fp64 = termA + termB + termC;
//    if (ntu_fp64 < 0) ntu_fp64 = 0;
//    if (ntu_fp64 > INT32_MAX) ntu_fp64 = INT32_MAX;
//    return (int32_t)ntu_fp64; // NTU * 1000
//
//}
int32_t calc_turb_fp(uint16_t adc_raw)
{
    int32_t V_mV = adc_to_mV_fp(adc_raw);
    int32_t Vuse_mV = ADC_VDDA_MV_FIXED - V_mV;
    if (Vuse_mV < 0) Vuse_mV = 0;
    int32_t V_fp = Vuse_mV;
    int64_t V2_fp = ((int64_t)V_fp * V_fp) / SCALE_FACTOR;

    int64_t ntu_fp64 =
        ((int64_t)TURB_A_FIXED * V2_fp) / SCALE_FACTOR +
        ((int64_t)TURB_B_FIXED * V_fp)  / SCALE_FACTOR +
        (int64_t)TURB_C_FIXED;

    if (ntu_fp64 < 0) ntu_fp64 = 0;
    if (ntu_fp64 > INT32_MAX) ntu_fp64 = INT32_MAX;
    ntu_fp64 -= (int64_t)NTU_CLEAR_FP;
    if (ntu_fp64 < 0) ntu_fp64 = 0;

    return (int32_t)ntu_fp64; // NTU*1000
}


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
