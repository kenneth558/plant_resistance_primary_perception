//        Before compiling this sketch, you must set or confirm the following appropriately for your configuration and preferences !!!
#define NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG 2                                                     //The number of consecutive analog pins to plot, beginning with PIN_A0
#define NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC 1                                            //The number of consecutive "highest-sensitivity ADC" pins to plot, beginning with A0 and, if double-ended, A1.  ADDON ADC ONLY - DOES _NOT_ INCLUDE INBOARD ANALOG INPUT PINS
#define HIGHEST_SENSI_ADDON_ADC_TYPE HX711                                                         //Proposing that "ADS1231" covers ADS1231; could make this "ADS1232" (ADS1232), "ADS1242" (ADS1242), "AD779x" (AD779x), "AD7780" (AD7780), "HX711" (HX711), "MAX112x0" (MAX112x0...) or "LTC2400" (LTC2400) but code not included in v.FREE
#define MAGNIFICATION_FACTOR 100                                                                   //To aid in viewing
#define HighestBitResFromHighestSensiAddonADC 24                                                   //All ADC values will get scaled to the single-ended aspect of this,  15 is ADS1115 single-ended, 16 for double-ended when two LM334s are used.  change to 11 for ADS1015 single-ended or 12 with two LM334s, (future: change to 24 for HX711--NO b/c there is the ADS1231 at 24 bits)
#define SAMPLE_TIMES 4                                                                             //To better average out artifacts we over-sample and average.  This value can be tweaked by you to ensure neutralization of power line noise or harmonics of power supplies, etc.....
#define MOST_PROBLEMATIC_INTERFERENCE_FREQ 60                                                      //This is here just in case you think that you might have some interference on a known frequency.
#define DELAY_TIME_BETWEEN_SAMPLES_MS ( 1000 / MOST_PROBLEMATIC_INTERFERENCE_FREQ / SAMPLE_TIMES ) //COARSE ADJUST
#define DELAY_TIME_BETWEEN_SAMPLES_US ( ( ( 1000000 / MOST_PROBLEMATIC_INTERFERENCE_FREQ ) - ( DELAY_TIME_BETWEEN_SAMPLES_MS * SAMPLE_TIMES * 1000 ) ) / SAMPLE_TIMES ) //FINE ADJUST.  THIS GETS ADDED TO COARSE ADJUST, PRECISION = TRUNCATED PRAGMATICALLY TO uSec TO ACKNOWLEDGE SOME OVERHEAD FOR LOOPING SUPPORT CODE   // End of this part of code update
#define FIRST_ANALOG_PIN_DIGITAL_NUMBER_FOR_BOARDS_NOT_HAVING_ANALOG_PINS_DEFINED_BY_PIN_A0_TYPE_DEFINES 14 //Some boards don't have good definitions and constants for the analog pins :-(
#define REPOSITION_RATIO_OF_MAGNIFIED_VIEW_WHEN_LIMITS_GET_EXCEEDED (.1)                           //BETWEEN 0 AND 1 indicating how much of the display region to skip when magnified view trace has to get repositioned because trace would be outside region bounds
#define AnalogInputBitsOfBoard 10                                                                  //Most Arduino boards are 10-bit resolution, but some can be 12 bits.  For known 12 bit boards (SAM, SAMD and TTGO XI architectures), this gets re-defined below, so generally this can be left as 10 even for those boards
#define SECONDS_THAT_A_LGT8FX8E_HARDWARE_SERIAL_NEEDS_TO_COME_UP_WORKING 9                         //8 works only usually
#define HIGHEST_SENSI_PGA_GAIN_FACTOR 128                                                          //For HX711 a gain of 128 gets applied to channel A. Available to you for your own use PGA=Programmable Gain Amplifier: many ADCs will correlate a gain of one with full-scale being rail-to-rail, while a gain of anything higher correlates to full-scale being in the mV range (most sensitive and most noise-susceptible).
#define MIN_WAIT_TIME_BETWEEN_PLOT_POINTS_MS 200                                                   //Sets maximum speed, but actual speed may be further limited by other factors
#define USING_LM334_WITH_MCP4162_POTS                                                              //Remove if using standard wheatstone bridge with only standard resistors.  make true if using bridge with upper resistive elements being LM334s controllable with the MCP4162-104 pots
//#define DEBUG true                                                                               //Don't forget that DEBUG is not formatted for Serial plotter, but might work anyway if you'd never print numbers only any DEBUG print line
//#define LEAVEPOTVALUESALONEDURINGSETUP                                                           //First run should leave this undefined to load digi pots with some values
//OTHER DEFINES OR RE-DEFINES ELSEWHERE: VERSION, NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG, NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC, HALFHighestBitResFromHighestSensiAddonADC, DIFFERENTIAL, PIN_FOR_DATA_TOFROM_HIGHEST_SENSI_ADC, PIN_FOR_CLK_TO_HIGHEST_SENSI_ADC, PlotterMaxScale, HundredthPlotterMaxScale, SAMPLE_TIMES, AnalogInputBitsOfBoard, SCALE_FACTOR_TO_PROMOTE_LOW_RES_ADC_TO_SAME_SCALE


//FUTURE #define DEBUG_BRIDGE_BALANCING true //This will cause both the probe signal and the opposing bridge leg reference point to be displayed in full-scale, non-magnified views.  Useful with boards having low-Z Analog Input pins or none at all.
//FUTURE #define FULL_SCALE_ADDON_ADC_TYPE ADS1X15  // Proposing that "ADS1231" covers ADS1231; could make this "ADS1232" (ADS1232), "ADS1242" (ADS1242), "AD779x" (AD779x), "AD7780" (AD7780), "HX711" (HX711), "MAX112x0" (MAX112x0...) or "LTC2400" (LTC2400) but code not included in v.FREE
/*******************(C)  COPYRIGHT 2018 KENNETH L ANDERSON *********************
* 
*      ARDUINO ELECTRICAL RESISTANCE/CONDUCTANCE MONITORING SKETCH 
*      
* File Name          : adc_for_plant_tissue.ino
* Author             : KENNETH L ANDERSON
* Version            : v.Free
* Date               : 13-July-2018
* Description        : To replicate Cleve Backster's findings that he attributed to a phenomenon he called "Primary Perception".  Basically, this sketch turns an Arduino MCU and optional (recommended) ADS1115 into a nicely functional poor man's polygraph in order to learn/demonstrate telempathic gardening.
* Boards tested on   : Uno using both ADS1115 and inboard analog inputs.  
*                    : TTGO XI using ADS1115.  
*                    : Many other configurations should work fine.  
*                    : Sadly, TTGO XI 12-bit analog inputs are unsuitable due to their irremoveable pullup conductance.  
*                    : The ATTINY85 is not suitable at all due to not having hardware serial
* 
* Known limitations  : No ability to accept user input from keyboard during run time due to Arduino plottter limitation
*                    : Re-compile is needed for any changes to configuration
*                    : Analog input pins being used MUST be the first available analog inputs
*                    : Only a single add-on ADS ADC device and only 1 or 2 differential channels on it is accommodated
*                    : Conventional add-on high-sensitivity ADCs limit their common mode differential input range to a few millivolts with active or passive clamping
*                    : Some of these limitations will be addressed in future not-for-free versions
*                    
********************************************************************************
* THE PRESENT PRODUCT WHICH IS ONLY FOR USE BY SOFTWARE- AND HARDWARE-COMPETENT USERS
* IS INTENDED TO OFFER VARIOUS CAPABILITIES.  BECAUSE ABSOLUTELY NO CONSIDERATION 
* IS EXPECTED IN RETURN, NO CONTRACT, WHETHER OFFERING SUITABILITY, RELIABILITY, 
* FUNCTIONALITY, SAFETY, OR OTHER, EXISTS.  BY USING THIS PRODUCT YOU EXPRESSLY AND
* TOTALLY ACCEPT FULL LIABILITY FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES 
* WITH RESPECT TO ANY CLAIMS ARISING FROM SOFTWARE AND/OR HARDWARE DERIVATIONS OF PRODUCT.
* I MAKE ABSOLUTELY NO CLAIM OF MY OWN COMPETENCE, LICENSURE, CERTIFICATION, EXPERIENCE,
* OR PROFESSIONAL STANDING OTHER THAN BEING AN ELECTRONICS TECHNICIAN BY FORMAL 
* DEGREE AND UNITED STATES FCC RADIOTELEPHONE OPERATORS LICENSE, AND 
* A SOFTWARE DEVELOPER BY LIMITED PRACTICAL EXPERIENCE, AND BEING FORMALLY
* DEGREED WITH BACHELOR OF GENERAL STUDIES MAJORED IN GENERAL SCIENCES
********************************************************************************
*
* TODO:  Arrange for run-time keyboard input to eliminate need to re-compile when changes are made or plotting adjustments are desired.  Will require non-Arduino IDE plotter
*        Use dual tightly thermally coupled LM334s to prevent power supply noise from plotting (i.e., convert from resistance to conductance)  5 VOLT DEVICES ONLY.   A toroid-based noise suppression coil might be needed.  The 2nd LM334 excites a fixed resistor circuit parallel to the plant circuit and connected to a second analog input for software temperature compensation.
*        Use digital potentiometer (MCP41XXX or MCP42XXX) for more automated balancing of the two serial resistances.  The charge pump noise generated by the x9c series rules it out.
*        Accommodate more 24-bit amps like ADS1231
* 
*  These planned enhancements will be reserved for NOT-FOR-FREE sketch versions in the future
*  
*  Changelog:  30 April 2018 :  Final inboard 10 bit operation and ADS1X15 operation has been achieved.  Future work on free version will be for 12 bit boards only
*              04 May   2018 :  Some 12 bit tweaking so that TTGO/WeMos XI, SAM and SAMD architectures compile with inboard analog inputs to computed at 12 bit even though AnalogInputBitsOfBoard is set at default.
*              18 May   2018 :  Anti-aliasing code
*              29 May   2018 :  Differential use of ADS1x15 to allow LM334 temperature comp to negative inputs so LM334 can be used on positive inputs
*              32 May   2018 :  Added skeleton preprocessor defines for user to compatibly code their own support for a few 24-bit ADCs
*              05 June  2018 :  HX711 accommodated, you'll just have to balance the bridge manually
*              08 June  2018 :  Bugfix for inboard analog inputs above the first one - they didn't print before this fix
*              17 June  2018 :  Corrected pointer reinitialize of A_PIN_ARRAY that disabled internal Analog Input pins.  Added "while ( !Serial );" for Leonardo's native USB; starting to add framework for programmable potentiometers, changed defines to allow for a separate full-scale, lower res ADC and a high res ADC for normal viewing, etc,
*              18 June  2018 :  Bug fixes relative to displaying multiple traces while one or more are from inboard analog pins
*              13 July  2018 :  Modified plotter timing trace to notch at range min and max for signal traces.  Incorporated digi pot adjustings in debug mode
*              NEXT          :  Accommodate ADS1232
*              NEXT          :  Made able to use MCP41XXX or MCP42XXX with LM334
*              NEXT          :  Software temperature compensation using a 2nd LM334 tightly thermally coupled to 1st LM334 feeding a fixed resistor circuit in parallel with the plant circuit and connected to a 2nd analog input.  Table of offsets from midpoint or one end of pot settings.
*               
*********************************************************************************************************************/
#define VERSION "v.Free"  // Since this never gets used anywhere, it doesn't compile in so no memory is wasted
#ifdef __LGT8FX8E__
    #if ( NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG > 0 )
