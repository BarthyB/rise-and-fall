
        //
//  NoteLengthSlider.h
//  RISE&FALL
//
//  Created by Barthélémy Bonhomme on 11.06.18.
//  Copyright © 2018 Barthy. All rights reserved.
//

#ifndef NoteLengthSlider_h
#define NoteLengthSlider_h

#include "../JuceLibraryCode/JuceHeader.h"
#include <stdlib.h>
#include <math.h>

class NoteLengthSlider : public Slider {

  String getTextFromValue(double value) override {
      short val = static_cast<int>(value);
      String textValue = String(pow(2, abs(val)));

      if (value < 0){
          return "1/" + textValue;
      } else {
          return textValue;
      }
  }
};

#endif /* NoteLengthSlider_h */
