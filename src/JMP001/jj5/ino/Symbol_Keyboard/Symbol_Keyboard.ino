

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2024-08-29 jj5 - includes...
//

// 2024-08-30 jj5 - the XC4630d library is our interface to our touch screen...
//
#include "XC4630d.c"

// 2024-08-30 jj5 - the Arduino Keyboard library allows us to emulate a USB keyboard...
//
#include "Keyboard.h"

// 2024-08-29 jj5 - if you want only the symbols you define in your binary specify and include bitmaps.h
//#include "bitmaps.h"
// 2024-08-29 jj5 - if you just want to install all keyboard symbols available include the gen/header.h
#include "gen/header.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2024-08-29 jj5 - defines...
//

// 2024-08-29 jj5 - if JJ5 is defined the symbols are the ones Jay Jay uses, otherwise they are the ones from the
// magazine.
//
//#define JJ5

// 2024-08-29 jj5 - we highlight the button on the keyboard if the button is pressed at least MIN_LIGHT microseconds
// and we actually send the keypress if the button is still pressed after at least MIN_PRESS microseconds. This debouncing
// helps us avoid false positive keypresses which the user did not intend. These settings worked pretty well for me during
// my development and testing, but if you find better values please do let me know!
//
#define MIN_LIGHT 25000
#define MIN_PRESS 75000

// 2024-08-29 jj5 - foreground colour...
//
#define FGC CYAN

// 2024-08-29 jj5 - background colour...
//
#define BGC BLACK

// 2024-08-29 jj5 - screen dimensions...
//
#define ROWS 3
#define COLUMNS 4

// 2024-08-29 jj5 - button dimensions, including padding (the fonts are 64 pixels square, padding is 8 pixels), so that's
// 8 + 64 + 8 = 80 pixels square...
//
#define BUTTON_WIDTH 80
#define BUTTON_HEIGHT 80

// 2024-08-29 jj5 - total number of buttons... we need to know this so that we can allocate storage to track the state of
// each button...
//
#define BUTTON_COUNT ( ROWS * COLUMNS )


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2024-08-30 jj5 - structs...
//

struct button {

  // 2024-08-29 jj5 - this is the four-digit alt-key code for this button...
  //
  int alt_code;

  // 2024-08-29 jj5 - this is the 64x64 pixel symbol font bitmap for this button...
  //
  const unsigned char* bitmap;

  // 2024-08-30 jj5 - button location...
  //
  int x;
  int y;

  // 2024-08-30 jj5 - button dimension (width)...
  //
  int width;

  // 2024-08-30 jj5 - button dimension (height)...
  //
  int height;

  // 2024-08-29 jj5 - indicate if button is pressed or not...
  //
  bool pressed;

  // 2024-08-29 jj5 - this tracks if a button on the keyboard is highlighted or not, we use this so we only need to
  // write to the display if something changes...
  //
  bool light_on;

