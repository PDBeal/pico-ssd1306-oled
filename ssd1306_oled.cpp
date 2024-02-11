/*!
* @file ssd1306_oled.cpp
* @brief   OLED driven by SSD1306 controller, Source file
* @author Gavin Lyons.
* @details <https://github.com/gavinlyonsrepo/SSD1306_OLED_RPI>
*
* @remarks Adapted for RP2040 using pico-sdk by PDBeal
*/

#include "ssd1306_oled.h"

/*!
	@brief init the screen object
	@param oledwidth width of OLED in pixels 
	@param oledheight height of OLED in pixels 
*/
SSD1306::SSD1306(int16_t oledwidth, int16_t oledheight) :SSD1306_graphics(oledwidth, oledheight)
{
	_OLED_HEIGHT = oledheight;
	_OLED_WIDTH = oledwidth;
	_OLED_PAGE_NUM = (_OLED_HEIGHT/8); 
	bufferWidth = _OLED_WIDTH;
	bufferHeight = _OLED_HEIGHT;
}

/*!
	@brief  begin Method initialise OLED
	\param i2c_instance - Either i2c0 or i2c1
	@param address by default 0x3C
*/
void SSD1306::OLEDbegin(i2c_inst *i2c_instance, uint16_t address)
{
	this->i2CInst = i2c_instance;
	this->address = address;
	
	//Initialize the OLED display
	OLEDinit();
}

/*!
	@brief sets the buffer pointer to the users screen data buffer
	@param width width of buffer in pixels
	@param height height of buffer in pixels
	@param pBuffer the buffer array which decays to pointer
	@param sizeOfBuffer size of buffer
	@return Will return true for success false for failures : ,
		Buffer size calculations are incorrect BufferSize = w * (h/8),
		or not a valid pointer object.
*/
bool SSD1306::OLEDSetBufferPtr(uint8_t width, uint8_t height , uint8_t* pBuffer, uint16_t sizeOfBuffer)
{
	if(sizeOfBuffer !=  width * (height/8))
	{
		printf("Error OLEDSetBufferPtr 1: buffer size does not equal : width * (height/8))\n");
		return false;
	}
	OLEDbuffer = pBuffer;
	if(OLEDbuffer ==  nullptr)
	{
		printf("Error OLEDSetBufferPtr 2: Problem assigning buffer pointer, not a valid pointer object\r\n");
		return false;
	}
	return true;
}

/*! 
	@brief Disables  OLED Call when powering down
*/
void SSD1306::OLEDPowerDown(void)
{
	OLEDEnable(0);
}

/*!
	@brief Called from OLEDbegin carries out Power on sequence and register init
*/
void SSD1306::OLEDinit()
 {
	SSD1306_command(SSD1306_DISPLAY_OFF);
	SSD1306_command(SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO);
	SSD1306_command(0x80);
	SSD1306_command(SSD1306_SET_MULTIPLEX_RATIO );
	SSD1306_command(_OLED_HEIGHT - 1 );
	SSD1306_command(SSD1306_SET_DISPLAY_OFFSET );
	SSD1306_command(0x00);
	SSD1306_command(SSD1306_SET_START_LINE);
	SSD1306_command(SSD1306_CHARGE_PUMP );
	SSD1306_command(0x14);
	SSD1306_command(SSD1306_MEMORY_ADDR_MODE );
	SSD1306_command(0x00);  //Horizontal Addressing Mode is Used
	SSD1306_command(SSD1306_SET_SEGMENT_REMAP| 0x01);
	SSD1306_command(SSD1306_COM_SCAN_DIR_DEC );

switch (_OLED_HEIGHT)
{
	case 64: 
		SSD1306_command( SSD1306_SET_COM_PINS );
		SSD1306_command( 0x12 );
		SSD1306_command( SSD1306_SET_CONTRAST_CONTROL );
		SSD1306_command(0xCF);
	break;
	case 32: 
		SSD1306_command( SSD1306_SET_COM_PINS );
		SSD1306_command( 0x02 );
		SSD1306_command( SSD1306_SET_CONTRAST_CONTROL );
		SSD1306_command(0x8F);
	break;
	case 16: // NOTE: not tested, lacking part.
		SSD1306_command( SSD1306_SET_COM_PINS );
		SSD1306_command( 0x2 );
		SSD1306_command( SSD1306_SET_CONTRAST_CONTROL );
		SSD1306_command(0xAF);
	break;
}

	SSD1306_command( SSD1306_SET_PRECHARGE_PERIOD );
	SSD1306_command( 0xF1 );
	SSD1306_command( SSD1306_SET_VCOM_DESELECT );
	SSD1306_command( 0x40 );
	SSD1306_command( SSD1306_DISPLAY_ALL_ON_RESUME );
	SSD1306_command( SSD1306_NORMAL_DISPLAY );
	SSD1306_command( SSD1306_DEACTIVATE_SCROLL );
	SSD1306_command( SSD1306_DISPLAY_ON );
}

