/*
 * Copyright (c) 2025
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_input_processor_layer_scroll

#include <zephyr/device.h>
#include <zephyr/input/input.h>
#include <zephyr/logging/log.h>
#include <zmk/input.h>
#include <zmk/layers.h>
#include <zmk/keymap.h>

LOG_MODULE_REGISTER(layer_scroll, CONFIG_ZMK_LOG_LEVEL);

struct layer_scroll_config {
    uint8_t layers_count;
    uint8_t layers[];
};

struct layer_scroll_data {
    // nothing needed for now
};

static bool is_scroll_layer(uint8_t current_layer, const struct layer_scroll_config *config) {
    for (int i = 0; i < config->layers_count; i++) {
        if (config->layers[i] == current_layer) {
            return true;
        }
    }
    return false;
}

static int layer_scroll_process(const struct device *dev, struct input_event *event,
                                 uint32_t param1, uint32_t param2, struct zmk_input_processor_state *state) {
    const struct layer_scroll_config *config = dev->config;
    
    // Only process relative X/Y movement
    if (event->type != INPUT_EV_REL) {
        return ZMK_INPUT_PROC_CONTINUE;
    }
    
    if (event->code != INPUT_REL_X && event->code != INPUT_REL_Y) {
        return ZMK_INPUT_PROC_CONTINUE;
    }
    
    // Check if we're on a scroll layer
    uint8_t current_layer = zmk_keymap_highest_layer_active();
    if (!is_scroll_layer(current_layer, config)) {
        return ZMK_INPUT_PROC_CONTINUE;
    }
    
    // Transform movement to scroll
    if (event->code == INPUT_REL_X) {
        event->code = INPUT_REL_HWHEEL;
        // Invert for natural feeling
        event->value = -event->value;
    } else if (event->code == INPUT_REL_Y) {
        event->code = INPUT_REL_WHEEL;
        // Invert for natural feeling
        event->value = -event->value;
    }
    
    return ZMK_INPUT_PROC_CONTINUE;
}

static const struct zmk_input_processor_driver_api layer_scroll_driver_api = {
    .process = layer_scroll_process,
};

#define LAYER_SCROLL_INIT(n)                                                                      \
    static struct layer_scroll_data layer_scroll_data_##n = {};                                   \
                                                                                                  \
    static const uint8_t layer_scroll_layers_##n[] = DT_PROP(n, layers);                          \
                                                                                                  \
    static const struct layer_scroll_config layer_scroll_config_##n = {                           \
        .layers_count = DT_PROP_LEN(n, layers),                                                   \
        .layers = layer_scroll_layers_##n,                                                        \
    };                                                                                            \
                                                                                                  \
    DEVICE_DT_INST_DEFINE(n, NULL, NULL, &layer_scroll_data_##n, &layer_scroll_config_##n,         \
                          POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,                        \
                          &layer_scroll_driver_api);

DT_INST_FOREACH_STATUS_OKAY(LAYER_SCROLL_INIT)
