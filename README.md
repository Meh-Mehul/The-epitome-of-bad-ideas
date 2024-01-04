# The-epitome-of-bad-ideas
This is a project for a webserver hosted on an ESP8266 hosting three websites for doing various things, like chatting, image upload, game.
## Why?
Because i had a lot of time at hand and kinda wanted to start learning about backend stuff too, this is bascially a very scaled down small version of an insecure webserver working via socket protocol
## How to set up?
Just put in ur credentials for WiFi (its preffered that the WiFi is not a public one or an institution's), find the IP of the webserver (if connected to a phone hotspot one can see it in thier phone settings).
### NOTE:
The functionalities of chatting and image flashing only works when both clients have loaded their websites and the time of request, if any connection problem occurs try refreshing and not, due to its small memory limits, we cant store much info on the server, so all things are lost on refreshing the page. (This might be fixed with better chips or moving to an actual webserver altogether)
## Uploading the code
Now, just give the ESP8266 the code and power and the webserver is set up and can be accessed by anyone in that network provided that they know the right IP
Here's how the websites will look after they are set up throught ESP8266
<img width="1280" alt="anonymous" src="https://github.com/Meh-Mehul/The-epitome-of-bad-ideas/assets/146803848/da79f306-ad93-4f60-9154-34abfe71b56d">
<img width="1280" alt="image" src="https://github.com/Meh-Mehul/The-epitome-of-bad-ideas/assets/146803848/6d20e3c9-4e23-4d1e-abb2-9512a118513a">
<img width="1280" alt="game" src="https://github.com/Meh-Mehul/The-epitome-of-bad-ideas/assets/146803848/38bdc09b-00bd-4ec2-ad99-86a8f50491f5">
## What's Next
If you carefully see the ```final.ino``` file, you will see the use of ```EEPROM.h```, Althought its not being utilized properly. Moving forward, if i get time again, i will see to making image storage capabilities on this. Due to the small size of the EEPROM of ESP8266 i can only store one image at best, so i could implement it, i definitely will. One may host some authorization webiste for a small company or something like that too, that doesnt require much of the server's resources and memory.
