# ese-ep5-mobile-robot
Conestoga College, Electronic Systems Engineering, Engineering Project 5 codebase. Mobile robot with wireless communication.

# Setup
1. Set up STM32-base
    a. https://stm32-base.org/guides/setup
    b. https://github.com/STM32-base
    c. Choose the F3 example project
2. Make the following changes to STM32-base/make/common.mk
    a. Line 148: $(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ 
    b. Line 151: $(CC) -c $(CPPFLAGS) $(CFLAGS) $^ -o $@

## Build
1. Run 'make' to compile to project
2. Run 'make clean' to compile project from scratch
3. Run 'make flash' to flash the device
