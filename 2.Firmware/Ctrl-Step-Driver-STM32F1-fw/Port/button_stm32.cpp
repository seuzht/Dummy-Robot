#include "button_stm32.h"
#include <gpio.h>

bool Button::ReadButtonPinIO(uint8_t _id)
{
    switch (_id)
    {
        case 1:
            return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_SET;
        case 2:
            return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_SET;
        default:
            return false;
    }
}
bool Button::IsPressed()
{
    return !ReadButtonPinIO(id); //检查HAL_GPIO_ReadPin管脚状态是否为GPIO_PIN_SET来判断按钮是否按下，与PinIO上拉还是下拉有关;
    //这里的按钮引脚是上拉，所以当HAL_GPIO_ReadPin() != GPIO_PIN_SET时为按下状态
}