/*!
	@brief Turns On Display
	@param bits   1  on , 0 off
*/
void SSD1306::OLEDEnable(uint8_t bits)
{
	bits ? SSD1306_command(SSD1306_DISPLAY_ON) : SSD1306_command(SSD1306_DISPLAY_OFF);
}

/*!
	@brief Adjusts contrast
	@param contrast 0x00 to 0xFF , default 0x80
*/
void SSD1306::OLEDContrast(uint8_t contrast)
{
	SSD1306_command( SSD1306_SET_CONTRAST_CONTROL );
	SSD1306_command(contrast);
}

/*!
	@brief invert the display
	@param value true invert , false normal
*/
void SSD1306::OLEDInvert(bool value)
{
	value ? SSD1306_command( SSD1306_INVERT_DISPLAY ) : SSD1306_command( SSD1306_NORMAL_DISPLAY );
}

/*!
	@brief Fill the screen NOT the buffer with a datapattern
	@param dataPattern can be set to zero to clear screen (not buffer) range 0x00 to 0ff
	@param delay in milliseconds can be set to zero normally.
*/
void SSD1306::OLEDFillScreen(uint8_t dataPattern, uint8_t delay)
{
	for (uint8_t row = 0; row < _OLED_PAGE_NUM; row++)
	{
		SSD1306_command( 0xB0 | row);
		SSD1306_command(SSD1306_SET_LOWER_COLUMN);
		SSD1306_command(SSD1306_SET_HIGHER_COLUMN);
		for (uint8_t col = 0; col < _OLED_WIDTH; col++)
		{
			SSD1306_data(dataPattern);
		}
	}
}

/*!
	@brief Fill the chosen page(1-8)  with a datapattern
	@param page_num chosen page (1-8)
	@param dataPattern can be set to 0 to FF (not buffer)
	@param mydelay optional delay in milliseconds can be set to zero normally.
*/
void SSD1306::OLEDFillPage(uint8_t page_num, uint8_t dataPattern,uint8_t mydelay)
{
	uint8_t Result =0xB0 | page_num; 
	SSD1306_command(Result);
	SSD1306_command(SSD1306_SET_LOWER_COLUMN);
	SSD1306_command(SSD1306_SET_HIGHER_COLUMN);
	uint8_t numofbytes = _OLED_WIDTH;
	for (uint8_t i = 0; i < numofbytes; i++)
	{
		SSD1306_data(dataPattern);
	}
}

