# LightMyFire
Arduino sketch do demo interactivity with Google Firebase


### Wifi
Make sure you put your wifi ssid, password, firebase host and db key in a separate file 'secrets.h', which is gitignored.

```
/*
Contains ssid and other secret info
which should not appear in github
*/

#define FIREBASE_HOST "###" //Do not include https:// in FIREBASE_HOST
// this is NOT the firebase key, but the database secret!
#define FIREBASE_AUTH "###"

// the following are two arrays of wifi SSIDs and their passwords. Too much hassle to set this up with proper structs in C++...
String wifiSSID[2]     = {"ssid1", "ssid2"};
String wifiPassword[2] = {"pw1", "pw2"};

// as C++ is totally oblivious of the size of the above arrays, you have to set that here manually.
#define wifiCount 2

```