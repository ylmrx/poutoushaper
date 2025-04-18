# poutoushaper

A waveshaper for the comrades

## Control

- `Shape`: Pick a socdem softclipper, a tanky hardclipper, or a liberal atan clipper.
- `Gain`: How much of the capital will you redistribute ?

## Sound

It can go louder than the builtin boost.

## How to build

Since Maximilian currently has some minor issues to use with logue SDK, use the patched fork of Maximilian, which I am providing at:

https://github.com/boochow/Maximilian

Place these repositories under logue-sdk/platforms/drumlogue/ like this:

```text
drumlogue/
├── common
├── Maximilian
└── poutoushaper
```

then type

```sh
../../docker/run_cmd.sh build drumlogue/poutoushaper
```

and copy maxisynth/maxisynth.drmlgunit to your drumlogue.
