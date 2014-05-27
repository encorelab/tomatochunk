#include "Tomato.h"

Tomato::Tomato() {
    inputBuffer = "";
}

void Tomato::signal_ready() {
    out("~");
}

void Tomato::present() {
    out("< present");
}

void Tomato::absent() {
    out("< absent");
}


void Tomato::check_input_from_serial() {
    char c;
    if (Serial.available() > 0) {
        c = Serial.read();
        if (c == '\n') {
            in(inputBuffer);
            inputBuffer = "";
        } else {
            inputBuffer.concat(c);
            // FIXME: what if we exceed inputBuffer's size?
        }
    }
}

// processes an incoming command line from Serial
void Tomato::in(String &str) {
    char input[255];
    str.toCharArray(input, 255);
    char *p = input;
    String ctrl = strtok_r(p, " ", &p);
    String ev = strtok_r(p, " ", &p);
    String arg = strtok_r(p, " ", &p);

    if (ctrl == ">") {
        // we have an incoming event
        if (ev == "you_are_present") {
            when_you_are_present();
        } else if (ev == "partner_is_present") {
            when_partner_is_present(arg); // arg is the name/id of the opponent
        } else if (ev == "error") {
            when_error();
        }
    }

    p = NULL;
}

// sends out a command line over Serial
void Tomato::out(const char *str) {
    Serial.println(str);
}

// sends out a command line over Serial
void Tomato::out(String &str) {
    Serial.println(str);
}