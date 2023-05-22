# shell-rc
At the Shell gas station, I grabbed some cheap Bluetooth RC cars during a promo by collecting points. I was supposed to drive them with a smartphone app, but the touch controller was really hard to use, and ineffective, and the APP is pretty bad overall. Since Stadia shut down in 2023, and I had controllers laying around, I decided to use the controller instead. Google was awesome and released a firmware update, so the controller functions as a [bluetooth controller.](https://stadia.google.com/controller/)

## Available cars
![car1](/cars/488-challenge-evo.png) |![car1](/cars/FXX-K-EVO.png) | ![car1](/cars/SF1000-Tuscan-GP-Ferrari-1000.png) | ![car1](/cars/SL-488%20GTE.png)
:--------:|:--------:|:--------:|:---------
SL-488 CHALLENGE Evo | SL-FXX-K Evo | SL-SF1000 | L-488 GTE

The cars have the following features:
* throttle forward/backward
* steering left/right
* manual steering correction at the back of the car
* turbo mode (it really goes faster)
* turn the highlights on/off
## Controller layout
![controller_layout](https://user-images.githubusercontent.com/96885207/215869592-64300917-0e33-4351-b163-09addbdf1487.png)

## Usage
<img src="https://user-images.githubusercontent.com/96885207/215851652-4090a1ca-239e-4263-9931-8a3c9e96c3f7.png" align="right"/>
To control the car, use the left stick for throttle and the right stick for steering. Gradually increase throttle with the left stick. Turn on or off this feature using the toggle direct drive switch. Give it a turbo boost by pressing the left trigger (L2). Just a heads up, the wheels will spin, so be prepared to make some donuts! 😊

You can use multiple controllers and cars, just move the cars up and down in the list to pair them. If you need to identify the controllers, look for the red flashing indicator on the corresponding one. To flash the car's headlights, press the Flash button in the list. If you don't have a controller, you can use the virtual joystick and keyboard.
<br clear="right"/>

## Technologie
I was using Qt 5.15.2 and MSVC2019 64bit compiler.

I used the [QJoysticks](https://github.com/alex-spataru/QJoysticks) library to interface with the game controllers
 
The message protocol over bluetooth was [reverse engineerd](https://gist.github.com/scrool/e79d6a4cb50c26499746f4fe473b3768) earlier by others for the Brandbase Nissan Formula RC car series. The communcation was encrypted there.

However communication for these models are not encrypted. I based my work on [this implementation](https://github.com/martonmiklos/sailfish-ble-rc) of an APP for sailfish OS.

[Conthrax Font Free](https://www.freefonts.io/conthrax-font-free-download)

Andoid app is coming soon! :)

## Binaries
Here You can find an istaller for Windows x64. Install the redistributable package vc_redist.x64.exe if necessary.
https://drive.google.com/file/d/16c5UyQQHvu-Hr1ywa6I8n9Ah5otda881/view?usp=sharing
