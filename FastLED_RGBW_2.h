// ----------------------------------------------------------------------------
// FastLED_RGBW_2.h
// ----------------------------------------------------------------------------
//
// Hack to enable SK6812 RGBW strips to work with FastLED on Arduino. At the
// time of this writing, FastLED doesn't support RGBW on that chipset.
//
// Original code:              Jim Bumgardner (http://krazydad.com).
// Further modifications by:   David Madison (http://partsnotincluded.com).
//                             Christoph Wempe
//                             Tony Fabris 
// ----------------------------------------------------------------------------

#ifndef FastLED_RGBW_h
#define FastLED_RGBW_h


LIB8STATIC void nscale8x4( uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& w, fract8 scale)
{
#if SCALE8_C == 1
#if (FASTLED_SCALE8_FIXED == 1)
  uint16_t scale_fixed = scale + 1;
  r = (((uint16_t)r) * scale_fixed) >> 8;
  g = (((uint16_t)g) * scale_fixed) >> 8;
  b = (((uint16_t)b) * scale_fixed) >> 8;
  w = (((uint16_t)w) * scale_fixed) >> 8;
#else
  r = ((int)r * (int)(scale) ) >> 8;
  g = ((int)g * (int)(scale) ) >> 8;
  b = ((int)b * (int)(scale) ) >> 8;
  w = ((int)w * (int)(scale) ) >> 8;
#endif
#elif SCALE8_AVRASM == 1
  r = scale8_LEAVING_R1_DIRTY(r, scale);
  g = scale8_LEAVING_R1_DIRTY(g, scale);
  b = scale8_LEAVING_R1_DIRTY(b, scale);
  w = scale8_LEAVING_R1_DIRTY(w, scale);
  cleanup_R1();
#else
#error "No implementation for nscale8x3 available."
#endif
}


struct CRGBW
{
    union 
    {
        struct
        {
          union { uint8_t g; uint8_t green; };
          union { uint8_t r; uint8_t red; };
          union { uint8_t b; uint8_t blue; };
          union { uint8_t w; uint8_t white; };
        };
        uint8_t raw[4];
    };

  // The "inline" and "__attribute__((always_inline))" statements improve code
  // speed by instructing the AVR compiler to insert the pixel byte updates
  // directly into my loop assemblies instead of processing a function jump.
	inline CRGBW() __attribute__((always_inline)){}
	inline CRGBW(uint8_t rd, uint8_t grn, uint8_t blu, uint8_t wht) __attribute__((always_inline))
  {
		r = rd;	g = grn; b = blu;	w = wht;
	}

	inline void operator = (const CRGB c) __attribute__((always_inline))
  { 
		this->r = c.r;
		this->g = c.g;
		this->b = c.b;
		this->white = 0;
	}

	inline void operator = (const CRGBW c) __attribute__((always_inline))
  { 
		this->r = c.r;
		this->g = c.g;
		this->b = c.b;
		this->w = c.w;
	}

  inline CRGBW& operator+= (const CRGB& rhs ) __attribute__((always_inline))
  {
    r = qadd8( r, rhs.r);
    g = qadd8( g, rhs.g);
    b = qadd8( b, rhs.b);
    w = 0;       // RGB-Only version.
    return *this;
  }
  
  inline CRGBW& operator+=(const CRGBW& rhs) __attribute__((always_inline))
  {
    r = qadd8( r, rhs.r);
    g = qadd8( g, rhs.g);
    b = qadd8( b, rhs.b);
    w = qadd8( w, rhs.w); // RGBW version.
    return *this;
  }

  inline CRGBW& nscale8 (uint8_t scaledown ) __attribute__((always_inline))
  {
    nscale8x4( r, g, b, w, scaledown);
    return *this;
  }

  inline CRGBW& nscale8 (const CRGBW & scaledown ) __attribute__((always_inline))
  {
    r = ::scale8(r, scaledown.r);
    g = ::scale8(g, scaledown.g);
    b = ::scale8(b, scaledown.b);
    w = ::scale8(w, scaledown.w);
    return *this;
  }
};


inline uint16_t getRGBWsize(uint16_t nleds)
{
	uint16_t nbytes = nleds * 4;
	if(nbytes % 3 > 0) return nbytes / 3 + 1;
	else return nbytes / 3;
}

inline void fill_solid( struct CRGBW * leds, int numToFill, CRGB color)
{
  uint16_t i = numToFill;
  if (numToFill <= 0) return;
  while (i--)
  {
    leds[i] = color;
  }
}

inline void fill_solid( struct CRGBW * leds, int numToFill, CRGBW color)
{
  uint16_t i = numToFill;
  if (numToFill <= 0) return;
  while (i--)
  {
    leds[i] = color;
  }
}

template <typename PALETTE>
inline void fill_palette(CRGBW* L, uint16_t N, uint8_t startIndex, uint8_t incIndex, const PALETTE& pal, uint8_t brightness, TBlendType blendType)
{
    uint8_t colorIndex = startIndex;
    for( uint16_t i = 0; i < N; i++)
    {
        L[i] = ColorFromPalette( pal, colorIndex, brightness, blendType);
        colorIndex += incIndex;
    }
}

inline void nscale8( CRGBW* leds, uint16_t num_leds, uint8_t scale)
{
  uint16_t i = num_leds;
  if (num_leds <= 0) return;
  while (i--)
  {
      leds[i].nscale8( scale);
  }
}

inline void fadeToBlackBy( CRGBW* leds, uint16_t num_leds, uint8_t fadeBy)
{
    nscale8( leds, num_leds, 255 - fadeBy);
}

inline void fill_rainbow( struct CRGBW * pFirstLED, int numToFill,
                  uint8_t initialhue,
                  uint8_t deltahue )
{
  uint16_t i = numToFill;
  if (numToFill <= 0) return;
  CHSV hsv;
  hsv.hue = initialhue;
  hsv.val = 255;
  hsv.sat = 240;
  while (i--)
  {
      pFirstLED[i] = hsv;
      hsv.hue += deltahue;
  }
}

inline void blur1d( CRGBW* leds, uint16_t numLeds, fract8 blur_amount)
{
  uint16_t i = numLeds;
  if (numLeds <= 0) return;
  uint8_t keep = 255 - blur_amount;
  uint8_t seep = blur_amount >> 1;
  CRGBW carryover = CRGBW(0,0,0,0);
  while (i--)
  {
      CRGBW cur = leds[i];
      CRGBW part = cur;
      part.nscale8( seep);
      cur.nscale8( keep);
      cur += carryover;
      if( i) leds[i-1] += part;
      leds[i] = cur;
      carryover = part;
  }
}

#endif