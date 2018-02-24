/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   soundtouch_config_h_in;
    const int            soundtouch_config_h_inSize = 131;

    extern const char*   background_png;
    const int            background_pngSize = 23230;

    extern const char*   minster_wav;
    const int            minster_wavSize = 2880044;

    extern const char*   room_impulse_response_LBS_wav;
    const int            room_impulse_response_LBS_wavSize = 270044;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 4;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}
