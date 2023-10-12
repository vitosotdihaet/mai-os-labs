from random import randint


GEN_NUM = 1_000_000

MIN_X, MAX_X = -100, 100
MIN_Y, MAX_Y = -100, 100


def random_coordinate():
    x = (randint(MIN_X, MAX_X), randint(MIN_Y, MAX_Y))
    y = (randint(MIN_X, MAX_X), randint(MIN_Y, MAX_Y))
    z = (randint(MIN_X, MAX_X), randint(MIN_Y, MAX_Y))
    return (x, y, z)

f = open('coords.txt', 'w')

f.write(f'{GEN_NUM}\n')
for i in range(GEN_NUM):
    x, y, z = random_coordinate()
    f.write(f'{x[0]} {x[1]} {y[0]} {y[1]} {z[0]} {z[1]}\n');