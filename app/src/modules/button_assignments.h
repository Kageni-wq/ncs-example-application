#ifndef _BUTTON_ASSIGNMENTS_H_
#define _BUTTON_ASSIGNMENTS_H_

#include <zephyr/drivers/gpio.h>

/** @brief List of buttons assignments.
 *
 *  @note If the switch node existed the button gpio pin is
 *        assigned to the selected button constant. Any none
 *        existent switch node is set as unassigned.
 *
 */
enum button_pin_names {
	BUTTON_0 = DT_GPIO_PIN(DT_ALIAS(sw_right), gpios),
	BUTTON_1 = DT_GPIO_PIN(DT_ALIAS(sw_left), gpios),
	BUTTON_2 = DT_GPIO_PIN(DT_ALIAS(nav_up), gpios),
	BUTTON_3 = DT_GPIO_PIN(DT_ALIAS(nav_down), gpios),
};

#endif /* _BUTTON_ASSIGNMENTS_H_ */