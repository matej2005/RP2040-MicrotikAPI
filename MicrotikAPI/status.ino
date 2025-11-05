void setupStatus() {


#ifdef PIN_NEOPIXEL
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();  // Set all pixel colors to 'off'
  pixels.show();
#else
  pinMode(PIN_LED, OUTPUT);
#endif

}



Status currentStatus = CLEAR;

void setStatus(Status status) {
  currentStatus = status;
#ifdef PIN_NEOPIXEL
  pixels.clear();  // Set all pixel colors to 'off'
  switch (status) {
    case OK:
      pixels.setPixelColor(0, pixels.Color(0, 50, 0));
      break;
    case WARN:
      pixels.setPixelColor(0, pixels.Color(64, 23, 0));
      break;
    case INFO:
      pixels.setPixelColor(0, pixels.Color(0, 0, 50));
      break;
    case ERROR:
      pixels.setPixelColor(0, pixels.Color(50, 0, 0));
      break;
    case CLEAR:
      break;
  }
  pixels.show();  // Send the updated pixel colors to the hardware.
#else

#endif
}