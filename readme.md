# Plant World

The [PlantWorld.ino](PlantWorld.ino) arduino code, was designed to provide our "Plant World" project with 6 seperate states, including 3 transitional states, that respond to outside world sensor data, to direct the behavior of two servo controlled motor mechanisms (a winch operated tall tree and a cam shaft operated forest of trees.)

## Acknowledgments

  - Oscillation code used from: [npucket/arduinoAnimation](https://github.com/npuckett/arduinoAnimation).
  - The building of the prototype used the following online tools:
    - Vertical Servo Bracket: https://www.thingiverse.com/thing:2293613
    - MakerCase: https://en.makercase.com/#/
    - GearGenerator: https://geargenerator.com

## State Machine Flow
![World State Management Flow Chart](/images/WorldState.jpg)
![Tree State Management Flow Chart](/images/TreedState.jpg)
![Cam State Management Flow Chart](/images/CamState.jpg)