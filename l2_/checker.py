f = open('coordinates.txt')

coordinates = []
for line in f.readlines():
    coordinates.append(tuple(map(float, line.split())))

lenght = len(coordinates)

biggest_coords = (None, None, None)
max_s = 0

for i in range(lenght - 2):
    for j in range(i + 1, lenght - 1):
        for k in range(j + 1, lenght):
            x1, y1 = coordinates[i]
            x2, y2 = coordinates[j]
            x3, y3 = coordinates[k]

            a = x1, y1
            b = x2, y2
            c = x3, y3

            s = 0.5 * abs((x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1))

            if s > max_s:
                biggest_coords = (a, b, c)
                max_s = s

print(f'Max square is {max_s}\nCoordinates of a triangle with this square are {biggest_coords}')