/*!
	@brief Draw a bitmap  to the buffer 
	@param x x axis offset 0-128
	@param y y axis offset 0-64
	@param w width 0-128
	@param h height 0-64
	@param data pointer to bitmap data
	@param invert color
	@return OLED_Return_Codes_e
	@note bitmap data must be horizontally addressed.
*/
OLED_Return_Codes_e  SSD1306::OLEDBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t* data, bool invert)
{

	// User error checks
	// 1. Completely out of bounds?
	if (x > _width || y > _height)
	{
		printf("Error drawBitmap 1: Bitmap co-ord out of bounds, check x and y\r\n");
		return OLED_BitmapScreenBounds ;
	}
	// 2. bitmap weight and height
	if (w > _width || h > _height)
	{
		printf("Error drawBitmap 2: Bitmap is larger than screen, check w and h\r\n");
		return OLED_BitmapLargerThanScreen;
	}
	// 3. bitmap is null
	if(data== nullptr)
	{
		printf("Error drawBitmap 3: Bitmap is is not valid pointer\r\n");
		return OLED_BitmapNullptr;
	}

	// 4.check bitmap width size
	if(w % 8 != 0)
	{
		printf("Error drawBitmap 4: Bitmap width size is incorrect must be divisible evenly by 8: %u\r\n", w);
		return OLED_BitmapHorizontalSize;
	}

	int16_t byteWidth = (w + 7) / 8; 
	uint8_t byte = 0;
	uint8_t color, bgcolor;
	if (invert == false)
	{
		color = WHITE;
		bgcolor = BLACK;
	}else
	{
		color = BLACK;
		bgcolor = WHITE;
	}

	for (int16_t j = 0; j < h; j++, y++) 
	{
		for (int16_t i = 0; i < w; i++) 
		{
			if (i & 7)
				byte <<= 1;
			else
				byte = data[j * byteWidth + i / 8];
				
			drawPixel(x + i, y, (byte & 0x80) ? color : bgcolor );
		}
	}
	return OLED_Success;
}

/*!
	@brief Writes a byte to I2C address, command or data, used internally
	@param value write the value to be written
	@param cmd command or data
*/
void SSD1306::I2C_Write_Byte(unsigned char value, unsigned char cmd)
{
	uint8_t buffer[2] = { cmd, value };
	i2c_write_blocking(this->i2CInst, this->address, buffer, 2, false); 
}

/*!
	@brief updates the buffer i.e. writes it to the screen
*/
void SSD1306::OLEDupdate()
{
	uint8_t x = 0; uint8_t y = 0; uint8_t w = this->bufferWidth; uint8_t h = this->bufferHeight;
	//OLEDBufferScreen( x,  y,  w,  h, (uint8_t*) this->OLEDbuffer); TODO
	OLEDBufferScreen( x,  y,  w,  h, this->OLEDbuffer);
}

/*!
	@brief clears the buffer memory i.e. does NOT write to the screen
*/
void SSD1306::OLEDclearBuffer()
{
	memset( this->OLEDbuffer, 0x00, (this->bufferWidth * (this->bufferHeight /8)));
}

/*!
	@brief Draw a bitmap directly to the screen
	@param x x axis  offset 0-128
	@param y y axis offset 0-64
	@param w width 0-128
	@param h height 0-64
	@param data the buffer data
	@note Called by OLEDupdate internally 
*/
void SSD1306::OLEDBufferScreen(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t* data)
{
	uint8_t tx, ty;
	uint16_t offset = 0;
		
	SSD1306_command( SSD1306_SET_COLUMN_ADDR );
	SSD1306_command(0);   // Column start address (0 = reset)
	SSD1306_command( _OLED_WIDTH-1 ); // Column end address (127 = reset)

	SSD1306_command( SSD1306_SET_PAGE_ADDR );
	SSD1306_command(0); // Page start address (0 = reset)
	
	switch (_OLED_HEIGHT)
	{
		case 64: SSD1306_command(7); break;
		case 32: SSD1306_command(3); break;
		case 16: SSD1306_command(1); break;
	}
	
	for (ty = 0; ty < h; ty = ty + 8)
		{
		if (y + ty < 0 || y + ty >= _OLED_HEIGHT) {continue;}
		for (tx = 0; tx < w; tx++)
		{

			if (x + tx < 0 || x + tx >= _OLED_WIDTH) {continue;}
			offset = (w * (ty /8)) + tx;
			SSD1306_data(data[offset++]);
		}
	}

}

