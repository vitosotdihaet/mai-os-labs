from random import randint


GEN_NUM = 1000

MIN_X, MAX_X = -100, 100
MIN_Y, MAX_Y = -100, 100


def random_coordinate():
    return (randint(MIN_X, MAX_X), randint(MIN_Y, MAX_Y))

header = open('coordinates.h', 'w')

header.write(
'typedef struct {\n\
    int x;\n\
    int y;\n\
} coordinate;\n'
)
header.write('coordinate coordinates[] = {\n')

# coordinate coords[] = {
#     {1, 2}, 
#     {2, 3},
#     {3, 6},
#     {1, 2}, 
#     {2, 3},
#     ...
# };


txt = open('coordinates.txt', 'w')

for i in range(GEN_NUM):
    x, y = random_coordinate()
    header.write(f'\t{{{x}, {y}}},\n')

    txt.write(f'{x} {y}\n');

header.write('};')
