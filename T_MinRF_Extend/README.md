

Wireless four-in-one module，CC1101、LR1121、NRF24L01、ST32R2916；

### CC1101：

Sets carrier frequency. Allowed values are in bands 300.0 to 348.0 MHz, 387.0 to 464.0 MHz and 779.0 to 928.0 MHz.

### LR1121：

Sets carrier frequency. Allowed values are in range from 150.0 to 960.0 MHz, 1900 - 2200 MHz and 2400 - 2500 MHz.

Will automatically perform image calibration if the frequency changes by more than RADIOLIB_LR11X0_CAL_IMG_FREQ_TRIG MHz.

When switching between sub-GHz and high-frequency bands, after changing the frequency, setOutputPower() must be called in order to set the correct power amplifier!

### NRF24L01：

Sets carrier frequency. Allowed values range from 2400 MHz to 2525 MHz.

### ST32R2916：

