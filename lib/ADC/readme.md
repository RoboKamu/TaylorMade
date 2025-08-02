ADC
-----

Using adc scan mode with continous enable for 5 ADC channels. Sending data to DMA.

### Deciding factors
Power signal is around 50 Hz. Therefore, the importance is:
1. Accuracy for correct Power calculations 
2. Fast channel scanning for low phase error
    
#### Accuracy 
Accuracy is achieved by setting ADC RCU prescaler to 8. With a core clock of 96 MHz this gives: 

96 MHz / 8 = 12 MHz ADC clock (provides minimum of 12-bit reolution)

Furthermore the cycles per conversoin is set to 239.5, the maximum. This is then combined with external pull down resistors (1k Ohm) for consistent ADC values of 0 when free and 4095 when connected to 3.3V

#### Fast scanning
239.5 cycles per conv. => 239.5 * (1 / 12 MHz ) ≈ 20 microseconds / channel

signal period of 20ms => 20us / 20ms = 0.0001 = 0.1% phase error per channel (max 0.5% on last channel). 

#### Further improvements (not implemented)
Improvements of phase error can be make with parallell sampling. Using one channel (ADC0) for voltage and 4 others (ADC1). Each channel on ADC1 converts in parallel with ADC0 then.