//HINT ONLY, NOT ENFORCED:  #error "We are sorry, but the Wemos board's analog inputs have unremoveable pull-up conductance, so this sketch is not designed to compile for the analog inputs to be used with this board.  Set NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG to zero and try with an outboard ADC."
    #endif
#endif
#include <math.h>
#if ( NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC > 0 ) //Since so many of the ADC libraries already use OO classes, we'll set that as a pattern - instantiate prior to executing any code
    #define HALFHighestBitResFromHighestSensiAddonADC ( HighestBitResFromHighestSensiAddonADC / 2 )
    #if ( ( HALFHighestBitResFromHighestSensiAddonADC * 2 ) == HighestBitResFromHighestSensiAddonADC )
        #define DIFFERENTIAL 
    #endif
    #if ( HighestBitResFromHighestSensiAddonADC < 17 ) && ( NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC > 0 )
        #include <Adafruit_ADS1015.h>//for systems using ADS1115/ADS1015  Data addressable I2C, shares bidirectional, open-collector data and clk only with other I2C slaves on same bus, must use the core-defined SDA/SCL pins except ESP8266 and ATtiny85 or see https://www.arduino.cc/en/reference/wire or see the Adafruit_ADS1X15-master README.md
/*  
*
*  With Adafruit_ADS1015.h, SDA, SCL Wemos XI/TTGO XI use A4 for SDA, and A5 is SCL.
*/
        #if ( HighestBitResFromHighestSensiAddonADC == 15 ) || ( HighestBitResFromHighestSensiAddonADC == 16 )
            Adafruit_ADS1115 ads;  //For when ADS1115 is being used Data addressable I2C and shares bidirectional, open-collector data and clk only with other I2C slaves on same bus, must use the core-defined SDA/SCL pins except ESP8266 and ATtiny85 or see https://www.arduino.cc/en/reference/wire or see the Adafruit_ADS1X15-master README.md
        #else
            #if ( HighestBitResFromHighestSensiAddonADC == 11 ) || ( HighestBitResFromHighestSensiAddonADC == 12 )
                Adafruit_ADS1015 ads;  //For when ADS1015 is being used Data addressable I2C and bidirectional, open-collector data and clk only with other I2C slaves on same bus, must use the core-defined SDA/SCL pins except ESP8266 and ATtiny85 or see https://www.arduino.cc/en/reference/wire or see the Adafruit_ADS1X15-master README.md
            #else
                #error “This add-on ADC resolution is not supported in this sketch version."
            #endif
        #endif
    #else
        #define PIN_FOR_DATA_TOFROM_HIGHEST_SENSI_ADC 2 //Must always be dedicated in cases where it doesn't go hi-Z (as in HX711).  This won't work if an ADS1x15 or any other I2C device is also used on this pin
        #define PIN_FOR_CLK_TO_HIGHEST_SENSI_ADC 3 //Must always be dedicated in cases where it doesn't go hi-Z (as in HX711).  This won't work if an ADS1x15 or any other I2C device is also used on this pin
/*
 * If ever would use an ADS1x15 along with this ADC, then see error msg below
        #ifdef ARDUINO_AVR_LEONARDO
            #if ( PIN_FOR_DATA_TOFROM_HIGHEST_SENSI_ADC == 2 ) || ( PIN_FOR_CLK_TO_HIGHEST_SENSI_ADC == 3 ) || ( PIN_FOR_CLK_TO_HIGHEST_SENSI_ADC == 2 ) || ( PIN_FOR_DATA_TOFROM_HIGHEST_SENSI_ADC == 3 )
                #error "The pins being used for clock and data of the ADC conflict with the I2C pins used by ADS1x15.  See https://www.arduino.cc/en/reference/wire and the Adafruit_ADS1X15-master README.md.  Remove this warning once you are satisfied one way or another"
            #endif
        #endif
*/
        #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == HX711 ) && ( HighestBitResFromHighestSensiAddonADC == 24 )  //The HX711 does NOT communicate over open-collector I2C; pick your own pins for comming with it other than SS, CLK, MOSI, and MISO of SPI.  The digital interface to the HX711 is a proprietary SPI-like interface using CLK to select and Data is always lo-z (can't share either)
            #include <HX711.h>  //From https://github.com/bogde/HX711  This ADC has no CS pin so the library must use software SPI with dedicated CLK pin.  Not data selectable as would be in I2C, nor CS selectable - must be on dedicated CLK & Data lines
            HX711 hx711( PIN_FOR_DATA_TOFROM_HIGHEST_SENSI_ADC, PIN_FOR_CLK_TO_HIGHEST_SENSI_ADC ); // This library allows us to set the pins and gain here or later in a .begin().   
        #else
            #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == ADS1232 ) && ( HighestBitResFromHighestSensiAddonADC == 24 )
            #else
                #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == ADS1242 ) && ( HighestBitResFromHighestSensiAddonADC == 24 ) //Which 24-bit ADC is single-ended?  ADS1242 It is 23 bits hopefully
                #else
                    #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == AD7780 ) && ( HighestBitResFromHighestSensiAddonADC == 24 )
                    #else
                        #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == AD779x ) && ( HighestBitResFromHighestSensiAddonADC == 24 )
                        #else
                            #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == MAX112x0 ) && ( HighestBitResFromHighestSensiAddonADC == 24 )
                            #else
                                #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == LTC2400 ) && ( HighestBitResFromHighestSensiAddonADC == 24 )
                                #else
                                    #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == ADS1231 ) && ( HighestBitResFromHighestSensiAddonADC == 24 )
                                    #else
                                        #error “This add-on ADC resolution is not supported in this sketch version."
                                    #endif
                                #endif
                            #endif
                        #endif
                    #endif
                #endif
            #endif
        #endif
    #endif
