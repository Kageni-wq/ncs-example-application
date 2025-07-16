#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/mfd/npm1300.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/drivers/sensor/npm1300_charger.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include "zbus_common.h"
#include "pmic.h"

LOG_MODULE_REGISTER(pmic, LOG_LEVEL_WRN);

K_MSGQ_DEFINE(power_queue, sizeof(struct power_msg), 6, 4);
K_THREAD_DEFINE(pmic_thread_id, CONFIG_PMIC_STACK_SIZE, pmic_thread, NULL, NULL, NULL,
                K_PRIO_PREEMPT(CONFIG_PMIC_THREAD_PRIO), 0, 0);


static void pmic_thread(void *dummy1, void *dummy2, void *dummy3)
{
    ARG_UNUSED(dummy1);
    ARG_UNUSED(dummy2);
    ARG_UNUSED(dummy3);

    while (1) {
        struct power_msg msg;
        k_msgq_get(&power_queue, &msg, K_FOREVER);

    }
}
