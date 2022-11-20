#include <stdio.h>

#include <p18f4620.h>
#include "I2C_Support.h"
#include "I2C_Soft.h"

#define ACCESS_CFG      0xAC
#define START_CONV      0xEE
#define READ_TEMP       0xAA
#define CONT_CONV       0x02
#define ACK     1
#define NAK     0


extern unsigned char second, minute, hour, dow, day, month, year;
extern unsigned char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
extern unsigned char alarm_second, alarm_minute, alarm_hour, alarm_date;
extern unsigned char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;


void DS1621_Init() //Initialize the device
{
    char Device = 0x48; //Determine which device is going to be used.
    I2C_Write_Cmd_Write_Data (Device, ACCESS_CFG, CONT_CONV); 
    I2C_Write_Cmd_Only(Device, START_CONV);
}


int DS1621_Read_Temp() //Code provided by professor: Reading the temperature off of the Ds1621 device 
{
    char Device = 0x48; //Indicate which device
    char Cmd = READ_TEMP;
    char Data_Ret;
    I2C_Start(); //start the device 
    I2C_Write((Device<<1)|0); // Device address Write mode
    I2C_Write(Cmd); //write cmd 
    I2C_ReStart();  
    I2C_Write((Device<<1)|1); //Write device address read mode
    Data_Ret = I2C_Read(NAK); //Nak to indicate we are done
    I2C_Stop(); //stop the device 
    return Data_Ret; //Return the data from the reading

}

void DS3231_Read_Time() 
{
    char Device = 0x68; //Indicate which device we are using
    char Address = 0x00; //Se address to 0
    I2C_Start(); //Start the device
    I2C_Write((Device<<1)|0); //Write device address write mode
    I2C_Write(Address); //write the address
    I2C_ReStart(); //Start again
    I2C_Write((Device<<1)|1); //Write device address read mode
    second = I2C_Read(ACK); //Read Second
    minute = I2C_Read(ACK); //Read minute
    hour = I2C_Read(ACK); // read hour
    dow = I2C_Read(ACK); //read the day of the week
    day = I2C_Read(ACK); //read the day
    month = I2C_Read(ACK); //read the month
    year = I2C_Read(NAK); //read the year. NAK to indicate to slaves that this is the end of bitstream
    I2C_Stop(); //stop the device]
}

void DS3231_Setup_Time() //set up the time on the ds3231 device
{
    char Device = 0x68; //indicate whic device to use
    char Address = 0x00; //set the address
    second = 0x06; //initialize second with 6
    minute = 0x25; // Initialzie minute with 25
    hour = 0x21; //set the hour to 21 (military time))
    dow = 0x05; // set the day of the week to 5, thursday 
    day = 0x21; //Set the date to 21
    month = 0x04; //Set the month to 4, april
    year = 0x22; //Set the year to 22
    I2C_Start(); //start the device
    I2C_Write((Device<<1)|0); //Write device address write mode
    I2C_Write(Address); //Write onto the device the initial address
    I2C_Write(second); //write onto the device the initial second
    I2C_Write(minute); //write onto the device the initial minute
    I2C_Write(hour); //write onto the device the initial hour
    I2C_Write(dow); //write onto the device the initial day of the week
    I2C_Write(day); // write onto the device the initial day
    I2C_Write(month); //write onto the device the initial month
    I2C_Write(year);//write onto the device the initial year
    I2C_Stop(); //stop the writing onto the device.
    
}

