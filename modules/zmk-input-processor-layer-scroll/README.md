# Layer Scroll Input Processor

Transforms trackball movement to scroll events when on specified layers.

## Usage

```dts
#include <input/processors/layer_scroll.dtsi>

&trackball_central_listener {
    input-processors = <&zip_layer_scroll 1 2>, <&zip_ble_report_rate_limit>;
};
```

Parameters are layer numbers that trigger scroll mode.
