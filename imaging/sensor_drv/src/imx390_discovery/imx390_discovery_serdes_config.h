/*
* The MIT License (MIT)
*
* Copyright (C) 2024 Define Design Deploy Corp.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the “Software”), to deal in
* the Software without restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
* Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef IMX390_DISCOVERY_SERDES_CONFIG_H_
#define IMX390_DISCOVERY_SERDES_CONFIG_H_

/**
7-bit Alias addresses for sensor and serializer
Physical addresses must be programmed in UB96x config
SoC will communicate with the devices using alias adresses 
*/

//#define FUSION_BOARD_VER 0 //RevB and older
#define FUSION_BOARD_VER 1 //RevC

#define IMX390_CM_I2C_ADDR 0x42
#define IMX390_I2C_ADDR 0x34
#define IMX390_DISCOVERY_I2C_ADDR 0x42
// 7bit address is 0x70, the 8b version is E0
#define IMX390_PCAL_ADDRESS 0xE0
#define IMX390_D3_SER_CFG_SIZE    (10U)
#define IMX390_DISCOVERY_SER_CFG_SIZE (10U)

#endif /* _IMX390_SERDES_H_ */


