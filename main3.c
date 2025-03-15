#include <stm32f031x6.h>
#include "display.h"
#include <stdlib.h>
#include <time.h>

#define MAZE_BLOCK_COUNT 5

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void spawnCoin();
void resetGame();
void LightOn();
void LightOff();

volatile uint32_t milliseconds;

uint16_t x = 50;      // Player's X position
uint16_t y = 50;      // Player's Y position
uint16_t oldx = 50;   // Previous X position (initialized to 50)
uint16_t oldy = 50;   // Previous Y position (initialized to 50)
uint16_t coinX = 0; // Coin's X position
uint16_t coinY = 0; // Coin's Y position
int coinActive = 0; // Flag to indicate if a coin is active

const uint16_t mazeX[] = {60, 60, 60, 60, 60};
const uint16_t mazeY[] = {20, 36, 52, 68, 84};

const uint16_t coin[] = {
    65535,65535,0,0,0,65535,65535,65535,0,64534,64534,64534,0,65535,0,64534,6926,57872,6926,64534,0,0,64534,6926,57872,6926,64534,0,0,64534,6926,57872,6926,64534,0,65535,0,64534,64534,64534,0,65535,65535,65535,0,0,0,65535,65535,
    // Coin image data
};

const uint16_t kratos[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38581,38581,38581,38581,38581,38581,38581,40224,40224,0,0,0,0,0,0,0,0,0,0,38581,38581,38581,38581,38581,38581,38581,40224,40224,40224,38581,0,0,0,0,0,0,0,0,0,38581,38581,38581,38581,38581,38581,38581,40224,40224,38581,38581,0,0,0,0,0,0,0,0,38581,38581,38581,65535,0,38581,38581,38581,65535,0,38581,38581,38581,0,0,0,0,0,0,0,0,38581,38581,0,0,38581,38581,38581,0,0,38581,38581,0,0,0,0,0,0,0,0,0,38581,38581,38581,38581,38581,38581,38581,40224,40224,38581,38581,0,0,0,0,0,0,0,0,0,0,38581,38581,38581,38581,38581,38581,38581,40224,40224,0,0,0,0,0,0,0,0,0,0,0,0,0,38581,38581,38581,38581,38581,0,0,0,0,0,0,0,0,0,0,0,4912,4912,38581,24334,0,0,0,0,0,40224,40224,0,0,0,0,0,0,0,0,4912,4912,24334,38581,24334,38581,51266,38581,38581,40224,40224,40224,38581,0,0,0,0,0,0,0,4912,24334,38581,0,38581,38581,51266,38581,38581,40224,40224,0,38581,40224,0,0,0,0,0,0,24334,38581,38581,0,51266,51266,38581,51266,40224,40224,40224,0,40224,24334,24334,0,0,0,0,0,21306,24334,0,0,38581,38581,38581,38581,40224,40224,38581,0,0,24334,21306,0,0,0,0,0,0,0,65535,0,21306,21306,21306,21306,21306,21306,21306,0,0,0,0,0,0,0,0,0,0,0,0,38581,4912,24334,24334,24334,24334,24334,4912,38581,0,0,0,0,0,0,0,0,0,0,0,21306,38581,38581,0,0,0,38581,38581,21306,0,0,0,0,0,0,0,0,0,0,0,4912,21306,0,0,0,0,0,21306,4912,0,0,0,0,0,0,0,0,0,0,38581,4912,4912,0,0,0,0,0,4912,4912,38581,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Kratos image data
};

const uint16_t krtrun[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38581,38581,38581,38581,38581,38581,38581,40224,40224,0,0,0,0,0,0,0,0,0,0,38581,38581,38581,38581,38581,38581,38581,40224,40224,40224,38581,0,0,0,0,0,0,0,0,0,38581,38581,38581,38581,38581,38581,38581,40224,40224,38581,38581,0,0,0,0,0,0,0,0,38581,38581,38581,65535,0,38581,38581,38581,65535,0,38581,38581,38581,0,0,0,0,0,0,0,0,38581,38581,0,0,38581,38581,38581,0,0,38581,38581,0,0,0,0,0,0,0,0,0,38581,38581,38581,38581,38581,38581,38581,40224,40224,38581,38581,0,0,0,0,0,0,0,0,0,0,38581,38581,38581,38581,38581,38581,38581,40224,40224,0,0,0,0,0,0,0,0,0,0,0,0,0,38581,38581,38581,38581,38581,0,0,0,0,0,0,0,0,0,0,0,4912,4912,38581,24334,0,0,0,0,0,40224,40224,0,0,0,0,0,0,0,0,4912,4912,24334,38581,24334,38581,51266,38581,38581,40224,40224,40224,38581,0,0,0,0,0,0,0,4912,24334,38581,0,38581,38581,51266,38581,38581,40224,40224,0,38581,40224,0,0,0,0,0,0,24334,38581,38581,0,51266,51266,38581,51266,40224,40224,40224,0,40224,24334,24334,0,0,0,0,0,21306,24334,0,0,38581,38581,38581,38581,40224,40224,38581,0,0,24334,21306,0,0,0,0,0,0,0,0,0,21306,21306,21306,21306,21306,21306,21306,0,0,0,0,0,0,0,0,0,0,0,0,38581,4912,24334,24334,24334,24334,24334,4912,0,0,0,0,0,0,0,0,0,0,4912,4912,21306,38581,38581,0,0,0,38581,38581,0,0,0,0,0,0,0,0,0,0,4912,4912,4912,21306,0,0,0,0,21306,21306,0,0,0,0,0,0,0,0,0,0,38581,0,0,0,0,0,0,0,4912,4912,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4912,4912,38581,0,0,0,0,0,
    // Kratos running image data
};

