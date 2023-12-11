from random import randint


GEN_NUM = 5_000_000

MIN_X, MAX_X = -100, 100
MIN_Y, MAX_Y = -100, 100


def random_coordinate():
    x = (randint(MIN_X, MAX_X), randint(MIN_Y, MAX_Y))
    y = (randint(MIN_X, MAX_X), randint(MIN_Y, MAX_Y))
    z = (randint(MIN_X, MAX_X), randint(MIN_Y, MAX_Y))
    return (x, y, z)

header = open('coordinates.h', 'w')

header.write(
'typedef struct {\n\
    int x;\n\
    int y;\n\
} coordinate;\n'
)
header.write('coordinate coordinates[][3] = {\n')

# int coords[][] = {
#     {{1, 2}, {2, 3}, {3, 6}},
#     {{1, 2}, {2, 3}, {3, 6}},
#     {{1, 2}, {2, 3}, {3, 6}},
#     {{1, 2}, {2, 3}, {3, 6}},
# };


txt = open('coordinates.txt', 'w')
txt.write(f'{GEN_NUM}')

for i in range(GEN_NUM):
    x, y, z = random_coordinate()
    header.write('\t{{' + f'{x[0]}' + ', ' + f'{x[1]}' + '}, '  )
    header.write(   '{' + f'{y[0]}' + ', ' + f'{y[1]}' + '}, '  )
    header.write(   '{' + f'{z[0]}' + ', ' + f'{z[1]}' + '}},\n')

    txt.write(f'{x[0]} {x[1]} {y[0]} {y[1]} {z[0]} {z[1]}\n');

header.write('};')