#else
    #undef HighestBitResFromHighestSensiAddonADC
    #define HighestBitResFromHighestSensiAddonADC 10
#endif
#if ( NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG > 0 )
    #ifndef NUM_ANALOG_INPUTS
Sorry, but you will have to manually define the define variable NUM_ANALOG_INPUTS somewhere above this line and re-compile...
    #endif
#else
    #if ( NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC == 0 )
You'll need to manually define at least one of the variables NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG or NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC where they appear in the lines above and re-compile...
If you only have the Arduino without an ADS1X15, then define NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG.  Otherwise, define NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC and/or both of them.
    #endif
#endif
#ifdef USING_LM334_WITH_MCP4162_POTS
/*
 Using a Microchip MCP4162 digital potentiometer 
    [http://bit.ly/iwDmnd]
    http://tronixstuff.com/tutorials > chapter 34 | CC by-sa-nc | John Boxall

    SS – digital 10 to CS of a single digital pot, not abstracted by library call, so can be any pin as determined within main function unidirectional NOT PIN 10 ON LEONARDO - IS FOUND ON LED_BUILTIN_RX (PIN 17) ONLY
    MOSI – digital 11 to inputs of all digital pots unidirectional NOT PIN 11 ON LEONARDO - IS FOUND ON 1CSP ONLY.  REQ'D IN ALL CASES.
    MISO – digital 12 to outputs of all digital pots unidirectional NOT PIN 12 ON EXCEPT LEONARDO - IS FOUND ON 1CSP ONLY.  OPTIONAL IN SOME CASES.
    SCK – digital 13 to CLK of all digital pots unidirectional NOT PIN 13 ON LEONARDO - IS FOUND ON 1CSP ONLY

*/
    #include "SPI.h" //This is the hardware implementatino of SPI with CS pin selectable devices, not addressable via data and unidirectional data in and data out pins.  The software implementation uses shiftIn() instead and is slower but flexible which pins get used and can be used adjunctive for cases where a chip has no CS pin.  Adafruit_ADS1015 uses the shiftIn() method, so those pins are user's choice.  Not data addressable as would be in I2C
    #define DIGITAL_POT_1 5  // first digital pot's CS line connected to here. coarse adjust A positive leg
    #define DIGITAL_POT_2 6  // second digital pot's CS line connected to here.  coarse adjust B positive leg
    #define DIGITAL_POT_3 7  // third digital pot's CS line connected to here.  fine adjust positive leg
    #define DIGITAL_POT_4 8  // fourth digital pot's CS line connected to here.  coarse adjust A negative leg
    #define DIGITAL_POT_5 9  // fifth digital pot's CS line connected to here.  coarse adjust B negative leg
    #define DIGITAL_POT_6 10 // sixth digital pot's CS line connected to here.  fine adjust negative leg
    #ifdef ARDUINO_AVR_LEONARDO
        #if ( DIGITAL_POT_1 == 2 ) || ( DIGITAL_POT_1 == 3 ) || ( DIGITAL_POT_2 == 2 ) || ( DIGITAL_POT_2 == 3 )
            #error "The pins being used for clock and data of the ADC conflict with the I2C pins used by ADS1x15.  See https://www.arduino.cc/en/reference/wire and the Adafruit_ADS1X15-master README.md.  Remove this warning once you are satisfied one way or another"
        #endif
    #endif
    #define STARTVALUE 29  //this value in all digipots and with 1 MOhm resistors for the LM334 loads put the LM334 output voltage at closest to half Vdd
    uint16_t digipot_1_value = STARTVALUE;  // Storing a copy of the digi pot values in sketch makes it possible to eliminate the MISO pin connection to the data out pins of all digi pots, besides the digi pots data out may not conform strictly to SPI since the MCP41X1 version has one bidirectional data pin
    uint16_t digipot_2_value = STARTVALUE;
    uint16_t digipot_3_value = STARTVALUE;
    uint16_t digipot_4_value = STARTVALUE;
    uint16_t digipot_5_value = STARTVALUE;
    uint16_t digipot_6_value = STARTVALUE;
    #define MAXPOTVALUE 257
#endif
/*
 * 
 * The following pin defines are for the WeMos XI/TTGO XI board only
 * Change per your board layout if you need some non-standard Arduino 10-bit analog inputs.  Uno et. al. should work just fine without needing your attention
 * 
 */

uint8_t *A_PIN_ARRAY;
struct magnify_adjustment_and_display_zero
{
    uint32_t magnify_adjustment;
    uint32_t zero_of_this_plotline;
    uint32_t high_limit_of_this_plotline;
} typedef magnify_adjustment_and_display_zero;

#ifndef NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC
    #define NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC 0
#endif
#ifndef NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG
    #define NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG 0
#endif

magnify_adjustment_and_display_zero screen_offsets[ NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC + NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG ];
bool graphline = false;
uint32_t value, valueTemp;
uint32_t lasttracepoints[ ( NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC + NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG ) * 2 ];
long millis_start;
char szFull[ ] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
                   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#define PlotterMaxScale ( ( ( uint32_t )pow( 2, HighestBitResFromHighestSensiAddonADC ) ) * ( uint32_t )( NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC + NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG ) )
#define HundredthPlotterMaxScale ( PlotterMaxScale / 100 );
#if ( SAMPLE_TIMES < 1 )
    #undef SAMPLE_TIMES
    #define SAMPLE_TIMES 1
#endif

