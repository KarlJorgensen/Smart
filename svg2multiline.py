#!/usr/bin/env python3


# See also: https://developer.mozilla.org/en-US/docs/Web/SVG/Tutorial/Paths

import argparse
from xml.dom import minidom

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('svg_file',
                        type=str,
                        default='/dev/stdin')
    args = parser.parse_args()

    doc = minidom.parse(args.svg_file)  # parseString also exists

    # Extract the paths
    path_strings = [(path.getAttribute('id'), path.getAttribute('d'))
                    for path in doc.getElementsByTagName('path')]

    svg = doc.getElementsByTagName('svg')[0]

    width = float(svg.attributes['width'].value)
    height = float(svg.attributes['height'].value)

    # print(f'// {path_strings}')

    print(f'''
// Generated from {args.svg_file}
//
// Do not modify this file directly - it will be overwritten.
//
// Instead: Modify "{args.svg_file}" and run `make headers`
//
''')

    for name, path in path_strings:
        i = iter(path.replace(',', ' ').split(' '))
        points = list()

        def get_coords():
            x = float(next(i))
            y = float(next(i))
            return (x, y)

        def addpoint(x, y):
            if not points:
                points.append((x, y))
                return
            if (x, y)!= points[-1]:
                points.append((x, y))

        try:
            cmd = next(i)
            while True:
                # print(f'// {points=}')

                if cmd == 'm':
                    x, y = get_coords()
                    addpoint(x, y)
                    cmd = next(i)
                    continue

                if cmd == 'c':
                    # Cubic Bezier curve - given in delta format.
                    #
                    # We expect:
                    #   c dx1 dy1 dx2 dy2 dx dy
                    # We don't _really_ do curves. So we ignore the
                    # control points and just pick the end point.
                    _c1 = get_coords()
                    _c2 = get_coords()
                    dx, dy = get_coords()
                    x, y = points[-1]
                    x = x + dx
                    y = y + dy
                    addpoint(x, y)
                    # print(f'// c(a) {points=}')

                    cmd = next(i)
                    while isfloat(cmd):
                        _c1y = next(i)
                        _c2 = get_coords()
                        dx, dy = get_coords()
                        x = x + dx
                        y = y + dy
                        addpoint(x, y)
                        # print(f'// c(b) {points=}')
                        cmd = next(i)
                    continue

                if cmd == 'C':
                    # Cubic Bezier curve - given in absolute format.
                    #
                    # We expect:
                    #   C x1 y1, x2 y2, x y
                    #
                    # We don't _really_ do curves. So we ignore the
                    # control points and just pick the end point.
                    _c1 = get_coords()
                    _c2 = get_coords()
                    x, y = get_coords()
                    addpoint(x, y)
                    # print(f'// c(a) {points=}')

                    cmd = next(i)
                    while isfloat(cmd):
                        _c1y = next(i)
                        _c2 = get_coords()
                        x, y = get_coords()
                        addpoint(x, y)
                        # print(f'// c(b) {points=}')
                        cmd = next(i)
                    continue

                if cmd == 'z':
                    # Close the loop
                    addpoint(points[0][0], points[0][1])
                    cmd = next(i).lower()
                    break

                raise ValueError(f'Now sure how to handle {cmd=}')

        except StopIteration:
            pass

        print(f'static const float {name}[] = ' + '{' + f' // {len(points)} points')
        for x, y in points:
            # Map the coordinates to values between -1 and +1
            x = 2 * (x/width - 0.5)
            y = 2 * (y/height - 0.5)
            print(f'  HANDS_RADIUS * {x}, HANDS_RADIUS * {y},')
        print('};')

    print('''
/* Local Variables: */
/* compile-command: "make headers" */
/* End: */
''')
    doc.unlink()

def isfloat(x):
    try:
        _dummy = float(x)
        return True
    except ValueError:
        return False

if __name__ == '__main__':
    main()