const uint16_t krttop[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38581,38581,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38581,38581,38581,38581,38581,38581,38581,38581,40224,40224,0,0,0,0,0,0,0,24334,24334,51266,38581,38581,38581,38581,38581,38581,38581,38581,40224,40224,4912,24334,24334,0,0,0,0,24334,24334,51266,38581,38581,38581,38581,38581,38581,38581,40224,40224,40224,4912,24334,24334,0,0,0,0,24334,24334,51266,38581,38581,38581,38581,38581,38581,38581,40224,40224,40224,51266,24334,24334,0,0,0,0,24334,24334,51266,38581,38581,38581,38581,38581,38581,38581,40224,40224,38581,51266,24334,24334,0,0,0,0,0,0,0,38581,38581,38581,38581,38581,38581,40224,40224,40224,38581,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // Kratos top image data
};

const uint16_t Frieza[] = {
    // Frieza image data
};

const uint16_t dg1[] = {
    0,0,16142,16142,16142,16142,16142,16142,16142,16142,0,0,0,0,0,16142,16142,16142,16142,16142,16142,0,0,0,0,0,16142,16142,16142,16142,16142,16142,16142,16142,0,0,0,0,16142,16142,16142,1994,1994,16142,16142,16142,0,0,0,0,16142,16142,16142,1994,16142,1994,16142,16142,0,0,0,0,16142,16142,16142,1994,16142,1994,16142,16142,0,0,0,0,16142,16142,16142,1994,16142,1994,16142,16142,0,0,0,0,16142,16142,16142,1994,1994,16142,16142,16142,0,0,0,0,16142,16142,16142,16142,16142,16142,16142,16142,0,0,0,0,16142,16142,16142,1994,1994,1994,16142,16142,0,0,0,0,16142,16142,16142,1994,16142,16142,16142,16142,0,0,0,0,16142,16142,16142,1994,16142,16142,16142,16142,0,0,0,0,16142,16142,16142,1994,16142,1994,16142,16142,0,0,0,0,16142,16142,16142,1994,1994,1994,16142,16142,0,0,0,0,0,16142,16142,16142,16142,16142,16142,0,0,0,0,0,0,16142,16142,16142,16142,16142,16142,0,0,0,
    // dg1 image data
};

int main() {
    int hinverted = 0;
    int vinverted = 0;
    int toggle = 0;
    int hmoved = 0;
    int vmoved = 0;

    srand(time(NULL)); // Seed the random number generator

    initClock();
    initSysTick();
    setupIO();
    putImage(20, 80, 12, 16, dg1, 0, 0);

    // Spawn the first coin
    spawnCoin();

    while (1) {
        hmoved = vmoved = 0;
        hinverted = vinverted = 0;

        // Check if the restart button (PA9) is pressed
        if ((GPIOA->IDR & (1 << 9)) == 0) { // Button pressed (active low)
            resetGame();
            delay(500); // Debounce delay
        }

        if ((GPIOB->IDR & (1 << 4)) == 0) { // right pressed
            if (x < 110) {
                x = x + 1;
                hmoved = 1;
                hinverted = 0;
            }
        }
        if ((GPIOB->IDR & (1 << 5)) == 0) { // left pressed
            if (x > 10) {
                x = x - 1;
                hmoved = 1;
                hinverted = 1;
            }
        }
        if ((GPIOA->IDR & (1 << 11)) == 0) { // down pressed
            if (y < 140) {
                y = y + 1;
                vmoved = 1;
                vinverted = 0;
            }
        }
        if ((GPIOA->IDR & (1 << 8)) == 0) { // up pressed
            if (y > 16) {
                y = y - 1;
                vmoved = 1;
                vinverted = 1;
            }
        }

        if ((vmoved) || (hmoved)) {
            fillRectangle(oldx, oldy, 12, 16, 0);
            oldx = x;
            oldy = y;

            if (hmoved) {
                if (toggle)
                    putImage(x, y, 20, 20, kratos, hinverted, 0);
                else
                    putImage(x, y, 20, 20, krtrun, hinverted, 0);

                toggle = toggle ^ 1;
            } else {
                putImage(x, y, 20, 20, krttop, 0, vinverted);
            }

            // Check if the player collides with the coin
            if (coinActive && (isInside(coinX, coinY, 8, 8, x, y) || 
                               isInside(coinX, coinY, 8, 8, x + 12, y) || 
                               isInside(coinX, coinY, 8, 8, x, y + 16) || 
                               isInside(coinX, coinY, 8, 8, x + 12, y + 16))) {
                // Player collided with the coin
                fillRectangle(coinX, coinY, 8, 8, 0); // Clear the coin from the screen
                coinActive = 0; // Mark the coin as inactive
                spawnCoin(); // Spawn a new coin
                //led flashing when player collects the coin 
                LightOn();
                delay(10);
                LightOff();
            }
        }

        delay(50);
    }

    return 0;
}

