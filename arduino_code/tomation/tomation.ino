/**************************************************************************/
/*!
    @file     tomatino_Arduino
    @author   Encore Lab
    Dependencies: Adafruit Adafruit_NFCShield_I2C Library at: https://github.com/adafruit/Adafruit_NFCShield_I2C
    Adapted from example code at https://github.com/adafruit/Adafruit_NFCShield_I2C/tree/master/examples/readMifare
    For use with the adafruit NFC/RFID Controller Shield for Arduino: https://www.adafruit.com/products/789

    This example will wait for any ISO14443A card or tag, and
    depending on the size of the UID will attempt to read from it.

    If the card has a 4-byte UID it is probably a Mifare
    Classic card, and the following steps are taken:

    - Authenticate block 4 (the first block of Sector 1) using
      the default KEYA of 0XFF 0XFF 0XFF 0XFF 0XFF 0XFF
    - If authentication succeeds, we can then read any of the
      4 blocks in that sector (though only block 4 is read here)

    If the card has a 7-byte UID it is probably a Mifare
    Ultralight card, and the 4 byte pages can be read directly.
    Page 4 is read by default since this is the first 'general-
    purpose' page on the tags.
*/
/**************************************************************************/

#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include "Tomato.h"

#define SERIAL_BAUDRATE 9600

#define IRQ   (2)
#define RESET (3)  // Not connected by default on the NFC Shield

int incomingByte = 0;
const int LED = 8;
boolean cardSeenBefore = false;

Adafruit_NFCShield_I2C nfc(IRQ, RESET);

// instantiate a RockPaperAwesome state machine
Tomato tomato = Tomato();

/*******************************************************************/
/** EVENT HANDLERS *************************************************/
/*******************************************************************/

// triggered when you're present and ready to play a game
void when_you_are_present() {
  // light up green light
}


// triggered when the other player is present and ready to play a game
void when_partner_is_present(String opponentName) {
  // blink green light
}

// triggered when some sort of serious error occurs (usually requiring a game reset)
void when_error() {
  //blink(R, 5, 600);
  // blink red on error?
}


/*******************************************************************/
/** SETUP AND MAIN LOOP ********************************************/
/*******************************************************************/
void setup(void) {
  Serial.begin(SERIAL_BAUDRATE);
  pinMode(LED, OUTPUT);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board, please try again.");
    while (1); // halt
  }
  
  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);

  // configure board to read RFID tags
  nfc.SAMConfig();

  //Serial.println("Waiting for an ISO14443A Card ...");
  Serial.println("< ready for pair tomato");
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
//  Serial.print('debug');
//  Serial.print(success);
//  Serial.println(cardSeenBefore);

  if (success && !cardSeenBefore) {
    // Display some basic information about the card
    
    // ok this is not what I want. I want to call a function and pass the card ID. Suggestions?
    Serial.print("< rfid_detected ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    
    // remember that we just read card and sent a message
    cardSeenBefore = true;
    
    delay(1000); //wait a bit before trying to read again.
  } else if (success && cardSeenBefore) {
    // read card succesfully but already sent message
    // nothing to do just sight tight and look good
  } else if (!success && cardSeenBefore) {
    // no card read, but saw one in last cycle --> card was removed
    tomato.abort_tomato();
   
    // card removed so adjust
    cardSeenBefore = false;
    
    delay(1000); //wait a bit before trying to read again.
  } else if (!success && !cardSeenBefore) {
    // nothing to do just sight tight and look good
  }
  
//  Serial.println(success);

  if (Serial.available() > 0) {
    incomingByte = Serial.read();

    // say what you got:
    //Serial.println(incomingByte, DEC);

    /* Do action corresponding to signals */
    digitalWrite(LED, HIGH);
    delay(2000);
    digitalWrite(LED, LOW);
  }
}

/***********************************************************************/
/** HELPER FUNCTIONS ***************************************************/
/***********************************************************************/

// here go functions to turn light off - mostly called from even handlers