#ifdef USING_LM334_WITH_MCP4162_POTS
    void setPotValue( uint8_t digital_pot_pin, uint16_t value )
    {
        switch( digital_pot_pin )
        {
            case DIGITAL_POT_1: digipot_1_value = value; break;
            case DIGITAL_POT_2: digipot_2_value = value; break;
            case DIGITAL_POT_3: digipot_3_value = value; break;
            case DIGITAL_POT_4: digipot_4_value = value; break;
            case DIGITAL_POT_5: digipot_5_value = value; break;
            case DIGITAL_POT_6: digipot_6_value = value; break;
        }
      digitalWrite( digital_pot_pin, LOW );
      SPI.transfer( ( value & 0x100 ) ? 1 : 0 );
      SPI.transfer( value & 0xff ); // send value (0~255)
      digitalWrite( digital_pot_pin, HIGH );
    }
    
    void offsetPotValue( uint8_t digital_pot_pin, int16_t offsetvalue )
    {
    #if DEBUG
//        offsetvalue += 40;  //A magnifier for troubleshooting
        while ( !Serial );
        Serial.print( F( "Digi pot on pin " ) );
    #endif
        switch( digital_pot_pin )
        {
            case DIGITAL_POT_1: if( offsetvalue < 0 ) offsetvalue = max( offsetvalue, 0 - digipot_1_value ); else if( offsetvalue > 0 ) offsetvalue = min( offsetvalue, MAXPOTVALUE - digipot_1_value ); digipot_1_value += offsetvalue; offsetvalue = digipot_1_value; break;
            case DIGITAL_POT_2: if( offsetvalue < 0 ) offsetvalue = max( offsetvalue, 0 - digipot_2_value ); else if( offsetvalue > 0 ) offsetvalue = min( offsetvalue, MAXPOTVALUE - digipot_2_value ); digipot_2_value += offsetvalue; offsetvalue = digipot_2_value; break;
            case DIGITAL_POT_3: if( offsetvalue < 0 ) offsetvalue = max( offsetvalue, 0 - digipot_3_value ); else if( offsetvalue > 0 ) offsetvalue = min( offsetvalue, MAXPOTVALUE - digipot_3_value ); digipot_3_value += offsetvalue; offsetvalue = digipot_3_value; break;
            case DIGITAL_POT_4: if( offsetvalue < 0 ) offsetvalue = max( offsetvalue, 0 - digipot_4_value ); else if( offsetvalue > 0 ) offsetvalue = min( offsetvalue, MAXPOTVALUE - digipot_4_value ); digipot_4_value += offsetvalue; offsetvalue = digipot_4_value; break;
            case DIGITAL_POT_5: if( offsetvalue < 0 ) offsetvalue = max( offsetvalue, 0 - digipot_5_value ); else if( offsetvalue > 0 ) offsetvalue = min( offsetvalue, MAXPOTVALUE - digipot_5_value ); digipot_5_value += offsetvalue; offsetvalue = digipot_5_value; break;
            case DIGITAL_POT_6: if( offsetvalue < 0 ) offsetvalue = max( offsetvalue, 0 - digipot_6_value ); else if( offsetvalue > 0 ) offsetvalue = min( offsetvalue, MAXPOTVALUE - digipot_6_value ); digipot_6_value += offsetvalue; offsetvalue = digipot_6_value; break;
        }
        #if DEBUG
            while ( !Serial );
            Serial.print( digital_pot_pin );
            Serial.print( F( " getting " ) );
            Serial.print( offsetvalue );
        #endif
      digitalWrite( digital_pot_pin, LOW );
      SPI.transfer( ( offsetvalue & 0x100 /*This restricts data to step numbers only.  Maybe you'd want to open it up for other type data...*/) ? 1 : 0 );
      SPI.transfer( offsetvalue & 0xff ); // send value (0~255)
      digitalWrite( digital_pot_pin, HIGH );
    #if DEBUG
        while ( !Serial );
        Serial.print( F( ". Digi pot on pin " ) );
        Serial.print( digital_pot_pin );
        Serial.print( F( " got a " ) );
        Serial.println( offsetvalue );
    #endif
    }

    bool adjust_values_for_this_leg( uint8_t MSB_pot_pin, uint16_t* MSB_pot_value, uint8_t MID_pot_pin, uint16_t* MID_pot_value, uint8_t LSB_pot_pin, uint16_t* LSB_pot_value, bool pos_or_neg = true ) //default direction will be positive
    {
        #if DEBUG
            while ( !Serial );
            Serial.print( F( "adjust_values_for_this_leg called on " ) );
            Serial.print( MSB_pot_pin );
            Serial.print( F( " going " ) );
            Serial.println( pos_or_neg ? "up": "down");
        #endif
        //if going up and total_value_coarse is already MAXPOTVALUE * 2, add one to value_fine if it is less than MAXPOTVALUE, else return false; save new value; return true
        //if going up still and either coarse value is MAXPOTVALUE, only increment the other value, save new, return true;
        //if going up and neither coarse value is MAXPOTVALUE, increment MID with a switch statement, save new
        uint16_t total_value_coarse = *MSB_pot_value;
        total_value_coarse += *MID_pot_value;  //We abstract it this way to force MSB and MID digi pot values to conform as namesaked
        //Next lines just force significant digit compliance inside digi pots
        *MSB_pot_value = ( total_value_coarse > MAXPOTVALUE ? total_value_coarse - MAXPOTVALUE : 0 );
        *MID_pot_value = ( total_value_coarse > MAXPOTVALUE ? MAXPOTVALUE : total_value_coarse );
        *LSB_pot_value = min( MAXPOTVALUE, *LSB_pot_value );
        
        setPotValue( MSB_pot_pin, *MSB_pot_value );
        setPotValue( MID_pot_pin, *MID_pot_value );
        setPotValue( LSB_pot_pin, *LSB_pot_value );

        if( pos_or_neg ) //Positive direction == true
        {
            if( total_value_coarse >= MAXPOTVALUE * 2 )
            {
                *MSB_pot_value = *MID_pot_value = MAXPOTVALUE;
                setPotValue( MSB_pot_pin, *MSB_pot_value ); //This shouldn't do anything if the pot values conform to MSB and MID already
                setPotValue( MID_pot_pin, *MID_pot_value ); //This shouldn't do anything if the pot values conform to MSB and MID already
                setPotValue( LSB_pot_pin, ++( *LSB_pot_value ) > MAXPOTVALUE ? MAXPOTVALUE : *LSB_pot_value );
                if( *LSB_pot_value > MAXPOTVALUE ) { *LSB_pot_value = MAXPOTVALUE; return false; }
                return true;
            }
            if( *LSB_pot_value + 1 > 9 ) //This catches *LSB_pot_value having or about to have a value higher than 9 when it shouldn't.  So we cycle it down
            {
                do 
                {
                    *LSB_pot_value = ( uint16_t )( ( int16_t )*LSB_pot_value - 10 ); //Yes, this can cause an unsigned to underflow to -1
                } while( ( ++total_value_coarse < MAXPOTVALUE * 2 ) && *LSB_pot_value + 1 > 9 );

                *MSB_pot_value = ( total_value_coarse > MAXPOTVALUE ?  total_value_coarse - MAXPOTVALUE : 0 );
                *MID_pot_value = ( total_value_coarse > MAXPOTVALUE ? MAXPOTVALUE : total_value_coarse );
                setPotValue( MSB_pot_pin, *MSB_pot_value );
                setPotValue( MID_pot_pin, *MID_pot_value );
            }
            setPotValue( LSB_pot_pin, ++( *LSB_pot_value ) );
            return true;
        }

//Being told to decrement
        if( total_value_coarse == 0 && *LSB_pot_value == 0 ) return false;
        if( ( int16_t )*LSB_pot_value == 0 ) //This catches *LSB_pot_value having or about to have a value lower than 0.  So we cycle it up
        {
            *LSB_pot_value = 10;
            *MSB_pot_value = ( --total_value_coarse > MAXPOTVALUE ? total_value_coarse - MAXPOTVALUE : 0 );
            *MID_pot_value = ( total_value_coarse > MAXPOTVALUE ? MAXPOTVALUE : total_value_coarse );
            setPotValue( MSB_pot_pin, *MSB_pot_value );
            setPotValue( MID_pot_pin, *MID_pot_value );
        }
        setPotValue( LSB_pot_pin, --( *LSB_pot_value ) );
        return true;
    }

    bool adjust_whole_bridge_positive( uint8_t channel ) //channel not utilized yet
    {
        // If all are maxed out positive, return false
        if( ( digipot_1_value /*When channel becomes relevent: ( channel == 0 ) ? digipot_1_value : digipot_7_value*/ + digipot_2_value + digipot_3_value >= ( 514 + MAXPOTVALUE ) ) && \
            ( digipot_4_value + digipot_5_value + digipot_6_value == 0 ) ) return false;
//Target leg to adjust is the one that brings the negative leg closet to its midpoint: determine how far away from midpoint the negative leg is:
        if( ( ( ( digipot_4_value + digipot_5_value ) * MAXPOTVALUE ) + ( digipot_6_value ) ) > 66177 /*This is half of total resistance in terms of digi pot units*/ )
        {
            if( adjust_values_for_this_leg( DIGITAL_POT_4, digipot_4_value, DIGITAL_POT_5, digipot_5_value, DIGITAL_POT_6, digipot_6_value, false ) ) return true;
            else return adjust_values_for_this_leg( DIGITAL_POT_1, digipot_1_value, DIGITAL_POT_2, digipot_2_value, DIGITAL_POT_3, digipot_3_value );
        }
        else if( adjust_values_for_this_leg( DIGITAL_POT_1, digipot_1_value, DIGITAL_POT_2, digipot_2_value, DIGITAL_POT_3, digipot_3_value, false ) ) return true;
        return adjust_values_for_this_leg( DIGITAL_POT_4, digipot_4_value, DIGITAL_POT_5, digipot_5_value, DIGITAL_POT_6, digipot_6_value );
    }
    
    bool adjust_whole_bridge_negative( uint8_t channel ) //channel not utilized yet
    { //Determine which leg is closest to Vcc/2 and tweak that first.  If all pots are maxed (positive leg to negative and negative leg to positive) return false
        if( ( digipot_1_value + digipot_2_value + digipot_3_value == 0 ) && \
            ( digipot_4_value + digipot_5_value >= MAXPOTVALUE * 2 ) && ( digipot_6_value > 256 ) ) return false;
//Target leg to adjust is the one that brings the negative leg closet to its midpoint: determine how far away from midpoint the negative leg is:
        if( ( ( ( digipot_4_value + digipot_5_value ) * MAXPOTVALUE ) + ( digipot_6_value ) ) < 66177 /*This is half of total resistance in terms of digi pot units*/ )
        {
            if( adjust_values_for_this_leg( DIGITAL_POT_4, digipot_4_value, DIGITAL_POT_5, digipot_5_value, DIGITAL_POT_6, digipot_6_value ) ) return true;
            else return adjust_values_for_this_leg( DIGITAL_POT_1, digipot_1_value, DIGITAL_POT_2, digipot_2_value, DIGITAL_POT_3, digipot_3_value, false );
        }
        else if( adjust_values_for_this_leg( DIGITAL_POT_1, digipot_1_value, DIGITAL_POT_2, digipot_2_value, DIGITAL_POT_3, digipot_3_value ) ) return true;
        return adjust_values_for_this_leg( DIGITAL_POT_4, digipot_4_value, DIGITAL_POT_5, digipot_5_value, DIGITAL_POT_6, digipot_6_value, false );
    }