void initSysTick(void) {
    SysTick->LOAD = 48000;
    SysTick->CTRL = 7;
    SysTick->VAL = 10;
    __asm(" cpsie i "); // enable interrupts
}

void SysTick_Handler(void) {
    milliseconds++;
}

void initClock(void) {
    // Set the PLL up
    RCC->CR &= ~(1u << 24);
    while ((RCC->CR & (1 << 25)));

    FLASH->ACR |= (1 << 0);
    FLASH->ACR &= ~((1u << 2) | (1u << 1));
    FLASH->ACR |= (1 << 4);

    RCC->CFGR &= ~((1u << 21) | (1u << 20) | (1u << 19) | (1u << 18));
    RCC->CFGR |= ((1 << 21) | (1 << 19));

    RCC->CFGR |= (1 << 14);

    RCC->CR |= (1 << 24);
    RCC->CFGR |= (1 << 1);
}

void delay(volatile uint32_t dly) {
    uint32_t end_time = dly + milliseconds;
    while (milliseconds != end_time)
        __asm(" wfi "); // sleep
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber) {
    Port->PUPDR = Port->PUPDR & ~(3u << BitNumber * 2);
    Port->PUPDR = Port->PUPDR | (1u << BitNumber * 2);
}

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode) {
    uint32_t mode_value = Port->MODER;
    Mode = Mode << (2 * BitNumber);
    mode_value = mode_value & ~(3u << (BitNumber * 2));
    mode_value = mode_value | Mode;
    Port->MODER = mode_value;
}

int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py) {
    uint16_t x2 = x1 + w;
    uint16_t y2 = y1 + h;
    return (px >= x1) && (px <= x2) && (py >= y1) && (py <= y2);
}

void setupIO() {
    RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
    display_begin();
    pinMode(GPIOB, 4, 0);
    pinMode(GPIOB, 5, 0);
    pinMode(GPIOA, 8, 0);
    pinMode(GPIOA, 11, 0);
    pinMode(GPIOA, 9, 0); // Configure PA9 as input
    pinMode(GPIOA, 10, 0);// Configure PA10 as output
    enablePullUp(GPIOA, 9); // Enable pull-up resistor for PA9
    enablePullUp(GPIOB, 4);
    enablePullUp(GPIOB, 5);
    enablePullUp(GPIOA, 11);
    enablePullUp(GPIOA, 8);
}

void spawnCoin() {
    coinX = ((rand() % 11) + 1) * 10; // Random X between 10 and 110
    coinY = ((rand() % 13) + 2) * 10; // Random Y between 20 and 140
    putImage(coinX, coinY, 8, 8, coin, 0, 0);
    coinActive = 1; // Mark the coin as active
}

void resetGame() {
    // Reset player position
    x = 50;
    y = 50;
    oldx = x;
    oldy = y;

    // Clear the screen
    fillRectangle(0, 0, 128, 154, 0); // Clear the entire screen (adjust dimensions as needed)

    // Redraw initial game elements
    putImage(20, 80, 12, 16, dg1, 0, 0);
    putImage(x, y, 20, 20, kratos, 0, 0);

    // Spawn the first coin
    spawnCoin();
}

void LightOn()
{
    GPIOA->ODR |= (1 << 10);//ON 
}// end LightOn()

void LightOff()
{
    GPIOA->ODR &= ~(1 << 10);//OFF
}// end LightoFF