#include "st7735.h"

// Команды контроллера
#define ST7735_SWRESET 0x01
#define ST7735_SLPOUT  0x11
#define ST7735_COLMOD  0x3A
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C

static void ST7735_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);

    // Используем таймаут 10мс, чтобы не виснуть вечно
    if (HAL_SPI_Transmit(&hspi4, &cmd, 1, 10) != HAL_OK) {
        // Если ошибка - выходим, чтобы не вешать МК
    }

    HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_SET);
}

static void ST7735_WriteData(uint8_t* buff, size_t buff_size) {
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(&ST7735_SPI_PORT, buff, buff_size, HAL_MAX_DELAY);
}

void ST7735_Init(void) {
    HAL_GPIO_WritePin(ST7735_BL_GPIO_Port, ST7735_BL_Pin, GPIO_PIN_SET); // Включить подсветку
	HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);

    ST7735_WriteCommand(ST7735_SWRESET);
    HAL_Delay(150);
    ST7735_WriteCommand(ST7735_SLPOUT);
    HAL_Delay(150);
    ST7735_WriteCommand(ST7735_COLMOD);
    uint8_t color_mode = 0x05; // 16-bit color
    ST7735_WriteData(&color_mode, 1);
    ST7735_WriteCommand(ST7735_DISPON);
}

void ST7735_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    ST7735_WriteCommand(ST7735_CASET);
    uint8_t data[] = { 0x00, x0 + 26, 0x00, x1 + 26 }; // +26 смещение для 0.96"
    ST7735_WriteData(data, 4);
    ST7735_WriteCommand(ST7735_RASET);
    uint8_t data2[] = { 0x00, y0 + 1, 0x00, y1 + 1 };
    ST7735_WriteData(data2, 4);
    ST7735_WriteCommand(ST7735_RAMWR);
}

void ST7735_FillScreen(uint16_t color) {
    ST7735_SetAddressWindow(0, 0, 159, 79);
    uint8_t data[] = { color >> 8, color & 0xFF };
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
    for (int i = 0; i < 160 * 80; i++) {
        HAL_SPI_Transmit(&ST7735_SPI_PORT, data, 2, HAL_MAX_DELAY);
    }
}

// Упрощенный вывод текста (встроенный шрифт 5x7)
void ST7735_DrawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg) {
    // В этой упрощенной версии текст выводится попиксельно или можно
    // добавить массив шрифта здесь. Для теста пока оставим очистку.
    ST7735_FillScreen(bg);
}
