# poutoushaper

A wave shaper for the comrades

Eat the rich.

ARM the homeless.

## Control

- `Shape`:
  - `Soft` a social democrat soft clipper

    > $`f(x) = \frac{2}{3} (x - \frac{x^3}{3})`$

  - `Hard` a tanky hard clipper

    > $`f(x) = \begin{cases}
      -1 & \text{if } x < -1 \\
      x & \text{if } -1 \leq x \leq 1 \\
      1 & \text{if } x > 1
      \end{cases}`$

  - `Atan` a liberal arc tan clipper

    > $`f(x) = \frac{x}{1 + 0.28x^2}`$

  - `PSoft` a progressive soft clipper

    > $`f(x) = x (1 - \frac{|x|}{2})`$

  more visually : https://www.desmos.com/calculator/wkniztgyra

- `Gain`: How much of the capital will you redistribute ?

- `Clip`:
  - Apply the hard clipper after the chosen clipper algorithm
  - Has most effect when Master volume is not at max
  - Does nothing when running the Hard algorithm

## Sound

It can go louder than the built-in boost.

## History

- First version, rely on Maximilian shapers
- Second version, rely on ARM NEON math (no more Maximilian dependencies),
  add a `PSoft` clipper (a different soft clipping curve)

## How to build

Place this repository under `logue-sdk/platforms/drumlogue/` like this:

```text
drumlogue/
├── common
└── poutoushaper
```

then type

```sh
../../docker/run_cmd.sh build drumlogue/poutoushaper
```

And copy `maxisynth/maxisynth.drmlgunit` to your Drumlogue.
