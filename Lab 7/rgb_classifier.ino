#include <Color_Detection_inferencing.h>
#include <Arduino_APDS9960.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!APDS.begin()) {
    Serial.println("Failed to initialize APDS9960 sensor.");
    while (1);
  }

  Serial.println("Impulse Color Classification");
}

void loop() {
  if (APDS.colorAvailable()) {
    int r, g, b, brightness;
    APDS.readColor(r, g, b, brightness);

    float input_data[] = { (float)r, (float)g, (float)b, (float)brightness };
    signal_t signal;
    int err = numpy::signal_from_buffer(input_data, sizeof(input_data) / sizeof(float), &signal);

    if (err != 0) {
      Serial.println("Error creating signal from buffer.");
      return;
    }

    ei_impulse_result_t result;
    err = run_classifier(&signal, &result, false);
    if (err != EI_IMPULSE_OK) {
      Serial.println("Error running classifier.");
      return;
    }

    Serial.println("Classification Results:");
    for (size_t ix = 0; ix < result.classification.size(); ix++) {
      Serial.print(result.classification[ix].label);
      Serial.print(": ");
      Serial.println(result.classification[ix].value, 5);
    }

    delay(500);
  }
}
