/*
 * taken from http://www.gammon.com.au/power
 * We use the watchdog to wake us from deep sleep. The length of the
 * deep sleep depends on the current battery voltage. If above 
 * warn_voltage, we wake every second, if between shutdown_voltage and
 * warn_voltage, we wake very 2 seconds, and if we are below shutdown_voltage
 * we only wake every 8 seconds. Our seconds counter is changed accordingly.
 */
void reset_watchdog () {
  uint8_t wd_value;

  if (bat_voltage <= shutdown_voltage) {
    // either startup or low power (includes bat_voltage == 0)
    // If we are starting then this gives us enough time to
    // initialize everything without any problems    
    wd_value = bit (WDIE) | bit (WDP3) | bit (WDP0);                 // set WDIE, and 8 seconds delay
    seconds += 8;
  } else if (bat_voltage <= warn_voltage) {
    // warn_voltage, we reduce signalling to every 2 seconds
    wd_value = bit (WDIE) | bit (WDP2) | bit (WDP1) | bit (WDP0);    // set WDIE, and 2 second delay
    seconds += 2;
  } else {
    // everything ok, we signal every second
    wd_value = bit (WDIE) | bit (WDP2) | bit (WDP1);                 // set WDIE, and 1 second delay
    seconds += 1;
  }

  // clear various "reset" flags
  MCUSR = 0;
  // allow changes, disable reset, clear existing interrupt, data sheet ch. 8.5.2, p.46ff
  WDTCR = bit (WDCE) | bit (WDE) | bit (WDIF);
  // data change, Ch. 8.4.1.2, the new timeout value has to be written within the next
  // 4 cycles. Thus we first determine the correct value (above) and then write it at once. 
  WDTCR = wd_value;

  wdt_reset();
}

// watchdog interrupt
ISR (WDT_vect) {
  wdt_disable();  // disable watchdog
}
