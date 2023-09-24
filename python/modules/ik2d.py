import modules.serial_io as serial_io

import math
import time


class ikSolver2D:
    def __init__(self, l1, w):
        self.l = l1
        self.w = w

    def solve(self, pos: tuple) -> tuple:  # Returns (a1, a2) in degrees or None if out of reach
        try:
            # Unpack pos
            x, y = pos

            # Calculate a1 and a2

            # q_{1}=\arccos\left(\frac{2l^{2}-\left(x_{1}+\frac{w}{2}\right)^{2}-y_{1}^{2}}{2l^{2}}\right)
            # q_{2}=\arccos\left(\frac{2l^{2}-\left(x_{1}-\frac{w}{2}\right)^{2}-y_{1}^{2}}{2l^{2}}\right)
            b1 = math.acos(
                (
                    2*self.l**2 -
                    (x + self.w/2)**2 -
                    y**2
                ) /
                (2*self.l**2)
            )
            b2 = math.acos(
                (
                    2*self.l**2 -
                    (x - self.w/2)**2 -
                    y**2
                ) /
                (2*self.l**2)
            )

            # \alpha_{1}=\pi-\arctan\left(\frac{x_{1}+\frac{w}{2}}{y_{1}}\right)-\arctan\left(\frac{l\sin\left(q_{1}\right)}{l+l\cos\left(q_{1}\right)}\right)
            # \alpha_{2}\ =\ -\arctan\left(\frac{x_{1}-\frac{w}{2}}{y_{1}}\right)+\arctan\left(\frac{l\sin\left(q_{2}\right)}{l+l\cos\left(q_{2}\right)}\right)
            a1 = (
                math.pi -
                math.atan((x + self.w/2)/y) -
                math.atan(
                    (self.l*math.sin(b1)) /
                    (self.l + self.l*math.cos(b1))
                )
            )
            a2 = (
                -math.atan((x - self.w/2)/y) +
                math.atan(
                    (self.l*math.sin(b2)) /
                    (self.l + self.l*math.cos(b2))
                )
            )
        except ValueError:
            return (None, None)

        # convert all to degrees
        a1 = math.degrees(a1)
        a2 = math.degrees(a2)
        b1 = math.degrees(b1)
        b2 = math.degrees(b2)

        # check if gamma angle is within bounds
        if (360 - a1 - (180-a2) - b1 - b2) > -20:
            return (None, None)

        return (a1, a2)

    def linterp(self, start_pos: tuple, end_pos: tuple, delta_t: int, freq: int) -> None:
        x0, y0 = start_pos
        x1, y1 = end_pos

        prev_a1, prev_a2 = self.solve((x0, y0))

        for i in range(int(delta_t*freq)):
            x = x0 + (x1 - x0)*i/(delta_t*freq)
            y = y0 + (y1 - y0)*i/(delta_t*freq)

            a1, a2 = self.solve((x, y))

            da1_dt = (a1 - prev_a1)/(delta_t*freq)  # degrees per second
            da2_dt = (a2 - prev_a2)/(delta_t*freq)  # degrees per second

            # report values
            print(x, y, a1, a2, da1_dt, da2_dt, sep=', ')

            # record angles
            prev_a1 = a1
            prev_a2 = a2

            time.sleep(1/freq)


if __name__ == '__main__':
    l = 96
    w = 47

    solver = ikSolver2D(l, w)

    x0 = float(input('X0: '))
    y0 = float(input('Y0: '))

    a1, a2 = solver.solve((x0, y0))

    print(f'Angles: {a1}, {a2}')
