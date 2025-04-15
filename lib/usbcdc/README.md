

Using the provided debugger libraries provided by the toolchain and trying to migrate it to a working project directory provided issues. This library is from an open source project by [linusreM](https://github.com/linusreM), who has examples on [other boards](https://github.com/linusreM/Longan-RISC-V-examples/tree/master/07-USB-serial) like [logan nano](https://github.com/linusreM/GD32V-Virtual-COM-Port) using USB Serial on GD32VF103. 

The used library in this project is the minimal USB communication template [GD32V-USB-serial-printf](https://github.com/linusreM/GD32V-USB-serial-printf/tree/main?tab=readme-ov-file).

The open souce project used from [linusreM](https://github.com/linusreM) was based on the [original firmware library](https://github.com/riscv-mcu/GD32VF103_Firmware_Library). 

----
<br>

----

### Extra note about CLOCK 
The USB clock has to be configured as 48MHz according to the user manual. Read more in linusreM projects for more detail aswell as the user manual.

Function that defines this in `usb_serial_if.c`: 

    set_usb_clock_96m_hxtal();


Clock can also be configure as: 

In `system_gd32vf103.c`

    ...
    60 | //#define __SYSTEM_CLOCK_72M_PLL_HXTAL       (uint32_t)(72000000)
    61 | #define __SYSTEM_CLOCK_96M_PLL_HXTAL       (uint32_t)(96000000)
    62 | //#define __SYSTEM_CLOCK_108M_PLL_HXTAL      (uint32_t)(108000000)
    ...

With a system clock of 96 MHz it gets initialized in `main.c` with `configure_usb_serial()`, here `usb_rcu_config()` get called which configures the USB clock. 

in `gd32vf103_hw.c`

    void usb_rcu_config(void)
    {
        uint32_t system_clock = rcu_clock_freq_get(CK_SYS);
        if...
        ...
        } else if (system_clock == 96000000) {
            usbfs_prescaler = RCU_CKUSB_CKPLL_DIV2;
            timer_prescaler = 7;
        ...
        rcu_usb_clock_config(usbfs_prescaler);
        rcu_periph_clock_enable(RCU_USBFS);
    }

Now the USB Clock is correctly configured as 48MHz as the user manual 21.3 block diagram. 
