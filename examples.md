# Fun Examples

This file has some fun examples and their generator functions. Most were stumbled
upon by just re-running the tool a bunch.

## Chrono Trigger

`rgb(div(sub(cos(sub(y, x)), y), y), add(div(div(mul(x, y), add(y, x)), mul(mul(0.395887, x), mod(y, y))), add(sin(sin(sin(y))), div(mul(x, y), cos(0.0271352)))), add(y, if(>=(add(y, y), x), sub(x, y), cos(x))))`

<center><img alt="image resembling Chrono Trigger's time travel effect" src="examples/chrono_trigger.png"></center>

## Geometric

`rgb(add(mod(rgb(div(-0.233838, mul(y, y)), div(mod(y, 0.0536739), y), rgb(x, y, div(rgb(cos(y), x, sin(y)), mul(y, -0.0782416)))), sub(y, add(x, 0.0102951))), add(0.3484, add(add(x, y), add(y, x)))), add(sub(rgb(mod(mod(y, y), mul(y, y)), 0.37293, y), div(mod(x, 0.678807), div(x, y))), add(sin(if(<=(y, y), y, y)), mul(x, mod(y, y)))), y)`

<center><img alt="image composed of satisfying geometric shapes" src="examples/geometric.png"></center>

## Fields

`rgb(add(add(mod(sub(y, x), y), mod(add(y, y), div(x, x))), y), sub(sub(if(>(y, x), y, y), div(rgb(sub(y, y), mul(-0.821135, 0.713009), mul(y, y)), mul(y, y))), add(mod(sub(y, -0.327699), y), sub(div(x, x), y))), mul(cos(y), -0.702842))`

<center><img alt="image resembling sprawling triangular fields" src="examples/fields.png"></center>

## Four Corners

`rgb(add(y, rgb(sub(cos(mod(x, 0.357165)), y), sub(div(y, 0.405363), sin(-0.880661)), div(-0.125798, sub(add(-0.0566517, y), y)))), mul(x, y), rgb(mod(cos(y), add(x, x)), mod(y, y), y))`

<center><img alt="image of a four-corner gradient" src="examples/four_corners.png"></center>

## Vaporwave

`f(x, y) = (rgb (sub (abs (exp (fract (abs (log x))))) 0.152519) (sqrt (mix (sqrt (min -0.54487 -0.0263508)) (exp (max -0.537712 x)) (cos (mod y -0.110695)))) (max (max (sqrt (sub x y)) (min (cos x) (abs -0.639572))) (sin (abs(log (fract (add x y)))))))`

<center><img alt="image that's kind of 3D, in blues and reds" src="examples/vaporwave.png"></center>

## Giygas

`f(x, y) = (rgb (sub (mod (sub (mul 0.947441 y) (add x 0.261177)) (mod (sub 0.16587 x) (div 0.93128 x))) (div (sub y (sub x 0.120607)) (mul (mod 0.955135 y) (sub 0.535825 x)))) (mul (add (sub (div y x) (add 0.561929 x)) (mod (sub y 0.662759) x)) x) y)`

<center><img alt="trippy GIF that looks kind of like Giygas (from Earthbound,) bursting out of itself in brilliant green rectangles" src="examples/gygas.gif"></center>

## Collapse

`f(x, y) = (rgb (mod (add (div (div x 0.211992) (div x y)) y) y) (div 0.122605 (add (add (mul x 0.806234) (add x y)) y)) (add (sub (sub (sub 0.295714 y) (div 0.934116 y)) y) (add y (mul (mod x x) (mul x y)))))`

<center><img alt="GIF with collapsing, merging, and shifting RGB waves under a chaotic rain of glitchy, vertical stripes" src="examples/collapse.gif"></center>

## Stroke

`f(x, y) = (rgb (sub (sub x (div (mul y 0.179603) (add y 0.0190984))) (div (sub (add x 0.405067) 0.0136063) (sub x (rgb y x 0.264116)))) (mod (sub (div (add 0.832435 x) (div y y)) (mul 0.711419 0.825349)) (sub (add y (mod 0.203747 x)) (mul (sub x y) (mix 0.894777 0.361406 y)))) (mul 0.343404 (mod (mul (add y 0.089752) (sub x 0.198278)) (div (add y y) (add x 0.119672)))))`

<center><img alt="image of asymptotical curves, underlined by a vector-like splatter, spreading eastward, like a graph having a stroke" src="examples/stroke.png"></center>

## Rainbow

`f(x, y) = (rgb (sin (sin (mod (cos (cos (div y y))) x))) (sin (fract (sin (cos (sin x))))) (cos (sin (cos (cos (add (add 0.249185 x) (sin x)))))))`

<center><img alt="image of vertical lines coalescing into a semi-rainbow in the middle (or, more accurately: Coldplay's 'X&Y' album cover)" src="examples/rainbow.png"></center>

## Starfighter

`f(x, y) = (rgb (exp (if (> (cos (fract y)) 0.638717) (sin (sin x)) (sin (fract 0.865973)))) (cos (cos (exp (cos (sin (fract rnd)))))) (cos (rgb (exp (if (!= x y) y x)) (sqrt (sin (sqrt 0.260944))) (sin (sin (cos 0.508175))))))`

<center><img alt="image of the sort of decal you'd see on the side of something named a 'starfighter,' or 'cosmic freighter.' Two red, fading, parallel horizontal lines against a blue background. To their left is a strange, grooved pattern in green. A layer of subtle noise coats the image, as if subjected to film grain, or resembling scratches from cosmic debris." src="examples/rainbow.png"></center>

## Less

`f(x, y) = (rgb (mod (add (sub y 0.288258) (div (div y rnd) (mod 0.334164 0.238114))) (div (div (mod y 0.700225) (sub x y)) (div (sub 0.212497 x) (sub 0.792568 0.743033)))) (div (div (sub (div y 0.765275) (div 0.870307 0.155784)) x) (mul (mod (add y 0.886776) (sub x 0.614713)) (div (div y rnd) (sub y rnd)))) (sub (sub (sub (mul x y) (sub y 0.206614)) (sub (mul x y) (sub 0.935633 rnd))) (mod (mul (mul y x) (sub y 0.640864)) (mod (mod 0.984834 x) (add y x)))))`

<center><img alt="image resembling the cover art for Pink Floyd's album 'More;' a vague image of something like a satellite dish covered in noise" src="examples/less.png"></center>

## Serpinski

`f(x, y) = (rgb x (add (mul y (add (mod 0.38666 x) (sub 0.309787 y))) 0.655852) (mod (mod (mod (add x y) (mod x 0.165794)) (div (mul x x) (sub x 0.938978))) (div (mod (sub 0.489531 0.0436554) y) (add y 0.948497))))`

<center><img alt="image of a graph having an existential crisis" src="examples/serpinski.png"></center>

## Rave Fireworks

`f(x, y) = (rgb (mod (mul x x) (mod (sub (div 0.202347 x) (add x 0.104789)) (mul (sub x y) (mod 0.577961 0.802299)))) 0.56576 (sub 0.840417 (div (mod (add x x) (div y 0.722084)) (div (mod 0.537801 y) (div 0.193499 x)))))`

<center><img alt="image of the kind of visual you'd see on the big screen behind the DJ at a rave, with blue prisms projected out of the horizon line and red beams and fireworks" src="examples/serpinski.png"></center>

## NATO

`f(x, y) = (rgb (div (mul (mul y x) (mul y x)) 0.371902) y x)`

<center><img alt="image of a colorful compass-looking star" src="examples/nato.png"></center>
