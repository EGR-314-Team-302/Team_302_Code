

#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/examples/i2c2_master_example.h"
#include "mcc_generated_files/i2c2_master.h"
#include <xc.h>
#define SPI1_h


// Define the variables
uint8_t dir1 = 0b11011111;
uint8_t dir2 = 0b11011101;
uint8_t data;
uint8_t ReadPress;
uint8_t WritePress;
uint8_t ReadTemp;
uint8_t WriteTemp;
uint16_t StoreTemp;


float error;
float DESIRED_OUTPUT=116.0;
float pidOutput;
 float Kp=0.1;//FASTER RESPONSE ERROR-proportional gain
  float Ki=0.01;//REDUCTION STEADY-STATE ERROR-integral gain
  float Kd=0.001;//REDUCTION OVERSHOOT & OSCILLATION-derivative gain
  
  float convertedValue;
 uint16_t conversionResult;
    float adcVoltage = 0.0; 
    
    
//Define the functions
void readTempSensor() //Temperature Reading
{
    //Write Temp data to register
  I2C2_Write1ByteRegister(0x4C, 0x0,0x0);
  //Read Temp data from Register
        ReadTemp = I2C2_Read1ByteRegister(0x4C, 0x00);
        //Store Temp data 
        StoreTemp=ReadTemp;
       if(EUSART2_is_tx_ready()) 
       {
           printf("temp=%d C\r\n",StoreTemp);
                    
            __delay_ms(1000); //delay 1/10 of a second*/
       }
}

void setActuatorSpeed(void) //Actuator Movement
{
    
  slave1_SetLow();

        data = SPI1_ExchangeByte(dir1);
        __delay_ms(1000);
        slave1_SetHigh();
        __delay_ms(1000);

        slave1_SetLow();

        data = SPI1_ExchangeByte(dir2);
        __delay_ms(1000);
        slave1_SetHigh();

        
        
        if(EUSART2_is_tx_ready())
        {
            printf("Value=%d \b \r \n", dir1);
        __delay_ms(100);
        }

        RC4_SetLow();

        data = SPI1_ExchangeByte(dir2);
        __delay_ms(1500);
        RC4_SetHigh();

        printf("Value=%d \b \r \n", dir2);
        __delay_ms(100);
    
}
void readPressureSensor()//Pressure Reading
{
    I2C2_Write1ByteRegister(0x66, 0xFF,0x0);
        //ReadPress = I2C2_Read1ByteRegister(0x66, 0x01);
        ReadPress = ADCC_GetSingleConversion(channel_ANA0);
        adcVoltage = ReadPress * (5.0/1023.0);
        if(EUSART2_is_tx_ready())
        {
            printf("Pressure=%d \r\n",adcVoltage);
                    __delay_ms(1000);
        }
}

void updatePID() //PID 
{
  // Calculate the PID output and store the value in the pidOutput variable
    pidOutput=Kp*error;
}


void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    ADCC_Initialize();
    EUSART2_Initialize();
     EUSART1_Initialize();
    IOC_SetInterruptHandler(setActuatorSpeed);
    PWM3_Initialize();
    TMR2_Initialize();
   SPI1_Initialize();
    SPI1_Open(SPI1_DEFAULT);

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    while (1)
    {
          // Read the water level sensor
    readTempSensor();
   // Read Pressure Level
  readPressureSensor();

   

   
    // Calculate the error
    error = DESIRED_OUTPUT - (StoreTemp+ReadPress);

    // Update the PID controller
    updatePID();

    // Set the actuator speed
    setActuatorSpeed(pidOutput);

    // Wait for a second
    __delay_ms(1000);
    
    
    
    }
}
/**
 End of File
*/