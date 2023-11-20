f = open('coords.txt')

biggest_coords = (None, None, None)
max_s = 0

coords = []

f.readline()
for line in f.readlines():
    x1, y1, x2, y2, x3, y3 = tuple(map(int, line.split()))
    a = x1, y1
    b = x2, y2
    c = x3, y3

    s = 0.5 * abs((x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1))

    if s > max_s:
        biggest_coords = (a, b, c)
        max_s = s

print(f'Max square is {max_s}\nCoordinates of a triangle with this square are {biggest_coords}')