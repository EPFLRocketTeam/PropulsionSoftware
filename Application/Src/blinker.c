/*
 * blinker.c
 *
 * author: Iacopo Sprenger
 */


#include <blinker.h>
#include <main.h>
#include <led.h>
#include <sensor.h>
#include "cmsis_os.h"


#define HSV_HUE_SEXTANT		256
#define HSV_HUE_STEPS		(6 * HSV_HUE_SEXTANT)

#define HSV_HUE_MIN		0
#define HSV_HUE_MAX		(HSV_HUE_STEPS - 1)
#define HSV_SAT_MIN		0
#define HSV_SAT_MAX		255
#define HSV_VAL_MIN		0
#define HSV_VAL_MAX		255

#define HSV_MONOCHROMATIC_TEST(s,v,r,g,b) \
	do { \
		if(!(s)) { \
			 *(r) = *(g) = *(b) = (v); \
			return; \
		} \
	} while(0)


#define HSV_SEXTANT_TEST(sextant) \
	do { \
		if((sextant) > 5) { \
			(sextant) = 5; \
		} \
	} while(0)

#define HSV_SWAPPTR(a,b)	do { uint8_t *tmp = (a); (a) = (b); (b) = tmp; } while(0)
#define HSV_POINTER_SWAP(sextant,r,g,b) \
	do { \
		if((sextant) & 2) { \
			HSV_SWAPPTR((r), (b)); \
		} \
		if((sextant) & 4) { \
			HSV_SWAPPTR((g), (b)); \
		} \
		if(!((sextant) & 6)) { \
			if(!((sextant) & 1)) { \
				HSV_SWAPPTR((r), (g)); \
			} \
		} else { \
			if((sextant) & 1) { \
				HSV_SWAPPTR((r), (g)); \
			} \
		} \
	} while(0)

void hsv2rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g , uint8_t *b)
{
		HSV_MONOCHROMATIC_TEST(s, v, r, g, b);	// Exit with grayscale if s == 0

		uint8_t sextant = h >> 8;

		HSV_SEXTANT_TEST(sextant);		// Optional: Limit hue sextants to defined space

		HSV_POINTER_SWAP(sextant, r, g, b);	// Swap pointers depending which sextant we are in

		*g = v;		// Top level

		// Perform actual calculations

		/*
		 * Bottom level: v * (1.0 - s)
		 * --> (v * (255 - s) + error_corr + 1) / 256
		 */
		uint16_t ww;		// Intermediate result
		ww = v * (255 - s);	// We don't use ~s to prevent size-promotion side effects
		ww += 1;		// Error correction
		ww += ww >> 8;		// Error correction
		*b = ww >> 8;

		uint8_t h_fraction = h & 0xff;	// 0...255
		uint32_t d;			// Intermediate result

		if(!(sextant & 1)) {
			// *r = ...slope_up...;
			d = v * (uint32_t)((255 << 8) - (uint16_t)(s * (256 - h_fraction)));
			d += d >> 8;	// Error correction
			d += v;		// Error correction
			*r = d >> 16;
		} else {
			// *r = ...slope_down...;
			d = v * (uint32_t)((255 << 8) - (uint16_t)(s * h_fraction));
			d += d >> 8;	// Error correction
			d += v;		// Error correction
			*r = d >> 16;
		}
}
/*   ARCENCIEL
void PP_blinkerFunc(void *argument) {
	  uint8_t r, g, b, s, v;
	  s = 255;
	  v = 255;
	  uint16_t h = 0;
	  for(;;)
	  {
		  hsv2rgb(h, s, v, &r, &g, &b);
		  PP_setLed(r/5, g/5, b/5);
		 h += 10;
		 if(h > HSV_HUE_MAX){
			 h = 0;
		 }
		 osDelay(100);
	  }
}
*/
void PP_blinkerFunc(void *argument) {
	  uint16_t r, g, b;
	  for(;;)
	  {
		  r = PP_getData(PP_PRESSURE_1);
		  g = PP_getData(PP_PRESSURE_2);
		  b = PP_getData(PP_TEMPERATURE_1);
		 PP_setLed((r>>4)/5, (g>>4)/5, (b>>4)/5);
		 osDelay(100);
	  }
}