/*!
	@brief Draws a Pixel to the screen overides the gfx lib if defined
	@param x x axis  position  
	@param y y axis  position
	@param color color of pixel.
*/
void SSD1306::drawPixel(int16_t x, int16_t y, uint8_t color)
{

	if ((x < 0) || (x >= this->bufferWidth) || (y < 0) || (y >= this->bufferHeight)) {
	return;
	}
	int16_t temp;
	uint8_t rotation = getRotation();
	switch (rotation) {
	case 1:
		temp = x;
		x = WIDTH - 1 - y;
		y = temp;
	break;
	case 2:
		x = WIDTH - 1 - x;
		y = HEIGHT - 1 - y;
	break;
	case 3:
		temp = x;
		x = y;
		y = HEIGHT - 1 - temp;
	break;
	}
		uint16_t tc = (bufferWidth * (y /8)) + x;
		switch (color)
		{
			case WHITE:  this->OLEDbuffer[tc] |= (1 << (y & 7)); break;
			case BLACK:  this->OLEDbuffer[tc] &= ~(1 << (y & 7)); break;
			case INVERSE: this->OLEDbuffer[tc] ^= (1 << (y & 7)); break;
		}
}

/*!
	@brief Scroll OLED data to the right
	@param start start position
	@param stop stop position 
*/
void SSD1306::OLEDStartScrollRight(uint8_t start, uint8_t stop) 
{
	SSD1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
	SSD1306_command(0X00);
	SSD1306_command(start);  // start page
	SSD1306_command(0X00);
	SSD1306_command(stop);   // end page
	SSD1306_command(0X00);
	SSD1306_command(0XFF);
	SSD1306_command(SSD1306_ACTIVATE_SCROLL);
}

/*!
	@brief Scroll OLED data to the left
	@param start start position
	@param stop stop position 
*/
void SSD1306::OLEDStartScrollLeft(uint8_t start, uint8_t stop) 
{
	SSD1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
	SSD1306_command(0X00);
	SSD1306_command(start);
	SSD1306_command(0X00);
	SSD1306_command(stop);
	SSD1306_command(0X00);
	SSD1306_command(0XFF);
	SSD1306_command(SSD1306_ACTIVATE_SCROLL);
}

/*!
	@brief Scroll OLED data diagonally to the right
	@param start start position
	@param stop stop position 
*/
void SSD1306::OLEDStartScrollDiagRight(uint8_t start, uint8_t stop) 
{
	SSD1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
	SSD1306_command(0X00);
	SSD1306_command(_OLED_HEIGHT);
	SSD1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
	SSD1306_command(0X00);
	SSD1306_command(start);
	SSD1306_command(0X00);
	SSD1306_command(stop);
	SSD1306_command(0X01);
	SSD1306_command(SSD1306_ACTIVATE_SCROLL);
}

/*!
	@brief Scroll OLED data diagonally to the left
	@param start start position
	@param stop stop position 
*/
void SSD1306::OLEDStartScrollDiagLeft(uint8_t start, uint8_t stop) 
{
	SSD1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
	SSD1306_command(0X00);
	SSD1306_command(_OLED_HEIGHT);
	SSD1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
	SSD1306_command(0X00);
	SSD1306_command(start);
	SSD1306_command(0X00);
	SSD1306_command(stop);
	SSD1306_command(0X01);
	SSD1306_command(SSD1306_ACTIVATE_SCROLL);
}

/*!
	@brief  Stop scroll mode
*/
void SSD1306::OLEDStopScroll(void) 
{
	SSD1306_command(SSD1306_DEACTIVATE_SCROLL);
}

/*! 
	@brief checks if OLED on I2C bus
	@return 1 = Success (returns number of bytes read)
*/ 
uint8_t SSD1306::OLEDCheckConnection(void)
{
	uint8_t rxdata; //buffer to hold return byte

	return i2c_read_blocking(this->i2CInst, this->address, &rxdata, 1, false); // returns number if bytes read
}
