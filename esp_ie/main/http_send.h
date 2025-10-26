#ifndef HTTP_SEND_H
#define HTTP_SEND_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gửi dữ liệu cảm biến lên webserver bằng HTTP POST
 *
 * @param temp  Nhiệt độ (°C)
 * @param ph    Giá trị pH
 * @param tds   Độ dẫn điện (ppm)
 * @param ntu   Độ đục (NTU)
 */
void send_data_to_server(float temp, float ph, float tds, float ntu);

#ifdef __cplusplus
}
#endif

#endif // HTTP_SEND_H
