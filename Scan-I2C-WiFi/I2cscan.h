
/* ======================================================================
  Function: i2cScan
  Purpose : scan I2C bus
  Input   : specifc address if looking for just 1 specific device
  Output  : number of I2C devices seens
  Comments: -
  ====================================================================== */
uint8_t i2c_scan(uint8_t address = 0xff)
{
  uint8_t error;
  int nDevices;
  uint8_t start = 1 ;
  uint8_t end   = 0x7F ;
  uint8_t index = 0;
  char device[16];
  char buffer[32];

  if (address >= start && address <= end) {
    start = address;
    end   = address + 1;
    Serial.print(F("Searching for device at address 0x"));
    Serial.printf("%02X ", address);
  } else {
    Serial.println(F("Scanning I2C bus ..."));
  }

  nDevices = 0;
  for (address = start; address < end; address++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.printf("Device ");

      if (address == 0x40)
        strcpy(device, "TH02" );
      else if (address == 0x29 || address == 0x39 || address == 0x49)
        strcpy(device, "TSL2561" );

      else if (address==0x50) {
        strcpy(device, "24AA02E64" );
        // This device respond to 0x50 to 0x57 address
        address+=0x07;
      }
      else if (address == 0x55 )
        strcpy(device, "BQ72441" );
      else if (address == I2C_DISPLAY_ADDRESS || address == I2C_DISPLAY_ADDRESS + 1)
        strcpy(device, "OLED SSD1306" );
      else if (address >= 0x60 && address <= 0x62 ) {
        strcpy(device, "MCP4725_Ax" );
        device[9]= '0' + (address & 0x03);
      } else if (address >= 0x68 && address <= 0x6A ) {
        strcpy(device, "MCP3421_Ax" );
        device[9]= '0' + (address & 0x03);
      } else if (address == 0x64)
        strcpy(device, "ATSHA204" );
      else
        strcpy(device, "Unknown" );

      sprintf(buffer, "0x%02X : %s", address, device );
      if (index < I2C_DISPLAY_DEVICE) {
        strcpy(i2c_dev[index++], buffer );
      }

      Serial.println(buffer);
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.printf("Unknow error at address 0x%02X", address);
    }

    yield();
  }
  if (nDevices == 0)
    Serial.println(F("No I2C devices found"));
  else
    Serial.printf("Scan done, %d device found\r\n", nDevices);

  return nDevices;
}
