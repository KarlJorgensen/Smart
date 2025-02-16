#!/usr/bin/env python3
"""Stupidly simple specialised SVG parser to extract text box positions from an SVG.

The output will be a fragment of C++ code which defines an array of
Box structures.

Boxes are expected to be <rect> elements with an inkscape:label which
begins with "Box".

The boxes will be emitted in "depth order" - the bottom one first, and
the top one last. Thus the depth ordering indicates the relative
preference for boxes.

Rounded corners will be ignored.

The idea is to minimise the amount of work which needs to be done at
run-time: The watch is not that powerful.

"""

# See also: https://developer.mozilla.org/en-US/docs/Web/SVG/Tutorial/Basic_Shapes#rectangle

# This file is part of Smart.

# Smart is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your
# option) any later version.

# Smart is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
# more details.

# You should have received a copy of the GNU General Public License along
# with Smart. If not, see <https://www.gnu.org/licenses/>.


import argparse
import dataclasses
import math
from xml.dom import minidom

CENTER_X = 100
CENTER_Y = 100

def round(x: str) -> int:
    return int(float(x) + 0.5)

def normalise(angle: float) -> float:
    angle = math.remainder(angle, 2*math.pi)
    if angle < 0:
        angle += 2*math.pi
    return angle

def angle_from_center(x: int, y: int) -> float:
    return normalise(math.atan2(y-CENTER_Y, x-CENTER_X))

def sign(x: float) -> int:
    if x > 0: return 1
    if x < 0: return -1
    return 0

def toDeg(radians: float) -> float:
    return 180 * radians/math.pi


@dataclasses.dataclass
class Rectangle:
    x: int
    y: int
    width: int
    height: int
    name: str

    @property
    def x2(self):
        return self.x + self.width

    @property
    def y2(self):
        return self.y + self.height

    def __post_init__(self):
        if (sign(self.x - CENTER_X) != sign(self.x2 - CENTER_X)
            and sign(self.y - CENTER_Y) != sign(self.y2 - CENTER_Y)):
            raise ValueError(f'Invalid box: covers the centre: {self}')

    @classmethod
    def from_svg(cls, element):
        return cls(name=element.getAttribute('inkscape:label'),
                   x=round(element.getAttribute('x')),
                   y=round(element.getAttribute('y')),
                   width=round(element.getAttribute('width')),
                   height=round(element.getAttribute('height')))

    def center(self):
        return (self.x + self.width/2, self.y + self.height/2)

    def to_box(self):
        # C uses curly brackets and so does python f-strings... so
        # easier to read if we just pretend to use something else
        # (square brackets []) and replace them at the end.
        min_angle, max_angle = self.angles()
        return f'["{self.name}", [{self.x}, {self.y}], [{self.x2}, {self.y2}],' \
            f' [{toDeg(min_angle):.1f}, {toDeg(max_angle):.1f}]]' \
            .replace('[', '{').replace(']', '}')

    def angles(self):
        """Calculate the exclusion angles"""

        # Somewhat complicated by the fact that the box may cover the
        # 0° angle (=0 radians) at 3 o'clock
        angles = [angle_from_center(x, y)
                  for x, y in [(self.x, self.y),
                               (self.x2, self.y),
                               (self.x2, self.y2),
                               (self.x, self.y2)]]
        if sign(self.y-CENTER_Y) == sign(self.y2-CENTER_Y):
            # If the box does not cross the center in the Y-direction,
            # then it is somewhat easy
            return (min(angles), max(angles))

        return (min(a for a in angles if a > PI), max(a for a in angles if a < PI))

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('svg_file',
                        type=str,
                        default='/dev/stdin')
    args = parser.parse_args()

    doc = minidom.parse(args.svg_file)  # parseString also exists

    # Extract the rectangles
    svgRects = [path
                for path in doc.getElementsByTagName('rect')
                if path.getAttribute('inkscape:label').startswith('Box')]
    rects = [Rectangle.from_svg(rect)
             for rect in svgRects]
    doc.unlink()

    print(f"""
// Generated from {args.svg_file}
//
// Do not modify this file directly - it will be overwritten.
//
// Instead: Modify "{args.svg_file}" and run `make headers`
//""")
    print("""
const Box boxes [] =
  {
""")

    # indent the box data by 4 spaces. Looks nice.
    print(',\n'.join(['    '+rect.to_box() for rect in rects]))
    print("""
   };

#define num_boxes (sizeof(boxes)/(sizeof(Box)))

/* Local Variables: */
/* compile-command: "make headers" */
/* End: */
""")

if __name__ == '__main__':
    main()