#endif

void printvaluesforalltraces( bool actuals = false )                  //If we don't execute the following construct, the signal traces will lag behind the graphline
{
    Serial.print( F( " " ) );
    for( uint8_t i = 0; i < NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG + NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC; i++ )
    {
        Serial.print( actuals ? lasttracepoints[ i * 2 ] : 0 );
        Serial.print( F( " " ) );
        Serial.print( actuals ? lasttracepoints[ ( i * 2 ) + 1 ] : 0 );
        Serial.print( F( " " ) );
    }
    Serial.println();
}

void setup() 
{
#ifdef USING_LM334_WITH_MCP4162_POTS   //This is important to put this first so LM334 get some resistance
    pinMode( SS, OUTPUT );
    digitalWrite( SS, HIGH ); //Not needed by slave devices but rather by this board to make it configure itself to be master
    
    pinMode( DIGITAL_POT_1, OUTPUT );
    pinMode( DIGITAL_POT_2, OUTPUT );
    pinMode( DIGITAL_POT_3, OUTPUT );
    pinMode( DIGITAL_POT_4, OUTPUT );
    pinMode( DIGITAL_POT_5, OUTPUT );
    pinMode( DIGITAL_POT_6, OUTPUT );
    digitalWrite( DIGITAL_POT_1, HIGH );
    digitalWrite( DIGITAL_POT_2, HIGH );
    digitalWrite( DIGITAL_POT_3, HIGH );
    digitalWrite( DIGITAL_POT_4, HIGH );
    digitalWrite( DIGITAL_POT_5, HIGH );
    digitalWrite( DIGITAL_POT_6, HIGH );
    SPI.begin();
    SPI.setBitOrder( MSBFIRST );
#ifndef LEAVEPOTVALUESALONEDURINGSETUP
    setPotValue( DIGITAL_POT_1, digipot_1_value );
    setPotValue( DIGITAL_POT_2, digipot_2_value );
    setPotValue( DIGITAL_POT_3, digipot_3_value );
    setPotValue( DIGITAL_POT_4, digipot_4_value );
    setPotValue( DIGITAL_POT_5, digipot_5_value );
    setPotValue( DIGITAL_POT_6, digipot_6_value );
#endif
    #if DEBUG
        while ( !Serial && ( millis() - millis_start < 8000 ) );
        Serial.println( F( "Digi pots set up" ) );
    #endif
#endif
    #if ( NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC > 0 ) 
        #if !( ARDUINO_ARCH_SAM || ARDUINO_ARCH_SAMD )
            analogReference( DEFAULT ); //This is the voltage level of max bit value on analog input
        #else
            analogReference( AR_DEFAULT ); //
        #endif
    #endif    
    //#ifdef __LGT8FX8E__
        Serial.begin( 19200 );//This speed is what works best with WeMos XI/TTGO XI board.
    #ifdef DEBUG
        millis_start = millis();
        while ( !Serial && ( millis() - millis_start < 8000 ) );
        Serial.print( F( "Starting setup with SDA & SCL & SCK & A4 & A5 = " ) );
        Serial.print( SDA );
        Serial.print( F( " & " ) );
        Serial.print( SCL );
        Serial.print( F( " & " ) );
        Serial.print( SCK );
        Serial.print( F( " & " ) );
        Serial.print( A4 );
        Serial.print( F( " & " ) );
        Serial.println( A5 );
    #endif
    //#else
    //    Serial.begin( 57600 );//Is there any reason for this higher speed?  I don't think so, but feel free to put this code back into action if you want to.
    //#endif
    //#ifndef ARDUINO_AVR_DIGISPARKPRO
    //    analogReadResolution( ADC_RES_BIT );
    //#endif
    #if ( NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC > 0 )
        #if ( HighestBitResFromHighestSensiAddonADC < 17 )
        //   ads.setGain( GAIN_TWOTHIRDS );  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)  //The extra ones are here for reference 
           ads.setGain( GAIN_ONE );        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV  //This allows a simple power rail excitation supply to voltage divider
        //   ads.setGain( GAIN_TWO );        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
        //   ads.setGain( GAIN_FOUR );       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
        //   ads.setGain( GAIN_EIGHT );      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
        //   ads.setGain( GAIN_SIXTEEN );    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
            ads.begin();
        #else
            #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == HX711 )
                #ifdef DEBUG
                    millis_start = millis();
                    while ( !Serial && ( millis() - millis_start < 8000 ) );
                    Serial.print( F( "Initializing HX711..." ) );
                #endif
/*
 * With the library being used, the .begin() method is not needed if the HX711 was declared earlier prior to setup().  The reason I'm doing it superfluously here is simply to suggest some measure of future consistency across all ADCs since other ADC libraries may not allow one or the other...
 */
                hx711.begin( PIN_FOR_DATA_TOFROM_HIGHEST_SENSI_ADC, PIN_FOR_CLK_TO_HIGHEST_SENSI_ADC, HIGHEST_SENSI_PGA_GAIN_FACTOR );  // if the third parameter is ommited the default gain value 128 & channel A is used by the library
                #ifdef DEBUG
                    millis_start = millis();
                    while ( !Serial && ( millis() - millis_start < 8000 ) ); //We limit the time to wait for serial ready
                    Serial.println( F( "done initializing HX711." ) );
                #endif
            #else
                #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == ADS1232 )
                #else
                    #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == ADS1242 )
                    #else
                        #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == AD7780 )
                        #else
                            #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == AD779x )
                            #else
                                #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == MAX112x0 )
                                #else
                                    #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == LTC2400 )
                                    #else
                                        #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == ADS1231 )
                                        #else
                                        #endif
                                    #endif
                                #endif
                            #endif
                        #endif
                    #endif
                #endif
            #endif
        #endif
    #endif
    
    #ifdef __LGT8FX8E__
        delay( SECONDS_THAT_A_LGT8FX8E_HARDWARE_SERIAL_NEEDS_TO_COME_UP_WORKING * 1000 );  // Needed by this board for serial to work
    //    analogReadResolution( 12 );  //not necessary
    #endif
    #if ( ARDUINO_ARCH_XI || ARDUINO_ARCH_SAM || ARDUINO_ARCH_SAMD ) //These are the boards known to have 12 bit analog inputs
        #ifdef AnalogInputBitsOfBoard
            #undef AnalogInputBitsOfBoard
        #endif
        #define AnalogInputBitsOfBoard 12  //These boards have 12 bit
    #endif

    #if ( NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC > 0 )
        #define SCALE_FACTOR_TO_PROMOTE_LOW_RES_ADC_TO_SAME_SCALE ( HighestBitResFromHighestSensiAddonADC - AnalogInputBitsOfBoard )
    #else
        #define SCALE_FACTOR_TO_PROMOTE_LOW_RES_ADC_TO_SAME_SCALE 0
    #endif
    
    for( uint8_t i = 0; i < NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC + NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG; i++ )
    {
        screen_offsets[ i ].magnify_adjustment = 0;
//where i > NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG results in 
        screen_offsets[ i ].high_limit_of_this_plotline = ( uint32_t )pow( 2, HighestBitResFromHighestSensiAddonADC ) * ( i > NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG ? ( ( uint32_t )( NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC - i ) ) : ( ( uint32_t )( NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG - i ) + ( uint32_t )NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC ) );
        screen_offsets[ i ].zero_of_this_plotline = ( uint32_t )pow( 2, HighestBitResFromHighestSensiAddonADC ) * ( i > NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG ? ( ( uint32_t )( NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC - ( i + 1 ) ) ) : ( uint32_t )( NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG - ( i + 1 ) ) + ( uint32_t )NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC );
    }

    while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
    for( uint8_t i = 1; i < NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG + NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC; i++ )
    {
        Serial.print( screen_offsets[ NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG + NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC - i ].zero_of_this_plotline );
        printvaluesforalltraces();
        Serial.print( screen_offsets[ NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG + NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC - i ].zero_of_this_plotline );
        printvaluesforalltraces();
        Serial.print( screen_offsets[ NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG + NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC - i ].high_limit_of_this_plotline );
        printvaluesforalltraces();
        Serial.print( screen_offsets[ NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG + NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC - i ].high_limit_of_this_plotline );
        printvaluesforalltraces();
    }
    Serial.println( PlotterMaxScale ); // graphline
//    printvaluesforalltraces();  //Commenting this out looks better graphing, but also causes a very tiny (one sample distance) lag in the signal traces during the first screenful

    #if ( NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG > 0 )
        A_PIN_ARRAY = (uint8_t *)malloc( NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG );
    
        //Herafter is the pattern.  If you have more analog pins, add them according to the pattern.
        #ifdef NUM_ANALOG_INPUTS
        #ifdef PIN_A0
            for( uint8_t i = 0; i < NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG; i++ )
            {
                if( i == 0 )
                    *(A_PIN_ARRAY + i) = PIN_A0;
                #ifdef PIN_A1
                    else if( i == 1 )
                        *(A_PIN_ARRAY + i) = PIN_A1;
                    #ifdef PIN_A2
                    else if( i == 2 )
                        *(A_PIN_ARRAY + i) = PIN_A2;
                        #ifdef PIN_A3
                    else if( i == 3 )
                        *(A_PIN_ARRAY + i) = PIN_A3;
                            #ifdef PIN_A4
                    else if( i == 4 )
                        *(A_PIN_ARRAY + i) = PIN_A4;
                                #ifdef PIN_A5
                    else if( i == 5 )
                        *(A_PIN_ARRAY + i) = PIN_A5;
                                    #ifdef PIN_A6
                    else if( i == 6 )
                        *(A_PIN_ARRAY + i) = PIN_A6;
                                        #ifdef PIN_A7
                    else if( i == 7 )
                        *(A_PIN_ARRAY + i) = PIN_A7;
                                            #ifdef PIN_A8
                    else if( i == 8 )
                        *(A_PIN_ARRAY + i) = PIN_A8;
                                                #ifdef PIN_A9
                    else if( i == 9 )
                        *(A_PIN_ARRAY + i) = PIN_A9;
                                                    #ifdef PIN_A10
                    else if( i == 10 )
                        *(A_PIN_ARRAY + i) = PIN_A10;
                                                        #ifdef PIN_A11
                    else if( i == 11 )
                        *(A_PIN_ARRAY + i) = PIN_A11;
                                                            #ifdef PIN_A12
                    else if( i == 12 )
                        *(A_PIN_ARRAY + i) = PIN_A12;
                                                                #ifdef PIN_A13
                    else if( i == 13 )
                        *(A_PIN_ARRAY + i) = PIN_A13;
                                                                    #ifdef PIN_A14
                    else if( i == 14 )
                        *(A_PIN_ARRAY + i) = PIN_A14;
                                                                        #ifdef PIN_A15
                    else if( i == 15 )
                        *(A_PIN_ARRAY + i) = PIN_A15;
                                                                            #ifdef PIN_A16
                    else if( i == 16 )
                        *(A_PIN_ARRAY + i) = PIN_A16;
                                                                                #ifdef PIN_A17
                    else if( i == 17 )
                        *(A_PIN_ARRAY + i) = PIN_A17;
                                                                                    #ifdef PIN_A18
                    else if( i == 18 )
                        *(A_PIN_ARRAY + i) = PIN_A18;
                                                                                        #ifdef PIN_A19
                    else if( i == 19 )
                        *(A_PIN_ARRAY + i) = PIN_A19;
                                                                                            #ifdef PIN_A20
                    else if( i == 20 )
                        *(A_PIN_ARRAY + i) = PIN_A20;
                                                                                                #ifdef PIN_A21
                    else if( i == 21 )
                        *(A_PIN_ARRAY + i) = PIN_A21;
                                                                                                    #ifdef PIN_A22
                    else if( i == 22 )
                        *(A_PIN_ARRAY + i) = PIN_A22;
                                                                                                    #endif //end of PIN_A22 check
                                                                                                #endif //end of PIN_A21 check
                                                                                            #endif //end of PIN_A20 check
                                                                                        #endif //end of PIN_A19 check
                                                                                    #endif //end of PIN_A18 check
                                                                                #endif //end of PIN_A17 check
                                                                            #endif //end of PIN_A16 check
                                                                        #endif //end of PIN_A15 check
                                                                    #endif //end of PIN_A14 check
                                                                #endif //end of PIN_A13 check
                                                            #endif //end of PIN_A12 check
                                                        #endif //end of PIN_A11 check
                                                    #endif //end of PIN_A10 check
                                                #endif //end of PIN_A9 check
                                            #endif //end of PIN_A8 check
                                        #endif //end of PIN_A7 check
                                    #endif //end of PIN_A6 check
                                #endif //end of PIN_A5 check
                            #endif //end of PIN_A4 check
                        #endif //end of PIN_A3 check
                    #endif //end of PIN_A2 check
                #endif //end of PIN_A1 check
                }
            #else
            //Some boards don't have good definitions and constants for the analog pins, TTGO XI, for example :-(
                #ifdef DEBUG
                    while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
                    Serial.println( F( "Creating analog pin definitions" ) );
                #endif
                for( uint8_t i = 0; i < NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG; i++ )
                    *(A_PIN_ARRAY + i) = i + FIRST_ANALOG_PIN_DIGITAL_NUMBER_FOR_BOARDS_NOT_HAVING_ANALOG_PINS_DEFINED_BY_PIN_A0_TYPE_DEFINES;
            #endif //end of PIN_A0 check
        #endif
    #endif
    #ifdef DEBUG
        while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
        #ifndef __LGT8FX8E__
            Serial.print( F( "Operator convenience delay so operator can prepare as needed..." ) );
            delay( 5000 );
            Serial.println( F( "done delaying for operator convenience." ) );
        #endif
        Serial.println( F( "Some running parameters:" ) );
        Serial.print( F( "A_PIN_ARRAY = " ) );
        Serial.println( (unsigned long)A_PIN_ARRAY );
        for( uint8_t i = 0; i < NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG; i++ )
            Serial.println( *( A_PIN_ARRAY + i ) );
        Serial.print( F( "PlotterMaxScale = " ) );
        Serial.print( PlotterMaxScale );
        Serial.print( F( " derived from ( uint32_t )pow( 2, HighestBitResFromHighestSensiAddonADC ) = " ) );
        Serial.print( ( uint32_t )pow( 2, HighestBitResFromHighestSensiAddonADC ) );
        Serial.print( F( " and ( uint32_t )( NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC + NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG ) = " ) );
        Serial.println( ( uint32_t )( NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC + NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG ) );
        
        #ifdef DIFFERENTIAL
            Serial.print( F( "( uint32_t )pow( 2, HighestBitResFromHighestSensiAddonADC - 1 ) = " ) );Serial.println( ( uint32_t )pow( 2, HighestBitResFromHighestSensiAddonADC - 1 ) );
        #else
            Serial.print( F( "( uint32_t )pow( 2, HighestBitResFromHighestSensiAddonADC ) = " ) );Serial.println( ( uint32_t )pow( 2, HighestBitResFromHighestSensiAddonADC ) );
        #endif
        //add other prints here to your liking
        
        Serial.println( F( "End of setup" ) );
    #endif
}


