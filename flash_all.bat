@echo off
echo --- Flashing Bootloader and Application ---

set PRJ_1=BUILD\project1\project1.bin
set FlashTool=tools\stm32-programmer\bin\STM32_Programmer_CLI.exe
set PRJ_1_ADDRESS=0x08000000
"%FlashTool%" -c port=SWD ^
    -d "%PRJ_1%"  "%PRJ_1_ADDRESS%" ^
    -rst

echo --- Done ---

@REM cmake --build BUILD --target flash_all  