  // 2024-08-29 jj5 - this records the earliest time of a keydown event for this button, if a button gets old enough (far
  // back in time enough) we send the keypress, otherwise we wait...
  //
  unsigned long waiting_since;

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2024-08-29 jj5 - globals...
//

// 2024-08-30 jj5 - this array of key codes is used to send keypresses to the USB keyboard... the index of the key code
// corresponds to the button on the keyboard... 0 is keypad key 0, 1 is keypad key 1, etc...
//
uint8_t keypad_keys[] = {
  KEY_KP_0,
  KEY_KP_1,
  KEY_KP_2,
  KEY_KP_3,
  KEY_KP_4,
  KEY_KP_5,
  KEY_KP_6,
  KEY_KP_7,
  KEY_KP_8,
  KEY_KP_9,
};

// 2024-08-30 jj5 - this is our array of button data...
//
struct button button[ BUTTON_COUNT ];

// 2024-08-29 jj5 - this is the current time (in microseconds)...
//
unsigned long now;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2024-08-29 jj5 - event handlers...
//

void setup() {

  // 2024-08-29 jj5 - initialise the timer...
  //
  now = micros();

  // 2024-08-29 jj5 - setup the screen...
  //
  XC4630_init();
  XC4630_rotate( 2 );
  // 2024-08-29 jj5 - blank the screen...
  XC4630_clear( BGC );

  // 2024-08-29 jj5 - setup the Serial debug line...
  //
  Serial.begin( 115200 );

  // 2024-08-29 jj5 - setup the USB keyboard...
  //
  Keyboard.begin();

  // 2024-08-29 jj5 - initialise 12 buttons...
  //
  int button_index = 0;

  #ifdef JJ5

  // 2024-08-29 jj5 - these are the symbols Jay Jay uses...

  //declare_button( button_index++,  169, u0169_Copyright );
  //declare_button( button_index++,  174, u0174_Registered_Trade_Mark );
  //declare_button( button_index++,  153, u0153_Trade_Mark );

  declare_button( button_index++,  176, u0176_Degree );
  declare_button( button_index++,  177, u0177_Plus_Minus );
  declare_button( button_index++,  167, u0167_Section );
  declare_button( button_index++,  955, u0955_Lambda_Lowercase );

  declare_button( button_index++,  916, u0916_Delta_Uppercase );
  declare_button( button_index++,  931, u0931_Sigma_Uppercase );
  declare_button( button_index++,  928, u0928_Pi_Uppercase );
  declare_button( button_index++,  937, u0937_Omega_Uppercase );

  declare_button( button_index++,  949, u0949_Epsilon_Lowercase );
  declare_button( button_index++,  181, u0181_Mu_Lowercase );
  declare_button( button_index++,  960, u0960_Pi_Lowercase );
  declare_button( button_index++,  952, u0952_Theta_Lowercase );

  #else

  // 2024-08-29 jj5 - these are the buttons that shipped with the original magazine article

  declare_button( button_index++,  176, u0176_Degree );
  declare_button( button_index++,  188, u0188_frac14 );
  declare_button( button_index++,  189, u0189_frac12 );
  declare_button( button_index++,  190, u0190_frac34 );
  declare_button( button_index++,  169, u0169_Copyright );
  declare_button( button_index++,  128, u0128_Euro_Sign );
  declare_button( button_index++,  163, u0163_Pound_Sign );
  declare_button( button_index++,  165, u0165_Yen_Sign );
  declare_button( button_index++,  177, u0177_Plus_Minus );
  declare_button( button_index++,  247, u0247_Division );
  declare_button( button_index++,  133, u0133_Ellipsis );
  declare_button( button_index++,  134, u0134_Dagger );

  #endif

  // 2024-08-29 jj5 - these are other buttons that we generated bitmaps for and tested...
  //
  /*
  declare_button( button_index++,  188, u0188_frac14 );
  declare_button( button_index++, 8531, u8531_frac13 );
  declare_button( button_index++,  189, u0189_frac12 );
  declare_button( button_index++, 8532, u8532_frac23 );
  declare_button( button_index++,  190, u0190_frac34 );
  declare_button( button_index++,  185, u0185_Superscript_1 );
  declare_button( button_index++,  178, u0178_Superscript_2 );
  declare_button( button_index++,  179, u0179_Superscript_3 );
  declare_button( button_index++,  169, u0169_Copyright );
  declare_button( button_index++,  174, u0174_Registered_Trade_Mark );
  declare_button( button_index++,  153, u0153_Trade_Mark );
  declare_button( button_index++, 8481, u8481_Telephone );
  */
  /*
  declare_button( button_index++,  176, u0176_Degree );
  declare_button( button_index++, 8451, u8451_Degree_Celsius );
  declare_button( button_index++, 8457, u8457_Degree_Fahrenheit );
  declare_button( button_index++,  167, u0167_Section );
  declare_button( button_index++,  191, u0191_Upside_Down_Question_Mark );
  declare_button( button_index++, 8469, u8469_Set_Natural );
  declare_button( button_index++, 8484, u8484_Set_Integer );
  declare_button( button_index++, 8474, u8474_Set_Rational );
  declare_button( button_index++, 8477, u8477_Set_Real );
  declare_button( button_index++, 8450, u8450_Set_Complex );
  declare_button( button_index++,  215, u0215_Multiplication );
  declare_button( button_index++,  247, u0247_Division );
  */
  /*
  declare_button( button_index++,  177, u0177_Plus_Minus );
  declare_button( button_index++,  172, u0172_Not );
  declare_button( button_index++, 8960, u8960_Diameter );
  declare_button( button_index++,  128, u0128_Euro_Sign );
  declare_button( button_index++,  163, u0163_Pound_Sign );
  declare_button( button_index++,  165, u0165_Yen_Sign );
  declare_button( button_index++, 8383, u8383_Bitcoin_Sign );
  declare_button( button_index++,  162, u0162_Cent_Sign );
  declare_button( button_index++, 8369, u8369_Peso_Sign );
  declare_button( button_index++,  913, u0913_Alpha_Uppercase );
  declare_button( button_index++,  945, u0945_Alpha_Lowercase );
  declare_button( button_index++,  914, u0914_Beta_Uppercase );
  */
  /*
  declare_button( button_index++,  946, u0946_Beta_Lowercase );
  declare_button( button_index++,  915, u0915_Gamma_Uppercase );
  declare_button( button_index++,  947, u0947_Gamma_Lowercase );
  declare_button( button_index++,  916, u0916_Delta_Uppercase );
  declare_button( button_index++,  948, u0948_Delta_Lowercase );
  declare_button( button_index++,  917, u0917_Epsilon_Uppercase );
  declare_button( button_index++,  949, u0949_Epsilon_Lowercase );
  declare_button( button_index++,  918, u0918_Zeta_Uppercase );
  declare_button( button_index++,  950, u0950_Zeta_Lowercase );
  declare_button( button_index++,  919, u0919_Eta_Uppercase );
  declare_button( button_index++,  951, u0951_Eta_Lowercase );
  declare_button( button_index++,  920, u0920_Theta_Uppercase );
  */
  /*
  declare_button( button_index++,  952, u0952_Theta_Lowercase );
  declare_button( button_index++,  921, u0921_Iota_Uppercase );
  declare_button( button_index++,  953, u0953_Iota_Lowercase );
  declare_button( button_index++,  922, u0922_Kappa_Uppercase );
  declare_button( button_index++,  954, u0954_Kappa_Lowercase );
  declare_button( button_index++,  923, u0923_Lambda_Uppercase );
  declare_button( button_index++,  955, u0955_Lambda_Lowercase );
  declare_button( button_index++,  924, u0924_Mu_Uppercase );
  declare_button( button_index++,  181, u0181_Mu_Lowercase );
  declare_button( button_index++,  925, u0925_Nu_Uppercase );
  declare_button( button_index++,  957, u0957_Nu_Lowercase );
  declare_button( button_index++,  926, u0926_Xi_Uppercase );
  */
  /*
  declare_button( button_index++,  958, u0958_Xi_Lowercase );
  declare_button( button_index++,  927, u0927_Omicron_Uppercase );
  declare_button( button_index++,  959, u0959_Omicron_Lowercase );
  declare_button( button_index++,  928, u0928_Pi_Uppercase );
  declare_button( button_index++,  960, u0960_Pi_Lowercase );
  declare_button( button_index++,  929, u0929_Rho_Uppercase );
  declare_button( button_index++,  961, u0961_Rho_Lowercase );
  declare_button( button_index++,  931, u0931_Sigma_Uppercase );
  declare_button( button_index++,  963, u0963_Sigma_Lowercase );
  declare_button( button_index++,  932, u0932_Tau_Uppercase );
  declare_button( button_index++,  964, u0964_Tau_Lowercase );
  declare_button( button_index++,  933, u0933_Upsilon_Uppercase );
  */
  /*
  declare_button( button_index++,  965, u0965_Upsilon_Lowercase );
  declare_button( button_index++,  934, u0934_Phi_Uppercase );
  declare_button( button_index++,  966, u0966_Phi_Lowercase );
  declare_button( button_index++,  935, u0935_Chi_Uppercase );
  declare_button( button_index++,  967, u0967_Chi_Lowercase );
  declare_button( button_index++,  936, u0936_Psi_Uppercase );
  declare_button( button_index++,  968, u0968_Psi_Lowercase );
  declare_button( button_index++,  937, u0937_Omega_Uppercase );
  declare_button( button_index++,  969, u0969_Omega_Lowercase );
  declare_button( button_index++, 1488, u1488_Alef );
  declare_button( button_index++, 1488, u1488_Alef );
  declare_button( button_index++, 1488, u1488_Alef );
  */

  // 2024-08-30 jj5 - if the button index is not BUTTON_COUNT then we have a problem... we should initialise exactly
  // BUTTON_COUNT buttons (twelve)...
  //
  if ( button_index != BUTTON_COUNT ) {

    warn( "button_index != BUTTON_COUNT" );

  }

  log( "Initialised." );

}

void loop() {

  // 2024-08-29 jj5 - update the timer...
  //
  now = micros();

  // 2024-08-29 jj5 - for each button...
  //
  for ( int button_index = 0; button_index < BUTTON_COUNT; button_index++ ) {

    // 2024-08-30 jj5 - figure out if this button is currently pressed or not by measuring the touch on the touch screen...
    //
    bool pressed = is_pressed( button_index );

    // 2024-08-30 jj5 - figure out how long this button has been pressed for (if it is pressed)...
    //
    unsigned long duration = now - button[ button_index ].waiting_since;

    if ( button[ button_index ].pressed && ! pressed ) {

      // 2024-08-29 jj5 - the button was pressed, but now it's not, so that's a keyup event

      if ( duration > MIN_PRESS ) {

        // 2024-08-29 jj5 - the button was down at least MIN_PRESS microseconds, so unhighlight the key and send the
        // alt-key combination, this keypress event is finished.

        highlight_off( button_index );

        send_alt_code( button[ button_index ].alt_code );

        log_int( "keyup sent for button '%d'.", button_index );

      }
      else {

        // 2024-08-29 jj5 - the button wasn't pressed for long enough so we will ignore it.

        log_int( "keyup aborted for button '%d'.", button_index );

      }
    }

    button[ button_index ].pressed = pressed;

    if ( ! pressed ) {

      // 2024-08-30 jj5 - the button is not pressed so update its age and make sure it's not highlighted.

      button[ button_index ].waiting_since = now;

      highlight_off( button_index );

    }
    else if ( duration > MIN_LIGHT ) {

      // 2024-08-30 jj5 - the button has been pressed for long enough for us to highlight the button, so do that now.

      highlight_on( button_index );

    }
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2024-08-29 jj5 - functions...
//

void declare_button( int button_index, int alt_code, const unsigned char* bitmap ) {

  if ( button_index < 0 ) {

    warn( "button_index < 0" );

    return;

  }

  if ( button_index >= BUTTON_COUNT ) {

    warn( "button_index >= BUTTON_COUNT" );

    return;

  }

  button[ button_index ].alt_code       = alt_code;
  button[ button_index ].bitmap         = bitmap;
  button[ button_index ].x              = calc_x( button_index, bitmap );
  button[ button_index ].y              = calc_y( button_index, bitmap );
  button[ button_index ].width          = calc_w( bitmap );
  button[ button_index ].height         = calc_h( bitmap );
  button[ button_index ].pressed        = false;
  button[ button_index ].waiting_since  = now;

  // 2024-08-29 jj5 - pretend the light is on...
  //
  button[ button_index ].light_on = true;

  // 2024-08-29 jj5 - initialise the button on the screen by turning the light off (this will work because we
  // pretend it is on above)...
  //
  highlight_off( button_index );

}

void highlight_off( int button_index ) {

  // 2024-08-29 jj5 - if the light is already off just return...
  //
  if ( ! button[ button_index ].light_on ) { return; }

  // 2024-08-29 jj5 - record that the light is off...
  //
  button[ button_index ].light_on = false;

  // 2024-08-29 jj5 - reset the foreground and background colours to signal release...
  //
  XC4630_mcimage( button[ button_index ].x, button[ button_index ].y, button[ button_index ].bitmap, FGC, BGC );

}

void highlight_on( int button_index ) {

  // 2024-08-29 jj5 - if the light is already on just return...
  //
  if ( button[ button_index ].light_on ) { return; }

  // 2024-08-29 jj5 - record that the light is on...
  //
  button[ button_index ].light_on = true;

  // 2024-08-29 jj5 - invert the foreground and background colours to signal a touch...
  //
  XC4630_mcimage( button[ button_index ].x, button[ button_index ].y, button[ button_index ].bitmap, BGC, FGC );

}

void send_alt_code( int alt_code ) {

  //Serial.println( alt_code );

  // 2024-08-30 jj5 - press the left alt key. it will stay pressed while we send the four keypad codes which follow...
  //
  Keyboard.press( KEY_LEFT_ALT );
  delay( 1 );

  // 2024-08-30 jj5 - send the first digit of the alt code...
  //
  Keyboard.write( keypad_keys[ ( alt_code / 1000 ) % 10 ] );
  delay( 1 );

  // 2024-08-30 jj5 - send the second digit of the alt code...
  //
  Keyboard.write( keypad_keys[ ( alt_code / 100 ) % 10 ] );
  delay( 1 );

  // 2024-08-30 jj5 - send the third digit of the alt code...
  //
  Keyboard.write( keypad_keys[ ( alt_code / 10 ) % 10 ] );
  delay( 1 );

  // 2024-08-30 jj5 - send the fourth digit of the alt code...
  //
  Keyboard.write( keypad_keys[ ( alt_code / 1 ) % 10 ] );
  delay( 1 );

  // 2024-08-30 jj5 - release the left alt key...
  //
  Keyboard.releaseAll();

}

int calc_x( int button_index, const unsigned char* button_bitmap ) {

  return ( button_index % COLUMNS ) * BUTTON_WIDTH + ( BUTTON_WIDTH - calc_w( button_bitmap ) ) / 2;

}

int calc_y( int button_index, const unsigned char* button_bitmap ) {

  return ( button_index / COLUMNS ) * BUTTON_HEIGHT + ( BUTTON_HEIGHT - calc_h( button_bitmap ) ) / 2;

}

int calc_w( const unsigned char* button_bitmap ) {

  return XC4630_imagewidth( button_bitmap );

}

int calc_h( const unsigned char* button_bitmap ) {

  return XC4630_imageheight( button_bitmap );

}

bool is_pressed( int button_index ) {

  int touch_x = XC4630_touchx();

  if ( touch_x < button[ button_index ].x                                 ) { return false; }
  if ( touch_x > button[ button_index ].x + button[ button_index ].width  ) { return false; }

  int touch_y = XC4630_touchy();

  if ( touch_y < button[ button_index ].y                                 ) { return false; }
  if ( touch_y > button[ button_index ].y + button[ button_index ].height ) { return false; }

  return true;

}

void warn( const char* msg ) {

  Serial.print( "WARNING: " );
  Serial.println( msg );

}

void log_int( const char* format_string, int n ) {

  char buffer[ 150 ];

  if ( strlen( format_string ) > 100 ) {

    // 2024-08-31 jj5 - we don't want to overflow our buffer so if our format string is too long we just warn and return...

    warn( "cowardly refusing to format string longer than 100 chars." );

    return;

  }

  sprintf( buffer, format_string, n );

  Serial.println( buffer );

}

void log( const char* line ) {

  Serial.println( line );

}