void plot_the_normal_and_magnified_signals( uint8_t channel )
{
    value = value / SAMPLE_TIMES;
    while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
    if( value + screen_offsets[ channel ].zero_of_this_plotline <= screen_offsets[ channel ].high_limit_of_this_plotline )
    {
        Serial.print( value + screen_offsets[ channel ].zero_of_this_plotline ); //This is color one
        lasttracepoints[ channel * 2 ] = value + screen_offsets[ channel ].zero_of_this_plotline;
    }
    else
    {
        Serial.print( screen_offsets[ channel ].high_limit_of_this_plotline );
        lasttracepoints[ channel * 2 ] = screen_offsets[ channel ].high_limit_of_this_plotline;
    }
    
    //Next lines plot a magnified version.  First, magnify_adjustment is determined
    Serial.print( F( " " ) );

//The following preprocessor directive is NOT TESTED 29 May 2018:  Submit a better formula if you determine it.
    #if ( ( HighestBitResFromHighestSensiAddonADC > 23 ) && ( MAGNIFICATION_FACTOR > 255 ) ) || ( ( HighestBitResFromHighestSensiAddonADC > 14 ) && ( MAGNIFICATION_FACTOR > 1000 ) ) || ( ( HighestBitResFromHighestSensiAddonADC > 10 ) && ( MAGNIFICATION_FACTOR > 2000 ) ) || ( ( HighestBitResFromHighestSensiAddonADC < 11 ) && ( MAGNIFICATION_FACTOR > 5000 ) )
        Serial.print( 0 ); //This is color two or four when magnification is too large 4294967296 is max
        lasttracepoints[ ( channel * 2 ) + 1 ] = 0;
    #else
        value *= MAGNIFICATION_FACTOR; //This needs to promote to float if normal cast produces overflow but I don't know how to do it without making it float always which is not native and thus too inefficient
    
        if( screen_offsets[ channel ].magnify_adjustment + screen_offsets[ channel ].zero_of_this_plotline > value )
            screen_offsets[ channel ].magnify_adjustment = value - screen_offsets[ channel ].zero_of_this_plotline - ( ( screen_offsets[ channel ].high_limit_of_this_plotline - screen_offsets[ channel ].zero_of_this_plotline ) * REPOSITION_RATIO_OF_MAGNIFIED_VIEW_WHEN_LIMITS_GET_EXCEEDED ) ;
    
        if( screen_offsets[ channel ].magnify_adjustment + screen_offsets[ channel ].high_limit_of_this_plotline < value )
            screen_offsets[ channel ].magnify_adjustment = value - screen_offsets[ channel ].high_limit_of_this_plotline + ( ( screen_offsets[ channel ].high_limit_of_this_plotline - screen_offsets[ channel ].zero_of_this_plotline ) * REPOSITION_RATIO_OF_MAGNIFIED_VIEW_WHEN_LIMITS_GET_EXCEEDED ) ;
            
            //Plot it now
        Serial.print( value - screen_offsets[ channel ].magnify_adjustment ); //This is color two or four
        lasttracepoints[ ( channel * 2 ) + 1 ] = value - screen_offsets[ channel ].magnify_adjustment;
    #endif
    Serial.print( F( " " ) );
}

