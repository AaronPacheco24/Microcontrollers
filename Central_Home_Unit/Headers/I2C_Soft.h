void I2C_Init(const unsigned long);
void I2C_Wait(void);
void I2C_Start(void);
void I2C_ReStart(void);
void I2C_Stop(void);
char I2C_Read(char);
void I2C_Write(unsigned char);
void I2C_Write_Cmd_Only(char, char);  
void I2C_Write_Cmd_Write_Data(char, char, char);
void I2C_Write_Address_Write_One_Byte(char, char, char);    
BYTE I2C_Write_Address_Read_One_Byte(char, char);
 
void i2cHighSda(void);
void i2cLowSda(void);
void i2cHighScl(void);
void i2cLowScl(void);
void i2cNack(void);
void i2cAck(void);

#define SCL_PIN PORTDbits.RD2// PIN is port
#define SCL_DIR TRISDbits.RD2// DIR is a tris
#define SDA_PIN PORTDbits.RD3// PIN is a port
#define SDA_DIR TRISDbits.RD3// DIR is a tris
//All functios for the I2C_Soft C file