void loop() 
{
    for( uint16_t plotter_loops = 0; plotter_loops < 500 / 3; plotter_loops++ ) 
    {
        millis_start = millis();
/* */
        if( graphline ) valueTemp = 0;
        else valueTemp = PlotterMaxScale;
        while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
        Serial.print( valueTemp ); //This was originally last printed in the group.  It needs to be first instead so the line can be "notched" without needing to reprint all values.
        Serial.print( F( " " ) );
/* */
        #if ( NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG > 0 ) //plot the inboard analogs first and above
            for( uint8_t i = 0; i < NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG; i++ )
            {
                #ifdef DEBUG
                    while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
//                    Serial.print( F( "A_PIN_ARRAY = " ) );
//                    Serial.println( (unsigned long)A_PIN_ARRAY );
                    Serial.print( F( "Reading pin " ) );
                    Serial.print( *( A_PIN_ARRAY + i ) );
                    Serial.print( F( ", level appearing as " ) );
                    Serial.print( analogRead( *( A_PIN_ARRAY + i ) ) );
                    if( i == 0 )
                    {
                        Serial.print( F( ", SCALE_FACTOR_TO_PROMOTE_LOW_RES_ADC_TO_SAME_SCALE " ) );
                        Serial.println( SCALE_FACTOR_TO_PROMOTE_LOW_RES_ADC_TO_SAME_SCALE );
                    }
                    else Serial.println();
                #endif
                value = analogRead( *( A_PIN_ARRAY + i ) );
                #ifdef DEBUG
                    while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
                    Serial.print( F( "Value was " ) );
                    Serial.print( value );;
                #endif
    
                for( uint8_t sampletimes = 1; sampletimes < SAMPLE_TIMES; sampletimes++ )
                {
                    #if ( defined DELAY_TIME_BETWEEN_SAMPLES_MS ) && ( DELAY_TIME_BETWEEN_SAMPLES_MS > 0 )
                                    delay( DELAY_TIME_BETWEEN_SAMPLES_MS );
                    #endif
                    #if ( defined DELAY_TIME_BETWEEN_SAMPLES_US ) && ( DELAY_TIME_BETWEEN_SAMPLES_US > 0 )
                                    delayMicroseconds( DELAY_TIME_BETWEEN_SAMPLES_US );
                    #endif
                    value += analogRead( *( A_PIN_ARRAY + i ) );
                }
                /*value = */ value <<= SCALE_FACTOR_TO_PROMOTE_LOW_RES_ADC_TO_SAME_SCALE;
                #ifdef DEBUG
                    while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
                    Serial.print( F( ".  Done reading pin " ) );
                    Serial.print( *( A_PIN_ARRAY + i ) );
                    Serial.print( F( ", " ) );
                    Serial.print( SAMPLE_TIMES );
                    Serial.print( F( " times, total value now " ) );
                    Serial.print( value );
                    Serial.print( F( " which should average to " ) );
                    Serial.print( value / SAMPLE_TIMES );
                    Serial.println( F( ", now plotting one inboard analog input with magnified version offset for proper positioning:" ) );
                #endif
                plot_the_normal_and_magnified_signals( i );
                #ifdef DEBUG
                    Serial.println();
                #endif
            }
        #endif

        #if ( NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC > 0 )
            for( uint8_t i = 0; i < NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC; i++ )
            {
                #ifdef USING_LM334_WITH_MCP4162_POTS 
Start_of_addon_ADC_acquisition:
                    bool overscale_is_unfixable = false;
                    while( true ) //comes back to here if value overscales
                    {
                #endif
                #if ( HighestBitResFromHighestSensiAddonADC == 11 ) || ( HighestBitResFromHighestSensiAddonADC == 15 )
                    value = ads.readADC_SingleEnded( i );
                    while( value > pow( 2, HighestBitResFromHighestSensiAddonADC ) )
                    {
                        value = ads.readADC_SingleEnded( i );
                    }
                #else
                    #ifdef DIFFERENTIAL
                        #if ( HighestBitResFromHighestSensiAddonADC < 17 )
                            value = ( ( ( i == 1 ) ? ( ads.readADC_Differential_2_3() ) : ( ads.readADC_Differential_0_1() ) ) + pow( 2, HighestBitResFromHighestSensiAddonADC - 1 ) );
                        #else
                            #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == HX711 )
//                                hx711.power_up();
                                value = hx711.read() + pow( 2, HighestBitResFromHighestSensiAddonADC - 1 );
//                                value = hx711.read_average( 9 );
                                
//                                hx711.power_down();
                            #else
                                #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == ADS1232 )
                                #else
                                    #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == ADS1242 )
                                    #else
                                        #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == AD7780 )
                                        #else
                                            #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == AD779x )
                                            #else
                                                #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == MAX112x0 )
                                                #else
                                                    #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == LTC2400 )
                                                    #else
                                                        #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == ADS1231 )
                                                        #else
                                                        #endif
                                                    #endif
                                                #endif
                                            #endif
                                        #endif
                                    #endif
                                #endif
                            #endif
                        #endif
                    #else //then figure it is Single-ended
                    #endif
                #endif
                #ifdef USING_LM334_WITH_MCP4162_POTS 
                        if( ( value > 0 ) && ( value < pow( 2, HighestBitResFromHighestSensiAddonADC ) ) ) break;
                        if( value == 0 ) 
                            if( !adjust_whole_bridge_positive( i ) ) { overscale_is_unfixable = true; break; } //
                        else
                            if( !adjust_whole_bridge_negative( i ) ) { overscale_is_unfixable = true; break; }
                    }
                #endif
//FUTURE: IF value IS OVER-SCALE ONE WAY OR THE OTHER, ADJUST DIGI POTS AND START THE ACQUISITION OVER UNTIL OPPOSING POTS ARE MAXED OUT
                #if ( defined SAMPLE_TIMES ) && ( SAMPLE_TIMES > 0 )
                    for( uint8_t sampletimes = 1; sampletimes < SAMPLE_TIMES; sampletimes++ )
                    {
                        #if ( defined DELAY_TIME_BETWEEN_SAMPLES_MS ) && ( DELAY_TIME_BETWEEN_SAMPLES_MS > 0 )
                            delay( DELAY_TIME_BETWEEN_SAMPLES_MS );
                        #endif
                        #if ( defined DELAY_TIME_BETWEEN_SAMPLES_US ) && ( DELAY_TIME_BETWEEN_SAMPLES_US > 0 )
                            delayMicroseconds( DELAY_TIME_BETWEEN_SAMPLES_US );
                        #endif
                        #if ( HighestBitResFromHighestSensiAddonADC == 11 ) || ( HighestBitResFromHighestSensiAddonADC == 15 )
                            valueTemp = ads.readADC_SingleEnded( i );
                            while( valueTemp > pow( 2, HighestBitResFromHighestSensiAddonADC ) ) valueTemp = ads.readADC_SingleEnded( i );
                        #else
                            #ifdef DIFFERENTIAL
                                #if ( HighestBitResFromHighestSensiAddonADC < 17 )
                                    valueTemp = ( ( ( i == 1 ) ? ( ads.readADC_Differential_2_3() ) : ( ads.readADC_Differential_0_1() ) ) + pow( 2, HighestBitResFromHighestSensiAddonADC - 1 ) );
                                #else
                                    #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == HX711 )
                                        #ifdef DEBUG
                                            while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
//                                            Serial.println( F( "Reading differential valueTemp" ) );
                                        #endif
//                                            hx711.power_up();
                                            valueTemp = hx711.read() + pow( 2, HighestBitResFromHighestSensiAddonADC - 1 );
//                                            valueTemp = hx711.read_average( 9 );
                                            
//                                            hx711.power_down();
                                    #else
                                        #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == ADS1232 )
                                        #else
                                            #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == ADS1242 )
                                            #else
                                                #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == AD7780 )
                                                #else
                                                    #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == AD779x )
                                                    #else
                                                        #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == MAX211x0 )
                                                        #else
                                                            #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == LTC2400 )
                                                            #else
                                                                #if ( HIGHEST_SENSI_ADDON_ADC_TYPE == ADS1231 )
                                                                #else
                                                                #endif
                                                            #endif
                                                        #endif
                                                    #endif
                                                #endif
                                            #endif
                                        #endif
                                    #endif
                                #endif
                                #ifdef DEBUG
                                    while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
//                                    Serial.print( F( "Read adjusted differential valueTemp " ) );
//                                    Serial.println( valueTemp );
                                #endif
                            #endif
                        #endif
                        #ifdef USING_LM334_WITH_MCP4162_POTS 
                    // If value IS MAX SCALE ONE WAY OR THE OTHER, ADJUST DIGI POTS AND START THE ACQUISITION OVER UNLESS POTS ARE MAXED OUT
                        if( ( valueTemp == 0 ) && !overscale_is_unfixable )
                            { if( adjust_whole_bridge_positive( i ) ) goto Start_of_addon_ADC_acquisition; }
                        else if( ( valueTemp >= pow( 2, HighestBitResFromHighestSensiAddonADC ) ) && !overscale_is_unfixable )
                            { if( adjust_whole_bridge_negative( i ) ) goto Start_of_addon_ADC_acquisition; }
                        #endif
                        value += valueTemp;
                }
                #endif
/*
        #ifdef USING_LM334_WITH_MCP4162_POTS && #ifdef DEBUG
            while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
            Serial.print( F( "digipot_1_value = " ) );
            Serial.print( digipot_1_value );
            Serial.print( F( ", digipot_2_value = " ) );
            Serial.print( digipot_2_value );
            Serial.print( F( ", digipot_3_value = " ) );
            Serial.print( digipot_3_value );
            while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
            Serial.print( F( ", digipot_4_value = " ) );
            Serial.print( digipot_4_value );
            Serial.print( F( ", digipot_5_value = " ) );
            Serial.print( digipot_5_value );
            Serial.print( F( ", digipot_6_value = " ) );
            Serial.println( digipot_6_value );
        #endif
*/
                plot_the_normal_and_magnified_signals( i + NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG );
            }
        #endif
/*
        if( graphline ) valueTemp = 0;
        else valueTemp = PlotterMaxScale;
        while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
        Serial.println( valueTemp );
*/
        Serial.println();

        while( millis() - millis_start < MIN_WAIT_TIME_BETWEEN_PLOT_POINTS_MS );
/* */
        char nextChar;
        nextChar = 0;
        while( Serial.available() )
        {
//            pinMode( LED_BUILTIN, OUTPUT );                // These lines for blinking the LED are here if you want the LED to blink when data is rec'd
//            digitalWrite( LED_BUILTIN, HIGH );             // These lines for blinking the LED are here if you want the LED to blink when data is rec'd
            nextChar = (char)Serial.read();
            if( nextChar != 0xD && nextChar != 0xA )
            {
                szFull[ strlen( szFull ) + 1 ] = 0;
                szFull[ strlen( szFull ) ] = nextChar;
            }
            else
            {
                if( Serial.available() ) Serial.read();
                nextChar = 0;
            }
        }
//        digitalWrite( LED_BUILTIN, LOW );                  // These lines for blinking the LED are here if you want the LED to blink when data is rec'd
        if( !( szFull[ 0 ] == 0 || nextChar != 0 ) )        //The way this and while loop is set up allows reception of lines with no endings but at a timing cost of one loop()
        {
            #ifdef DEBUG
                while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
                Serial.print( F( "Received " ) );
                Serial.println( szFull );
            #endif
    #ifdef USING_LM334_WITH_MCP4162_POTS
            uint8_t DIGITAL_POT_;
            int offset_integer;
            if( szFull[ 1 ] == 'D' || szFull[ 1 ] == 'd' ) szFull[ 1 ] = '-';
            else if( szFull[ 1 ] == 'U' || szFull[ 1 ] == 'u' ) szFull[ 1 ] = '+';
            if( szFull[ 1 ] == '-' )
            {
                if( szFull[ 2 ] == 0 )
                    offset_integer = -1;
                else
                    offset_integer = 0 - atoi( &szFull[ 2 ] );
            }
            else if( szFull[ 1 ] == '+' )
            {
                if( szFull[ 2 ] == 0 )
                    offset_integer = 1;
                else
                    offset_integer = atoi( &szFull[ 2 ] );
            }
            else goto NoPotChange;
            if( offset_integer > MAXPOTVALUE || offset_integer < 0 - MAXPOTVALUE ) goto NoPotChange;
            if( szFull[ 0 ] == '1' ) DIGITAL_POT_ = DIGITAL_POT_1;
            else if( szFull[ 0 ] == '2' ) DIGITAL_POT_ = DIGITAL_POT_2;
            else if( szFull[ 0 ] == '3' ) DIGITAL_POT_ = DIGITAL_POT_3;
            else if( szFull[ 0 ] == '4' ) DIGITAL_POT_ = DIGITAL_POT_4;
            else if( szFull[ 0 ] == '5' ) DIGITAL_POT_ = DIGITAL_POT_5;
            else if( szFull[ 0 ] == '6' ) DIGITAL_POT_ = DIGITAL_POT_6;
            else goto NoPotChange;
            offsetPotValue( DIGITAL_POT_, offset_integer );
NoPotChange:
    #endif
            szFull[ 0 ] = 0;
            #ifdef DEBUG
                while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
                Serial.println();
            #endif
        }
    }
    while ( !Serial ); // wait for serial port to connect. Needed for Leonardo's native USB
    for( uint8_t i = 0; i < NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG + NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC; i++ )
    {
        if( !graphline )
        {
            Serial.print( screen_offsets[ i ].high_limit_of_this_plotline );
            printvaluesforalltraces( true );
            Serial.print( screen_offsets[ i ].high_limit_of_this_plotline );
            printvaluesforalltraces( true );
            Serial.print( screen_offsets[ i ].zero_of_this_plotline );
            printvaluesforalltraces( true );
            Serial.print( screen_offsets[ i ].zero_of_this_plotline );
            printvaluesforalltraces( true );
        }
        else
        {
            Serial.print( screen_offsets[ NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG + NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC - i ].zero_of_this_plotline );
            printvaluesforalltraces( true );
            Serial.print( screen_offsets[ NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG + NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC - i ].zero_of_this_plotline );
            printvaluesforalltraces( true );
            Serial.print( screen_offsets[ NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG + NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC - i ].high_limit_of_this_plotline );
            printvaluesforalltraces( true );
            Serial.print( screen_offsets[ NUM_INPUTS_TO_PLOT_OF_INBOARD_ANALOG + NUM_INPUTS_TO_PLOT_OF_ADDON_HIGHEST_SENSI_ADC - i ].high_limit_of_this_plotline );
            printvaluesforalltraces( true );
        }
    }
    graphline = !graphline; // graphline started as false, so it becomes true here on the first complementing of it
}
